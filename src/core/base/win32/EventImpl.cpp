//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Script Event Handling and Dispatching
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "EventImpl.h"
#include "MainFormUnit.h"
#include "ThreadIntf.h"
#include "TickCount.h"
#include "TimerIntf.h"
#include "SysInitIntf.h"
#include "DebugIntf.h"
#include "WindowImpl.h"
#include <mmsystem.h>
#include <ddraw.h>


//---------------------------------------------------------------------------
// TVPInvokeEvents
//---------------------------------------------------------------------------
bool TVPEventInvoked = false;
void TVPInvokeEvents()
{
	if(TVPEventInvoked) return;
	TVPEventInvoked = true;
	if(TVPMainForm)
	{
		TVPMainForm->InvokeEvents();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPEventReceived
//---------------------------------------------------------------------------
void TVPEventReceived()
{
	TVPEventInvoked = false;
	TVPMainForm->NotifyEventDelivered();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCallDeliverAllEventsOnIdle
//---------------------------------------------------------------------------
void TVPCallDeliverAllEventsOnIdle()
{
	if(TVPMainForm)
	{
		TVPMainForm->CallDeliverAllEventsOnIdle();
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPBreathe
//---------------------------------------------------------------------------
static bool TVPBreathing = false;
void TVPBreathe()
{
	TVPEventDisabled = true; // not to call TVP events...
	TVPBreathing = true;
	try
	{
		Application->ProcessMessages(); // do Windows message pumping
	}
	catch(...)
	{
		TVPBreathing = false;
		TVPEventDisabled = false;
		throw;
	}

	TVPBreathing = false;
	TVPEventDisabled = false;
}
//---------------------------------------------------------------------------
bool TVPGetBreathing()
{
	// return whether now is in event breathing
	return TVPBreathing;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPSystemEventDisabledState
//---------------------------------------------------------------------------
void TVPSetSystemEventDisabledState(bool en)
{
	TVPMainForm->EventButton->Down = !en;
	if(!en) TVPDeliverAllEvents();
}
//---------------------------------------------------------------------------
bool TVPGetSystemEventDisabledState()
{
	return !TVPMainForm->EventButton->Down;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// tTVPContinuousHandlerCallLimitThread
//---------------------------------------------------------------------------
class tTVPContinuousHandlerCallLimitThread : public tTVPThread
{
	tjs_uint64 NextEventTick;
	tjs_uint64 Interval;
	tTVPThreadEvent Event;
	tTJSCriticalSection CS;

	bool Enabled;
	HWND UtilWindow;

public:
	tTVPContinuousHandlerCallLimitThread();
	~tTVPContinuousHandlerCallLimitThread();

protected:
	void Execute();

	void __fastcall UtilWndProc(Messages::TMessage &Msg)
	{
		Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
		return;
	}

public:
	void SetEnabled(bool enabled);

	void SetInterval(tjs_uint64 interval) { Interval = interval; }

} static * TVPContinuousHandlerCallLimitThread = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPContinuousHandlerCallLimitThread::tTVPContinuousHandlerCallLimitThread()
	 : tTVPThread(true)
{
	NextEventTick = 0;
	Interval = (1<<TVP_SUBMILLI_FRAC_BITS)*1000/60; // default 60Hz
	Enabled = false;
	UtilWindow = AllocateHWnd(UtilWndProc);
	Resume();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPContinuousHandlerCallLimitThread::~tTVPContinuousHandlerCallLimitThread()
{
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
	DeallocateHWnd(UtilWindow);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPContinuousHandlerCallLimitThread::Execute()
{
	while(!GetTerminated())
	{
		tjs_uint64 curtick = TVPGetTickCount() << TVP_SUBMILLI_FRAC_BITS;
		DWORD sleeptime;

		{	// thread-protected
			tTJSCriticalSectionHolder holder(CS);

			if(Enabled)
			{
				if(NextEventTick <= curtick)
				{
					TVPProcessContinuousHandlerEventFlag = true; // set flag to process event on next idle
					::PostMessage(UtilWindow, WM_APP+2, 0, 0);
					while(NextEventTick <= curtick) NextEventTick += Interval;
				}
				tjs_uint64 sleeptime_64 = NextEventTick - curtick;
				sleeptime = (DWORD)(sleeptime_64 >> TVP_SUBMILLI_FRAC_BITS) +
						((sleeptime_64 & ((1<<TVP_SUBMILLI_FRAC_BITS)-1))?1:0);
							// add 1 if fraction exists
			}
			else
			{
				sleeptime = 10000; // how long to sleep when disabled does not matter
			}


		}	// end-of-thread-protected

		if(sleeptime == 0) sleeptime = 1; // 0 will let thread sleeping forever ...
		Event.WaitFor(sleeptime);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPContinuousHandlerCallLimitThread::SetEnabled(bool enabled)
{
	tTJSCriticalSectionHolder holder(CS);

	Enabled = enabled;
	if(enabled)
	{
		tjs_uint64 curtick = TVPGetTickCount() << TVP_SUBMILLI_FRAC_BITS;
		NextEventTick = ((curtick + 1) / Interval) * Interval;
		Event.Set();
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static tjs_int TVPContinousHandlerLimitFrequency = 0;
//---------------------------------------------------------------------------
void TVPBeginContinuousEvent()
{
	// read commandline options
	static tjs_int ArgumentGeneration = 0;
	if(ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();

		tTJSVariant val;
		if( TVPGetCommandLine(TJS_W("-contfreq"), &val) )
		{
			TVPContinousHandlerLimitFrequency = (tjs_int)val;
		}
	}

	if(!TVPGetWaitVSync())
	{
		if(TVPContinousHandlerLimitFrequency == 0)
		{
			// no limit
			// this notifies continuous calling of TVPDeliverAllEvents.
			if(TVPMainForm) TVPMainForm->BeginContinuousEvent();
		}
		else
		{
			// has limit
			if(!TVPContinuousHandlerCallLimitThread)
				TVPContinuousHandlerCallLimitThread = new tTVPContinuousHandlerCallLimitThread();
			TVPContinuousHandlerCallLimitThread->SetInterval(
				 (1<<TVP_SUBMILLI_FRAC_BITS)*1000/TVPContinousHandlerLimitFrequency);
			TVPContinuousHandlerCallLimitThread->SetEnabled(true);
		}
	}


	TVPEnsureVSyncTimingThread();
	// if we wait vsync, the continuous handler will be executed at the every timing of
	// vsync.
}
//---------------------------------------------------------------------------
void TVPEndContinuousEvent()
{
	// anyway
	if(TVPContinuousHandlerCallLimitThread)
		TVPContinuousHandlerCallLimitThread->SetEnabled(false);

	// anyway
	if(TVPMainForm) TVPMainForm->EndContinuousEvent();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void TVPReleaseContinuousHandlerCallLimitThread()
{
	if(TVPContinuousHandlerCallLimitThread)
		delete TVPContinuousHandlerCallLimitThread,
			TVPContinuousHandlerCallLimitThread = NULL;
}
// to release TVPContinuousHandlerCallLimitThread at exit
static tTVPAtExit TVPTimerThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	TVPReleaseContinuousHandlerCallLimitThread);
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
static bool TVPWaitVSync = 0;
//---------------------------------------------------------------------------
bool TVPGetWaitVSync()
{
	static tjs_int ArgumentGeneration = 0;
	if(ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();
		TVPWaitVSync = false;

		tTJSVariant val;
		if(TVPGetCommandLine(TJS_W("-waitvsync"), &val))
		{
			ttstr str(val);
			if(str == TJS_W("yes"))
			{
				TVPWaitVSync = true;
				TVPEnsureDirectDrawObject();
			}
		}
	}
	return TVPWaitVSync;
}
//---------------------------------------------------------------------------e











//---------------------------------------------------------------------------
// VSync喘のタイミングを�k伏させるためのスレッド
//---------------------------------------------------------------------------
class tTVPVSyncTimingThread : public tTVPThread
{
	DWORD SleepTime;
	tTVPThreadEvent Event;
	tTJSCriticalSection CS;
	DWORD VSyncInterval; //!< VSync の�g侯(歌深��)
	DWORD LastVBlankTick; //!< 恷瘁の vblank の�r�g

	bool Enabled;
	HWND UtilWindow;

public:
	tTVPVSyncTimingThread();
	~tTVPVSyncTimingThread();

protected:
	void Execute();

	void __fastcall UtilWndProc(Messages::TMessage &Msg);

public:
	void MeasureVSyncInterval(); // VSyncInterval を���yする
} static * TVPVSyncTimingThread = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPVSyncTimingThread::tTVPVSyncTimingThread()
	 : tTVPThread(true)
{
	SleepTime = 1;
	LastVBlankTick = 0;
	VSyncInterval = 16; // 兜豚�｡�
	Enabled = false;
	UtilWindow = AllocateHWnd(UtilWndProc);
	MeasureVSyncInterval();
	Resume();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPVSyncTimingThread::~tTVPVSyncTimingThread()
{
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
	DeallocateHWnd(UtilWindow);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPVSyncTimingThread::Execute()
{
	while(!GetTerminated())
	{
		// SleepTime と LastVBlankTick を誼る
		DWORD sleep_time, last_vblank_tick;
		{	// thread-protected
			tTJSCriticalSectionHolder holder(CS);
			sleep_time = SleepTime;
			last_vblank_tick = LastVBlankTick;
		}

		// SleepTime 蛍蓄る
		// LastVBlankTick から軟麻し、SleepTime 蛍蓄る
		DWORD sleep_start_tick = timeGetTime();

		DWORD sleep_time_adj = sleep_start_tick - last_vblank_tick;

		if(sleep_time_adj < sleep_time)
		{
			Sleep(sleep_time - sleep_time_adj);
		}
		else
		{
			// 噸宥、メインスレッド坪で Event.Set() したならば、
			// タイムスライス(�Lくて10ms) が�Kわる��は
			// ここに栖ているはずである。
			// sleep_time は宥械 10ms より�Lいので、
			// ここに栖るってのは��械。
			// よほどシステムが嶷たい彜�Bになってると深えられる。
			// そこで羨て�Aけに イベントをポストするわけにはいかないので
			// �m輝な�r�g(云輝に�m輝) 蓄る。
			Sleep(5);
		}

		// イベントをポストする
		::PostMessage(UtilWindow, WM_APP+2, 0, (LPARAM)sleep_start_tick);

		Event.WaitFor(0x7fffffff); // vsync まで棋つ
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall tTVPVSyncTimingThread::UtilWndProc(Messages::TMessage &Msg)
{
	if(Msg.Msg != WM_APP+2)
	{
		Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
		return;
	}

	// tTVPVSyncTimingThread から誘げられたメッセ�`ジ

	// いま vblank 嶄��
	IDirectDraw2 * DirectDraw2 = TVPGetDirectDrawObjectNoAddRef();
	BOOL in_vblank = false;
	if(DirectDraw2)
		DirectDraw2->GetVerticalBlankStatus(&in_vblank);

	// �r�gをチェック
	bool drawn = false;
//	DWORD vblank_wait_start = timeGetTime();

	// VSync 棋ちを佩う
	bool delayed = false;
	if(!drawn)
	{
		if(!in_vblank)
		{
			// vblank から�iけるまで棋つ
			DWORD timeout_target_tick = timeGetTime() + 1;

			BOOL in_vblank = false;
			do
			{
				DirectDraw2->GetVerticalBlankStatus(&in_vblank);
			} while(in_vblank && (long)(timeGetTime() - timeout_target_tick) <= 0);

			// vblank に秘るまで棋つ
			in_vblank = true;
			do
			{
				DirectDraw2->GetVerticalBlankStatus(&in_vblank);
			} while(!in_vblank && (long)(timeGetTime() - timeout_target_tick) <= 0);

			if((int)(timeGetTime() - timeout_target_tick) > 0)
			{
				// フレ�`ムスキップが�k伏したと深えてよい
				delayed  =true;
			}
		}
	}

//	DWORD vblank_wait_end = timeGetTime();

	// タイマの�r�g圻泣を�O協する
	if(!delayed)
	{
		tTJSCriticalSectionHolder holder(CS);
		LastVBlankTick = timeGetTime(); // これが肝に蓄る�r�gの軟麻泣になる
	}
	else
	{
		tTJSCriticalSectionHolder holder(CS);
		LastVBlankTick += VSyncInterval; // これが肝に蓄る�r�gの軟麻泣になる(おおざっぱ)
		if((long) (timeGetTime() - (LastVBlankTick + SleepTime)) <= 0)
		{
			// 蓄った瘁、肝に軟きようとする�r�gがすでに�^肇なので蓄れません
			LastVBlankTick = timeGetTime(); // ��崙議に書の�r震にします
		}
	}

	// 鮫中の厚仟を佩う (DrawDeviceのShowメソッドを柵ぶ)
	if(!drawn) TVPDeliverDrawDeviceShow();

	// もし vsync 棋ちを佩う岷念、すでに vblank に秘っていた��栽は、
	// 棋つ�r�gが�Lすぎたと冱うことである
	if(in_vblank)
	{
		// その��栽は SleepTime を�pらす
		tTJSCriticalSectionHolder holder(CS);
		if(SleepTime > 8) SleepTime --;
	}
	else
	{
		// vblank で�oかった��栽は屈つの��栽が深えられる
		// 1. vblank 念だった
		// 2. vblank 瘁だった
		// どっちかは蛍からないが
		// SleepTime を��やす。ただしこれが VSyncInterval を階えるはずはない。
		tTJSCriticalSectionHolder holder(CS);
		SleepTime ++;
		if(SleepTime > VSyncInterval) SleepTime = VSyncInterval;
	}

	// タイマを軟�咾垢�
	Event.Set();

	// ContinuousHandler を柵ぶ
	// これは噴蛍な�r�gをとれるよう、vsync 棋ちの岷瘁に柵ばれる
	TVPProcessContinuousHandlerEventFlag = true; // set flag to invoke continuous handler on next idle

/*
static DWORD last_report_tick;

if(timeGetTime() > last_report_tick + 5000)
{
	last_report_tick = timeGetTime();
	TVPAddLog(TJS_W("SleepTime : ") + ttstr((int)SleepTime));
	TVPAddLog(TJS_W("VSyncInterval : ") + ttstr((int)VSyncInterval));
	TVPAddLog(TJS_W("VSync wait time : ") + ttstr((int)(vblank_wait_end - vblank_wait_start)));
}
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPVSyncTimingThread::MeasureVSyncInterval()
{
	TVPEnsureDirectDrawObject();

	DWORD vsync_interval = 10000;

	// vsync 巓豚を ms で誼る。
	// ms �g了なのであまり屎�_な�､狼辰蕕譴覆い�、まぁ蒙に���}ないこととする。

	// まず、DirectDraw が聞喘辛嬬な��栽、 WaitForVerticalBlank あるいは
	// GetScanLine を busy loop で�O��して巓豚を誼ることを��す。
	IDirectDraw2 * dd2 = TVPGetDirectDrawObjectNoAddRef();
	if(dd2)
	{
		// 歌深: http://hpcgi1.nifty.com/MADIA/Vcbbs/wwwlng.cgi?print+200605/06050028.txt

		// それにしても GetScanLine は佚喘ならない
		// これで屎械に巓豚を誼られない�h廠が謹すぎて僅廷
		DWORD start_tick;
		DWORD timeout;

		DWORD last_sync_tick;
		int repeat_count;
/*
		// まず、GetScanLine による巓豚の函誼を��みる
		DWORD last_scanline = 65536;

		// 恠�望�が圷に��るまで腎ル�`プ
		// ここからが云栖の���y。
		last_sync_tick = timeGetTime();
		timeout = 250;
		start_tick = timeGetTime();
		repeat_count = 0;
		while(timeGetTime() - start_tick < timeout)
		{
			DWORD scanline = 65536;
			if(FAILED(dd2->GetScanLine(&scanline))) scanline = 65536;
			if(scanline < last_scanline && last_scanline - scanline > 100)
			{
				// 恠�望�が圷に��った
				// 念指チェックした了崔よりも念に�､���った��栽は
				// ��｢したとみなす
				// 念指と曳べて100ライン參貧��ってることを�_�Jする。
				// これは W.Dee の�h廠 (GeForce 7600 GT) で、なぜか
				// まれにスキャンラインが1だけ��ることがあるという�F�鵑�
				// あったため。
				// しかしこの��貨をとってもまともに巓豚を函誼できない�h廠がある´´
				DWORD tick = timeGetTime();
				if(repeat_count > 2)
				{
					// 恷兜の方指の�Y惚は�里討�
					// 恷弌の�g侯を���hする
					if(tick - last_sync_tick < vsync_interval)
						vsync_interval = tick - last_sync_tick;
				}
				last_sync_tick = tick;
				repeat_count ++;
			}
			last_scanline = scanline;
		}

		TVPAddLog(TJS_W("Rough VSync interval measured by GetScanLine() : " + ttstr((int)vsync_interval)));

		// vsync 巓豚は�m俳っぽい��
		if(vsync_interval < 6 || vsync_interval > 66)
		{
			TVPAddLog(TJS_W("VSync interval by GetScanLine() seems to be strange, trying WaitForVerticalBlank() ..."));
*/
			// どうも��
			vsync_interval = 10000;
			// WaitForVerticalBlank による�y協を��みる
			// 恷兜のvblankを棋つ
			dd2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
			// 採指かル�`プを指して�g侯を�y協する
			timeout = 250;
			last_sync_tick = timeGetTime();
			start_tick = timeGetTime();
			repeat_count = 0;
			while(timeGetTime() - start_tick < timeout)
			{
				// vblank から�iけるまで棋つ
				BOOL in_vblank = false;
				do
				{
					dd2->GetVerticalBlankStatus(&in_vblank);
				} while(in_vblank && timeGetTime() - start_tick < timeout);

				DWORD aux_wait = timeGetTime();
				while(timeGetTime() - aux_wait < 2) ;
					// 1ms2msほどまつ
					// どうも、玉豚�gに vblank に秘ったり vblank から�iけたりするような
					// �Y惚が誼られることがある。
					// ��しい圻咀は蛍からないが、ここに�m輝なウェイトを秘れることで
					// なんとか���Iを��みる。

				// vblank に秘るまで棋つ
				in_vblank = true;
				do
				{
					dd2->GetVerticalBlankStatus(&in_vblank);
				} while(!in_vblank && timeGetTime() - start_tick < timeout);

				DWORD tick = timeGetTime();
				if(repeat_count > 2)
				{
					// 恷兜の方指の�Y惚は�里討�
					// 恷弌の�g侯を���hする
					// 恷弌の�g侯を���hする
					if(tick - last_sync_tick < vsync_interval)
						vsync_interval = tick - last_sync_tick;
				}
				last_sync_tick = tick;
				repeat_count ++;
			}

			TVPAddLog(TJS_W("Rough VSync interval measured by GetVerticalBlankStatus() : " + ttstr((int)vsync_interval)));
/*
		}
*/
	}


	// vsync 巓豚は�m俳っぽい��
	if(!dd2 || vsync_interval < 6 || vsync_interval > 66)
	{
		// どうもこれでも vsync 巓豚をうまくとれていないっぽい
		// そうなると、肝は API による函誼。
		// 歌深: http://www.interq.or.jp/moonstone/person/del/zenact01.htm
		DWORD vsync_rate = 0;

		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		GetVersionEx(&osvi);

		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			HDC dc = GetDC(0);
			vsync_rate = GetDeviceCaps(dc, VREFRESH);
			ReleaseDC(0, dc);
		}
		else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion >= 10) // Windows 98 or lator
		{
			tTVP_devicemodeA dm;
			ZeroMemory(&dm, sizeof(tTVP_devicemodeA));
			dm.dmSize = sizeof(tTVP_devicemodeA);
			dm.dmDriverExtra = 0;
			EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, reinterpret_cast<DEVMODE*>(&dm));
			vsync_rate = dm.dmDisplayFrequency;
		}

		if(vsync_rate != 0)
			vsync_interval = 1000 / vsync_rate;
		else
			vsync_interval = 0;

		TVPAddLog(TJS_W("Rough VSync interval read from API : " + ttstr((int)vsync_interval)));
	}

	// vsync 巓豚は�m俳っぽい��
	if(vsync_interval < 6 || vsync_interval > 66)
	{
		TVPAddLog(TJS_W("Rough VSync interval still seems wrong, assuming default value (16)"));
		vsync_interval = 16;
	}

	VSyncInterval = vsync_interval;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TVPEnsureVSyncTimingThread()
{
	// (もし駅勣ならば) VSyncTimingThread を恬撹する
	if(TVPGetWaitVSync())
	{
		if(!TVPVSyncTimingThread)
			TVPVSyncTimingThread = new tTVPVSyncTimingThread();
	}
	else
	{
		TVPReleaseVSyncTimingThread();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TVPReleaseVSyncTimingThread()
{
	if(TVPVSyncTimingThread)
		delete TVPVSyncTimingThread,
			TVPVSyncTimingThread = NULL;
}
//---------------------------------------------------------------------------
// to release TVPContinuousHandlerCallLimitThread at exit
static tTVPAtExit TVPVSyncTimingThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	TVPReleaseVSyncTimingThread);
//---------------------------------------------------------------------------






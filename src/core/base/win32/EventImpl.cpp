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
// VSync�äΥ����ߥ󥰤�k�������뤿��Υ���å�
//---------------------------------------------------------------------------
class tTVPVSyncTimingThread : public tTVPThread
{
	DWORD SleepTime;
	tTVPThreadEvent Event;
	tTJSCriticalSection CS;
	DWORD VSyncInterval; //!< VSync ���g��(�ο���)
	DWORD LastVBlankTick; //!< ����� vblank �Εr�g

	bool Enabled;
	HWND UtilWindow;

public:
	tTVPVSyncTimingThread();
	~tTVPVSyncTimingThread();

protected:
	void Execute();

	void __fastcall UtilWndProc(Messages::TMessage &Msg);

public:
	void MeasureVSyncInterval(); // VSyncInterval ��Ӌ�y����
} static * TVPVSyncTimingThread = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPVSyncTimingThread::tTVPVSyncTimingThread()
	 : tTVPThread(true)
{
	SleepTime = 1;
	LastVBlankTick = 0;
	VSyncInterval = 16; // ���ڂ���
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
		// SleepTime �� LastVBlankTick ��ä�
		DWORD sleep_time, last_vblank_tick;
		{	// thread-protected
			tTJSCriticalSectionHolder holder(CS);
			sleep_time = SleepTime;
			last_vblank_tick = LastVBlankTick;
		}

		// SleepTime ���ߤ�
		// LastVBlankTick �������㤷��SleepTime ���ߤ�
		DWORD sleep_start_tick = timeGetTime();

		DWORD sleep_time_adj = sleep_start_tick - last_vblank_tick;

		if(sleep_time_adj < sleep_time)
		{
			Sleep(sleep_time - sleep_time_adj);
		}
		else
		{
			// ��ͨ���ᥤ�󥹥�å��ڤ� Event.Set() �����ʤ�С�
			// �����ॹ�饤��(�L����10ms) ���K���핤�
			// ���������Ƥ���Ϥ��Ǥ��롣
			// sleep_time ��ͨ�� 10ms ����L���Τǡ�
			// ����������äƤΤϮ�����
			// ��ۤɥ����ƥब�ؤ���״�B�ˤʤäƤ�ȿ������롣
			// ���������ƾA���� ���٥�Ȥ�ݥ��Ȥ���櫓�ˤϤ����ʤ��Τ�
			// �m���ʕr�g(�������m��) �ߤ롣
			Sleep(5);
		}

		// ���٥�Ȥ�ݥ��Ȥ���
		::PostMessage(UtilWindow, WM_APP+2, 0, (LPARAM)sleep_start_tick);

		Event.WaitFor(0x7fffffff); // vsync �ޤǴ���
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

	// tTVPVSyncTimingThread ����Ͷ����줿��å��`��

	// ���� vblank �У�
	IDirectDraw2 * DirectDraw2 = TVPGetDirectDrawObjectNoAddRef();
	BOOL in_vblank = false;
	if(DirectDraw2)
		DirectDraw2->GetVerticalBlankStatus(&in_vblank);

	// �r�g������å�
	bool drawn = false;
//	DWORD vblank_wait_start = timeGetTime();

	// VSync �������Ф�
	bool delayed = false;
	if(!drawn)
	{
		if(!in_vblank)
		{
			// vblank ����i����ޤǴ���
			DWORD timeout_target_tick = timeGetTime() + 1;

			BOOL in_vblank = false;
			do
			{
				DirectDraw2->GetVerticalBlankStatus(&in_vblank);
			} while(in_vblank && (long)(timeGetTime() - timeout_target_tick) <= 0);

			// vblank �����ޤǴ���
			in_vblank = true;
			do
			{
				DirectDraw2->GetVerticalBlankStatus(&in_vblank);
			} while(!in_vblank && (long)(timeGetTime() - timeout_target_tick) <= 0);

			if((int)(timeGetTime() - timeout_target_tick) > 0)
			{
				// �ե�`�ॹ���åפ��k�������ȿ����Ƥ褤
				delayed  =true;
			}
		}
	}

//	DWORD vblank_wait_end = timeGetTime();

	// �����ޤΕr�gԭ����O������
	if(!delayed)
	{
		tTJSCriticalSectionHolder holder(CS);
		LastVBlankTick = timeGetTime(); // ���줬�Τ��ߤ�r�g�������ˤʤ�
	}
	else
	{
		tTJSCriticalSectionHolder holder(CS);
		LastVBlankTick += VSyncInterval; // ���줬�Τ��ߤ�r�g�������ˤʤ�(�������ä�)
		if((long) (timeGetTime() - (LastVBlankTick + SleepTime)) <= 0)
		{
			// �ߤä��ᡢ�Τ��𤭤褦�Ȥ���r�g�����Ǥ��^ȥ�ʤΤ��ߤ�ޤ���
			LastVBlankTick = timeGetTime(); // ���ƵĤ˽�Εr�̤ˤ��ޤ�
		}
	}

	// ����θ��¤��Ф� (DrawDevice��Show�᥽�åɤ����)
	if(!drawn) TVPDeliverDrawDeviceShow();

	// �⤷ vsync �������Ф�ֱǰ�����Ǥ� vblank ����äƤ������Ϥϡ�
	// ���ĕr�g���L���������Ԥ����ȤǤ���
	if(in_vblank)
	{
		// ���Έ��Ϥ� SleepTime ��p�餹
		tTJSCriticalSectionHolder holder(CS);
		if(SleepTime > 8) SleepTime --;
	}
	else
	{
		// vblank �ǟo���ä����Ϥ϶��ĤΈ��Ϥ���������
		// 1. vblank ǰ���ä�
		// 2. vblank ����ä�
		// �ɤä����Ϸ֤���ʤ���
		// SleepTime �򉈤䤹�����������줬 VSyncInterval �򳬤���Ϥ��Ϥʤ���
		tTJSCriticalSectionHolder holder(CS);
		SleepTime ++;
		if(SleepTime > VSyncInterval) SleepTime = VSyncInterval;
	}

	// �����ޤ����Ӥ���
	Event.Set();

	// ContinuousHandler �����
	// �����ʮ�֤ʕr�g��Ȥ��褦��vsync ������ֱ��˺��Ф��
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

	// vsync ���ڤ� ms �ǵä롣
	// ms �gλ�ʤΤǤ��ޤ����_�ʂ��ϵä��ʤ������ޤ��ؤˆ��}�ʤ����ȤȤ��롣

	// �ޤ���DirectDraw ��ʹ�ÿ��ܤʈ��ϡ� WaitForVerticalBlank ���뤤��
	// GetScanLine �� busy loop �ǱOҕ�������ڤ�ä뤳�Ȥ�ԇ����
	IDirectDraw2 * dd2 = TVPGetDirectDrawObjectNoAddRef();
	if(dd2)
	{
		// �ο�: http://hpcgi1.nifty.com/MADIA/Vcbbs/wwwlng.cgi?print+200605/06050028.txt

		// ����ˤ��Ƥ� GetScanLine �����äʤ�ʤ�
		// ��������������ڤ�ä��ʤ��h�����ह���ƶ���
		DWORD start_tick;
		DWORD timeout;

		DWORD last_sync_tick;
		int repeat_count;
/*
		// �ޤ���GetScanLine �ˤ�����ڤ�ȡ�ä�ԇ�ߤ�
		DWORD last_scanline = 65536;

		// �ߖ˾���Ԫ�ˑ���ޤǿե�`��
		// �������餬������Ӌ�y��
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
				// �ߖ˾���Ԫ�ˑ��ä�
				// ǰ�إ����å�����λ�ä���ǰ�˂������ä����Ϥ�
				// �͎������Ȥߤʤ�
				// ǰ�ؤȱȤ٤�100�饤�����ϑ��äƤ뤳�Ȥ�_�J���롣
				// ����� W.Dee �έh�� (GeForce 7600 GT) �ǡ��ʤ���
				// �ޤ�˥������饤��1�������뤳�Ȥ�����Ȥ����F��
				// ���ä����ᡣ
				// ���������Ό��ߤ�ȤäƤ�ޤȤ�����ڤ�ȡ�äǤ��ʤ��h�������롭��
				DWORD tick = timeGetTime();
				if(repeat_count > 2)
				{
					// ��������ؤνY���ϒΤƤ�
					// ��С���g����ӛ�h����
					if(tick - last_sync_tick < vsync_interval)
						vsync_interval = tick - last_sync_tick;
				}
				last_sync_tick = tick;
				repeat_count ++;
			}
			last_scanline = scanline;
		}

		TVPAddLog(TJS_W("Rough VSync interval measured by GetScanLine() : " + ttstr((int)vsync_interval)));

		// vsync ���ڤ��m�Фäݤ���
		if(vsync_interval < 6 || vsync_interval > 66)
		{
			TVPAddLog(TJS_W("VSync interval by GetScanLine() seems to be strange, trying WaitForVerticalBlank() ..."));
*/
			// �ɤ����
			vsync_interval = 10000;
			// WaitForVerticalBlank �ˤ��y����ԇ�ߤ�
			// �����vblank�����
			dd2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
			// �λؤ���`�פ�ؤ����g����y������
			timeout = 250;
			last_sync_tick = timeGetTime();
			start_tick = timeGetTime();
			repeat_count = 0;
			while(timeGetTime() - start_tick < timeout)
			{
				// vblank ����i����ޤǴ���
				BOOL in_vblank = false;
				do
				{
					dd2->GetVerticalBlankStatus(&in_vblank);
				} while(in_vblank && timeGetTime() - start_tick < timeout);

				DWORD aux_wait = timeGetTime();
				while(timeGetTime() - aux_wait < 2) ;
					// 1ms2ms�ۤɤޤ�
					// �ɤ��⡢�����g�� vblank ����ä��� vblank ����i�����ꤹ��褦��
					// �Y�����ä��뤳�Ȥ����롣
					// Ԕ����ԭ��Ϸ֤���ʤ������������m���ʥ������Ȥ����뤳�Ȥ�
					// �ʤ�Ȥ����I��ԇ�ߤ롣

				// vblank �����ޤǴ���
				in_vblank = true;
				do
				{
					dd2->GetVerticalBlankStatus(&in_vblank);
				} while(!in_vblank && timeGetTime() - start_tick < timeout);

				DWORD tick = timeGetTime();
				if(repeat_count > 2)
				{
					// ��������ؤνY���ϒΤƤ�
					// ��С���g����ӛ�h����
					// ��С���g����ӛ�h����
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


	// vsync ���ڤ��m�Фäݤ���
	if(!dd2 || vsync_interval < 6 || vsync_interval > 66)
	{
		// �ɤ��⤳��Ǥ� vsync ���ڤ򤦤ޤ��Ȥ�Ƥ��ʤ��äݤ�
		// �����ʤ�ȡ��Τ� API �ˤ��ȡ�á�
		// �ο�: http://www.interq.or.jp/moonstone/person/del/zenact01.htm
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

	// vsync ���ڤ��m�Фäݤ���
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
	// (�⤷��Ҫ�ʤ��) VSyncTimingThread �����ɤ���
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






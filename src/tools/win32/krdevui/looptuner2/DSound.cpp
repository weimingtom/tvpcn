//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <syncobjs.hpp>

#define DIRECTSOUND_VERSION 0x0300
#include <mmsystem.h>
#include <dsound.h>


#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>

#include "DSound.h"
#include "WaveLoopManager.h"

//---------------------------------------------------------------------------
static void __fastcall ResetSoundBuffer();
static void __fastcall FillSecondaryBuffer(void);
//---------------------------------------------------------------------------
static TCriticalSection *WriteLock = NULL;
class TBufferWriteLock // ロック喘クラス
{
public:
	__fastcall TBufferWriteLock(void) { if(WriteLock) WriteLock->Enter(); };
	__fastcall ~TBufferWriteLock(void) { if(WriteLock) WriteLock->Leave(); };
};
//---------------------------------------------------------------------------
class TPlayerThread : public TThread
{
public:
	__fastcall TPlayerThread(void) : TThread(true)
	{
		Priority = tpHigher;
		Suspended = false;
	}

	__fastcall ~TPlayerThread(void)
	{
		Suspended = false;
		Terminate();
		WaitFor();
	}

	void __fastcall Execute(void)
	{
		while(!Terminated)
		{
			FillSecondaryBuffer();
			Sleep(50);
		}
	}

} ;
//---------------------------------------------------------------------------
static HRESULT WINAPI (* procDirectSoundCreate)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN)=NULL;
static HMODULE DirectSoundDLLHandle=NULL;
static IDirectSound * Sound = NULL;
static IDirectSoundBuffer * SoundPrimaryBuffer = NULL;
static IDirectSoundBuffer * SoundBuffer = NULL;
static DWORD SoundBufferBytes;
static DWORD SoundBufferBytesHalf;
static DSBUFFERDESC SoundBufferDesc;
static int Frequency;
static int Channels;
static int SampleSize;
static bool DecodeEnded;
static int BufferRemainBlock;
static DWORD SoundBufferWritePos;
static bool FirstWrite;
static bool Playing = false;
static tTVPWaveLoopManager * Manager = NULL;
static TPlayerThread *PlayerThread = NULL;
static std::vector<tTVPWaveLoopSegment> Segments[2];
static std::vector<tTVPWaveLabel> Labels[2];
//---------------------------------------------------------------------------
void __fastcall InitDirectSound(HWND wnd)
{
	if(!Sound)
	{
		// DLL のロ�`ド

		if(DirectSoundDLLHandle==NULL)
		{
			DirectSoundDLLHandle = LoadLibrary("dsound.dll");
			if(!DirectSoundDLLHandle) throw Exception("dsound.dll をロ�`ドできません");
		}

		if(procDirectSoundCreate==NULL)
		{
			procDirectSoundCreate = (HRESULT (WINAPI*)(_GUID *,IDirectSound **,IUnknown*))
				GetProcAddress(DirectSoundDLLHandle,"DirectSoundCreate");
			if(!procDirectSoundCreate)
			{
				FreeLibrary(DirectSoundDLLHandle);
				DirectSoundDLLHandle=NULL;
				throw Exception("dsound.dll が��械です");
			}
		}
		// オブジェクト恬撹
		HRESULT hr;
		hr=procDirectSoundCreate(NULL,&Sound,NULL);
		if(FAILED(hr))
		{
			throw Exception(
				AnsiString("DirectSound を兜豚晒できません(麿でWaveを壅伏嶄?)/HRESULT:")+
					IntToHex((int)hr,8));
				// 恬撹に払�　）�に WAVE output が聞われていた��栽など、
				// こうなる
		}

		// �f�{レベルの�O協
		hr=Sound->SetCooperativeLevel(wnd, DSSCL_PRIORITY);
		if(FAILED(hr))
		{
			Sound->Release(); // サウンドオブジェクトをリリ�`ス
			Sound=NULL;
			throw Exception(AnsiString("DirectSound の�f�{レベルの�O協に払�，靴泙靴�(phase1)/HRESULT:")+
				IntToHex((int)hr,8));
		}

//--
		// プライマリバッファの恬撹

		SoundPrimaryBuffer= NULL; // �eに恬撹できなきゃそれでもいい

		ZeroMemory(&SoundBufferDesc,sizeof(DSBUFFERDESC));
		SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
		SoundBufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER;
		hr=Sound->CreateSoundBuffer(&SoundBufferDesc,
			&SoundPrimaryBuffer,NULL);

		if(FAILED(hr) || SoundPrimaryBuffer == NULL)
		{
			// DbgMsgAdd("少御:DirectSound プライマリバッファを恬撹できません");
			hr=Sound->SetCooperativeLevel(wnd, DSSCL_NORMAL);
			if(FAILED(hr))
			{
				Sound->Release(); // サウンドオブジェクトをリリ�`ス
				Sound = NULL;
				throw Exception(AnsiString("DirectSound の�f�{レベルの�O協に払�，靴泙靴�(phase2)/HRESULT:")+
					IntToHex((int)hr,8));
			}
		}

		
		if(SoundPrimaryBuffer)
		{
			WAVEFORMATEX wfx;
			wfx.cbSize=0;
			wfx.wFormatTag=WAVE_FORMAT_PCM;
			wfx.nChannels=2;
			wfx.nSamplesPerSec=44100;
			wfx.wBitsPerSample=16;
			wfx.nBlockAlign=(WORD)(wfx.wBitsPerSample/8*wfx.nChannels);
			wfx.nAvgBytesPerSec=wfx.nSamplesPerSec*wfx.nBlockAlign;
			SoundPrimaryBuffer->SetFormat(&wfx);
		}
		if(!WriteLock) WriteLock = new TCriticalSection;

		if(!PlayerThread)
			PlayerThread= new TPlayerThread();
	}
}
//---------------------------------------------------------------------------
void __fastcall FreeDirectSound(void)
{
	// サウンドバッファの篤��
	if(PlayerThread)
	{
		delete PlayerThread;
	}

	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}

	if(Sound)
	{
		if(SoundPrimaryBuffer) SoundPrimaryBuffer->Release();
		Sound->Release();
		SoundPrimaryBuffer=NULL;
		Sound=NULL;

		// DLL の盾慧
		if(DirectSoundDLLHandle)
		{
			FreeLibrary(DirectSoundDLLHandle);
			DirectSoundDLLHandle=NULL;
			procDirectSoundCreate=NULL;
		}
	}


	if(WriteLock) delete WriteLock;
}
//---------------------------------------------------------------------------
void __fastcall SetPrimaryBufferFormat(WAVEFORMATEX *wfx)
{
	if(SoundPrimaryBuffer)
		SoundPrimaryBuffer->SetFormat(wfx);
}
//---------------------------------------------------------------------------
void __fastcall CreateSoundBuffer(const WAVEFORMATEXTENSIBLE * wfx)
{
	// 0.25 昼�gのセカンダリバッファを恬撹する

	/* wfx.Format の nAvgBytesPerSec や nBlockAlign は�笋��､砲覆辰討い覆い�どうか
	   チェックが駅勣 */

	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}

	// プライマリバッファのフォ�`マットを壅�O協
	if(SoundPrimaryBuffer)
	{
		SoundPrimaryBuffer->SetFormat((const WAVEFORMATEX *)wfx);
	}

	// セカンダリバッファの恬撹
	memset(&SoundBufferDesc,0,sizeof(DSBUFFERDESC));
	SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	SoundBufferDesc.dwFlags=
		DSBCAPS_GETCURRENTPOSITION2/* | DSBCAPS_CTRLPAN */| DSBCAPS_CTRLVOLUME
		 |DSBCAPS_GLOBALFOCUS ;
	SoundBufferDesc.dwBufferBytes = wfx->Format.nAvgBytesPerSec /4;
	SoundBufferDesc.dwBufferBytes /=  wfx->Format.nBlockAlign * 2;
	SoundBufferDesc.dwBufferBytes *=  wfx->Format.nBlockAlign * 2;
	SoundBufferBytes = SoundBufferDesc.dwBufferBytes;
	SoundBufferBytesHalf = SoundBufferBytes / 2;

	Frequency = wfx->Format.nSamplesPerSec;
	Channels = wfx->Format.nChannels;
	SampleSize = wfx->Format.nBlockAlign;

	SoundBufferDesc.lpwfxFormat = (WAVEFORMATEX*)wfx;


	// セカンダリバッファを恬撹する
	HRESULT hr = Sound->CreateSoundBuffer(&SoundBufferDesc,&SoundBuffer,NULL);
	if(FAILED(hr))
	{
		SoundBuffer=NULL;
		throw Exception("セカンダリバッファの恬撹に払�，靴泙靴�"
			"/巓襖方:"+AnsiString(wfx->Format.nSamplesPerSec)+
			"/チャネル方:"+AnsiString(wfx->Format.nChannels)+
			"/ビット方:"+AnsiString(16)+
			"/HRESULT:"+IntToHex((int)hr,8));
	}

	ResetSoundBuffer();
}
//---------------------------------------------------------------------------
void __fastcall DestroySoundBuffer(void)
{
	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}
}
//---------------------------------------------------------------------------
static void __fastcall ResetSoundBuffer()
{
	if(!SoundBuffer) return;

	TBufferWriteLock lock;

	SoundBufferWritePos = 0;
	DecodeEnded = false;
	BufferRemainBlock = 0;
	Playing = false;


	BYTE *p1,*p2;
	DWORD b1,b2;

	if(SUCCEEDED(SoundBuffer->Lock(0,SoundBufferBytes,(void**)&p1,
		&b1,(void**)&p2,&b2,0)))
	{
		ZeroMemory(p1,SoundBufferBytes);
		SoundBuffer->Unlock((void*)p1,b1,(void*)p2,b2);
	}

	SoundBuffer->SetCurrentPosition(0);

	FirstWrite = true;
}
//---------------------------------------------------------------------------
void __fastcall StartPlay(const WAVEFORMATEXTENSIBLE * wfx, tTVPWaveLoopManager * manager)
{
	TBufferWriteLock lock;
	Manager = manager;

	CreateSoundBuffer(wfx);

	Playing = true;
	FillSecondaryBuffer();
	SoundBuffer->Play(0,0,DSBPLAY_LOOPING);
}
//---------------------------------------------------------------------------
void __fastcall StopPlay(void)
{
	if(!SoundBuffer) return;

	TBufferWriteLock lock;
	Playing = false;
	SoundBuffer->Stop();
	ResetSoundBuffer();
}
//---------------------------------------------------------------------------
static void __fastcall FillSecondaryBuffer(void)
{
	if(!Playing) return;
	if(!SoundBuffer) return;
	if(!Manager) return;

	TBufferWriteLock lock;

	int writepos;

	if(FirstWrite)
	{
		FirstWrite = false;

		writepos = 0;
		SoundBufferWritePos = 1;
	}
	else
	{
		DWORD pp=0,wp=0;
		SoundBuffer->GetCurrentPosition(&pp,&wp);

		wp/=SoundBufferBytesHalf;
		pp/=SoundBufferBytesHalf;

		DWORD d = wp-SoundBufferWritePos;
		if(d<0) d+=2;

		wp-=pp;
		if(wp<0) wp+=2;
			// この�r泣で wp =pp と wp の餓

		if(d <= wp)
		{
			// pp と wp の�gは�F壓處怙嶄のため、そこに��き�zむことはできない
			return;
		}

		writepos = SoundBufferWritePos?SoundBufferBytesHalf:0;
		SoundBufferWritePos ^= 1;

		// pp と wp の曝�gは處怙嶄のため、そこに��き�zんではならない
	}

	if(DecodeEnded)
	{
		BufferRemainBlock--;
		if(!BufferRemainBlock)
		{
			SoundBuffer->Stop();
			Playing = false;
			return;
		}
	}

	BYTE *p1,*p2;
	DWORD b1,b2;

	if(SUCCEEDED(SoundBuffer->Lock(writepos, SoundBufferBytesHalf,
		(void**)&p1, &b1, (void**)&p2, &b2, 0)))
	{
		tjs_uint written;
		Manager->Decode(p1, SoundBufferBytesHalf/SampleSize,
			written, Segments[writepos ? 1: 0],
				Labels[writepos ? 1: 0]);

		written *= SampleSize;
		if(written < SoundBufferBytesHalf)
		{
			// 怎りない蛍は 0 で�困燭�
			if(!DecodeEnded)
			{
				DecodeEnded = true;
				BufferRemainBlock = 2;
			}
			ZeroMemory(p1 + written, SoundBufferBytesHalf - written);
		}
		SoundBuffer->Unlock((void*)p1,b1,(void*)p2,b2);
	}
}
//---------------------------------------------------------------------------
DWORD __fastcall GetCurrentPlayingPos(void)
{
	// �F壓の處怙了崔を誼る
    if(!Playing) return -1;

	TBufferWriteLock lock;

	DWORD pp=0,wp=0;
	SoundBuffer->GetCurrentPosition(&pp,&wp);

	int idx;
	if(pp >= SoundBufferBytesHalf)
	{
		idx = 1;
		pp -= SoundBufferBytesHalf;
	}
	else
	{
		idx = 0;
	}

	pp /= SampleSize;

	tjs_uint pos = 0;
	for(unsigned int i = 0 ; i < Segments[idx].size(); i++)
	{
		tjs_uint limit = pos + (tjs_uint)Segments[idx][i].Length;
		if(pp >= pos && pp < limit)
			return (DWORD)(Segments[idx][i].Start + (pp - pos));
		pos = limit;
	}

	return 0; // will no be reached
}
//---------------------------------------------------------------------------
bool __fastcall GetPlaying(void)
{
	return Playing;
}
//---------------------------------------------------------------------------


#pragma package(smart_init)

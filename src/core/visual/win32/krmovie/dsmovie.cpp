//---------------------------------------------------------------------------
// ( part of KRMOVIE.DLL )
// (c)2001-2009, W.Dee <dee@kikyou.info> and contributors
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.


	Modified by T.Imoto <http://www.kaede-software.com>
*/

#include "dsmovie.h"
#include "CIStream.h"

#include "DShowException.h"
#include <strsafe.h>
#include <dmodshow.h>
#include <dmoreg.h>
#include "CDemuxSource.h"
#include "CWMReader.h"

// {BAE59473-019E-4f1f-8A8C-3D41A9F4921E}
static const GUID CLSID_WMReaderSource = 
{ 0xbae59473, 0x19e, 0x4f1f, { 0x8a, 0x8c, 0x3d, 0x41, 0xa9, 0xf4, 0x92, 0x1e } };

// WMVとWMAのDecoderのクラスID
static const GUID CLSID_WMVDecoderDMO = 
{ 0x82d353df, 0x90bd, 0x4382, { 0x8b, 0xc2, 0x3f, 0x61, 0x92, 0xb7, 0x6e, 0x34 } };
static const GUID CLSID_WMADecoderDMO = 
{ 0x2eeb4adf, 0x4578, 0x4d10, { 0xbc, 0xa7, 0xbb, 0x95, 0x5f, 0x56, 0x32, 0x0a } };

tTVPDSMovie::tTVPDSMovie()
{
	m_dwROTReg = 0xfedcba98;
	m_RegisteredROT = false;

	m_Proxy = NULL;
	m_Reader = NULL;

	CoInitialize(NULL);
	OwnerWindow = NULL;
	Visible = false;
	Rect.left = 0; Rect.top = 0; Rect.right = 320; Rect.bottom = 240;
	RefCount = 1;
	Shutdown = false;
}

tTVPDSMovie::~tTVPDSMovie()
{
	Shutdown = true;
	ReleaseAll();
	CoUninitialize();
}
//----------------------------------------------------------------------------
//! @brief	  	歌孚カウンタのインクリメント
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::AddRef()
{
	RefCount++;
}
//----------------------------------------------------------------------------
//! @brief	  	歌孚カウンタのデクリメント。1ならdelete。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Release()
{
	if(RefCount == 1)
		delete this;
	else
		RefCount--;
}
//----------------------------------------------------------------------------
//! @brief	  	インタ�`フェイスを盾慧する
//!
//! デバック�r、ROTにまだ鞠�hされている��栽は、ここで鞠�hを盾茅する。@n
//! しかし、云栖はこのクラスを�@覚したクラスで並念に鞠�h盾茅をコ�`ルした圭がよい
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::ReleaseAll()
{
	if( m_RegisteredROT )	// 鞠�hがまだ盾茅されていない�rはここで盾茅
		RemoveFromROT( m_dwROTReg );

	if( m_MediaControl.p != NULL )
	{
		m_MediaControl->Stop();
		m_MediaControl.Release();
	}
	if( m_MediaPosition.p != NULL )
		m_MediaPosition.Release();

	if( m_MediaSeeking.p != NULL )
		m_MediaSeeking.Release();

	if( m_MediaEventEx.p != NULL )
		m_MediaEventEx.Release();

	if( m_StreamSelect.p != NULL )
		m_StreamSelect.Release();

	if( m_BasicAudio.p != NULL )
		m_BasicAudio.Release();

	if( m_BasicVideo.p != NULL )
		m_BasicVideo.Release();

	if( m_GraphBuilder.p != NULL )
		m_GraphBuilder.Release();

	if( m_Proxy )
	{
		delete m_Proxy;
		m_Proxy = NULL;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを壅伏する
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Play()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Run()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Run.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを唯峭する
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Stop()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Stop()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Stop.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを匯�r唯峭する
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Pause()
{
	HRESULT	hr;
	if( FAILED(hr = Controller()->Pause()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Pause.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓のム�`ビ�`�r�gを�O協する
//! @param 		tick : �O協する�F壓の�r�g
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetPosition( unsigned __int64 tick )
{
	HRESULT	hr;
	if(Shutdown) return;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetPosition).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	requestTime = (LONGLONG)(tick * 10000);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetPosition).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::SetPosition).", hr);
		}
		LONGLONG	requestFrame = (LONGLONG)(((tick / 1000.0) / AvgTimePerFrame) + 0.5);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetPosition).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓のム�`ビ�`�r�gを函誼する
//! @param 		tick : �F壓の�r�gを卦す�篳�
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetPosition( unsigned __int64 *tick )
{
	if(Shutdown) return;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetCurrentPosition (in tTVPDSMovie::GetPosition).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetPosition).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		LONGLONG	curTime = (Current + 5000) / 10000; // 匯鬄∨��領緘襪靴討�く
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetPosition).", hr);
		}
		LONGLONG	curTime = (LONGLONG)(Current * AvgTimePerFrame * 1000.0);
		*tick = (unsigned __int64)( curTime < 0 ? 0 : curTime);
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓のム�`ビ�`の彜�Bを函誼する
//! @param 		status : �F壓の彜�Bを卦す�篳�
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetStatus(tTVPVideoStatus *status)
{
	if(Shutdown) return;
	HRESULT hr;
	OAFilterState state;
	hr = Controller()->GetState(50, &state);
	if(hr == VFW_S_STATE_INTERMEDIATE)
		*status = vsProcessing;
	else if(state == State_Stopped)
		*status = vsStopped;
	else if(state == State_Running)
		*status = vsPlaying;
	else if(state == State_Paused)
		*status = vsPaused;
}
//----------------------------------------------------------------------------
//! @brief	  	A sample has been delivered. Copy it to the texture.
//! @param 		evcode : イベントコ�`ド
//! @param 		param1 : パラメ�`タ1。坪否はイベントコ�`ドにより��なる。
//! @param 		param2 : パラメ�`タ2。坪否はイベントコ�`ドにより��なる。
//! @param 		got : 函誼の屎倦
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetEvent( long *evcode, long *param1, long *param2, bool *got )
{
	if(Shutdown) return;
	HRESULT hr;
	*got = false;
	hr = Event()->GetEvent(evcode, param1, param2, 0);
	if( SUCCEEDED(hr) )
	{
		if( *evcode == EC_ERRORABORT )
		{
			ThrowDShowException(L"Error Abort.", static_cast<HRESULT>(*param1) );
		}
		*got = true;
	}
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	イベントを盾慧する
//! 
//! GetEventでイベントを誼て、�I尖した瘁、このメソッドによってイベントを盾慧すること
//! @param 		evcode : 盾慧するイベントコ�`ド
//! @param 		param1 : 盾慧するパラメ�`タ1。坪否はイベントコ�`ドにより��なる。
//! @param 		param2 : 盾慧するパラメ�`タ2。坪否はイベントコ�`ドにより��なる。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::FreeEventParams(long evcode, long param1, long param2)
{
	if(Shutdown) return;

	Event()->FreeEventParams(evcode, param1, param2);
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	ム�`ビ�`を恷兜の了崔まで��き��す
//! @note		IMediaPositionは掲容�Xのようだが、サンプルでは聞喘されていたので、
//! 			揖じままにしておく。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::Rewind()
{
	if(Shutdown) return;

	HRESULT	hr;
	if( FAILED(hr = Position()->put_CurrentPosition(0)) )
	{
		ThrowDShowException(L"Failed to call IMediaPosition::put_CurrentPosition(0).", hr);
	}
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	峺協されたフレ�`ムへ卞�咾垢�
//! 
//! このメソッドによって�O協された了崔は、峺協したフレ�`ムと頼畠に匯崑するわけではない。
//! フレ�`ムは、峺協したフレ�`ムに恷も除いキ�`フレ�`ムの了崔に�O協される。
//! @param		f : 卞�咾垢襯侫讒`ム
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetFrame( int f )
{
	if(Shutdown) return;

	HRESULT	hr;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::SetFrame).", hr);
		}
		LONGLONG	requestTime = (LONGLONG)(AvgTimePerFrame * 10000000.0 * f);
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestTime, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetFrame).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		LONGLONG	requestFrame = f;
		if( FAILED(hr = MediaSeeking()->SetPositions( &requestFrame, AM_SEEKING_AbsolutePositioning | AM_SEEKING_SeekToKeyFrame, NULL, AM_SEEKING_NoPositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetFrame).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓のフレ�`ムを函誼する
//! @param		f : �F壓のフレ�`ムを秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetFrame( int *f )
{
	if(Shutdown) return;

	HRESULT		hr;
	LONGLONG	Current;
	if( FAILED(hr = MediaSeeking()->GetCurrentPosition( &Current ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetCurrentPosition (in tTVPDSMovie::GetFrame).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetFrame).", hr);
		}
		double	currentTime = Current / 10000000.0;
		*f = (int)(currentTime / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)Current;
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}

//----------------------------------------------------------------------------
//! @brief	  	峺協されたフレ�`ムで壅伏を唯峭させる
//! @param		f : 壅伏を唯峭させるフレ�`ム
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetStopFrame( int f )
{
	if(Shutdown) return;

	HRESULT	hr;
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetStopFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::SetStopFrame).", hr);
		}
		LONGLONG	requestTime = (LONGLONG)(AvgTimePerFrame * 10000000.0 * f);
		if( FAILED(hr = MediaSeeking()->SetPositions( NULL, AM_SEEKING_NoPositioning, &requestTime, AM_SEEKING_AbsolutePositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetStopFrame).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		LONGLONG	requestFrame = f;
		if( FAILED(hr = MediaSeeking()->SetPositions( NULL, AM_SEEKING_NoPositioning, &requestFrame, AM_SEEKING_AbsolutePositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetStopFrame).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓の壅伏が唯峭するフレ�`ムを函誼する
//! @param		f : �F壓の壅伏が唯峭するフレ�`ムを秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetStopFrame( int *f )
{
	if(Shutdown) return;

	HRESULT		hr;
	LONGLONG	Stop;
	if( FAILED(hr = MediaSeeking()->GetStopPosition( &Stop ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetStopPosition (in tTVPDSMovie::GetStopFrame).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetStopFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetStopFrame).", hr);
		}
		double	stopTime = Stop / 10000000.0;
		*f = (int)(stopTime / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)Stop;
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏を唯峭するフレ�`ムを兜豚彜�Bに��す。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetDefaultStopFrame()
{
	if(Shutdown) return;

	HRESULT	hr;
	GUID	Format;

	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::SetDefaultStopFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::GetDuration (in tTVPDSMovie::SetDefaultStopFrame).", hr);
		}
		if( FAILED(hr = MediaSeeking()->SetPositions( NULL, AM_SEEKING_NoPositioning, &totalTime, AM_SEEKING_AbsolutePositioning)) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_MEDIA_TIME, in tTVPDSMovie::SetDefaultStopFrame).", hr);
		}
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::GetDuration (in tTVPDSMovie::SetDefaultStopFrame).", hr);
		}
		if( FAILED(hr = MediaSeeking()->SetPositions( NULL, AM_SEEKING_NoPositioning, &totalTime, AM_SEEKING_AbsolutePositioning )) )
		{
			ThrowDShowException(L"Failed to call IMediaSeeking::SetPositions (TIME_FORMAT_FRAME, in tTVPDSMovie::SetDefaultStopFrame).", hr);
		}
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	FPSを函誼する
//! @param		f : FPSを秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetFPS( double *f )
{
	if(Shutdown) return;

	HRESULT	hr;
	REFTIME	AvgTimePerFrame;
	if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
	{
		ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetFPS).", hr);
	}
	*f = 1.0 / AvgTimePerFrame;
}
//----------------------------------------------------------------------------
//! @brief	  	畠フレ�`ム方を函誼する
//! @param		f : 畠フレ�`ム方を秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetNumberOfFrame( int *f )
{
	if(Shutdown) return;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetDuration (in tTVPDSMovie::GetNumberOfFrame).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetNumberOfFrame).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetNumberOfFrame).", hr);
		}
		double	totalSec = totalTime / 10000000.0;
		*f = (int)(totalSec / AvgTimePerFrame + 0.5);
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		*f = (int)totalTime;
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ム�`ビ�`の�Lさ(msec)を函誼する
//! @param		f : ム�`ビ�`の�Lさを秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetTotalTime( __int64 *t )
{
	if(Shutdown) return;

	HRESULT	hr;
	LONGLONG	totalTime;
	if( FAILED(hr = MediaSeeking()->GetDuration( &totalTime )) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetDuration (in tTVPDSMovie::GetTotalTime).", hr);
	}
	GUID	Format;
	if( FAILED(hr = MediaSeeking()->GetTimeFormat( &Format ) ) )
	{
		ThrowDShowException(L"Failed to call IMediaSeeking::GetTimeFormat (in tTVPDSMovie::GetTotalTime).", hr);
	}
	if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
	{
		*t = (__int64)(totalTime / 10000.0 );
	}
	else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
	{
		REFTIME	AvgTimePerFrame;
		if( FAILED(hr = GetAvgTimePerFrame( &AvgTimePerFrame )) )
		{
			ThrowDShowException(L"Failed to call IBasicVideo::get_AvgTimePerFrame (in tTVPDSMovie::GetTotalTime).", hr);
		}
		// フレ�`ムから昼へ、昼からmsecへ
		*t = (__int64)((totalTime * AvgTimePerFrame) * 1000.0 );
	}
	else
	{
		TVPThrowExceptionMessage(L"Not supported time format.");
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの鮫�颯汽ぅ困鯣ゝ辰垢�
//! @param		width : 嫌
//! @param		height : 互さ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetVideoSize( long *width, long *height )
{
	if(Shutdown) return;

	if( width != NULL )
		Video()->get_SourceWidth( width );

	if( height != NULL )
		Video()->get_SourceHeight( height );
}
//----------------------------------------------------------------------------
//! @brief	  	buffにNULLを�O協する。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetFrontBuffer( BYTE **buff )
{
	*buff = NULL;
	return;
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetWindow( HWND window )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetMessageDrainWindow( HWND window )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetRect( RECT *rect )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetVisible( bool b )
{
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏堀業を�O協する
//! @param	rate : 壅伏レ�`ト。1.0が吉堀。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetPlayRate( double rate )
{
	if(Shutdown) return;

	HRESULT hr;
	if( rate > 0.0 )
	{
		if( FAILED(hr = MediaSeeking()->SetRate(rate)) )
			ThrowDShowException(L"Failed to call IMediaSeeking::SetRate.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏堀業を函誼する
//! @param	*rate : 壅伏レ�`ト。1.0が吉堀。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetPlayRate( double *rate )
{
	if(Shutdown) { *rate = 1.0; return; }

	HRESULT hr;
	if( rate != NULL )
	{
		if( FAILED(hr = MediaSeeking()->GetRate(rate)) )
			ThrowDShowException(L"Failed to call IMediaSeeking::GetRate.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	オ�`ディオバランスを�O協する
//! @param	balance : バランスを峺協する。�､� -10,000  10,000 の���譴盃原┐任�る。
//! �､� -10,000 の��栽、嘔チャンネルは 100 dB �p縫され、�o咄となることを吭龍している。
//! �､� 10,000 の��栽、恣チャンネルが�o咄であることを吭龍している。
//! 寔嶄の�､� 0 で、これは�I圭のチャンネルがフル ボリュ�`ムであることを吭龍している。
//! 匯圭のチャンネルが�p縫されても、もう匯圭のチャンネルはフル ボリュ�`ムのままである。 
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetAudioBalance( long balance )
{
	if(Shutdown) return;

	HRESULT	hr;
	if( Audio() != NULL )
	{
		if( balance >= -10000 && balance <= 10000 )
		{
			if( FAILED( hr = Audio()->put_Balance(balance) ) )
				ThrowDShowException(L"Failed to call IBasicAudio::put_Balance.", hr);
		}
	}
}
//----------------------------------------------------------------------------
//! @brief	  	オ�`ディオバランスを函誼する
//! @param	*balance : バランスの���譴� -10,000  10,000までである。
//! �､� -10,000 の��栽、嘔チャンネルは 100 dB �p縫され、�o咄となることを吭龍している。
//! �､� 10,000 の��栽、恣チャンネルが�o咄であることを吭龍している。
//! 寔嶄の�､� 0 で、これは�I圭のチャンネルがフル ボリュ�`ムであることを吭龍している。
//! 匯圭のチャンネルが�p縫されても、もう匯圭のチャンネルはフル ボリュ�`ムのままである。 
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetAudioBalance( long *balance )
{
	if(Shutdown) { *balance = 0; return; }

	HRESULT	hr;
	if( Audio() != NULL && balance != NULL )
	{
		if( FAILED( hr = Audio()->get_Balance(balance) ) )
			ThrowDShowException(L"Failed to call IBasicAudio::get_Balance.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	オ�`ディオボリュ�`ムを�O協する
//! @param volume : ボリュ�`ムを -10,000  0 の方�､盃原┐垢襦�
//! 恷寄ボリュ�`ムは 0、�o咄は -10,000。
//! 駅勣なデシベル�､� 100 蔚する。たとえば、-10,000 = -100 dB。 
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetAudioVolume( long volume )
{
	if(Shutdown) return;

	HRESULT	hr;
	if( Audio() != NULL )
	{
		if( volume >= -10000 && volume <= 0 )
		{
			if( FAILED( hr = Audio()->put_Volume( volume ) ) )
				ThrowDShowException(L"Failed to call IBasicAudio::put_Volume.", hr);
		}
	}
}
//----------------------------------------------------------------------------
//! @brief	  	オ�`ディオボリュ�`ムを�O協する
//! @param volume : ボリュ�`ムを -10,000  0 の方�､盃原┐垢襦�
//! 恷寄ボリュ�`ムは 0、�o咄は -10,000。
//! 駅勣なデシベル�､� 100 蔚する。たとえば、-10,000 = -100 dB。 
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetAudioVolume( long *volume )
{
	if(Shutdown) { *volume = 0; } return;

	HRESULT	hr;
	if( Audio() != NULL && volume != NULL )
	{
		if( FAILED( hr = Audio()->get_Volume( volume ) ) )
			ThrowDShowException(L"Failed to call IBasicAudio::gut_Volume.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	オ�`ディオストリ�`ム方を函誼する
//! @param streamCount : オ�`ディオストリ�`ム方を秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetNumberOfAudioStream( unsigned long *streamCount )
{
	if(Shutdown) return;

	if( streamCount != NULL )
		*streamCount = m_AudioStreamInfo.size();
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したオ�`ディオストリ�`ム桑催のストリ�`ムを嗤�燭砲垢�
//! @param num : 嗤�燭砲垢襯��`ディオストリ�`ム桑催
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SelectAudioStream( unsigned long num )
{
	if(Shutdown) return;

	SelectStream( num, m_AudioStreamInfo );
}
//----------------------------------------------------------------------------
// @brief		嗤�燭淵��`ディオストリ�`ム桑催を誼る
// 匯桑兜めに��つかった嗤�燭淵好肇蟋`ム桑催を卦す。
// グル�`プ坪のすべてのストリ�`ムが嗤�燭任△訖苗榻圓發△襪�、tTVPDSMovie::SelectAudioStreamを聞喘した��栽、グル�`プ坪で1つだけか嗤�燭砲覆襦�
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetEnableAudioStreamNum( long *num )
{
	if(Shutdown) return;

	GetEnableStreamNum( num, m_AudioStreamInfo );
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオストリ�`ム方を函誼する
//! @param streamCount : ビデオストリ�`ム方を秘れる�篳�へのポインタ
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetNumberOfVideoStream( unsigned long *streamCount )
{
	if(Shutdown) return;

	if( streamCount != NULL )
		*streamCount = m_VideoStreamInfo.size();
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したビデオストリ�`ム桑催のストリ�`ムを嗤�燭砲垢�
//! @param num : 嗤�燭砲垢襯咼妊�ストリ�`ム桑催
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SelectVideoStream( unsigned long num )
{
	if(Shutdown) return;

	SelectStream( num, m_VideoStreamInfo );
}
//----------------------------------------------------------------------------
// @brief		嗤�燭淵咼妊�ストリ�`ム桑催を誼る
// 匯桑兜めに��つかった嗤�燭淵好肇蟋`ム桑催を卦す。
// グル�`プ坪のすべてのストリ�`ムが嗤�燭任△訖苗榻圓發△襪�、tTVPDSMovie::SelectAudioStreamを聞喘した��栽、グル�`プ坪で1つだけか嗤�燭砲覆襦�
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetEnableVideoStreamNum( long *num )
{
	if(Shutdown) return;

	GetEnableStreamNum( num, m_VideoStreamInfo );
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したストリ�`ム桑催のストリ�`ムを嗤�燭砲垢�
//! @param num : 嗤�燭砲垢襯好肇蟋`ム桑催
//! @param si : ビデオかオ�`ディオのストリ�`ム秤��
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SelectStream( unsigned long num, std::vector<StreamInfo> &si )
{
	if(Shutdown) return;

	HRESULT	hr;
	if( StreamSelect() != NULL && num < si.size() )
	{
		if( FAILED(hr = StreamSelect()->Enable( si[num].index, AMSTREAMSELECTENABLE_ENABLE )) )
			ThrowDShowException(L"Failed to call IAMStreamSelect::Enable.", hr);
	}
}
//----------------------------------------------------------------------------
// @brief		嗤�燭淵咼妊�ストリ�`ム桑催を誼る
//! @param num : 嗤�燭淵好肇蟋`ム桑催を秘れる�篳�へのポインタ
//! @param si : ビデオかオ�`ディオのストリ�`ム秤��
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetEnableStreamNum( long *num, std::vector<StreamInfo> &si )
{
	if(Shutdown) return;

	HRESULT	hr;
	*num = -1;
	if( StreamSelect() != NULL && si.size() > 0)
	{
		long strNum = 0;
		for( std::vector<StreamInfo>::iterator i = si.begin(); i != si.end(); i++ )
		{
			DWORD	dwFlags;
			if( FAILED(hr = StreamSelect()->Info( (*i).index, NULL, &dwFlags, NULL, NULL, NULL, NULL, NULL ) ) )
				ThrowDShowException(L"Failed to call IAMStreamSelect::Info.", hr);

			if( (dwFlags == AMSTREAMSELECTINFO_ENABLED) || (dwFlags == AMSTREAMSELECTINFO_EXCLUSIVE) )
			{
				*num = strNum;
				break;
			}
			strNum++;
		}
	}
}
//----------------------------------------------------------------------------
// @brief		オ�`ディオストリ�`ムを�o�燭砲垢�
// MPEG Iの�r、この荷恬は竃栖ない
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::DisableAudioStream( void )
{
	if(Shutdown) return;

	HRESULT	hr;
	if( StreamSelect() != NULL && m_AudioStreamInfo.size() > 0)
	{
		if( FAILED(hr = StreamSelect()->Enable( m_AudioStreamInfo[0].index, 0 )) )
			ThrowDShowException(L"Failed to call IAMStreamSelect::Enable.", hr);
#if 0
		for( std::vector<StreamInfo>::iterator i = m_AudioStreamInfo.begin(); i != m_AudioStreamInfo.end(); i++ )
		{
			if( FAILED(hr = StreamSelect()->Enable( (*i).index, 0 )) )
				ThrowDShowException(L"Failed to call IAMStreamSelect::Enable.", hr);
		}
#endif
	}
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetMixingBitmap( HDC hdc, RECT *dest, float alpha )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::ResetMixingBitmap()
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetMixingMovieAlpha( float a )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetMixingMovieAlpha( float *a )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetMixingMovieBGColor( unsigned long col )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetMixingMovieBGColor( unsigned long *col )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::PresentVideoImage()
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetContrastRangeMin( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetContrastRangeMax( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetContrastDefaultValue( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetContrastStepSize( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetContrast( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetContrast( float v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetBrightnessRangeMin( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetBrightnessRangeMax( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetBrightnessDefaultValue( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetBrightnessStepSize( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetBrightness( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetBrightness( float v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetHueRangeMin( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetHueRangeMax( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetHueDefaultValue( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetHueStepSize( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetHue( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetHue( float v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetSaturationRangeMin( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetSaturationRangeMax( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetSaturationDefaultValue( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetSaturationStepSize( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::GetSaturation( float *v )
{
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::SetSaturation( float v )
{
}

//----------------------------------------------------------------------------
//! @brief	  	ROT ( Running Object Table )にグラフを鞠�hする。
//!
//! Running Object Table functions: Used to debug. By registering the graph
//! in the running object table, GraphEdit is able to connect to the running
//! graph. This code should be removed before the application is shipped in
//! order to avoid third parties from spying on your graph.
//! @param		ROTreg : 鞠�hI.D.。なんか、それっぽいのを局せばいいんでないの。@n
//! 				サンプルではdwROTReg = 0xfedcba98というのを局している。
//! @return		屎倦
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::AddToROT( DWORD ROTreg )
{
	IUnknown			*pUnkGraph = m_GraphBuilder;
	IMoniker			*pmk;
	IRunningObjectTable	*pirot;

	if( FAILED(GetRunningObjectTable(0, &pirot)) )
		return E_FAIL;

	WCHAR	wsz[256];
//	wsprintfW(wsz, L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
//	swprintf(wsz, L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
	(void)StringCchPrintfW(wsz, NUMELMS(wsz), L"FilterGraph %08x  pid %08x", (DWORD_PTR) 0, GetCurrentProcessId());
	HRESULT hr = CreateItemMoniker(L"!", wsz, &pmk);
	if( SUCCEEDED(hr) )
	{
		hr = pirot->Register( 0, pUnkGraph, pmk, &ROTreg );
		m_RegisteredROT = true;
		pmk->Release();
	}
	pirot->Release();
	return hr;
}

//----------------------------------------------------------------------------
//! @brief	  	ROT ( Running Object Table )からグラフの鞠�hを盾茅する。
//! @param		ROTreg : AddToROTで局したのと揖じ麗を局す。
//----------------------------------------------------------------------------
void __stdcall tTVPDSMovie::RemoveFromROT( DWORD ROTreg )
{
	IRunningObjectTable *pirot;
	if (SUCCEEDED( GetRunningObjectTable(0, &pirot) ))
	{
		pirot->Revoke( ROTreg );
		m_RegisteredROT = false;
		pirot->Release();
	}
}
//----------------------------------------------------------------------------
//! @brief	  	1フレ�`ムの峠譲燕幣�r�gを函誼します
//! @param		pAvgTimePerFrame : 1フレ�`ムの峠譲燕幣�r�g
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT __stdcall tTVPDSMovie::GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame )
{
	if(Shutdown) return S_OK;

	return Video()->get_AvgTimePerFrame( pAvgTimePerFrame );
}
//----------------------------------------------------------------------------
//! @brief	  	����徨からム�`ビ�`のタイプを登�eします
//! @param		mt : メディアタイプを卦す�篳�への歌孚
//! @param		type : ム�`ビ�`ファイルの����徨
//----------------------------------------------------------------------------
void tTVPDSMovie::ParseVideoType( CMediaType &mt, const wchar_t *type )
{
	// note: audio-less mpeg stream must have an extension of
	// ".mpv" .
	if      (wcsicmp(type, L".mpg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpeg") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".mpv") == 0) 
		mt.subtype = MEDIASUBTYPE_MPEG1Video;
//		mt.subtype = MEDIASUBTYPE_MPEG1System;
	else if (wcsicmp(type, L".m1v") == 0) 
		mt.subtype = MEDIASUBTYPE_MPEG1Video;
	else if (wcsicmp(type, L".dat") == 0)
		mt.subtype = MEDIASUBTYPE_MPEG1VideoCD;
	else if (wcsicmp(type, L".avi") == 0)
		mt.subtype = MEDIASUBTYPE_Avi;
	else if (wcsicmp(type, L".mov") == 0)
		mt.subtype = MEDIASUBTYPE_QTMovie;
//	else if (wcsicmp(type, L".mp4") == 0)
//		mt.subtype = MEDIASUBTYPE_QTMovie;
//	else if (wcsicmp(type, L".wmv") == 0)
//		mt.subtype = SubTypeGUID_WMV3;
	else
		TVPThrowExceptionMessage(L"Unknown video format extension."); // unknown format
}
//----------------------------------------------------------------------------
//! @brief	  	����徨からム�`ビ�`がWindows Media Fileかどうか登�eします
//! @param		type : ム�`ビ�`ファイルの����徨
//! @return		Windows Media Fileかどうか
//----------------------------------------------------------------------------
bool tTVPDSMovie::IsWindowsMediaFile( const wchar_t *type ) const
{
	if( (wcsicmp(type, L".asf") == 0) || (wcsicmp(type, L".wma") == 0) ||
		(wcsicmp(type, L".wmv") == 0) )
	{
		return true;
	}
	else
	{
		return false;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	メディアタイプの�_慧
//!				IEnumMediaTypesで函誼したAM_MEDIA_TYPEは、このメソッドで��茅すること
//! @param		pmt : IEnumMediaTypesで函誼したAM_MEDIA_TYPE
//----------------------------------------------------------------------------
void tTVPDSMovie::UtilDeleteMediaType( AM_MEDIA_TYPE *pmt )
{
	// Allow NULL pointers for coding simplicity
	if( pmt == NULL )
		return;

	// Free media type's format data
	if (pmt->cbFormat != 0) 
	{
		CoTaskMemFree((PVOID)pmt->pbFormat);

		// Strictly unnecessary but tidier
		pmt->cbFormat = 0;
		pmt->pbFormat = NULL;
	}

	// Release interface
	if (pmt->pUnk != NULL) 
	{
		pmt->pUnk->Release();
		pmt->pUnk = NULL;
	}

	// Free media type
	CoTaskMemFree((PVOID)pmt);
}
//----------------------------------------------------------------------------
//! @brief	  	ピンに俊�A辛嬬なメディアタイプをデバッグ竃薦に預く
//! @param		pPin : 竃薦���鵑箸覆襯團�
//----------------------------------------------------------------------------
void tTVPDSMovie::DebugOutputPinMediaType( IPin *pPin )
{
	if( pPin == NULL ) return;
	CComPtr< IEnumMediaTypes > pMediaEnum;
	pPin->EnumMediaTypes(&pMediaEnum);
	if( pMediaEnum )
	{
		ULONG Fetched = 0;
		AM_MEDIA_TYPE *pMediaType;
		while( pMediaEnum->Next(1, &pMediaType, &Fetched) == S_OK )
		{
			if(Fetched)
			{
#if _DEBUG
				DisplayType( "DebugOutputPinMediaType:\n", pMediaType );
#endif
				UtilDeleteMediaType(pMediaType);
			}
		}
	}
}
//----------------------------------------------------------------------------
//! @brief	  	メディアタイプの�_慧
//!
//! AM_MEDIA_TYPEが隠隔しているデ�`タのみを�_慧する
//! @param		mt : �_慧するデ�`タを隠隔しているAM_MEDIA_TYPE
//----------------------------------------------------------------------------
void tTVPDSMovie::UtilFreeMediaType(AM_MEDIA_TYPE& mt)
{
	if(mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);

		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if(mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	グラフ坪からレンダ�`フィルタを冥して、函誼する
//! @param		mediatype : ���鵑箸垢襯譽鵐制`フィルタがサポ�`トするメディアタイプ
//! @param		ppFilter : ��つかったレンダ�`フィルタを鞭け函るポインタへのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::FindRenderer( const GUID *mediatype, IBaseFilter **ppFilter)
{
	HRESULT hr;
	IEnumFilters *pEnum = NULL;
	IBaseFilter *pFilter = NULL;
	IPin *pPin;
	ULONG ulFetched, ulInPins, ulOutPins;
	BOOL bFound=FALSE;

	// Verify graph builder interface
	if( !GraphBuilder() )
		return E_NOINTERFACE;

	// Verify that a media type was passed
	if( !mediatype )
		return E_POINTER;

	// Clear the filter pointer in case there is no match
	if( ppFilter )
		*ppFilter = NULL;

	// Get filter enumerator
	hr = GraphBuilder()->EnumFilters(&pEnum);
	if( FAILED(hr) )
		return hr;

	pEnum->Reset();

	// Enumerate all filters in the graph
	while(!bFound && (pEnum->Next(1, &pFilter, &ulFetched) == S_OK))
	{
#ifdef DEBUG
		// Read filter name for debugging purposes
		FILTER_INFO FilterInfo;
		TCHAR szName[256];

		hr = pFilter->QueryFilterInfo(&FilterInfo);
		if (SUCCEEDED(hr))
		{
#ifdef UNICODE	// Show filter name in debugger
			lstrcpyn(szName, FilterInfo.achName, 256);
#else
			WideCharToMultiByte(CP_ACP, 0, FilterInfo.achName, -1, szName, 256, 0, 0);
#endif
			FilterInfo.pGraph->Release();
		}
		szName[255] = 0;        // Null-terminate
#endif

		// Find a filter with one input and no output pins
		hr = CountFilterPins(pFilter, &ulInPins, &ulOutPins);
		if( FAILED(hr) )
			break;

		if( (ulInPins == 1) && (ulOutPins == 0) )
		{
			// Get the first pin on the filter
			pPin = 0;
			pPin = GetInPin(pFilter, 0);

			// Read this pin's major media type
			AM_MEDIA_TYPE type = {0};
			hr = pPin->ConnectionMediaType(&type);
			if( FAILED(hr) )
				break;

			// Is this pin's media type the requested type?
			// If so, then this is the renderer for which we are searching.
			// Copy the interface pointer and return.
			if( type.majortype == *mediatype )
			{
				// Found our filter
				*ppFilter = pFilter;
				bFound = TRUE;
			}
			else	// This is not the renderer, so release the interface.
				pFilter->Release();

			// Delete memory allocated by ConnectionMediaType()
			UtilFreeMediaType(type);
		}
		else
		{	// No match, so release the interface
			pFilter->Release();
		}
	}

	pEnum->Release();
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	グラフ坪からビデオレンダ�`フィルタを冥して、函誼する
//! @param		ppFilter : ��つかったビデオレンダ�`フィルタを鞭け函るポインタへのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::FindVideoRenderer( IBaseFilter **ppFilter)
{
	return FindRenderer( &MEDIATYPE_Video, ppFilter);
}
//----------------------------------------------------------------------------
//! @brief	  	ピンを函誼する
//! @param		ppFilter : ピンを隠隔しているフィルタ
//! @param		dirrequired : ピンの圭�� INPUT or OUTPUT
//! @param		iNum : 函誼したいピンの桑催 0
//! @param		ppPin : ピンを鞭け函るポインタへのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
	CComPtr< IEnumPins > pEnum;
	*ppPin = NULL;

	if( !pFilter )
		return E_POINTER;

	HRESULT hr = pFilter->EnumPins(&pEnum);
	if( FAILED(hr) )
		return hr;

	ULONG ulFound;
	IPin *pPin;
	hr = E_FAIL;

	while(S_OK == pEnum->Next(1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = (PIN_DIRECTION)3;

		pPin->QueryDirection(&pindir);
		if(pindir == dirrequired)
		{
			if(iNum == 0)
			{
				*ppPin = pPin;  // Return the pin's interface
				hr = S_OK;      // Found requested pin, so clear error
				break;
			}
			iNum--;
		}
		pPin->Release();
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	秘薦ピンを函誼する
//! @param		ppFilter : ピンを隠隔しているフィルタ
//! @param		nPin : 函誼したいピンの桑催 0
//! @return		ピンへのポインタ
//----------------------------------------------------------------------------
IPin *tTVPDSMovie::GetInPin( IBaseFilter * pFilter, int nPin )
{
	CComPtr<IPin> pComPin;
	GetPin(pFilter, PINDIR_INPUT, nPin, &pComPin);
	return pComPin;
}
//----------------------------------------------------------------------------
//! @brief	  	竃薦ピンを函誼する
//! @param		ppFilter : ピンを隠隔しているフィルタ
//! @param		nPin : 函誼したいピンの桑催 0
//! @return		ピンへのポインタ
//----------------------------------------------------------------------------
IPin *tTVPDSMovie::GetOutPin( IBaseFilter * pFilter, int nPin )
{
    CComPtr<IPin> pComPin;
    GetPin(pFilter, PINDIR_OUTPUT, nPin, &pComPin);
    return pComPin;
}
//----------------------------------------------------------------------------
//! @brief	  	フィルタが隠隔しているピンの方を函誼する
//! @param		ppFilter : ピンを隠隔しているフィルタ
//! @param		pulInPins : 秘薦ピンの方を鞭け函るための�篳�へのポインタ
//! @param		pulOutPins : 竃薦ピンの方を鞭け函るための�篳�へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::CountFilterPins(IBaseFilter *pFilter, ULONG *pulInPins, ULONG *pulOutPins)
{
	HRESULT hr=S_OK;
	IEnumPins *pEnum=0;
	ULONG ulFound;
	IPin *pPin;

	// Verify input
	if( !pFilter || !pulInPins || !pulOutPins )
		return E_POINTER;

	// Clear number of pins found
	*pulInPins = 0;
	*pulOutPins = 0;

	// Get pin enumerator
	hr = pFilter->EnumPins(&pEnum);
	if(FAILED(hr))
		return hr;

	pEnum->Reset();

	// Count every pin on the filter
	while(S_OK == pEnum->Next(1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = (PIN_DIRECTION) 3;
		hr = pPin->QueryDirection(&pindir);
		if(pindir == PINDIR_INPUT)
			(*pulInPins)++;
		else
			(*pulOutPins)++;
		pPin->Release();
	}
	pEnum->Release();
	return hr;
}

//----------------------------------------------------------------------------
//! @brief	  	MPEG1 喘のグラフを返�咾����Bする
//! @param		pRdr : グラフに歌紗しているレンダ�`フィルタ
//! @param		pSrc : グラフに歌紗しているソ�`スフィルタ
//----------------------------------------------------------------------------
void tTVPDSMovie::BuildMPEGGraph( IBaseFilter *pRdr, IBaseFilter *pSrc )
{
	HRESULT	hr;

	// Connect to MPEG 1 splitter filter
	CComPtr<IBaseFilter>	pMPEG1Splitter;	// for MPEG 1 splitter filter

	if( FAILED(hr = pMPEG1Splitter.CoCreateInstance(CLSID_MPEG1Splitter, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create MPEG 1 splitter filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEG1Splitter, L"MPEG-I Stream Splitter")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pMPEG1Splitter, L\"MPEG-I Stream Splitter\").", hr);
	if( FAILED(hr = ConnectFilters( pSrc, pMPEG1Splitter )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pSrc, pMPEG1Splitter ).", hr);

	// Connect to MPEG 1 video codec filter
	CComPtr<IBaseFilter>	pMPEGVideoCodec;	// for MPEG 1 video codec filter
	if( FAILED(hr = pMPEGVideoCodec.CoCreateInstance(CLSID_CMpegVideoCodec, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create MPEG 1 video codec filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGVideoCodec, L"MPEG Video Decoder")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pMPEGVideoCodec, L\"MPEG Video Decoder\").", hr);
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGVideoCodec )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pMPEG1Splitter, pMPEGVideoCodec ).", hr);

	// Connect to render filter
	if( FAILED(hr = ConnectFilters( pMPEGVideoCodec, pRdr )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pMPEGVideoCodec, pRdr ).", hr);

	// Connect to MPEG audio codec filter
	CComPtr<IBaseFilter>	pMPEGAudioCodec;	// for MPEG audio codec filter
	if( FAILED(hr = pMPEGAudioCodec.CoCreateInstance(CLSID_CMpegAudioCodec, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create MPEG audio codec filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pMPEGAudioCodec, L"MPEG Audio Decoder")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pMPEGAudioCodec, L\"MPEG Audio Decoder\").", hr);
	if( FAILED(hr = ConnectFilters( pMPEG1Splitter, pMPEGAudioCodec )) )
	{	// not have Audio.
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pMPEGAudioCodec)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pMPEGAudioCodec).", hr);
		return;
	}

	// Connect to DDS render filter
	CComPtr<IBaseFilter>	pDDSRenderer;	// for sound renderer filter
	if( FAILED(hr = pDDSRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create sound render filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pDDSRenderer, L"Sound Renderer")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pDDSRenderer, L\"Sound Renderer\").", hr);
	if( FAILED(hr = ConnectFilters( pMPEGAudioCodec, pDDSRenderer ) ) )
	{
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pMPEGAudioCodec)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pMPEGAudioCodec).", hr);
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pDDSRenderer)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pDDSRenderer).", hr);
	}
	else
	{	// This MPEG file have a audio stream.
		if( FAILED(hr = pMPEG1Splitter.QueryInterface( &m_StreamSelect ) ) )
			ThrowDShowException(L"Failed to query IAMStreamSelect.", hr);

		DWORD	numOfStream;
		if( FAILED(hr = StreamSelect()->Count( &numOfStream )) )
			ThrowDShowException(L"Failed to call StreamSelect()->Count(&numOfStream).", hr);

		for( int i = 0; i < (int)numOfStream; i++ )
		{
			DWORD	dwFlags;
			DWORD	dwGroup;
			AM_MEDIA_TYPE	*pmt;
			if( S_OK == StreamSelect()->Info( i, &pmt, &dwFlags, NULL, &dwGroup, NULL, NULL, NULL ) )
			{
				StreamInfo	si;
				si.groupNum = dwGroup;
				si.index = i;
				if( pmt->majortype == MEDIATYPE_Audio )
				{
					m_AudioStreamInfo.push_back( si );
				}
				else if( pmt->majortype == MEDIATYPE_Video )
				{
					m_VideoStreamInfo.push_back( si );
				}
				DeleteMediaType(pmt);
			}
		}
	}

	return;
}
//----------------------------------------------------------------------------
//! @brief	  	WMV 喘のグラフを返�咾����Bする
//! @param		pRdr : レンダ�`フィルタ
//! @param		pStream : ソ�`スストリ�`ム (WMVであること)
//----------------------------------------------------------------------------
void tTVPDSMovie::BuildWMVGraph( IBaseFilter *pRdr, IStream *pStream )
{
	HRESULT	hr = S_OK;

	CComPtr<IBaseFilter>	pWMSource;
	CWMReader		*pReader = new CWMReader();
	CDemuxSource	*pWMAS = new CDemuxSource(NULL, &hr, pReader, CLSID_WMReaderSource );
	if( FAILED(hr) )
		ThrowDShowException(L"Failed to create Windows Media stream source object.", hr);

	pWMSource = pWMAS;
	if( FAILED(hr = pWMAS->OpenStream( pStream ) ) )
		ThrowDShowException(L"Failed to call CDemuxSource::OpenStream( stream ).", hr);

	if( FAILED(hr = GraphBuilder()->AddFilter( pWMSource, L"Windows Media stream source")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter( pWMSource, L\"Windows Media stream source\").", hr);

	CComPtr<IBaseFilter>	pWMVDec;
	if( FAILED(pWMVDec.CoCreateInstance(CLSID_DMOWrapperFilter, NULL, CLSCTX_INPROC_SERVER )) )
		ThrowDShowException(L"Failed to create DMOWrapperFilter.", hr);

	{	// Set WMV Decoder DMO
		CComPtr<IDMOWrapperFilter>	pWmvDmoWrapper;
		if( FAILED(hr = pWMVDec.QueryInterface( &pWmvDmoWrapper )) )
			ThrowDShowException(L"Failed to query IDMOWrapperFilter.", hr);
		if( FAILED(hr = pWmvDmoWrapper->Init(CLSID_WMVDecoderDMO, DMOCATEGORY_VIDEO_DECODER)) )
			ThrowDShowException(L"Failed to call IDMOWrapperFilter::Init.", hr);
	}
	if( FAILED(hr = GraphBuilder()->AddFilter( pWMVDec, L"Windows Media Video Decoder (DMO Wrapper)")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter( pWMVDec, L\"Windows Media Video Decoder (DMO Wrapper)\").", hr);

	// Connect to decoder filter
	if( FAILED(hr = ConnectFilters( pWMSource, pWMVDec )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pWMSource, pWMVDec ).", hr);

	// Connect to render filter
	if( FAILED(hr = ConnectFilters( pWMVDec, pRdr )) )
		ThrowDShowException(L"Failed to call ConnectFilters( pWMVDec, pRdr ).", hr);

	CComPtr<IBaseFilter>	pWMADec;
	if( FAILED(pWMADec.CoCreateInstance(CLSID_DMOWrapperFilter, NULL, CLSCTX_INPROC_SERVER )) )
		ThrowDShowException(L"Failed to create DMOWrapperFilter.", hr);

	{	// Set WMA Decoder DMO
		CComPtr<IDMOWrapperFilter>	pWmaDmoWrapper;
		if( FAILED(hr = pWMADec.QueryInterface( &pWmaDmoWrapper )) )
			ThrowDShowException(L"Failed to query IDMOWrapperFilter.", hr);
		if( FAILED(hr = pWmaDmoWrapper->Init(CLSID_WMADecoderDMO, DMOCATEGORY_AUDIO_DECODER)) )
			ThrowDShowException(L"Failed to call IDMOWrapperFilter::Init.", hr);
	}
	if( FAILED(hr = GraphBuilder()->AddFilter( pWMADec, L"Windows Media Audio Decoder (DMO Wrapper)")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter( pWMADec, L\"Windows Media Audio Decoder (DMO Wrapper)\").", hr);

	// Connect to decoder filter
	if( FAILED(hr = ConnectFilters( pWMSource, pWMADec )) )
	{	// オ�`ディオがない
		if( FAILED(hr = GraphBuilder()->RemoveFilter( pWMADec)) )
			ThrowDShowException(L"Failed to call GraphBuilder()->RemoveFilter( pDDSRenderer).", hr);
		return;
	}

	CComPtr<IBaseFilter>	pDDSRenderer;	// for sound renderer filter
	if( FAILED(hr = pDDSRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER)) )
		ThrowDShowException(L"Failed to create sound render filter object.", hr);
	if( FAILED(hr = GraphBuilder()->AddFilter(pDDSRenderer, L"Sound Renderer")) )
		ThrowDShowException(L"Failed to call GraphBuilder()->AddFilter(pDDSRenderer, L\"Sound Renderer\").", hr);
	if( FAILED(hr = ConnectFilters( pWMADec, pDDSRenderer ) ) )
		ThrowDShowException(L"Failed to call ConnectFilters( pWMADec, pDDSRenderer ).", hr);

}
//----------------------------------------------------------------------------
//! @brief	  	2つのフィルタ�`を俊�Aする
//! @param		pFilterUpstream : アップストリ�`ムフィルタ
//! @param		pFilterDownstream : ダウンストリ�`ムフィルタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT tTVPDSMovie::ConnectFilters( IBaseFilter* pFilterUpstream, IBaseFilter* pFilterDownstream )
{
	HRESULT			hr = E_FAIL;
	CComPtr<IPin>	pIPinUpstream;
	PIN_INFO		PinInfoUpstream;
	PIN_INFO		PinInfoDownstream;
#if _DEBUG
	char debug[256];
#endif

	// validate passed in filters
	ASSERT(pFilterUpstream);
	ASSERT(pFilterDownstream);

	// grab upstream filter's enumerator
	CComPtr<IEnumPins> pIEnumPinsUpstream;
	if( FAILED(hr = pFilterUpstream->EnumPins(&pIEnumPinsUpstream)) )
		ThrowDShowException(L"Failed to call pFilterUpstream->EnumPins(&pIEnumPinsUpstream).", hr);

	// iterate through upstream filter's pins
	while( pIEnumPinsUpstream->Next (1, &pIPinUpstream, 0) == S_OK )
	{
		if( FAILED(hr = pIPinUpstream->QueryPinInfo(&PinInfoUpstream)) )
			ThrowDShowException(L"Failed to call pIPinUpstream->QueryPinInfo(&PinInfoUpstream).", hr);
#if _DEBUG
		sprintf(debug, "upstream: %ls\n", PinInfoUpstream.achName);
		OutputDebugString(debug);

		DebugOutputPinMediaType(pIPinUpstream);
#endif

		CComPtr<IPin>	 pPinDown;
		pIPinUpstream->ConnectedTo( &pPinDown );

		// bail if pins are connected
		// otherwise check direction and connect
		if( (PINDIR_OUTPUT == PinInfoUpstream.dir) && (pPinDown == NULL) )
		{
			// grab downstream filter's enumerator
			CComPtr<IEnumPins>	pIEnumPinsDownstream;
			hr = pFilterDownstream->EnumPins (&pIEnumPinsDownstream);

			// iterate through downstream filter's pins
			CComPtr<IPin>	pIPinDownstream;
			while( pIEnumPinsDownstream->Next (1, &pIPinDownstream, 0) == S_OK )
			{
				// make sure it is an input pin
				if( SUCCEEDED(hr = pIPinDownstream->QueryPinInfo(&PinInfoDownstream)) )
				{
#if _DEBUG
					sprintf(debug, "    downstream: %ls\n", PinInfoDownstream.achName);
					OutputDebugString(debug);

					DebugOutputPinMediaType(pIPinDownstream);
#endif
					CComPtr<IPin>	 pPinUp;
					pIPinDownstream->ConnectedTo( &pPinUp );
					if( (PINDIR_INPUT == PinInfoDownstream.dir) && (pPinUp == NULL) )
					{
						if( SUCCEEDED(hr = m_GraphBuilder->ConnectDirect( pIPinUpstream, pIPinDownstream, NULL)) )
						{
							PinInfoDownstream.pFilter->Release();
							PinInfoUpstream.pFilter->Release();
							return S_OK;
						}
#if _DEBUG
						else
							OutputDebugString( DShowException(hr).what() );
#endif
					}
				}

				PinInfoDownstream.pFilter->Release();
				pIPinDownstream = NULL;
			} // while next downstream filter pin

			//We are now back into the upstream pin loop
		} // if output pin

		pIPinUpstream = NULL;
		PinInfoUpstream.pFilter->Release();
	} // while next upstream filter pin

	return E_FAIL;
}


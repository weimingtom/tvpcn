//---------------------------------------------------------------------------
// ( part of KRMOVIE.DLL )
// (c)2001-2009, W.Dee <dee@kikyou.info> and contributors
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.


	Modified by T.Imoto <http://www.kaede-software.com> 2004
*/

#include "dsoverlay.h"
#include "CIStream.h"

#include "DShowException.h"
#include "OptionInfo.h"

//----------------------------------------------------------------------------
//! @brief	  	採もしない
//----------------------------------------------------------------------------
tTVPDSVideoOverlay::tTVPDSVideoOverlay()
{}
//----------------------------------------------------------------------------
//! @brief	  	インタ�`フェイスを盾慧する
//----------------------------------------------------------------------------
tTVPDSVideoOverlay::~tTVPDSVideoOverlay()
{
	ReleaseAll();
}
//----------------------------------------------------------------------------
//! @brief	  	フィルタグラフの���B
//! @param 		callbackwin : メッセ�`ジを僕佚するウィンドウ
//! @param 		stream : �iみ�zみ圷ストリ�`ム
//! @param 		streamname : ストリ�`ムの兆念
//! @param 		type : メディアタイプ(����徨)
//! @param 		size : メディアサイズ
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::BuildGraph( HWND callbackwin, IStream *stream,
	const wchar_t * streamname, const wchar_t *type, unsigned __int64 size )
{
	HRESULT			hr;

//	CoInitialize(NULL);

	// detect CMediaType from stream's extension ('type')
	try {
		if( FAILED(hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)) )
			ThrowDShowException(L"Failed to call CoInitializeEx.", hr);

		// create IFilterGraph instance
		if( FAILED(hr = m_GraphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC)) )
			ThrowDShowException(L"Failed to create FilterGraph.", hr);

		// Register to ROT
		if(GetShouldRegisterToROT())
		{
			AddToROT(m_dwROTReg);
		}

		if( IsWindowsMediaFile(type) )
		{
			CComPtr<IBaseFilter>	pVRender;	// for video renderer filter
			if( FAILED(hr = pVRender.CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER)) )
				ThrowDShowException(L"Failed to create video renderer filter object.", hr);
			if( FAILED(hr = GraphBuilder()->AddFilter(pVRender, L"Video Renderer")) )
				ThrowDShowException(L"Failed to call IFilterGraph::AddFilter.", hr);

			BuildWMVGraph( pVRender, stream );
		}
		else
		{
			CMediaType mt;
			mt.majortype = MEDIATYPE_Stream;
			ParseVideoType( mt, type ); // may throw an exception

			// create proxy filter
			m_Proxy = new CIStreamProxy( stream, size );
			hr = S_OK;
			m_Reader = new CIStreamReader( m_Proxy, &mt, &hr );

			if( FAILED(hr) || m_Reader == NULL )
				ThrowDShowException(L"Failed to create proxy filter object.", hr);

			m_Reader->AddRef();
			// add fliter
			if( FAILED(hr = GraphBuilder()->AddFilter( m_Reader, NULL)) )
				ThrowDShowException(L"Failed to call IFilterGraph::AddFilter.", hr);

			// AddFilterしたのでRelease
			m_Reader->Release();

			if( mt.subtype == MEDIASUBTYPE_Avi || mt.subtype == MEDIASUBTYPE_QTMovie )
			{
				// render output pin
				if( FAILED(hr = GraphBuilder()->Render(m_Reader->GetPin(0))) )
					ThrowDShowException(L"Failed to call IGraphBuilder::Render.", hr);
			}
			else
			{
				CComPtr<IBaseFilter>	pVRender;	// for video renderer filter
				if( FAILED(hr = pVRender.CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC_SERVER)) )
					ThrowDShowException(L"Failed to create video renderer filter object.", hr);
				if( FAILED(hr = GraphBuilder()->AddFilter(pVRender, L"Video Renderer")) )
					ThrowDShowException(L"Failed to call IFilterGraph::AddFilter.", hr);

				BuildMPEGGraph( pVRender, m_Reader); // may throw an exception
			}
		}

		// query each interfaces
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaControl )) )
			ThrowDShowException(L"Failed to query IMediaControl", hr);
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaPosition )) )
			ThrowDShowException(L"Failed to query IMediaPosition", hr);
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaSeeking )) )
			ThrowDShowException(L"Failed to query IMediaSeeking", hr);
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_MediaEventEx )) )
			ThrowDShowException(L"Failed to query IMediaEventEx", hr);

		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_BasicVideo )) )
			ThrowDShowException(L"Failed to query IBasicVideo", hr);

		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_VideoWindow )) )
			ThrowDShowException(L"Failed to query IVideoWindow", hr);
		if( FAILED(hr = m_GraphBuilder.QueryInterface( &m_BasicAudio )) )
			ThrowDShowException(L"Failed to query IBasicAudio", hr);

		// check whether the stream has video 
		if(!m_VideoWindow || !m_BasicVideo )
			TVPThrowExceptionMessage(L"The stream has no video components.");

		{
			long visible;
			if(FAILED(hr = VideoWindow()->get_Visible(&visible)))
				ThrowDShowException(L"The stream has no video components or has unsupported video format.", hr);
		}
	
		// disable AutoShow
		VideoWindow()->put_AutoShow(OAFALSE);

		// set notify event
		if(callbackwin)
		{
			if(FAILED(hr = Event()->SetNotifyWindow((OAHWND)callbackwin, WM_GRAPHNOTIFY, (long)(this))))
				ThrowDShowException(L"Failed to set IMediaEventEx::SetNotifyWindow.", hr);
		}
	}
	catch(const wchar_t *msg)
	{
		MakeAPause(true);
		ReleaseAll();
		CoUninitialize();
		TVPThrowExceptionMessage(msg);
	}
	catch(...)
	{
		MakeAPause(true);
		ReleaseAll();
		CoUninitialize();
		throw;
	}

	MakeAPause(false);
	CoUninitialize();	// ここでこれを柵ぶとまずそうな�櫃�するけど、寄嬋健なのかなぁ
}

//----------------------------------------------------------------------------
//! @brief	  	インタ�`フェイスを盾慧する
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::ReleaseAll()
{
	// Reset owner window
//	VideoWindow()->put_Owner(NULL);

	if( m_MediaControl.p != NULL )
	{
		m_MediaControl->Stop();
	}

	if( m_VideoWindow.p )
		m_VideoWindow.Release();

	tTVPDSMovie::ReleaseAll();
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオ ウィンドウを侭嗤する�Hウィンドウを�O協する。 
//! 
//! ビデオ ウィンドウを侭嗤する�Hウィンドウを�O協し、燕幣裳侘も揖�rに�O協する。
//! @param 		window : �Hウィンドウ
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::SetWindow( HWND window )
{
	if(Shutdown) return;

	HRESULT hr;
	if(OwnerWindow != window)
	{
		if(window == NULL)
		{
			if( FAILED(hr = VideoWindow()->put_Visible(OAFALSE)) )
			{
				ThrowDShowException(L"Failed to call IVideoWindow::put_Visible(OAFALSE).", hr);
			}
		}
		if( FAILED(hr = VideoWindow()->put_Owner((OAHWND)window)) )
		{
			ThrowDShowException(L"Failed to call IVideoWindow::put_Owner.", hr);
		}
		if(window != NULL)
		{
			if( FAILED(hr = VideoWindow()->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)) )
			{
				ThrowDShowException(L"Failed to call IVideoWindow::put_WindowStyle.", hr);
			}
			if( FAILED(hr = VideoWindow()->SetWindowPosition(Rect.left, Rect.top,
				Rect.right-Rect.left, Rect.bottom-Rect.top)) )
			{
				ThrowDShowException(L"Failed to call IVideoWindow::SetWindowPosition.", hr);
			}
			if( FAILED(hr = VideoWindow()->put_Visible(Visible?OATRUE:OAFALSE)) )
			{
				ThrowDShowException(L"Failed to call IVideoWindow::put_Visible.", hr);
			}
		}
		OwnerWindow = window;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオ ウィンドウのメッセ�`ジの僕佚枠ウィンドウを峺協する。
//! @param 		window : 僕佚枠ウィンドウ
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::SetMessageDrainWindow( HWND window )
{
	HRESULT hr;
	if( FAILED(hr = VideoWindow()->put_MessageDrain((OAHWND)window)) )
	{
		ThrowDShowException(L"Failed to call IVideoWindow::put_MessageDrain.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	燕幣裳侘を�O協する
//! @param 		rect : 燕幣裳侘
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::SetRect( RECT *rect )
{
	if(Shutdown) return;
	HRESULT hr;
	Rect = *rect;
	if( FAILED(hr = VideoWindow()->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top)) )
	{
		ThrowDShowException(L"Failed to call IVideoWindow::SetWindowPosition.", hr);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	燕幣/掲燕幣を�O協する
//! @param 		b : 燕幣/掲燕幣
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::SetVisible( bool b )
{
	if(Shutdown) return;
	HRESULT hr;
	if (OwnerWindow )
	{
		if( FAILED(hr = VideoWindow()->put_Visible(b?OATRUE:OAFALSE)) )
		{
			ThrowDShowException(L"Failed to call IVideoWindow::put_Visible.", hr);
		}
	}
	Visible = b;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオを壅伏する
//----------------------------------------------------------------------------
void __stdcall tTVPDSVideoOverlay::Play()
{
	if( Shutdown ) return;

	HRESULT hr;
	if( FAILED(hr = VideoWindow()->SetWindowPosition( Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top )) )
	{
		ThrowDShowException(L"Failed to call IVideoWindow::SetWindowPosition.", hr);
	}

	if( FAILED(hr = Controller()->Run()) )
	{
		ThrowDShowException(L"Failed to call IMediaControl::Run.", hr);
	}

	if( FAILED(hr = VideoWindow()->SetWindowPosition(Rect.left, Rect.top,
		Rect.right-Rect.left, Rect.bottom-Rect.top)) )
	{
		ThrowDShowException(L"Failed to call IVideoWindow::SetWindowPosition.", hr);
	}
}
/****************************************************************************/
/*! @file
@brief DirectShow

バッファヘレンダリングする
-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/05
@note
			2004/08/05	T.Imoto		
*****************************************************************************/

#include <streams.h>
#include <atlbase.h>
#include "BufferRenderer.h"

#ifdef _DEBUG
#include <stdio.h>
#include "DShowException.h"
#endif

//----------------------------------------------------------------------------
//##	TBufferRenderer
//----------------------------------------------------------------------------
//! @brief	  	This goes in the factory template table to create new filter instances
//! @param		pUnk : 鹿撹した IUnknown インタ�`フェイスへのポインタ。
//! @param		phr : メソッドの撹孔払�，鯤召� HRESULT �､鯤椶曳，��篳�へのポインタ。
//----------------------------------------------------------------------------
CUnknown * WINAPI TBufferRenderer::CreateInstance( LPUNKNOWN pUnk, HRESULT *phr )
{
	CUnknown	*punk = new TBufferRenderer( NAME("Buffer Renderer"), pUnk, phr );
	if( punk == NULL )
		*phr = E_OUTOFMEMORY;
	return punk;
}
#pragma warning(disable: 4355)	// コンストラクタのベ�`スメンバ兜豚晒�rにthisを聞うとワ�`ニングが竃るのでそれを雙峭
//----------------------------------------------------------------------------
//! @brief	  	TBufferRenderer constructor
//! @param		pName : デバッグのために聞喘される��峰へのポインタ。
//! @param		pUnk : 鹿撹された侭嗤宀オブジェクトへのポインタ。
//! @param		phr : HRESULT �､悗離櫂ぅ鵐拭�
//----------------------------------------------------------------------------
TBufferRenderer::TBufferRenderer( TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr )
 : CBaseVideoRenderer( CLSID_BufferRenderer, pName, pUnk, phr )
, m_InputPin( this, &m_InterfaceLock, phr, L"Input" )
, m_Allocator( this, NAME("Allocator"), GetOwner(), phr )
{
	//CBaseRender::m_pInputPinにポインタを�O協する。
	m_pInputPin = &m_InputPin;

	// Store and AddRef the texture for our use.
	*phr = S_OK;
	m_Buffer[0] = NULL;
	m_Buffer[1] = NULL;

	m_IsBufferOwner[0] = false;
	m_IsBufferOwner[1] = false;

	m_FrontBuffer = 0;

	m_StartFrame = 0;
}
#pragma warning(default: 4355)
//----------------------------------------------------------------------------
//! @brief	  	TBufferRenderer destructor
//----------------------------------------------------------------------------
TBufferRenderer::~TBufferRenderer()
{
	//CBaseRender::m_pInputPinにポインタをリセットする。
	//これをしないとCBaseRenderのデストラクタでdeleteされてしまうので廣吭��
    m_pInputPin = NULL;

	// 徭蛍で�_隠している��栽バッファの盾慧
	FreeFrontBuffer();
	FreeBackBuffer();
}
//----------------------------------------------------------------------------
//! @brief	  	勣箔されたインタ�`フェイスを卦す
//! 
//! Overriden to say what interfaces we support and where
//! @param		riid : インタ�`フェイスのIID
//! @param		ppv : インタ�`フェイスを卦すポインタ�`へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRenderer::NonDelegatingQueryInterface( REFIID riid, void **ppv )
{
	CheckPointer( ppv, E_POINTER );
	if(riid == IID_IRendererBufferAccess) {
		*ppv = static_cast<IRendererBufferAccess*>(this);
		static_cast<IUnknown*>(*ppv)->AddRef();
		return S_OK;
	} else if(riid == IID_IRendererBufferVideo) {
		*ppv = static_cast<IRendererBufferVideo*>(this);
		static_cast<IUnknown*>(*ppv)->AddRef();
		return S_OK;
	}
	return CBaseVideoRenderer::NonDelegatingQueryInterface( riid, ppv );
}
//----------------------------------------------------------------------------
//! @brief	  	蒙協のメディア タイプをフィルタが鞭け秘れるかどうかを�_�Jする
//! 
//! This method forces the graph to give us an R8G8B8 video type, making our copy 
//! to texture memory trivial.
//! @param		pmt : 戻宛されたメディア タイプを根む CMediaType オブジェクトへのポインタ
//! @return		戻宛されたメディア タイプが鞭け秘れられるなら S_OK を卦す。
//!				そうでなければ S_FALSE かエラ�` コ�`ドを卦す。
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::CheckMediaType( const CMediaType *pmt )
{
	HRESULT		hr = E_FAIL;
	VIDEOINFO	*pvi;

	// Reject the connection if this is not a video type
	if( *pmt->FormatType() != FORMAT_VideoInfo )
		return E_INVALIDARG;

	// Only accept RGB32
	pvi = (VIDEOINFO *)pmt->Format();
	if( IsEqualGUID( *pmt->Type(), MEDIATYPE_Video) )
	{
		if( IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB32) || 
			IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_ARGB32) )
		{
			hr = S_OK;
			m_MtIn = (*pmt);
		}
	}

	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	Graph connection has been made. 
//! @param		pmt : メディア タイプを峺協する CMediaType オブジェクトへのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::SetMediaType( const CMediaType *pmt )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション

	// Retrive the size of this media type
	VIDEOINFO *pviBmp;						// Bitmap info header
	pviBmp = (VIDEOINFO *)pmt->Format();
	m_VideoWidth  = pviBmp->bmiHeader.biWidth;
	m_VideoHeight = abs(pviBmp->bmiHeader.biHeight);
	m_VideoPitch = m_VideoWidth * 4;	// RGB32に�Qめ嬉ち

	if( !IsAllocatedFrontBuffer() )
		AllocFrontBuffer( GetBufferSize() );

	if( !IsAllocatedBackBuffer() )
		AllocBackBuffer( GetBufferSize() );

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	A sample has been delivered. Copy it to the texture.
//! @param		pSample : サンプルの IMediaSample インタ�`フェイスへのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::DoRenderSample( IMediaSample * pSample )
{
	DWORD	*pBmpBuffer, *pTxtBuffer;	// Bitmap buffer, texture buffer
	BYTE	*pTxtOrgPos;

//	if( m_bEOS ) return S_OK;

	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション

	// Get the video bitmap buffer
	pSample->GetPointer( reinterpret_cast<BYTE**>(&pBmpBuffer) );

	// Get the texture buffer & pitch
	pTxtBuffer = reinterpret_cast<DWORD*>(GetBackBuffer());
	pTxtOrgPos = reinterpret_cast<BYTE*>(pTxtBuffer);

	HRESULT		hr;
	LONG		EventParam1 = -1;
	LONGLONG	TimeStart = 0;
	LONGLONG	TimeEnd = 0;

	if( SUCCEEDED(hr = pSample->GetMediaTime( &TimeStart, &TimeEnd )) )
	{
		EventParam1 = (LONG)TimeStart;
	}
	if( m_StopFrame && EventParam1 >= m_StopFrame )
		return S_OK;	// 壅伏しないフレ�`ム

	if( pTxtBuffer == pBmpBuffer )	// 徭念のアロケ�`タ�`が聞われている
	{
		SwapBuffer( pSample );	// FrontとBackバッファを秘れ紋える
		if( m_pSink )
			m_pSink->Notify( EC_UPDATE, EventParam1, NULL );
		return S_OK;
	}

	// 徭念のアロケ�`タ�`ではないのでメモリをコピ�`する
#if 0
	// 和から貧にコピ�`(貧和郡��晒)
	{
		int		height = m_VideoHeight;
		int		width = m_VideoWidth;
		pBmpBuffer += width * (height-1);
		for( int j = 0; j < height; j++ )
		{
			for( int i = 0; i < width; i++ )
			{
				pTxtBuffer[i] = pBmpBuffer[i];
			}
			pBmpBuffer -= width;
			pTxtBuffer += width;
		}
	}
#else
	// 貧から和にコピ�`
	{
		int		height = m_VideoHeight;
		int		width = m_VideoWidth;
		for( int j = 0; j < height; j++ )
		{
			for( int i = 0; i < width; i++ )
			{
				pTxtBuffer[i] = pBmpBuffer[i];
			}
			pBmpBuffer += width;
			pTxtBuffer += width;
		}
	}
#endif
	if( m_pSink )
		m_pSink->Notify( EC_UPDATE, EventParam1, NULL );
	SwapBuffer( pSample );	// FrontとBackバッファを秘れ紋える
	return S_OK;
}
//---------------------------------------------------------------------------
//! @brief	  	フロントバッファとバックバッファを秘れ紋える
//! @param		pSample : サンプル。この嶄のポインタを�筝�する
//----------------------------------------------------------------------------
void TBufferRenderer::SwapBuffer( IMediaSample *pSample )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( m_FrontBuffer == 1 )
	{
		SetPointer( pSample, m_Buffer[1] );
		m_FrontBuffer = 0;
	}
	else
	{
		SetPointer( pSample, m_Buffer[0] );
		m_FrontBuffer = 1;
	}
}
//---------------------------------------------------------------------------
//! @brief	  	フロントバッファにメモリを護り輝てる
//! @param		size : 護り輝てるサイズ
//----------------------------------------------------------------------------
void TBufferRenderer::AllocFrontBuffer( size_t size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	BYTE	*buff = NULL;

	FreeFrontBuffer();
	if( m_FrontBuffer == 1 )
	{
		buff = m_Buffer[1] = reinterpret_cast<BYTE*>(CoTaskMemAlloc(size));
		m_IsBufferOwner[1] = true;
	}
	else
	{
		buff = m_Buffer[0] = reinterpret_cast<BYTE*>(CoTaskMemAlloc(size));
		m_IsBufferOwner[0] = true;
	}

	if( buff == NULL )
		throw L"Cannot allocate memory in filter.";
}
//---------------------------------------------------------------------------
//! @brief	  	バックバッファにメモリを護り輝てる。
//! @param		size : 護り輝てるサイズ
//----------------------------------------------------------------------------
void TBufferRenderer::AllocBackBuffer( size_t size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	BYTE	*buff = NULL;

	FreeBackBuffer();
	if( m_FrontBuffer == 1 )
	{
		buff = m_Buffer[0] = reinterpret_cast<BYTE*>(CoTaskMemAlloc(size));
		m_IsBufferOwner[0] = true;
	}
	else
	{
		buff = m_Buffer[1] = reinterpret_cast<BYTE*>(CoTaskMemAlloc(size));
		m_IsBufferOwner[1] = true;
	}

	if( buff == NULL )
		throw L"Cannot allocate memory in filter.";
}
//---------------------------------------------------------------------------
//! @brief	  	フロントバッファに護り輝てられているメモリを�_慧する
//!
//! もし、護り輝てられているメモリが、このクラスによって護り輝てられたものでない��栽は、
//! 盾慧しない。
//----------------------------------------------------------------------------
void TBufferRenderer::FreeFrontBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( m_FrontBuffer == 1 )
	{
		if( m_Buffer[1] != NULL )
		{
			if( m_IsBufferOwner[1] )
				CoTaskMemFree( m_Buffer[1] );
			m_Buffer[1] = NULL;
		}
		m_IsBufferOwner[1] = false;
	}
	else
	{
		if( m_Buffer[0] != NULL )
		{
			if( m_IsBufferOwner[0] )
				CoTaskMemFree( m_Buffer[0] );
			m_Buffer[0] = NULL;
		}
		m_IsBufferOwner[0] = false;
	}
}
//---------------------------------------------------------------------------
//! @brief	  	バックバッファに護り輝てられているメモリを�_慧する
//!
//! もし、護り輝てられているメモリが、このクラスによって護り輝てられたものでない��栽は、
//! 盾慧しない。
//----------------------------------------------------------------------------
void TBufferRenderer::FreeBackBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( m_FrontBuffer == 1 )
	{
		if( m_Buffer[0] != NULL )
		{
			if( m_IsBufferOwner[0] )
				CoTaskMemFree( m_Buffer[0] );
			m_Buffer[0] = NULL;
		}
		m_IsBufferOwner[0] = false;
	}
	else
	{
		if( m_Buffer[1] != NULL )
		{
			if( m_IsBufferOwner[1] )
				CoTaskMemFree( m_Buffer[1] );
			m_Buffer[1] = NULL;
		}
		m_IsBufferOwner[1] = false;
	}
}
//---------------------------------------------------------------------------
//! @brief	  	フロントバッファにバッファへのポインタを�O協する
//! @param		buff : バッファへのポインタ
//----------------------------------------------------------------------------
void TBufferRenderer::SetFrontBuffer( BYTE *buff )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	FreeFrontBuffer();
	if( m_FrontBuffer == 1 )
		m_Buffer[1] = buff;
	else
		m_Buffer[0] = buff;
}
//---------------------------------------------------------------------------
//! @brief	  	バックバッファにバッファへのポインタを�O協する
//! @param		buff : バッファへのポインタ
//----------------------------------------------------------------------------
void TBufferRenderer::SetBackBuffer( BYTE *buff )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	FreeBackBuffer();
	if( m_FrontBuffer == 1 )
		m_Buffer[0] = buff;
	else
		m_Buffer[1] = buff;
	
	SetPointer( buff );
}
//---------------------------------------------------------------------------
//! @brief	  	フロントバッファへのポインタを函誼する
//! @return		バッファへのポインタ
//----------------------------------------------------------------------------
BYTE *TBufferRenderer::GetFrontBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( m_FrontBuffer == 1 )
		return m_Buffer[1];
	else
		return m_Buffer[0];
}
//---------------------------------------------------------------------------
//! @brief	  	バックバッファへのポインタを函誼する
//! @return		バッファへのポインタ
//----------------------------------------------------------------------------
BYTE *TBufferRenderer::GetBackBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( m_FrontBuffer == 1 )
		return m_Buffer[0];
	else
		return m_Buffer[1];
}
//----------------------------------------------------------------------------
//! @brief	  	フロントバッファを�O協します。
//! @param		buff : フロントバッファ喘バッファへのポインタ
//! @param		size : バッファのサイズを局す�篳�へのポインタ。@n
//!					buffがNULLの�r、ここに圀しいサイズが卦る
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::SetFrontBuffer( BYTE *buff, long *size )
{
	if( m_State == State_Running )
		return S_FALSE;

	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( buff == NULL && size != NULL )
	{
		*size = GetBufferSize();
		return S_OK;
	}
	if( buff == NULL || size == NULL )
		return E_POINTER;

	if( (*size) != GetBufferSize() )
		return E_INVALIDARG;

	FreeFrontBuffer();
	SetFrontBuffer(buff);
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	バックバッファを�O協します。
//! @param		buff : バックバッファ喘バッファへのポインタ
//! @param		size : バッファのサイズを局す�篳�へのポインタ。@n
//!					buffがNULLの�r、ここに圀しいサイズが卦る
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::SetBackBuffer( BYTE *buff, long *size )
{
	if( m_State == State_Running )
		return S_FALSE;

	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	if( buff == NULL && size != NULL )
	{
		*size = GetBufferSize();
		return S_OK;
	}
	if( buff == NULL || size == NULL )
		return E_POINTER;

	if( (*size) != GetBufferSize() )
		return E_INVALIDARG;

	FreeBackBuffer();
	SetBackBuffer(buff);
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief		フロントバッファへのポインタを函誼します。
//! @param		buff : フロントバッファへのポインタを卦すためのバッファへのポインタ
//! @param		size : バッファのサイズを卦す�篳�へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::GetFrontBuffer( BYTE **buff, long *size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	*buff = GetFrontBuffer();
	*size = GetBufferSize();
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	バックバッファへのポインタを函誼します。
//! @param		buff : バックバッファへのポインタを卦すためのバッファへのポインタ
//! @param		size : バッファのサイズを卦す�篳�へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::GetBackBuffer( BYTE **buff, long *size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// クリティカルセクション
	*buff = GetBackBuffer();
	*size = GetBufferSize();
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	1フレ�`ムの峠譲燕幣�r�gを函誼します
//! @param		pAvgTimePerFrame : 1フレ�`ムの峠譲燕幣�r�g
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::get_AvgTimePerFrame( REFTIME *pAvgTimePerFrame )
{
	if( pAvgTimePerFrame ) {
		*pAvgTimePerFrame = (reinterpret_cast<VIDEOINFOHEADER *>(m_MtIn.Format())->AvgTimePerFrame) / 10000000.0;
		return S_OK;
	}
	else
		return E_POINTER;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの嫌を函誼します
//! @param		pVideoWidth : ビデオの嫌
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::get_VideoWidth( long *pVideoWidth )
{
	if( pVideoWidth ) {
		*pVideoWidth = reinterpret_cast<VIDEOINFOHEADER *>(m_MtIn.Format())->bmiHeader.biWidth;
		return S_OK;
	}
	else
		return E_POINTER;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの互さを函誼します
//! @param		pVideoHeight : ビデオの互さ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::get_VideoHeight( long *pVideoHeight )
{
	if( pVideoHeight ) {
		*pVideoHeight = reinterpret_cast<VIDEOINFOHEADER *>(m_MtIn.Format())->bmiHeader.biHeight;
		return S_OK;
	}
	else
		return E_POINTER;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ミングが�_兵された�rにコ�`ルされる
//!
//! �_兵フレ�`ムを���hする。
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::OnStartStreaming(void)
{
	HRESULT		hr;
	CComPtr<IMediaSeeking>	mediaSeeking;
	
	if( m_pGraph )
	{
		if( m_pGraph->QueryInterface( &mediaSeeking ) != S_OK )
			mediaSeeking = NULL;
	}

	bool		bGetTime = false;
	LONGLONG	Current = 0;
	if( mediaSeeking.p != NULL )
	{	// IMediaSeekingを聞って�r�gの函誼を��みる
		GUID	Format;
		if( SUCCEEDED(hr = mediaSeeking->GetTimeFormat( &Format ) ) )
		{
			if( SUCCEEDED(hr = mediaSeeking->GetCurrentPosition( &Current )) )
			{
				if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
				{
					double	renderTime = Current / 10000000.0;
					REFTIME	AvgTimePerFrame;	// REFTIME :  昼方を幣す弌方を燕す蔚娼業検�嗤（�泣方。
					if( SUCCEEDED( hr = get_AvgTimePerFrame( &AvgTimePerFrame ) ) )
					{
						m_StartFrame = (LONG)(renderTime / AvgTimePerFrame + 0.5);
						bGetTime = true;
					}
				}
				else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
				{
					m_StartFrame = (LONG)Current;
					bGetTime = true;
				}
			}
		}
	}
	if( bGetTime == false )
		m_StartFrame = 0;
	
	mediaSeeking.Release();

	return CBaseVideoRenderer::OnStartStreaming();
}
//----------------------------------------------------------------------------
//! @brief	  	レンダリング念にコ�`ルされる
//!
//! メディアサンプルにメディアタイムを���hする。
//! メディアタイムは�_兵フレ�`ムに�F壓のストリ�`ム�r�gを紗麻したものになる。
//! もし、フィルタのIMediaSeekingインタ�`フェイスが旋喘できない��栽は、
//! このレンダ�`フィルタが宙鮫したフレ�`ム方とドロップしたフレ�`ム方を紗麻する。
//! この��栽、より貧了のフィルタでドロップしたフレ�`ム方はわからないので、
//! 飛孤娼業が鯛ちる。
//! @param		pMediaSample : メディアサンプル
//----------------------------------------------------------------------------
void TBufferRenderer::OnRenderStart( IMediaSample *pMediaSample )
{
	CBaseVideoRenderer::OnRenderStart(pMediaSample);

	HRESULT		hr;
	bool		bGetTime = false;
	LONGLONG	Current = 0, Stop = 0;
	IMediaSeeking	*mediaSeeking = NULL;
	if( GetMediaPositionInterface( IID_IMediaSeeking, (void**)&mediaSeeking) == S_OK )
	{
		GUID	Format;
		if( SUCCEEDED(hr = mediaSeeking->GetTimeFormat( &Format ) ) )
		{
			if( SUCCEEDED(hr = mediaSeeking->GetCurrentPosition( &Current )) &&
				SUCCEEDED(hr = mediaSeeking->GetStopPosition( &Stop )) )
			{
				if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
				{
					double	renderTime = Current / 10000000.0;
					double	stopTime = Stop / 10000000.0;
					REFTIME	AvgTimePerFrame;	// REFTIME :  昼方を幣す弌方を燕す蔚娼業検�嗤（�泣方。
					if( SUCCEEDED( hr = get_AvgTimePerFrame( &AvgTimePerFrame ) ) )
					{
						Current = (LONGLONG)(renderTime / AvgTimePerFrame + 0.5);
						Stop = (LONGLONG)(stopTime / AvgTimePerFrame + 0.5);
						bGetTime = true;
					}
				}
				else if( IsEqualGUID( TIME_FORMAT_FRAME, Format ) )
				{
					bGetTime = true;
				}
			}
		}
		mediaSeeking->Release();
		mediaSeeking = NULL;
	}
	LONGLONG	TimeStart = m_StartFrame + m_cFramesDrawn + m_cFramesDropped;;
	LONGLONG	TimeEnd = m_StartFrame + m_cFramesDrawn + m_cFramesDropped;;
	if( bGetTime == true )
	{
		TimeStart = m_StartFrame + Current;
		TimeEnd = m_StartFrame + Current;
		m_StopFrame = m_StartFrame + static_cast<LONG>(Stop);
	}
	else
	{
		TimeStart = m_StartFrame + m_cFramesDrawn + m_cFramesDropped;;
		TimeEnd = m_StartFrame + m_cFramesDrawn + m_cFramesDropped;;
		m_StopFrame = 0;
	}
	pMediaSample->SetMediaTime( &TimeStart, &TimeEnd );
}
//----------------------------------------------------------------------------
//##	TBufferRendererInputPin
//----------------------------------------------------------------------------
//! @brief	  	秘薦ピンオブジェクトを���Bします。
//! @param		pRenderer : レンダ�`オブジェクトを峺協します。
//! @param		pInterfaceLock : CCritSec ロックへのポインタで、彜�B卞佩を�@�Aするために聞喘する。@n
//!					これはフィルタ ロック CBaseFilter.m_pLock と揖��のクリティカル セクションとなりうる。 
//! @param		phr : メソッドの撹孔払�，鯤召� HRESULT �､鯣ゝ辰垢��篳�のポインタ。
//! @param		name : オブジェクトのデバッグ喘の兆念が秘る猟忖双。
//----------------------------------------------------------------------------
TBufferRendererInputPin::TBufferRendererInputPin( TBufferRenderer *pRenderer, CCritSec *pInterfaceLock, HRESULT *phr, LPCWSTR name)
 : CRendererInputPin( pRenderer, phr, name ), m_pRenderer( pRenderer ), m_pInterfaceLock( pInterfaceLock ),
 m_ActiveAllocator( false )
{
}
//----------------------------------------------------------------------------
//! @brief	  	デストラクタ。�F壓は採もしない。
//----------------------------------------------------------------------------
TBufferRendererInputPin::~TBufferRendererInputPin()
{
}
//----------------------------------------------------------------------------
//! @brief	  	徭念のアロケ�`タが嗤�燭�どうか�{べます。
//! @return		嗤�燭覆�TRUEを卦します。
//----------------------------------------------------------------------------
bool TBufferRendererInputPin::ActiveAllocator( void ) const
{
	return m_ActiveAllocator;
}
//----------------------------------------------------------------------------
//! @brief	  	徭念のアロケ�`タオブジェクトを護り輝てます。
//! @param		ppAllocator : 卦すアロケ�`タ�`
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRendererInputPin::GetAllocator( IMemAllocator **ppAllocator )
{
	CAutoLock cInterfaceLock(m_pInterfaceLock);
	CheckPointer(ppAllocator,E_POINTER);

	// アロケ�`タがまだ�O協されていないとき
	if (m_pAllocator == NULL) {
		m_pAllocator = &(m_pRenderer->m_Allocator);
		m_pAllocator->AddRef();
	}
	// 歌孚カウントを火すのはインタフェ�`スの碧��です。
	m_pAllocator->AddRef();
	*ppAllocator = m_pAllocator;

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	アロケ�`タが�Qまったときに柵び竃されます。
//! @param		pAllocator 書指の俊�Aで聞喘するアロケ�`タを峺協します。
//! @param		bReadOnly このアロケ�`タからのサンプルが�iみとり��喘ならTRUEを峺協します。
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRendererInputPin::NotifyAllocator( IMemAllocator * pAllocator, BOOL bReadOnly )
{
	CAutoLock cInterfaceLock(m_pInterfaceLock);

	// 児久クラス柵び竃し
	HRESULT hr = CBaseInputPin::NotifyAllocator(pAllocator, bReadOnly);
	if( FAILED(hr) )
		return hr;

	//徭念のアロケ�`タが嗤�燭�どうかを���hします
	m_ActiveAllocator = (pAllocator == (&(m_pRenderer->m_Allocator)));

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したメディアサンプルにポインタを�O協します
//! @param		media : メディアサンプル
//! @param		ptr : �O協するポインタ
//----------------------------------------------------------------------------
void TBufferRendererInputPin::SetPointer( IMediaSample *media, BYTE *ptr )
{
	m_pRenderer->m_Allocator.SetPointer( media, ptr );
}
//----------------------------------------------------------------------------
//! @brief	  	アロケ�`タ�`が隔つメディアサンプルにポインタを�O協します
//! @param		ptr : �O協するポインタ
//----------------------------------------------------------------------------
void TBufferRendererInputPin::SetPointer( BYTE *ptr )
{
	m_pRenderer->m_Allocator.SetPointer( ptr );
}
//----------------------------------------------------------------------------
//## TBufferRendererAllocator
//----------------------------------------------------------------------------
//! @brief	  	コンストラクタ
//! @param		pRenderer : レンダ�`オブジェクトを峺協します。
//! @param		pName : オブジェクトのデバッグ喘の兆念が秘る猟忖双。
//! @param		pUnk : 鹿撹された侭嗤宀オブジェクトへのポインタ。
//! @param		phr : メソッドの撹孔払�，鯤召� HRESULT �､鯣ゝ辰垢��篳�のポインタ。
//----------------------------------------------------------------------------
TBufferRendererAllocator::TBufferRendererAllocator( TBufferRenderer *pRenderer, TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr)
 : CBaseAllocator(pName, pUnk, phr), m_pMediaSample(NULL), m_pRenderer(pRenderer)
{}
//----------------------------------------------------------------------------
//! @brief	  	CBaseAllocator::Decommitをコ�`ルしなければならないので、コ�`ルする。
//----------------------------------------------------------------------------
TBufferRendererAllocator::~TBufferRendererAllocator()
{
	Decommit();
	ASSERT(m_lAllocated == m_lFree.GetCount());
	// Free up all the CMediaSamples
	CMediaSample *pSample;
	for(;;)
	{
		pSample = m_lFree.RemoveHead();
		if( pSample != NULL )
			delete pSample;
		else
			break;
	}
	m_lAllocated = 0;
}
//----------------------------------------------------------------------------
//! @brief	  	採もしない。アロケ�`タ�`がメモリを盾慧することはない。
//----------------------------------------------------------------------------
void TBufferRendererAllocator::Free( void )
{
}
//----------------------------------------------------------------------------
//! @brief	  	メモリを護り輝てて、それをリストに弖紗する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT TBufferRendererAllocator::Alloc( void )
{
	CAutoLock	lck(this);

	/* Check he has called SetProperties */
	HRESULT hr = CBaseAllocator::Alloc();
	if( FAILED(hr) )
		return hr;

	/* If the requirements haven't changed then don't reallocate */
	if( hr == S_FALSE )
		return NOERROR;

	ASSERT(hr == S_OK); // we use this fact in the loop below
	LPBYTE	pBuffer = static_cast<LPBYTE>(m_pRenderer->GetBackBuffer());

	CMediaSample *pSample;
	pSample = new CMediaSample( NAME("buffer media sample"), this, &hr, pBuffer, m_lSize );
	ASSERT(SUCCEEDED(hr));
	if( pSample == NULL ) {
		return E_OUTOFMEMORY;
	}
	m_lFree.Add(pSample);
	m_lAllocated++;
	m_pMediaSample = pSample;
	return NOERROR;
}

//----------------------------------------------------------------------------
//! @brief	  	勣箔するメモリの����を�O協する。
//! @param		pRequest : バッファ勣箔を根む ALLOCATOR_PROPERTIES ��夛悶のポインタ
//! @param		pActual : �g�Hのバッファ プロパティを鞭け函る ALLOCATOR_PROPERTIES ��夛悶のポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRendererAllocator::SetProperties( ALLOCATOR_PROPERTIES* pRequest, ALLOCATOR_PROPERTIES* pActual )
{
	CheckPointer( pActual, E_POINTER );
	ValidateReadWritePtr( pActual, sizeof(ALLOCATOR_PROPERTIES) );
	CAutoLock	cObjectLock(this);

    ZeroMemory( pActual, sizeof(ALLOCATOR_PROPERTIES) );

	if( m_bCommitted == TRUE)
		return VFW_E_ALREADY_COMMITTED;

	if( m_lFree.GetCount() < m_lAllocated )	// m_lAllocatedが1��參貧の�rはエラ�`にした圭がよい��
		return VFW_E_BUFFERS_OUTSTANDING;

	if( pRequest->cBuffers == 1 && pRequest->cbBuffer == m_pRenderer->GetBufferSize() &&
		pRequest->cbAlign == 1 && pRequest->cbPrefix == 0 )
	{
		*pActual = *pRequest;
		m_lSize = pRequest->cbBuffer;
		m_lCount = pRequest->cBuffers;
		m_lAlignment = pRequest->cbAlign;
		m_lPrefix = pRequest->cbPrefix;
		m_bChanged = TRUE;
		return S_OK;
	}

	return VFW_E_BADALIGN;
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したメディアサンプルにポインタを�O協します
//! @param		media : メディアサンプル
//! @param		ptr : �O協するポインタ
//----------------------------------------------------------------------------
void TBufferRendererAllocator::SetPointer( IMediaSample *media, BYTE *ptr )
{
	BYTE	*pBufferParam, *pBufferOwn;
	if( media )
	{
		media->GetPointer( &pBufferParam);
		if( m_pMediaSample != NULL )
		{
			m_pMediaSample->GetPointer( &pBufferOwn );
			if( pBufferOwn == pBufferParam )	// 揖じバッファを峺しているので、隠隔しているサンプルと揖じと��なす
			{
				LONG	cBytes = m_pMediaSample->GetSize();	// サイズは�笋錣辰討い覆い頒�なす、並念にチェックしておくこと
				m_pMediaSample->SetPointer( ptr, cBytes );
			}
		}
	}
}

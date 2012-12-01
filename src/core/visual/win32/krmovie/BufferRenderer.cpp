/****************************************************************************/
/*! @file
@brief DirectShow

�Хåե��إ�����󥰤���
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
//! @param		pUnk : ���ɤ��� IUnknown ���󥿩`�ե������ؤΥݥ��󥿡�
//! @param		phr : �᥽�åɤγɹ�ʧ����ʾ�� HRESULT �����ܤ�ȡ������ؤΥݥ��󥿡�
//----------------------------------------------------------------------------
CUnknown * WINAPI TBufferRenderer::CreateInstance( LPUNKNOWN pUnk, HRESULT *phr )
{
	CUnknown	*punk = new TBufferRenderer( NAME("Buffer Renderer"), pUnk, phr );
	if( punk == NULL )
		*phr = E_OUTOFMEMORY;
	return punk;
}
#pragma warning(disable: 4355)	// ���󥹥ȥ饯���Υ٩`�����г��ڻ��r��this��ʹ���ȥ�`�˥󥰤�����ΤǤ������ֹ
//----------------------------------------------------------------------------
//! @brief	  	TBufferRenderer constructor
//! @param		pName : �ǥХå��Τ����ʹ�ä����ӛ���ؤΥݥ��󥿡�
//! @param		pUnk : ���ɤ��줿�����ߥ��֥������ȤؤΥݥ��󥿡�
//! @param		phr : HRESULT ���ؤΥݥ��󥿡�
//----------------------------------------------------------------------------
TBufferRenderer::TBufferRenderer( TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr )
 : CBaseVideoRenderer( CLSID_BufferRenderer, pName, pUnk, phr )
, m_InputPin( this, &m_InterfaceLock, phr, L"Input" )
, m_Allocator( this, NAME("Allocator"), GetOwner(), phr )
{
	//CBaseRender::m_pInputPin�˥ݥ��󥿤��O�����롣
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
	//CBaseRender::m_pInputPin�˥ݥ��󥿤�ꥻ�åȤ��롣
	//����򤷤ʤ���CBaseRender�Υǥ��ȥ饯����delete����Ƥ��ޤ��Τ�ע�⣡
    m_pInputPin = NULL;

	// �Է֤Ǵ_�����Ƥ�����ϥХåե��ν��
	FreeFrontBuffer();
	FreeBackBuffer();
}
//----------------------------------------------------------------------------
//! @brief	  	Ҫ�󤵤줿���󥿩`�ե������򷵤�
//! 
//! Overriden to say what interfaces we support and where
//! @param		riid : ���󥿩`�ե�������IID
//! @param		ppv : ���󥿩`�ե������򷵤��ݥ��󥿩`�ؤΥݥ���
//! @return		����`���`��
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
//! @brief	  	�ض��Υ�ǥ��� �����פ�ե��륿���ܤ����뤫�ɤ�����_�J����
//! 
//! This method forces the graph to give us an R8G8B8 video type, making our copy 
//! to texture memory trivial.
//! @param		pmt : �᰸���줿��ǥ��� �����פ򺬤� CMediaType ���֥������ȤؤΥݥ���
//! @return		�᰸���줿��ǥ��� �����פ��ܤ�������ʤ� S_OK �򷵤���
//!				�����Ǥʤ���� S_FALSE ������` ���`�ɤ򷵤���
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
//! @param		pmt : ��ǥ��� �����פ�ָ������ CMediaType ���֥������ȤؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::SetMediaType( const CMediaType *pmt )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������

	// Retrive the size of this media type
	VIDEOINFO *pviBmp;						// Bitmap info header
	pviBmp = (VIDEOINFO *)pmt->Format();
	m_VideoWidth  = pviBmp->bmiHeader.biWidth;
	m_VideoHeight = abs(pviBmp->bmiHeader.biHeight);
	m_VideoPitch = m_VideoWidth * 4;	// RGB32�˛Q����

	if( !IsAllocatedFrontBuffer() )
		AllocFrontBuffer( GetBufferSize() );

	if( !IsAllocatedBackBuffer() )
		AllocBackBuffer( GetBufferSize() );

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	A sample has been delivered. Copy it to the texture.
//! @param		pSample : ����ץ�� IMediaSample ���󥿩`�ե������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::DoRenderSample( IMediaSample * pSample )
{
	DWORD	*pBmpBuffer, *pTxtBuffer;	// Bitmap buffer, texture buffer
	BYTE	*pTxtOrgPos;

//	if( m_bEOS ) return S_OK;

	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������

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
		return S_OK;	// �������ʤ��ե�`��

	if( pTxtBuffer == pBmpBuffer )	// ��ǰ�Υ����`���`��ʹ���Ƥ���
	{
		SwapBuffer( pSample );	// Front��Back�Хåե�������椨��
		if( m_pSink )
			m_pSink->Notify( EC_UPDATE, EventParam1, NULL );
		return S_OK;
	}

	// ��ǰ�Υ����`���`�ǤϤʤ��Τǥ���򥳥ԩ`����
#if 0
	// �¤����Ϥ˥��ԩ`(���·�ܞ��)
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
	// �Ϥ����¤˥��ԩ`
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
	SwapBuffer( pSample );	// Front��Back�Хåե�������椨��
	return S_OK;
}
//---------------------------------------------------------------------------
//! @brief	  	�ե��ȥХåե��ȥХå��Хåե�������椨��
//! @param		pSample : ����ץ롣�����ФΥݥ��󥿤�������
//----------------------------------------------------------------------------
void TBufferRenderer::SwapBuffer( IMediaSample *pSample )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief	  	�ե��ȥХåե��˥�����굱�Ƥ�
//! @param		size : ��굱�Ƥ륵����
//----------------------------------------------------------------------------
void TBufferRenderer::AllocFrontBuffer( size_t size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief	  	�Хå��Хåե��˥�����굱�Ƥ롣
//! @param		size : ��굱�Ƥ륵����
//----------------------------------------------------------------------------
void TBufferRenderer::AllocBackBuffer( size_t size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief	  	�ե��ȥХåե��˸�굱�Ƥ��Ƥ��������_�Ť���
//!
//! �⤷����굱�Ƥ��Ƥ�����꤬�����Υ��饹�ˤ�äƸ�굱�Ƥ�줿��ΤǤʤ����Ϥϡ�
//! ��Ť��ʤ���
//----------------------------------------------------------------------------
void TBufferRenderer::FreeFrontBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief	  	�Хå��Хåե��˸�굱�Ƥ��Ƥ��������_�Ť���
//!
//! �⤷����굱�Ƥ��Ƥ�����꤬�����Υ��饹�ˤ�äƸ�굱�Ƥ�줿��ΤǤʤ����Ϥϡ�
//! ��Ť��ʤ���
//----------------------------------------------------------------------------
void TBufferRenderer::FreeBackBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief	  	�ե��ȥХåե��˥Хåե��ؤΥݥ��󥿤��O������
//! @param		buff : �Хåե��ؤΥݥ���
//----------------------------------------------------------------------------
void TBufferRenderer::SetFrontBuffer( BYTE *buff )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
	FreeFrontBuffer();
	if( m_FrontBuffer == 1 )
		m_Buffer[1] = buff;
	else
		m_Buffer[0] = buff;
}
//---------------------------------------------------------------------------
//! @brief	  	�Хå��Хåե��˥Хåե��ؤΥݥ��󥿤��O������
//! @param		buff : �Хåե��ؤΥݥ���
//----------------------------------------------------------------------------
void TBufferRenderer::SetBackBuffer( BYTE *buff )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
	FreeBackBuffer();
	if( m_FrontBuffer == 1 )
		m_Buffer[0] = buff;
	else
		m_Buffer[1] = buff;
	
	SetPointer( buff );
}
//---------------------------------------------------------------------------
//! @brief	  	�ե��ȥХåե��ؤΥݥ��󥿤�ȡ�ä���
//! @return		�Хåե��ؤΥݥ���
//----------------------------------------------------------------------------
BYTE *TBufferRenderer::GetFrontBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
	if( m_FrontBuffer == 1 )
		return m_Buffer[1];
	else
		return m_Buffer[0];
}
//---------------------------------------------------------------------------
//! @brief	  	�Хå��Хåե��ؤΥݥ��󥿤�ȡ�ä���
//! @return		�Хåե��ؤΥݥ���
//----------------------------------------------------------------------------
BYTE *TBufferRenderer::GetBackBuffer()
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
	if( m_FrontBuffer == 1 )
		return m_Buffer[0];
	else
		return m_Buffer[1];
}
//----------------------------------------------------------------------------
//! @brief	  	�ե��ȥХåե����O�����ޤ���
//! @param		buff : �ե��ȥХåե��åХåե��ؤΥݥ���
//! @param		size : �Хåե��Υ�������ɤ������ؤΥݥ��󥿡�@n
//!					buff��NULL�Εr��������������������������
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::SetFrontBuffer( BYTE *buff, long *size )
{
	if( m_State == State_Running )
		return S_FALSE;

	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief	  	�Хå��Хåե����O�����ޤ���
//! @param		buff : �Хå��Хåե��åХåե��ؤΥݥ���
//! @param		size : �Хåե��Υ�������ɤ������ؤΥݥ��󥿡�@n
//!					buff��NULL�Εr��������������������������
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::SetBackBuffer( BYTE *buff, long *size )
{
	if( m_State == State_Running )
		return S_FALSE;

	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
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
//! @brief		�ե��ȥХåե��ؤΥݥ��󥿤�ȡ�ä��ޤ���
//! @param		buff : �ե��ȥХåե��ؤΥݥ��󥿤򷵤�����ΥХåե��ؤΥݥ���
//! @param		size : �Хåե��Υ������򷵤������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::GetFrontBuffer( BYTE **buff, long *size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
	*buff = GetFrontBuffer();
	*size = GetBufferSize();
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	�Хå��Хåե��ؤΥݥ��󥿤�ȡ�ä��ޤ���
//! @param		buff : �Хå��Хåե��ؤΥݥ��󥿤򷵤�����ΥХåե��ؤΥݥ���
//! @param		size : �Хåե��Υ������򷵤������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT TBufferRenderer::GetBackBuffer( BYTE **buff, long *size )
{
	CAutoLock cAutoLock(&m_BufferLock);	// ����ƥ����륻�������
	*buff = GetBackBuffer();
	*size = GetBufferSize();
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	1�ե�`���ƽ����ʾ�r�g��ȡ�ä��ޤ�
//! @param		pAvgTimePerFrame : 1�ե�`���ƽ����ʾ�r�g
//! @return		����`���`��
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
//! @brief	  	�ӥǥ��η���ȡ�ä��ޤ�
//! @param		pVideoWidth : �ӥǥ��η�
//! @return		����`���`��
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
//! @brief	  	�ӥǥ��θߤ���ȡ�ä��ޤ�
//! @param		pVideoHeight : �ӥǥ��θߤ�
//! @return		����`���`��
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
//! @brief	  	���ȥ�`�ߥ󥰤��_ʼ���줿�r�˥��`�뤵���
//!
//! �_ʼ�ե�`���ӛ�h���롣
//! @return		����`���`��
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
	{	// IMediaSeeking��ʹ�äƕr�g��ȡ�ä�ԇ�ߤ�
		GUID	Format;
		if( SUCCEEDED(hr = mediaSeeking->GetTimeFormat( &Format ) ) )
		{
			if( SUCCEEDED(hr = mediaSeeking->GetCurrentPosition( &Current )) )
			{
				if( IsEqualGUID( TIME_FORMAT_MEDIA_TIME, Format ) )
				{
					double	renderTime = Current / 10000000.0;
					REFTIME	AvgTimePerFrame;	// REFTIME :  ������ʾ��С����������ȸ���С��������
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
//! @brief	  	�������ǰ�˥��`�뤵���
//!
//! ��ǥ�������ץ�˥�ǥ����������ӛ�h���롣
//! ��ǥ�����������_ʼ�ե�`��ˬF�ڤΥ��ȥ�`���r�g����㤷����Τˤʤ롣
//! �⤷���ե��륿��IMediaSeeking���󥿩`�ե����������äǤ��ʤ����Ϥϡ�
//! ���Υ����`�ե��륿���軭�����ե�`�����ȥɥ�åפ����ե�`��������㤹�롣
//! ���Έ��ϡ������λ�Υե��륿�ǥɥ�åפ����ե�`�����Ϥ狼��ʤ��Τǡ�
//! ���ɾ��Ȥ�����롣
//! @param		pMediaSample : ��ǥ�������ץ�
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
					REFTIME	AvgTimePerFrame;	// REFTIME :  ������ʾ��С����������ȸ���С��������
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
//! @brief	  	�����ԥ󥪥֥������Ȥ򘋺B���ޤ���
//! @param		pRenderer : �����`���֥������Ȥ�ָ�����ޤ���
//! @param		pInterfaceLock : CCritSec ��å��ؤΥݥ��󥿤ǡ�״�B���Ф�@�A���뤿���ʹ�ä��롣@n
//!					����ϥե��륿 ��å� CBaseFilter.m_pLock ��ͬ���Υ���ƥ����� ���������Ȥʤꤦ�롣 
//! @param		phr : �᥽�åɤγɹ�ʧ����ʾ�� HRESULT ����ȡ�ä�������Υݥ��󥿡�
//! @param		name : ���֥������ȤΥǥХå��ä���ǰ����������С�
//----------------------------------------------------------------------------
TBufferRendererInputPin::TBufferRendererInputPin( TBufferRenderer *pRenderer, CCritSec *pInterfaceLock, HRESULT *phr, LPCWSTR name)
 : CRendererInputPin( pRenderer, phr, name ), m_pRenderer( pRenderer ), m_pInterfaceLock( pInterfaceLock ),
 m_ActiveAllocator( false )
{
}
//----------------------------------------------------------------------------
//! @brief	  	�ǥ��ȥ饯�����F�ڤϺΤ⤷�ʤ���
//----------------------------------------------------------------------------
TBufferRendererInputPin::~TBufferRendererInputPin()
{
}
//----------------------------------------------------------------------------
//! @brief	  	��ǰ�Υ����`�����Є����ɤ����{�٤ޤ���
//! @return		�Є��ʤ�TRUE�򷵤��ޤ���
//----------------------------------------------------------------------------
bool TBufferRendererInputPin::ActiveAllocator( void ) const
{
	return m_ActiveAllocator;
}
//----------------------------------------------------------------------------
//! @brief	  	��ǰ�Υ����`�����֥������Ȥ��굱�Ƥޤ���
//! @param		ppAllocator : ���������`���`
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRendererInputPin::GetAllocator( IMemAllocator **ppAllocator )
{
	CAutoLock cInterfaceLock(m_pInterfaceLock);
	CheckPointer(ppAllocator,E_POINTER);

	// �����`�����ޤ��O������Ƥ��ʤ��Ȥ�
	if (m_pAllocator == NULL) {
		m_pAllocator = &(m_pRenderer->m_Allocator);
		m_pAllocator->AddRef();
	}
	// ���ե�����Ȥ�Ф��Τϥ��󥿥ե��`�����˘��Ǥ���
	m_pAllocator->AddRef();
	*ppAllocator = m_pAllocator;

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	�����`�����Q�ޤä��Ȥ��˺��ӳ�����ޤ���
//! @param		pAllocator ��ؤνӾA��ʹ�ä��륢���`����ָ�����ޤ���
//! @param		bReadOnly ���Υ����`������Υ���ץ뤬�i�ߤȤꌟ�äʤ�TRUE��ָ�����ޤ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRendererInputPin::NotifyAllocator( IMemAllocator * pAllocator, BOOL bReadOnly )
{
	CAutoLock cInterfaceLock(m_pInterfaceLock);

	// ���ץ��饹���ӳ���
	HRESULT hr = CBaseInputPin::NotifyAllocator(pAllocator, bReadOnly);
	if( FAILED(hr) )
		return hr;

	//��ǰ�Υ����`�����Є����ɤ�����ӛ�h���ޤ�
	m_ActiveAllocator = (pAllocator == (&(m_pRenderer->m_Allocator)));

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ָ��������ǥ�������ץ�˥ݥ��󥿤��O�����ޤ�
//! @param		media : ��ǥ�������ץ�
//! @param		ptr : �O������ݥ���
//----------------------------------------------------------------------------
void TBufferRendererInputPin::SetPointer( IMediaSample *media, BYTE *ptr )
{
	m_pRenderer->m_Allocator.SetPointer( media, ptr );
}
//----------------------------------------------------------------------------
//! @brief	  	�����`���`���֤ĥ�ǥ�������ץ�˥ݥ��󥿤��O�����ޤ�
//! @param		ptr : �O������ݥ���
//----------------------------------------------------------------------------
void TBufferRendererInputPin::SetPointer( BYTE *ptr )
{
	m_pRenderer->m_Allocator.SetPointer( ptr );
}
//----------------------------------------------------------------------------
//## TBufferRendererAllocator
//----------------------------------------------------------------------------
//! @brief	  	���󥹥ȥ饯��
//! @param		pRenderer : �����`���֥������Ȥ�ָ�����ޤ���
//! @param		pName : ���֥������ȤΥǥХå��ä���ǰ����������С�
//! @param		pUnk : ���ɤ��줿�����ߥ��֥������ȤؤΥݥ��󥿡�
//! @param		phr : �᥽�åɤγɹ�ʧ����ʾ�� HRESULT ����ȡ�ä�������Υݥ��󥿡�
//----------------------------------------------------------------------------
TBufferRendererAllocator::TBufferRendererAllocator( TBufferRenderer *pRenderer, TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr)
 : CBaseAllocator(pName, pUnk, phr), m_pMediaSample(NULL), m_pRenderer(pRenderer)
{}
//----------------------------------------------------------------------------
//! @brief	  	CBaseAllocator::Decommit�򥳩`�뤷�ʤ���Фʤ�ʤ��Τǡ����`�뤹�롣
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
//! @brief	  	�Τ⤷�ʤ��������`���`��������Ť��뤳�ȤϤʤ���
//----------------------------------------------------------------------------
void TBufferRendererAllocator::Free( void )
{
}
//----------------------------------------------------------------------------
//! @brief	  	������굱�Ƥơ������ꥹ�Ȥ�׷�Ӥ���
//! @return		����`���`��
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
//! @brief	  	Ҫ�󤹤�����Ԕ�����O�����롣
//! @param		pRequest : �Хåե�Ҫ��򺬤� ALLOCATOR_PROPERTIES ������Υݥ���
//! @param		pActual : �g�H�ΥХåե� �ץ�ѥƥ����ܤ�ȡ�� ALLOCATOR_PROPERTIES ������Υݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP TBufferRendererAllocator::SetProperties( ALLOCATOR_PROPERTIES* pRequest, ALLOCATOR_PROPERTIES* pActual )
{
	CheckPointer( pActual, E_POINTER );
	ValidateReadWritePtr( pActual, sizeof(ALLOCATOR_PROPERTIES) );
	CAutoLock	cObjectLock(this);

    ZeroMemory( pActual, sizeof(ALLOCATOR_PROPERTIES) );

	if( m_bCommitted == TRUE)
		return VFW_E_ALREADY_COMMITTED;

	if( m_lFree.GetCount() < m_lAllocated )	// m_lAllocated��1�����ϤΕr�ϥ���`�ˤ��������褤��
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
//! @brief	  	ָ��������ǥ�������ץ�˥ݥ��󥿤��O�����ޤ�
//! @param		media : ��ǥ�������ץ�
//! @param		ptr : �O������ݥ���
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
			if( pBufferOwn == pBufferParam )	// ͬ���Хåե���ָ���Ƥ���Τǡ����֤��Ƥ��륵��ץ��ͬ����Ҋ�ʤ�
			{
				LONG	cBytes = m_pMediaSample->GetSize();	// �������ω��äƤ��ʤ���Ҋ�ʤ�����ǰ�˥����å����Ƥ�������
				m_pMediaSample->SetPointer( ptr, cBytes );
			}
		}
	}
}

/****************************************************************************/
/*! @file
@brief Demux output pin

デマルチプレクサの竃薦ピンを�g廾する
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/22
@note
*****************************************************************************/

#include "CDemuxOutputPin.h"
#include "CDemuxSource.h"
//----------------------------------------------------------------------------
//! @brief	  	CDemuxOutputPin constructor
//! @param		szName : デバッグのために聞喘される��峰へのポインタ。
//! @param		pFilter : このピンを恬撹したフィルタ
//! @param		pHr : HRESULT �､悗離櫂ぅ鵐拭�
//! @param		pszName : このピンの兆念
//! @param		pSeek : ソ�`スフィルタのIMediaSeeking
//! @param		outstream : このピンの竃薦ストリ�`ム
//! @param		lock : ロックオブジェクト
//----------------------------------------------------------------------------
CDemuxOutputPin::CDemuxOutputPin( TCHAR *szName, CSource *pFilter, HRESULT *pHr, LPCWSTR pszName, IMediaSeeking *pSeek, IOutputStream *outstream, CCritSec *lock )
 : CSourceStream( szName, pHr, pFilter, pszName ), m_SeekProxy(pSeek), m_Stream(outstream), m_Lock(lock)
{}
//----------------------------------------------------------------------------
//! @brief	  	CDemuxOutputPin destructor
//----------------------------------------------------------------------------
CDemuxOutputPin::~CDemuxOutputPin()
{}
//----------------------------------------------------------------------------
//! @brief	  	勣箔されたインタ�`フェイスを卦す
//! @param		riid : インタ�`フェイスのIID
//! @param		ppv : インタ�`フェイスを卦すポインタ�`へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CDemuxOutputPin::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	if( riid == IID_IMediaSeeking ) {
		return GetInterface( &m_SeekProxy, ppv );
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	竃薦辛嬬なメディアタイプを卦す
//! @param		iPosition : メディアタイプの會方
//! @param		pmt : メディアタイプを鯉�{する�篳�
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::GetMediaType( int iPosition, CMediaType *pmt )
{
	CAutoLock cAutoLock(m_Lock);
	return m_Stream->GetMediaType( iPosition, pmt );
}
//----------------------------------------------------------------------------
//! @brief	  	峺協されたメディアタイプが旋喘できるかどうか�_�Jする
//! @param		pmt : 竃薦竃栖るかどうか�_�Jをするメディアタイプ
//! @return		エラ�`コ�`ド
//! @note		�F壓、DirectX VAを械にはじくようにしている
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::CheckMediaType( const CMediaType *pmt )
{
	CAutoLock cAutoLock(m_Lock);
	if( m_Stream->IsDXVASubtype( pmt ) )
		return VFW_E_TYPE_NOT_ACCEPTED;

	CMediaType	mt;
	int i = 0;
	while( GetMediaType( i, &mt ) == S_OK )
	{
		if( mt.majortype == pmt->majortype && mt.subtype == pmt->subtype && mt.formattype == pmt->formattype )
			return S_OK;
		++i;
	}
	return VFW_E_TYPE_NOT_ACCEPTED;
}
//----------------------------------------------------------------------------
//! @brief	  	峺協されたメディアタイプが旋喘できるかどうか�_�Jする
//! @param		pmt : 竃薦竃栖るかどうか�_�Jをするメディアタイプ
//! @return		エラ�`コ�`ド
//! @note		�F壓、DirectX VAを械にはじくようにしている
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::SetMediaType( const CMediaType *pmt )
{
	CAutoLock cAutoLock(m_Lock);
	HRESULT hr = CSourceStream::SetMediaType(pmt);
	if( hr == S_OK ) {
		hr = m_Stream->SetMediaType(pmt);
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	バッファのサイズを�Q協する
//! @param		pIMemAllocator : アロケ�`タ�`へのポインタ
//! @param		pProp : 勣箔サイズ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::DecideBufferSize( IMemAllocator *pIMemAllocator, ALLOCATOR_PROPERTIES *pProp )
{
	CAutoLock cAutoLock(m_Lock);
	HRESULT	hr = S_OK;
	ALLOCATOR_PROPERTIES	reqAlloc;
	ALLOCATOR_PROPERTIES	actualAlloc;

	long	buffers = 0;
	long	bufsize = 0;
	m_Stream->GetNeedBufferSize( buffers, bufsize );

	//Allignment of data
	if( pProp->cbAlign <= 0 )
		reqAlloc.cbAlign = 1;
	else
		reqAlloc.cbAlign = pProp->cbAlign;

	//Size of each buffer
	if( pProp->cbBuffer < bufsize )
		reqAlloc.cbBuffer = bufsize;
	else
		reqAlloc.cbBuffer = pProp->cbBuffer;

	//How many prefeixed bytes
	if( pProp->cbPrefix < 0 )
		reqAlloc.cbPrefix = 0;
	else
		reqAlloc.cbPrefix = pProp->cbPrefix;

	//Number of buffers in the allcoator
	if( pProp->cBuffers < buffers )
		reqAlloc.cBuffers = buffers;
	else
		reqAlloc.cBuffers = pProp->cBuffers;

	//Set the properties in the allocator
	hr = pIMemAllocator->SetProperties(&reqAlloc, &actualAlloc);

	//Check the response
	if( hr != S_OK ) {
		switch( hr ) {
			case E_POINTER:
				DbgLog(( LOG_ERROR, 0, "DecideBufferSize : SetProperties - NULL POINTER" ));
				return hr;
			case VFW_E_ALREADY_COMMITTED:
				DbgLog(( LOG_ERROR, 0, "DecideBufferSize : SetProperties - Already COMMITED" ));
				return hr;
			case VFW_E_BADALIGN:
				DbgLog(( LOG_ERROR, 0, "DecideBufferSize : SetProperties - Bad ALIGN" ));
				return hr;
			case VFW_E_BUFFERS_OUTSTANDING:
				DbgLog(( LOG_ERROR, 0, "DecideBufferSize : SetProperties - BUFFS OUTSTANDING" ));
				return hr;
			default:
				DbgLog(( LOG_ERROR, 0, "DecideBufferSize : SetProperties - UNKNOWN ERROR" ));
				break;
		}
	}
	// 撹孔した��栽も、�g�Hの�Y惚を�_�Jする。
	if( pProp->cbBuffer > actualAlloc.cbBuffer )
	{
		return E_FAIL;
	}
	hr = m_Stream->SetDecidedBufferSize( actualAlloc.cBuffers, actualAlloc.cbBuffer, actualAlloc.cbAlign, actualAlloc.cbPrefix );
	if( FAILED(hr) )
		return hr;

//	hr = pIMemAllocator->Commit();
// 竃薦ピンがアクティブになるまでCommitは�Wらせる

#ifdef _DEBUG
	switch( hr ) {
		case E_FAIL:
			DbgLog(( LOG_ERROR, 0, "DecideBufferSize : Commit - FAILED " ));
			return hr;
		case E_POINTER:
			DbgLog(( LOG_ERROR, 0, "DecideBufferSize : Commit - NULL POINTER " ));
			return hr;
		case E_INVALIDARG:
			DbgLog(( LOG_ERROR, 0, "DecideBufferSize : Commit - INVALID ARG " ));
			return hr;
		case E_NOTIMPL:
			DbgLog(( LOG_ERROR, 0, "DecideBufferSize : Commit - NOT IMPL" ));
			return hr;
		case S_OK:
			DbgLog(( LOG_ERROR, 0, "DecideBufferSize : Commit - ** SUCCESS **" ));
			break;
		default:
			DbgLog(( LOG_ERROR, 0, "DecideBufferSize : Commit - UNKNOWN ERROR " ));
			return hr;
	}
#endif

	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	サンプルをダウンストリ�`ムへ僕り�Aける�I尖
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::DoBufferProcessingLoop(void)
{
	Command com;
	HRESULT	hr;
	OnThreadStartPlay();
	do {
		while( !CheckRequest(&com) ) {
			// Virtual function user will override.
			IMediaSample *pSample;
			hr = RetrieveBuffer(&pSample);
			if( hr == S_OK ) {
				hr = Deliver(pSample);
				pSample->Release();
				if(hr != S_OK)
				{
					DbgLog((LOG_TRACE, 2, TEXT("Deliver() returned %08x; stopping"), hr));
					return S_OK;
				}
			} else if (hr == S_FALSE) {
				// derived class wants us to stop pushing data
				DeliverEndOfStream();
				return S_OK;
			} else {
				// derived class encountered an error
				DbgLog((LOG_ERROR, 1, TEXT("Error %08lX from FillBuffer!!!"), hr));
				DeliverEndOfStream();
				m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
				return hr;
			}
		}
		if( com == CMD_RUN || com == CMD_PAUSE ) {
			Reply(NOERROR);
		} else if( com != CMD_STOP ) {
			Reply((DWORD) E_UNEXPECTED);
			DbgLog((LOG_ERROR, 1, TEXT("Unexpected command!!!")));
		}
	} while( com != CMD_STOP );

	return S_FALSE;
}
//----------------------------------------------------------------------------
//! @brief	  	サンプルをストリ�`ムから函誼する
//! @param		pSample : サンプルへのポインタのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::RetrieveBuffer( IMediaSample **pSample )
{
	CAutoLock cAutoLock(m_Lock);
	HRESULT hr = m_Stream->GetNextSample( pSample );
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	俊�A�I尖
//! @param		pReceivePin : 俊�Aを勣箔されているピン
//! @param		pmt : 俊�Aで聞喘することを勣箔されているメディアタイプ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CDemuxOutputPin::Connect( IPin *pReceivePin, const AM_MEDIA_TYPE *pmt )
{
	return CSourceStream::Connect( pReceivePin, pmt );
}
//----------------------------------------------------------------------------
//! @brief	  	俊�A頼阻�I尖
//! @param		pReceivePin : 俊�Aに聞われたピン
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::CompleteConnect( IPin *pReceivePin )
{
	HRESULT	hr = CSourceStream::CompleteConnect( pReceivePin );
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	アロケ�`タ�`を�Q協する�I尖
//! @param		pPin : 秘薦ピン
//! @param		pAlloc : アロケ�`タ�`
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxOutputPin::DecideAllocator( IMemInputPin *pPin, IMemAllocator **pAlloc )
{
	HRESULT	hr = CSourceStream::DecideAllocator( pPin, pAlloc );
	if( SUCCEEDED(hr) )
	{
		hr = m_Stream->SetAllocator( *pAlloc );
	}
	return hr;
}

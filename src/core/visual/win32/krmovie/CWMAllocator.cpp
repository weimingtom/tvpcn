/****************************************************************************/
/*! @file
@brief DirectShow allocator wrapper for Windows media format

DirectShowのアロケーターをラップしたWindows Media Format SDK用のアロケーター
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/28
@note
*****************************************************************************/

#include "CWMAllocator.h"
#include "CWMBuffer.h"
#include <assert.h>


//----------------------------------------------------------------------------
//! @brief	  	デストラクタ
//----------------------------------------------------------------------------
CWMAllocator::~CWMAllocator()
{
}

//----------------------------------------------------------------------------
//! @brief	  	要求されたインターフェイスを返す
//! @param		riid : インターフェイスのIID
//! @param		ppv : インターフェイスを返すポインターへのポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CWMAllocator::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	if( IID_IWMReaderAllocatorEx == riid ) {
		return GetInterface( static_cast<IWMReaderAllocatorEx*>(this), ppv );
	} else if( IID_IMemAllocator == riid ) {
		return GetInterface( static_cast<IMemAllocator*>(m_MemAlloc), ppv );
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	バッファを確保する ( ストリームから要求された場合にこのメソッドが呼ばれる )
//! @param		wStreamNum : ストリーム番号
//! @param		cbBuffer : 要求されたバッファサイズ
//! @param		ppBuffer : バッファを返すポインタのポインタ
//! @param		dwFlags : 用途フラグ
//! @param		cnsSampleTime : サンプルタイム
//! @param		cnsSampleDuration : サンプル表示時間
//! @param		pvContext : IWMReader::Startコール時に渡されたポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CWMAllocator::AllocateForStreamEx( WORD wStreamNum, DWORD cbBuffer, INSSBuffer **ppBuffer,
	DWORD dwFlags, QWORD cnsSampleTime, QWORD cnsSampleDuration, void *pvContext )
{
	HRESULT	hr;

	if( ppBuffer == NULL )
		return E_INVALIDARG;

	DWORD	flag = 0;
	if( dwFlags & WM_SFEX_NOTASYNCPOINT )	// 非 Key frame
		flag |= AM_GBF_NOTASYNCPOINT;

	REFERENCE_TIME	StartTime = cnsSampleTime;
	REFERENCE_TIME	EndTime = cnsSampleTime + cnsSampleDuration;

	IMediaSample	*pSample = NULL;
	hr = Allocator()->GetBuffer( &pSample, &StartTime, &EndTime, flag );
	if( hr == S_OK )
	{
		*ppBuffer = new CWMBuffer(pSample);
		(*ppBuffer)->AddRef();
		(*ppBuffer)->SetLength(cbBuffer);
		pSample->Release();	// CWMBufferへ渡した時点で参照カウントが増える
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	バッファを確保する ( アウトプットから要求された場合にこのメソッドが呼ばれる )
//! @param		wStreamNum : ストリーム番号
//! @param		cbBuffer : 要求されたバッファサイズ
//! @param		ppBuffer : バッファを返すポインタのポインタ
//! @param		dwFlags : 用途フラグ
//! @param		cnsSampleTime : サンプルタイム
//! @param		cnsSampleDuration : サンプル表示時間
//! @param		pvContext : IWMReader::Startコール時に渡されたポインタ
//! @return		エラーコード
//! @note		現在このメソッドは使っていない。たぶん正常に機能しない。
//----------------------------------------------------------------------------
STDMETHODIMP CWMAllocator::AllocateForOutputEx( DWORD dwOutputNum, DWORD cbBuffer, INSSBuffer **ppBuffer,
	DWORD dwFlags, QWORD cnsSampleTime, QWORD cnsSampleDuration, void *pvContext )
{
	HRESULT	hr;

	if( ppBuffer == NULL )
		return E_INVALIDARG;

	DWORD	flag = 0;
	if( dwFlags & WM_SFEX_NOTASYNCPOINT )	// 非 Key frame
		flag |= AM_GBF_NOTASYNCPOINT;

	REFERENCE_TIME	StartTime = cnsSampleTime;
	REFERENCE_TIME	EndTime = cnsSampleTime + cnsSampleDuration;

	IMediaSample	*pSample;
	hr = Allocator()->GetBuffer( &pSample, &StartTime, &EndTime, flag );
	if( hr == S_OK )
	{
		*ppBuffer = new CWMBuffer(pSample);
		(*ppBuffer)->AddRef();
		pSample->Release();	// CWMBufferへ渡した時点で参照カウントが増える
	}
	return hr;
}

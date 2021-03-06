/****************************************************************************/
/*! @file
@brief DirectShow media sample wrapper for Windows media format

DirectShowのMedia SampleをラップしたWindows Media Format SDK喘のBuffer
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/28
@note
*****************************************************************************/

#include "CWMBuffer.h"

//----------------------------------------------------------------------------
//! @brief	  	勣箔されたインタ�`フェイスを卦す
//! @param		riid : インタ�`フェイスのIID
//! @param		ppv : インタ�`フェイスを卦すポインタ�`へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	if( IID_INSSBuffer == riid ) {
		return GetInterface(static_cast<INSSBuffer*>(this), ppv);
	} else if( riid == IID_IMediaSample ) {
		return GetInterface(static_cast<IMediaSample*>(m_Sample), ppv);
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	バッファを函誼する
//! @param		ppdwBuffer : ポインタのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetBuffer( BYTE **ppdwBuffer )
{
	return Sample()->GetPointer( ppdwBuffer );
}
//----------------------------------------------------------------------------
//! @brief	  	バッファとサイズを函誼する
//! @param		ppdwBuffer : ポインタのポインタ
//! @param		pdwLength : サイズ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetBufferAndLength( BYTE **ppdwBuffer, DWORD *pdwLength )
{
	*pdwLength = static_cast<DWORD>( Sample()->GetActualDataLength() );
	return Sample()->GetPointer( ppdwBuffer );
}
//----------------------------------------------------------------------------
//! @brief	  	デ�`タが鯉�{されている�gサイズを函誼する
//! @param		pdwLength : サイズ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetLength( DWORD *pdwLength )
{
	*pdwLength = static_cast<DWORD>( Sample()->GetActualDataLength() );
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	バッファのサイズを函誼する
//! @param		pdwLength : サイズ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetMaxLength( DWORD *pdwLength )
{
	*pdwLength = static_cast<DWORD>( Sample()->GetSize() );
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	サイズを�O協する
//! @param		dwLength : サイズ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::SetLength( DWORD dwLength )
{
	return Sample()->SetActualDataLength( static_cast<long>( dwLength ) );
}


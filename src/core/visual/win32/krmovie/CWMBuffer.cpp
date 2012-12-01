/****************************************************************************/
/*! @file
@brief DirectShow media sample wrapper for Windows media format

DirectShow��Media Sample���åפ���Windows Media Format SDK�ä�Buffer
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/28
@note
*****************************************************************************/

#include "CWMBuffer.h"

//----------------------------------------------------------------------------
//! @brief	  	Ҫ�󤵤줿���󥿩`�ե������򷵤�
//! @param		riid : ���󥿩`�ե�������IID
//! @param		ppv : ���󥿩`�ե������򷵤��ݥ��󥿩`�ؤΥݥ���
//! @return		����`���`��
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
//! @brief	  	�Хåե���ȡ�ä���
//! @param		ppdwBuffer : �ݥ��󥿤Υݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetBuffer( BYTE **ppdwBuffer )
{
	return Sample()->GetPointer( ppdwBuffer );
}
//----------------------------------------------------------------------------
//! @brief	  	�Хåե��ȥ�������ȡ�ä���
//! @param		ppdwBuffer : �ݥ��󥿤Υݥ���
//! @param		pdwLength : ������
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetBufferAndLength( BYTE **ppdwBuffer, DWORD *pdwLength )
{
	*pdwLength = static_cast<DWORD>( Sample()->GetActualDataLength() );
	return Sample()->GetPointer( ppdwBuffer );
}
//----------------------------------------------------------------------------
//! @brief	  	�ǩ`������{����Ƥ���g��������ȡ�ä���
//! @param		pdwLength : ������
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetLength( DWORD *pdwLength )
{
	*pdwLength = static_cast<DWORD>( Sample()->GetActualDataLength() );
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	�Хåե��Υ�������ȡ�ä���
//! @param		pdwLength : ������
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::GetMaxLength( DWORD *pdwLength )
{
	*pdwLength = static_cast<DWORD>( Sample()->GetSize() );
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	���������O������
//! @param		dwLength : ������
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMBuffer::SetLength( DWORD dwLength )
{
	return Sample()->SetActualDataLength( static_cast<long>( dwLength ) );
}


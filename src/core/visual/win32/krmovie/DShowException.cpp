/****************************************************************************/
/*! @file
@brief DirectShow��HRESULT���å��`���ˉ䤨�����⥯�饹

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/17
@note
			2004/08/17	T.Imoto		
*****************************************************************************/

#include "DShowException.h"


//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//----------------------------------------------------------------------------
DShowException::DShowException()
: m_Hr(S_OK)
{
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}
//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		hr : DirectShow���v���η���
//----------------------------------------------------------------------------
DShowException::DShowException( HRESULT hr ) : m_Hr(hr)
{
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}

//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		right : ���ԩ`���
//----------------------------------------------------------------------------
DShowException::DShowException(const DShowException& right)
{
	*this = right;
}

//----------------------------------------------------------------------------
//! @brief	  	����
//! @param 		right : ���ԩ`���
//! @return		����
//----------------------------------------------------------------------------
DShowException& DShowException::operator=(const DShowException& right)
{
	m_Hr = right.m_Hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
	return *this;
}


//----------------------------------------------------------------------------
//! @brief	  	����`���`�ɤ���뤷����å��`�������ɤ���
//! @param 		right : ���ԩ`���
//! @return		����
//----------------------------------------------------------------------------
void DShowException::SetHResult( HRESULT hr )
{
	m_Hr = hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}


//----------------------------------------------------------------------------
//! @brief	  	�ǥ��ȥ饯���Ǥ��ؤ˺Τ⤷�ʤ�
//----------------------------------------------------------------------------
DShowException::~DShowException()
{
}
//----------------------------------------------------------------------------
//! @brief	  	����`��Ԕ���򆖤��Ϥ碌��
//! @return		����`��å��`��
//----------------------------------------------------------------------------
const TCHAR *DShowException::what( ) const
{
	return m_ErrorMes;
}



//----------------------------------------------------------------------------
//! @brief	  	DirectShow ����� ���Ｊ������Ȥ����ͳ�
//! �����v���ϑ���ʤ�(���⤬�k�����뤿��)
//! @param 		comment : ������
//! @param		hr : HRESULT
//----------------------------------------------------------------------------
void ThrowDShowException(const tjs_char *comment, HRESULT hr)
{
	TVPThrowExceptionMessage((ttstr(comment) + TJS_W(" : [0x") +
		TJSInt32ToHex((tjs_uint32)hr) + TJS_W("] ") +
		DShowException(hr).what()).c_str());
}


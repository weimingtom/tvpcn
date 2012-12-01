/****************************************************************************/
/*! @file
@brief DirectShow��HRESULT�����b�Z�[�W�ɕς����O�N���X

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
	ZeroMemory( m_ErrorMes, MAX_ERROR_TEXT_LEN );
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}
//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		hr : DirectShow�̊֐��̕Ԓl
//----------------------------------------------------------------------------
DShowException::DShowException( HRESULT hr ) : m_Hr(hr)
{
	ZeroMemory( m_ErrorMes, MAX_ERROR_TEXT_LEN );
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}

//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		right : �R�s�[����
//----------------------------------------------------------------------------
DShowException::DShowException(const DShowException& right)
{
	*this = right;
}

//----------------------------------------------------------------------------
//! @brief	  	���
//! @param 		right : �R�s�[����
//! @return		���g
//----------------------------------------------------------------------------
DShowException& DShowException::operator=(const DShowException& right)
{
	m_Hr = right.m_Hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
	return *this;
}


//----------------------------------------------------------------------------
//! @brief	  	�G���[�R�[�h�������A���b�Z�[�W�𐶐�����
//! @param 		right : �R�s�[����
//! @return		���g
//----------------------------------------------------------------------------
void DShowException::SetHResult( HRESULT hr )
{
	m_Hr = hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}


//----------------------------------------------------------------------------
//! @brief	  	�f�X�g���N�^�ł͓��ɉ������Ȃ�
//----------------------------------------------------------------------------
DShowException::~DShowException()
{
}
//----------------------------------------------------------------------------
//! @brief	  	�G���[�̏ڍׂ�₢���킹��
//! @return		�G���[���b�Z�[�W
//----------------------------------------------------------------------------
const TCHAR *DShowException::what( ) const
{
	return m_ErrorMes;
}



//----------------------------------------------------------------------------
//! @brief	  	DirectShow ��O�� �g���g����O�Ƃ��đ��o
//! ���̊֐��͖߂�Ȃ�(��O���������邽��)
//! @param 		comment : �R�����g
//! @param		hr : HRESULT
//----------------------------------------------------------------------------
void ThrowDShowException(const tjs_char *comment, HRESULT hr)
{
	TVPThrowExceptionMessage((ttstr(comment) + TJS_W(" : [0x") +
		TJSInt32ToHex((tjs_uint32)hr) + TJS_W("] ") +
		DShowException(hr).what()).c_str());
}


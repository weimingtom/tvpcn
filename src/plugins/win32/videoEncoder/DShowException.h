/****************************************************************************/
/*! @file
@brief DirectShow��HRESULT�����b�Z�[�W�ɕς����O�N���X

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/16
@note
			2004/08/16	T.Imoto		
*****************************************************************************/

#ifndef __DSHOW_EXCEPTION_H__
#define __DSHOW_EXCEPTION_H__

#include <dshow.h>
#include "tp_stub.h"

//----------------------------------------------------------------------------
//! @brief DirectShow��HRESULT�����b�Z�[�W�ɕς����O�N���X
//! 
//! ������̏����������܂߂ɍs���̂ŁA�g�p���̓G���[���݂̂Ɍ���A
//! �G���[������if���̒��ŃC���X�^���X������悤�ɂ��������悢�B
//----------------------------------------------------------------------------
class DShowException
{
	TCHAR	m_ErrorMes[MAX_ERROR_TEXT_LEN];
	HRESULT	m_Hr;

public:
	DShowException( ) throw( );
	DShowException(const DShowException& right) throw( );
	DShowException( HRESULT hr ) throw( );
	DShowException& operator=(const DShowException& right) throw( );
	virtual ~DShowException() throw( );
	virtual const TCHAR *what( ) const throw( );
	void SetHResult( HRESULT hr ) throw( );
};


void ThrowDShowException(const tjs_char *comment, HRESULT hr);

#endif	// __DSHOW_EXCEPTION_H__

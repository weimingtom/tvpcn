/****************************************************************************/
/*! @file
@brief DirectShow��HRESULT���å��`���ˉ䤨�����⥯�饹

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
//! @brief DirectShow��HRESULT���å��`���ˉ䤨�����⥯�饹
//! 
//! �����Фγ��ڻ��򤳤ޤ���Ф��Τǡ�ʹ�Õr�ϥ���`�r�Τߤ��ޤꡢ
//! ����`�I���if�Ĥ��Фǥ��󥹥��󥹻�����褦�ˤ��������褤��
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

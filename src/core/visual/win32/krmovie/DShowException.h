/****************************************************************************/
/*! @file
@brief DirectShowのHRESULTをメッセ�`ジに�笋┐訐�翌クラス

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
//! @brief DirectShowのHRESULTをメッセ�`ジに�笋┐訐�翌クラス
//! 
//! 猟忖双の兜豚晒をこまめに佩うので、聞喘�rはエラ�`�rのみに�泙蝓�
//! エラ�`�I尖のif猟の嶄でインスタンス晒するようにした圭がよい。
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

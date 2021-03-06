/****************************************************************************/
/*! @file
@brief DirectShowのHRESULTをメッセ�`ジに�笋┐訐�翌クラス

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
//! @param 		hr : DirectShowの�v方の卦��
//----------------------------------------------------------------------------
DShowException::DShowException( HRESULT hr ) : m_Hr(hr)
{
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}

//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		right : コピ�`もと
//----------------------------------------------------------------------------
DShowException::DShowException(const DShowException& right)
{
	*this = right;
}

//----------------------------------------------------------------------------
//! @brief	  	旗秘
//! @param 		right : コピ�`もと
//! @return		徭附
//----------------------------------------------------------------------------
DShowException& DShowException::operator=(const DShowException& right)
{
	m_Hr = right.m_Hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
	return *this;
}


//----------------------------------------------------------------------------
//! @brief	  	エラ�`コ�`ドを旗秘し、メッセ�`ジを伏撹する
//! @param 		right : コピ�`もと
//! @return		徭附
//----------------------------------------------------------------------------
void DShowException::SetHResult( HRESULT hr )
{
	m_Hr = hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}


//----------------------------------------------------------------------------
//! @brief	  	デストラクタでは蒙に採もしない
//----------------------------------------------------------------------------
DShowException::~DShowException()
{
}
//----------------------------------------------------------------------------
//! @brief	  	エラ�`の����を��い栽わせる
//! @return		エラ�`メッセ�`ジ
//----------------------------------------------------------------------------
const TCHAR *DShowException::what( ) const
{
	return m_ErrorMes;
}



//----------------------------------------------------------------------------
//! @brief	  	DirectShow 箭翌を 耳戦耳戦箭翌として僕竃
//! この�v方は��らない(箭翌が�k伏するため)
//! @param 		comment : コメント
//! @param		hr : HRESULT
//----------------------------------------------------------------------------
void ThrowDShowException(const tjs_char *comment, HRESULT hr)
{
	TVPThrowExceptionMessage((ttstr(comment) + TJS_W(" : [0x") +
		TJSInt32ToHex((tjs_uint32)hr) + TJS_W("] ") +
		DShowException(hr).what()).c_str());
}


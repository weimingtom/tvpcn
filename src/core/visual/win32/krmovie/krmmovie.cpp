/****************************************************************************/
/*! @file
@brief part of KRLMOVIE.DLL

�g�Фˤ�DirectX9�Խ�����Ҫ
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/09/25
@note
			2005/09/25	T.Imoto		����
*****************************************************************************/


#include "dsmixer.h"
#include "..\krmovie.h"

#include "asyncio.h"
#include "asyncrdr.h"

#include "tp_stub.h"

#include "OptionInfo.h"


//----------------------------------------------------------------------------
//! @brief	  	VideoOverlay Object (�ߥ�����������) ��ȡ�ä���
//! @param		callbackwin : ���`��Хå�������ɥ��Υϥ�ɥ�
//! @param		stream : �������ȥ�`��
//! @param		streamname : ���ȥ�`�����ǰ
//! @param		type : ���ȥ�`��Β�����
//! @param		size : ���ȥ�`��Υ�����
//! @param		out : VideoOverlay Object
//! @return		����`������
//----------------------------------------------------------------------------
void __stdcall GetMixingVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPDSMixerVideoOverlay;

	if( *out )
		static_cast<tTVPDSMixerVideoOverlay*>(*out)->BuildGraph( callbackwin, stream, streamname, type, size );
}

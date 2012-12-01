//---------------------------------------------------------------------------
// krmovie.cpp ( part of KRMOVIE.DLL )
// (c)2001-2009, W.Dee <dee@kikyou.info> and contributors
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.

	Modified by T.Imoto <http://www.kaede-software.com>
*/

//---------------------------------------------------------------------------


#include "dsoverlay.h"
#include "..\krmovie.h"

#include "asyncio.h"
#include "asyncrdr.h"

#include "tp_stub.h"

#include "OptionInfo.h"

//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
//---------------------------------------------------------------------------
// GetVideoOverlayObject
//---------------------------------------------------------------------------
const void __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPDSVideoOverlay;

	if( *out )
		static_cast<tTVPDSVideoOverlay*>(*out)->BuildGraph( callbackwin, stream, streamname, type, size );
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// GetAPIVersion
//---------------------------------------------------------------------------
void __stdcall GetAPIVersion(DWORD *ver)
{
	*ver = TVP_KRMOVIE_VER;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// V2Link : Initialize TVP plugin interface
//---------------------------------------------------------------------------
HRESULT __stdcall V2Link(iTVPFunctionExporter *exporter)
{
// ����_��λ�äǥ֥�`�����N��ˤ����¤Υ᥽�åɤǴ_�����Ť�ָ�����롣
// �֥�`���������ä���ϡ����ӳ����Ěs(���`�륹���å�)��Ҋ�ơ��ɤ��Ǵ_�����줿���꤬��`�����Ƥ��뤫̽�롣
// _CrtDumpMemoryLeaks �ǥǥХå������˥�`����������δ_�����Ť�����Τǡ�����������OK
// �_��혤����_���ʈ��Ϥ���������������ץȤ�̶�����Фۤ�ͬ��혤Ǵ_�������Ϥ���
//	_CrtSetBreakAlloc(53);	// ָ�����줿����Ŀ�Υ���_���r�˥֥�`�����N��

	TVPInitImportStub(exporter);

	return S_OK;
}
//---------------------------------------------------------------------------
// V2Unlink : Uninitialize TVP plugin interface
//---------------------------------------------------------------------------
HRESULT __stdcall V2Unlink()
{
	TVPUninitImportStub();

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return S_OK;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// GetOptionDesc : Give option information to kirikiri configurator
//---------------------------------------------------------------------------
extern "C" const wchar_t * _stdcall GetOptionDesc()
{
	return GetOptionInfoString();
}
//---------------------------------------------------------------------------


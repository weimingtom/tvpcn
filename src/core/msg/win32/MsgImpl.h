//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
#ifndef MsgImplH
#define MsgImplH

#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
#endif

//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL(TVPScriptExceptionRaised,
	TJS_W("�ű��з�������"));

TVP_MSG_DECL(TVPHardwareExceptionRaised,
	TJS_W("������Ӳ������"));

TVP_MSG_DECL(TVPMainCDPName,
	TJS_W("�ű��༭������Ҫ��"));

TVP_MSG_DECL(TVPExceptionCDPName,
	TJS_W("�ű��༭��������֪ͨ��"));

TVP_MSG_DECL(TVPCannnotLocateUIDLLForFolderSelection,
	TJS_W("��Ҫ��ʾ�ļ���/�ļ���ѡ����ʱ ")
		TJS_W("�޷��ҵ� krdevui.dll ��\n")
		TJS_W("�뽫Ҫִ�е�Ŀ¼/�ļ�����Ϊ�����еĲ���ָ��"));

TVP_MSG_DECL(TVPInvalidUIDLL,
	TJS_W("krdevui.dll�쳣��汾����"));

TVP_MSG_DECL(TVPInvalidBPP,
	TJS_W("��Ч��ɫ��"));

TVP_MSG_DECL(TVPCannotLoadPlugin,
	TJS_W("�޷���ȡ��� %1 "));

TVP_MSG_DECL(TVPNotValidPlugin,
	TJS_W("%1 ������Ч�Ĳ��"));

TVP_MSG_DECL(TVPPluginUninitFailed,
	TJS_W("����ͷ�ʧ��"));

TVP_MSG_DECL(TVPCannnotLinkPluginWhilePluginLinking,
	TJS_W("����������޷������������"));

TVP_MSG_DECL(TVPNotSusiePlugin,
	TJS_W("�쳣�� Susie ���"));

TVP_MSG_DECL(TVPSusiePluginError,
	TJS_W("Susie �з�������/������� %1"));

TVP_MSG_DECL(TVPCannotReleasePlugin,
	TJS_W("ָ���Ĳ����ʹ���У��޷��ͷ�"));

TVP_MSG_DECL(TVPNotLoadedPlugin,
	TJS_W("%1 δ����ȡ"));

TVP_MSG_DECL(TVPCannotAllocateBitmapBits,
	TJS_W("�޷�����λͼ���ڴ�/%1(size=%2)"));

TVP_MSG_DECL(TVPScanLineRangeOver,
	TJS_W("ɨ���� %1 ������Χ(0��%2)"));

TVP_MSG_DECL(TVPPluginError,
	TJS_W("��������˴���/%1"));

TVP_MSG_DECL(TVPInvalidCDDADrive,
	TJS_W("�޷���ָ��������������CDDA"));

TVP_MSG_DECL(TVPCDDADriveNotFound,
	TJS_W("δ�ҵ��ɲ��� CD-DA ��CD������"));

TVP_MSG_DECL(TVPMCIError,
	TJS_W("���� MCI ���� : %1"));

TVP_MSG_DECL(TVPInvalidSMF,
	TJS_W("������Ч�� SMF �ļ� : %1"));

TVP_MSG_DECL(TVPMalformedMIDIMessage,
	TJS_W("ָ������Ϣ���Ǹ�ʽ��Ч��MIDI��Ϣ"));

TVP_MSG_DECL(TVPCannotInitDirectSound,
	TJS_W("DirectSound �޷���ʼ�� : %1"));

TVP_MSG_DECL(TVPCannotCreateDSSecondaryBuffer,
	TJS_W("DirectSound �޷������ڶ������� : %1/%2"));

TVP_MSG_DECL(TVPInvalidLoopInformation,
	TJS_W("������Ϣ %1 �쳣"));

TVP_MSG_DECL(TVPNotChildMenuItem,
	TJS_W("ָ����Ŀ¼��Ŀ���Ǹ�Ŀ¼��Ŀ������Ŀ"));

TVP_MSG_DECL(TVPCannotInitDirectDraw,
	TJS_W("DirectDraw �޷���ʼ�� : %1"));

TVP_MSG_DECL(TVPCannotFindDisplayMode,
	TJS_W("�޷��ҵ��ʺϵĻ���ģʽ : %1"));

TVP_MSG_DECL(TVPCannotSwitchToFullScreen,
	TJS_W("�޷��л���ȫ�� : %1"));

TVP_MSG_DECL(TVPInvalidPropertyInFullScreen,
	TJS_W("�趨��ȫ��ģʽ�޷��޸ĵ�����"));

TVP_MSG_DECL(TVPInvalidMethodInFullScreen,
	TJS_W("������ȫ��ģʽ���޷������ķ���"));

TVP_MSG_DECL(TVPCannotLoadCursor,
	TJS_W("��ȡ���ָ�� %1 ʧ��"));

TVP_MSG_DECL(TVPCannotLoadKrMovieDLL,
	TJS_W("Ϊ�˲�����Ƶ/Shockwave Flash ��Ҫ krmovie.dll / krflash.dll ")
		TJS_W("�ļ���֧�֡��޷���ȡ����Ҫ���ļ���"));

TVP_MSG_DECL(TVPInvalidKrMovieDLL,
	TJS_W("krmovie.dll/krflash.dll Ϊ�쳣��֧�ֵİ汾"));

TVP_MSG_DECL(TVPErrorInKrMovieDLL,
	TJS_W("krmovie.dll/krflash.dll �ڷ����˴���/%1"));

TVP_MSG_DECL(TVPWindowAlreadyMissing,
	TJS_W("Window��������"));

TVP_MSG_DECL(TVPPrerenderedFontMappingFailed,
	TJS_W("����Ⱦ�����ӳ��ʧ�� : %1"));

TVP_MSG_DECL_CONST(TVPConfigFailOriginalFileCannotBeRewritten,
	TJS_W("%1 �޷���д�롣���ܸ��ļ����ڱ�ʹ�ã���û��д��Ȩ��"));

TVP_MSG_DECL(TVPConfigFailTempExeNotErased,
	TJS_W("�޷�ȷ�� %1 �Ƿ��˳�������޷�ɾ����(����ɾ�����ļ�)"));

TVP_MSG_DECL_CONST(TVPExecutionFail,
	TJS_W("%1 �޷�ִ��"));

TVP_MSG_DECL(TVPPluginUnboundFunctionError,
	TJS_W("���Ҫ���� %1 �����ú����ڱ����ڲ����ڡ�")
			TJS_W("��ȷ�ϲ���뱾��İ汾�Ƿ��Ӧ��"));

TVP_MSG_DECL(TVPExceptionHadBeenOccured,
	TJS_W(" = (���ⷢ��)"));

TVP_MSG_DECL(TVPConsoleResult,
	TJS_W("����̨ : "));


//---------------------------------------------------------------------------



#endif

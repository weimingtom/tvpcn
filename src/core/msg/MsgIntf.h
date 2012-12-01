//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Definition of Messages and Message Related Utilities
//---------------------------------------------------------------------------
#ifndef MsgIntfH
#define MsgIntfH

#include "tjs.h"
#include "tjsMessage.h"

#ifndef TVP_MSG_DECL
	#define TVP_MSG_DECL(name, msg) extern tTJSMessageHolder name;
	#define TVP_MSG_DECL_CONST(name, msg) extern tTJSMessageHolder name;
#endif

#include "MsgImpl.h"
#include "svn_revision.h"

//---------------------------------------------------------------------------
// Message Strings ( these should be localized )
//---------------------------------------------------------------------------
// Japanese localized messages
TVP_MSG_DECL_CONST(TVPAboutString,
	
TJS_W("���Ｊ��[Kirikiri] 2 ִ���ں� version %1 zh-CN ( TJS version %2 )\n")
TJS_W("Compiled on ") __DATE__ TJS_W(" ") __TIME__ TJS_W("\n")
TJS_W("SVN Revision: ") TVP_SVN_REVISION TJS_W("\n")
TJS_W("Copyright (C) 1997-2012 W.Dee and contributors All rights reserved.\n")
TJS_W("Contributors in alphabetical order:\n")
TJS_W("  Go Watanabe, Kenjo, Kiyobee, Kouhei Yanagita, mey, MIK, Takenori Imoto, yun\n")
TJS_W("\n���ķ��룺[Key Fans Club] the Council of Doujin-game Development and Promotion(KCDDP).\n")
TJS_W("  ������:MiliMeow\n\n")
TJS_W("���ڼ��Ｊ��ִ���ں˵�ʹ��/����/�ĉ䣬\n")
TJS_W("��ο�������SDK������license.txt\n")
TJS_W("------------------------------------------------------------------------------\n")
TJS_W("Thanks for many libraries, contributers and supporters not listible here.\n")
TJS_W("This software is based in part on the work of Independent JPEG Group.\n")
TJS_W("Regex++ Copyright (c) 1998-2003 Dr John Maddock\n")
TJS_W("ERINA-Library Copyright (C) 2001 Leshade Entis, Entis-soft.\n")
TJS_W("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n")
TJS_W("Using \"A C-program for MT19937\"\n")
TJS_W("\n")
TJS_W("   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,\n")
TJS_W("   All rights reserved.\n")
TJS_W("\n")
TJS_W("   Redistribution and use in source and binary forms, with or without\n")
TJS_W("   modification, are permitted provided that the following conditions\n")
TJS_W("   are met:\n")
TJS_W("\n")
TJS_W("     1. Redistributions of source code must retain the above copyright\n")
TJS_W("        notice, this list of conditions and the following disclaimer.\n")
TJS_W("\n")
TJS_W("     2. Redistributions in binary form must reproduce the above copyright\n")
TJS_W("        notice, this list of conditions and the following disclaimer in the\n")
TJS_W("        documentation and/or other materials provided with the distribution.\n")
TJS_W("\n")
TJS_W("     3. The names of its contributors may not be used to endorse or promote\n")
TJS_W("        products derived from this software without specific prior written\n")
TJS_W("        permission.\n")
TJS_W("\n")
TJS_W("   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n")
TJS_W("   \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n")
TJS_W("   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n")
TJS_W("   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR\n")
TJS_W("   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n")
TJS_W("   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n")
TJS_W("   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n")
TJS_W("   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n")
TJS_W("   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n")
TJS_W("   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n")
TJS_W("   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n")
TJS_W("------------------------------------------------------------------------------\n")
TJS_W("������Ϣ\n")
  // important log (environment information, Debug.notice etc.) comes here
);

TVP_MSG_DECL_CONST(TVPVersionInformation,
	
TJS_W("���Ｊ��[Kirikiri] 2 ִ���ں�/%1 zh-CN")
TJS_W("(SVN revision:") TVP_SVN_REVISION TJS_W("; Compiled on ") __DATE__ TJS_W(" ") __TIME__ TJS_W(") TJS2/%2 ")
TJS_W("Copyright (C) 1997-2012 W.Dee and contributors All rights reserved."));

TVP_MSG_DECL_CONST(TVPVersionInformation2,
	TJS_W("�汾�鱨��ϸ���԰� Ctrl + F12 ��ϸ����"));

TVP_MSG_DECL_CONST(TVPDownloadPageURL,
	TJS_W("http://kikyou.info/tvp/"));

TVP_MSG_DECL(TVPInternalError,
	TJS_W("�����ڲ�����: at %1 line %2"));

TVP_MSG_DECL(TVPInvalidParam,
	TJS_W("�Ƿ�����"));

TVP_MSG_DECL(TVPWarnDebugOptionEnabled,
	TJS_W("����ָ����-debugѡ����ڼ��Ｊ�﹤���ڵ���ģʽ����ע�⣬����ģʽ�£�ִ���ٶ��޷��ﵽ���š�"));

TVP_MSG_DECL(TVPCommandLineParamIgnoredAndDefaultUsed,
	TJS_W("�����в��� %1 ��ָ����ֵ %2 ʹ���˷Ƿ��ĸ�ʽ"));

TVP_MSG_DECL(TVPInvalidCommandLineParam,
	TJS_W("�����в��� %1 ��ָ����ֵ %2 Ϊ�Ƿ�"));

TVP_MSG_DECL(TVPNotImplemented,
	TJS_W("������δʵ�ֵĹ���"));

TVP_MSG_DECL(TVPCannotOpenStorage,
	TJS_W("�޷����ļ� %1"));

TVP_MSG_DECL(TVPCannotFindStorage,
	TJS_W("�޷��ҵ��ļ� %1"));

TVP_MSG_DECL(TVPCannotOpenStorageForWrite,
	TJS_W("�޷���д��ģʽ���ļ� %1 ����ȷ���Ƿ���ļ�����ֻ�����ԣ���û��д����ļ���Ȩ�ޣ����߸��ļ������޷�д���ý���ϡ�"));

TVP_MSG_DECL(TVPStorageInArchiveNotFound,
	TJS_W("�ڰ��ļ� %2 ���޷��ҵ��ļ� %1��"));

TVP_MSG_DECL(TVPInvalidPathName,
	TJS_W("·���� %1 ��ʽ��Ч��"));

TVP_MSG_DECL(TVPUnsupportedMediaName,
	TJS_W("\"%1\" Ϊ��֧�ֵ�ý���ʽ��"));

TVP_MSG_DECL(TVPCannotUnbindXP3EXE,
	TJS_W("%1 ��Ȼ�ǿ�ִ���ļ������޷��ҵ������ϵ�XP3ѹ������"));

TVP_MSG_DECL(TVPCannotFindXP3Mark,
	TJS_W("%1 ����XP3ѹ�����������޷�֧�ֵĸ�ʽ����Ӧ��ָ��XP3���ĵط�û��ָ������ָ�����޷�֧�ֵİ��ļ�ʱ�������ô��������ȷ�ϡ�"));

TVP_MSG_DECL(TVPMissingPathDelimiterAtLast,
	TJS_W("����·��������ƶ���>�����ߡ�/�����š����Ӽ��Ｊ��2.19 beta14���ļ���·���ķ����ɡ�#����Ϊ�ˡ�>����"));

TVP_MSG_DECL(TVPFilenameContainsSharpWarn,
	TJS_W("(ע��) '#' ���������ļ�·�� \"%1\" �С��Ӽ��Ｊ��2.19 beta14���ļ���·���ķ����ɡ�#����Ϊ�ˡ�>����")
	TJS_W("���ʹ���ˡ�#����Ϊ�ļ���·�����ţ��뾡���޸�Ϊ��>����"));

TVP_MSG_DECL(TVPCannotGetLocalName,
	TJS_W("�洢�� %1 �޷��任Ϊ�����ļ������޷��任���ļ��ͷǱ����ļ����ļ���"));

TVP_MSG_DECL(TVPReadError,
	TJS_W("��ȡ���󡣴����ļ��𻵣����豸�����ȡʧ�ܵĿ����ԡ�"));

TVP_MSG_DECL(TVPWriteError,
	TJS_W("д�����"));

TVP_MSG_DECL(TVPSeekError,
	TJS_W("�ļ���Ѱַʧ�ܡ������ļ��𻵣����豸�����ȡʧ�ܵĿ����ԡ�"));

TVP_MSG_DECL(TVPTruncateError,
	TJS_W("�ļ���ȡʧ��"));

TVP_MSG_DECL(TVPInsufficientMemory,
	TJS_W("�ڴ����ʧ�ܡ�"));

TVP_MSG_DECL(TVPUncompressionFailed,
	TJS_W("�ļ���ѹʧ�ܡ�������ָ���˲�֧�ֵ�ѹ����ʽ�����ļ����𻵡�"));

TVP_MSG_DECL(TVPCompressionFailed,
	TJS_W("�ļ�ѹ��ʧ�ܡ�"));

TVP_MSG_DECL(TVPCannotWriteToArchive,
	TJS_W("���ܽ�����д���ļ����С�"));

TVP_MSG_DECL(TVPUnsupportedCipherMode,
	TJS_W("%1 �ǲ�֧�ֵļ�����ʽ�����ļ����𻵡�"));

TVP_MSG_DECL(TVPUnsupportedModeString,
	TJS_W("ָ�����޷�ʶ���ģʽ�ַ���(%1)"));

TVP_MSG_DECL(TVPUnknownGraphicFormat,
	TJS_W("%1 Ϊδ֪��ͼ���ʽ"));

TVP_MSG_DECL(TVPCannotSuggestGraphicExtension,
	TJS_W("�޷��ҵ����ļ���Ϊ %1 �����ʵ���չ�����ļ���"));

TVP_MSG_DECL(TVPMaskSizeMismatch,
	TJS_W("����ͼ�����ͼ���С��ͬ"));

TVP_MSG_DECL(TVPProvinceSizeMismatch,
	TJS_W("����ͼ�� %1 ����ͼ���С��ͬ"));

TVP_MSG_DECL(TVPImageLoadError,
	TJS_W("ͼ���ȡ�з�������/%1"));

TVP_MSG_DECL(TVPJPEGLoadError,
	TJS_W("JPEG ͼ���ȡ�з�������/%1"));

TVP_MSG_DECL(TVPPNGLoadError,
	TJS_W("PNG ͼ���ȡ�з�������/%1"));

TVP_MSG_DECL(TVPERILoadError,
	TJS_W("ERI ͼ���ȡ�з�������/%1"));

TVP_MSG_DECL(TVPTLGLoadError,
	TJS_W("TLG ͼ���ȡ�з�������/%1"));

TVP_MSG_DECL(TVPInvalidImageSaveType,
	TJS_W("�Ƿ���ͼ��洢��ʽ(%1)"));

TVP_MSG_DECL(TVPInvalidOperationFor8BPP,
	TJS_W("ִ���˶��� 8bpp ͼ����ִ�еĲ���"));

TVP_MSG_DECL(TVPSpecifyWindow,
	TJS_W("��ָ�� Window ���͵Ķ���"));

TVP_MSG_DECL(TVPSpecifyLayer,
	TJS_W("��ָ�� Layer ���͵Ķ���"));

TVP_MSG_DECL(TVPCannotCreateEmptyLayerImage,
	TJS_W("�޷���ͼ��ĳ����߿�ָ��Ϊ 0 ������ֵ"));

TVP_MSG_DECL(TVPCannotSetPrimaryInvisible,
	TJS_W("��Ҫ���޷�����Ϊ���ɼ�"));

TVP_MSG_DECL(TVPCannotMovePrimary,
	TJS_W("��Ҫ���޷��ƶ�"));

TVP_MSG_DECL(TVPCannotSetParentSelf,
	TJS_W("�޷���Ϊ����ĸ�����"));

TVP_MSG_DECL(TVPCannotMoveNextToSelfOrNotSiblings,
	TJS_W("�޷��ƶ�������ǰ���Ǹ����ǰ��"));

TVP_MSG_DECL(TVPCannotMovePrimaryOrSiblingless,
	TJS_W("�޷�ǰ���ƶ������û���ֵܵĲ�"));

TVP_MSG_DECL(TVPCannotMoveToUnderOtherPrimaryLayer,
	TJS_W("�޷������ƶ���������������"));

TVP_MSG_DECL(TVPInvalidImagePosition,
	TJS_W("�����������û��ͼ�������"));

TVP_MSG_DECL(TVPCannotSetModeToDisabledOrModal,
	TJS_W("�޷�Ϊ�Ѿ�Ϊģ̬�Ļ���Ч/���ɼ��Ĳ��趨ģʽ��"));

TVP_MSG_DECL(TVPNotDrawableLayerType,
	TJS_W("�����͵Ĳ��޷�������桢��ȡͼ���Լ�ͼ�������/�ƶ�/��ȡ��Ϣ��"));

TVP_MSG_DECL(TVPSourceLayerHasNoImage,
	TJS_W("����Դ��û��ͼ��"));

TVP_MSG_DECL(TVPUnsupportedLayerType,
	TJS_W("%1 �޷��ڱ����͵Ĳ���ʹ��"));

TVP_MSG_DECL(TVPNotDrawableFaceType,
	TJS_W("%1 �޷��ڱ����ͱ��������"));

TVP_MSG_DECL(TVPCannotConvertLayerTypeUsingGivenDirection,
	TJS_W("�޷�����ָ�����͵Ĳ�任"));

TVP_MSG_DECL(TVPNegativeOpacityNotSupportedOnThisFace,
	TJS_W("�������޷��ƶ�����͸����"));

TVP_MSG_DECL(TVPSrcRectOutOfBitmap,
	TJS_W("����Դ����λͼ֮���������ָ���ܹ�������ȷ��Χ�Ĵ���Դ"));

TVP_MSG_DECL(TVPBoxBlurAreaMustContainCenterPixel,
	TJS_W("����ģ���ķ�Χ�������(0,0)��left��right����ͬ����ͬ������top��bottomҲһ����"));

TVP_MSG_DECL(TVPBoxBlurAreaMustBeSmallerThan16Million,
	TJS_W("����ģ���ķ�Χ���󡣾���ģ���ķ�Χ����С��1677������"));

TVP_MSG_DECL(TVPCannotChangeFocusInProcessingFocus,
	TJS_W("�޷��ڱ任����ʱ�ٴν��н���任."));

TVP_MSG_DECL(TVPWindowHasNoLayer,
	TJS_W("����û�в�"));

TVP_MSG_DECL(TVPWindowHasAlreadyPrimaryLayer,
	TJS_W("�ô����Ѿ�������Ҫ��"));

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter,
	TJS_W("�¼� %1 ��Ҫ����"));

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter2,
	TJS_W("�¼� %1 ��Ҫ���� %2 "));

TVP_MSG_DECL(TVPSpecifiedEventNameIsUnknown,
	TJS_W("�¼��� %1 Ϊδ֪�¼�"));

TVP_MSG_DECL(TVPOutOfRectangle,
	TJS_W("ָ�����˾���֮��"));

TVP_MSG_DECL(TVPInvalidMethodInUpdating,
	TJS_W("����������޷�ִ�б�����"));

TVP_MSG_DECL(TVPCannotCreateInstance,
	TJS_W("�����޷�����ʵ��"));

TVP_MSG_DECL(TVPUnknownWaveFormat,
	TJS_W("%1 �ǲ�֧�ֵ�������ʽ"));

TVP_MSG_DECL(TVPSpecifyMenuItem,
	TJS_W("��ָ�� MenuItem ��Ķ���"));

TVP_MSG_DECL(TVPCurrentTransitionMustBeStopping,
	TJS_W("�������ڵĽ���ֹ֮ͣ���ٽ����µĽ��䡣�����һ������ж�����佫�ᷢ��������"));

TVP_MSG_DECL(TVPTransHandlerError,
	TJS_W("�������������� : %1"));

TVP_MSG_DECL(TVPTransAlreadyRegistered,
	TJS_W("�������� %1 �Ѿ�ע��"));

TVP_MSG_DECL(TVPCannotFindTransHander,
	TJS_W("�޷��ҵ������� %1 "));

TVP_MSG_DECL(TVPSpecifyTransitionSource,
	TJS_W("��ָ������Դ"));

TVP_MSG_DECL(TVPLayerCannotHaveImage,
	TJS_W("�ò��޷�ӵ��ͼ��"));

TVP_MSG_DECL(TVPTransitionSourceAndDestinationMustHaveImage,
	TJS_W("����Դ�ͽ�����󶼱���ӵ��ͼ��"));

TVP_MSG_DECL(TVPCannotLoadRuleGraphic,
	TJS_W("�޷���ȡ����ͼ�� %1 "));

TVP_MSG_DECL(TVPSpecifyOption,
	TJS_W("��ָ��ѡ�� %1 "));

TVP_MSG_DECL(TVPTransitionLayerSizeMismatch,
	TJS_W("����Դ(%1)�뽥�����(%2)���С��ͬ"));

TVP_MSG_DECL(TVPTransitionMutualSource,
	TJS_W("����Դ�Ľ���ԴΪ������"));

TVP_MSG_DECL(TVPHoldDestinationAlphaParameterIsNowDeprecated,
	TJS_W("���� : ���� %1 �ĵ� %2 �����������ݵ� hda �������ڼ��Ｊ� 2.23 beta 2 �Ժ󽫱����ԡ���ʹ�� Layer.holdAlpha ���档"));

TVP_MSG_DECL(TVPCannotConnectMultipleWaveSoundBufferAtOnce,
	TJS_W("�����ö�� WaveSoundBuffer ͬʱʹ��һ�� Filter"));

TVP_MSG_DECL(TVPInvalidWindowSizeMustBeIn64to32768,
	TJS_W("window ������ 2 ���ݣ��ҷ�Χ�� 64��32768 ��"));

TVP_MSG_DECL(TVPInvalidOverlapCountMustBeIn2to32,
	TJS_W("overlap ������ 2 ���ݣ��ҷ�Χ�� 2��32 ��"));

TVP_MSG_DECL(TVPKAGNoLine,
	TJS_W("��ȡ�ľ籾�ļ� %1 Ϊ��"));

TVP_MSG_DECL(TVPKAGCannotOmmitFirstLabelName,
	TJS_W("�籾�ļ�����ʡ������Ķ����ǩ��"));

TVP_MSG_DECL(TVPKAGLabelNotFound,
	TJS_W("�籾�ļ� %1 ���޷��ҵ������ǩ %2 "));

TVP_MSG_DECL(TVPKAGInlineScriptNotEnd,
	TJS_W("δ�ҵ�[endscript] �� @endscript "));

TVP_MSG_DECL(TVPKAGSyntaxError,
	TJS_W("KAG��ǩ��Tag���﷨��������'[' �� ']' ����ԡ�\" �� \" ����ԡ���������ո񡢶���Ļ��С�macro����endmacro ����ԡ���Ҫ�������Ƿ���ȷ���"));

TVP_MSG_DECL(TVPKAGMacroEntityNotAvailable,
	TJS_W("��ʵ�岻���ں�֮��ʹ��"));

TVP_MSG_DECL(TVPKAGCallStackUnderflow,
	TJS_W("return ��ǩû�ж�Ӧ�� call ��ǩ ( return ��ǩ���� )"));

TVP_MSG_DECL(TVPKAGReturnLostSync,
	TJS_W("���ھ籾�ļ��Ѿ��ı䣬�޷��ҵ�return��Ҫ�ص���λ��"));

TVP_MSG_DECL(TVPKAGSpecifyKAGParser,
	TJS_W("��ָ�� KAGParser ��Ķ���"));

TVP_MSG_DECL(TVPKAGMalformedSaveData,
	TJS_W("�浵�����쳣�����ܴ��������𻵵Ŀ�����"));

TVP_MSG_DECL(TVPLabelOrScriptInMacro,
	TJS_W("�����ǩ�� iscript �����ں���ʹ�á�"));

TVP_MSG_DECL(TVPUnknownMacroName,
	TJS_W("�� \"%1\" δ��ע��"));

//---------------------------------------------------------------------------
// Utility Functions
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(ttstr, TVPFormatMessage, (const tjs_char *msg, const ttstr & p1));
TJS_EXP_FUNC_DEF(ttstr, TVPFormatMessage, (const tjs_char *msg, const ttstr & p1,
	const ttstr & p2));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg,
	const ttstr &p1, tjs_int num));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg, const ttstr &p1));
TJS_EXP_FUNC_DEF(void, TVPThrowExceptionMessage, (const tjs_char *msg,
	const ttstr & p1, const ttstr & p2));

TJS_EXP_FUNC_DEF(ttstr, TVPGetAboutString, ());
TJS_EXP_FUNC_DEF(ttstr, TVPGetVersionInformation, ());
TJS_EXP_FUNC_DEF(ttstr, TVPGetVersionString, ());

#define TVPThrowInternalError \
	TVPThrowExceptionMessage(TVPInternalError, __FILE__,  __LINE__)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// version retrieving
//---------------------------------------------------------------------------
extern tjs_int TVPVersionMajor;
extern tjs_int TVPVersionMinor;
extern tjs_int TVPVersionRelease;
extern tjs_int TVPVersionBuild;
//---------------------------------------------------------------------------
extern void TVPGetVersion();
/*
	implement in each platforms;
	fill these four version field.
*/
//---------------------------------------------------------------------------
TJS_EXP_FUNC_DEF(void, TVPGetSystemVersion, (tjs_int &major, tjs_int &minor,
	tjs_int &release, tjs_int &build));
TJS_EXP_FUNC_DEF(void, TVPGetTJSVersion, (tjs_int &major, tjs_int &minor,
	tjs_int &release));
//---------------------------------------------------------------------------


#endif

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
	
TJS_W("吉里吉里[Kirikiri] 2 执行内核 version %1 zh-CN ( TJS version %2 )\n")
TJS_W("Compiled on ") __DATE__ TJS_W(" ") __TIME__ TJS_W("\n")
TJS_W("SVN Revision: ") TVP_SVN_REVISION TJS_W("\n")
TJS_W("Copyright (C) 1997-2012 W.Dee and contributors All rights reserved.\n")
TJS_W("Contributors in alphabetical order:\n")
TJS_W("  Go Watanabe, Kenjo, Kiyobee, Kouhei Yanagita, mey, MIK, Takenori Imoto, yun\n")
TJS_W("\n中文翻译：[Key Fans Club] the Council of Doujin-game Development and Promotion(KCDDP).\n")
TJS_W("  翻译者:MiliMeow\n\n")
TJS_W("关于吉里吉里执行内核的使用/发布/改変，\n")
TJS_W("请参考并遵照SDK附带的license.txt\n")
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
TJS_W("环境信息\n")
  // important log (environment information, Debug.notice etc.) comes here
);

TVP_MSG_DECL_CONST(TVPVersionInformation,
	
TJS_W("吉里吉里[Kirikiri] 2 执行内核/%1 zh-CN")
TJS_W("(SVN revision:") TVP_SVN_REVISION TJS_W("; Compiled on ") __DATE__ TJS_W(" ") __TIME__ TJS_W(") TJS2/%2 ")
TJS_W("Copyright (C) 1997-2012 W.Dee and contributors All rights reserved."));

TVP_MSG_DECL_CONST(TVPVersionInformation2,
	TJS_W("版本情报详细可以按 Ctrl + F12 详细阅览"));

TVP_MSG_DECL_CONST(TVPDownloadPageURL,
	TJS_W("http://kikyou.info/tvp/"));

TVP_MSG_DECL(TVPInternalError,
	TJS_W("发生内部错误: at %1 line %2"));

TVP_MSG_DECL(TVPInvalidParam,
	TJS_W("非法参数"));

TVP_MSG_DECL(TVPWarnDebugOptionEnabled,
	TJS_W("由于指定了-debug选项，现在吉里吉里工作在调试模式。请注意，调试模式下，执行速度无法达到最优。"));

TVP_MSG_DECL(TVPCommandLineParamIgnoredAndDefaultUsed,
	TJS_W("命令行参数 %1 所指定的值 %2 使用了非法的格式"));

TVP_MSG_DECL(TVPInvalidCommandLineParam,
	TJS_W("命令行参数 %1 所指定的值 %2 为非法"));

TVP_MSG_DECL(TVPNotImplemented,
	TJS_W("调用了未实现的功能"));

TVP_MSG_DECL(TVPCannotOpenStorage,
	TJS_W("无法打开文件 %1"));

TVP_MSG_DECL(TVPCannotFindStorage,
	TJS_W("无法找到文件 %1"));

TVP_MSG_DECL(TVPCannotOpenStorageForWrite,
	TJS_W("无法以写入模式打开文件 %1 。请确认是否该文件带有只读属性，您没有写入该文件的权限，或者该文件处在无法写入的媒体上。"));

TVP_MSG_DECL(TVPStorageInArchiveNotFound,
	TJS_W("在包文件 %2 中无法找到文件 %1。"));

TVP_MSG_DECL(TVPInvalidPathName,
	TJS_W("路径名 %1 格式无效。"));

TVP_MSG_DECL(TVPUnsupportedMediaName,
	TJS_W("\"%1\" 为不支持的媒体格式。"));

TVP_MSG_DECL(TVPCannotUnbindXP3EXE,
	TJS_W("%1 虽然是可执行文件，但无法找到与其结合的XP3压缩包。"));

TVP_MSG_DECL(TVPCannotFindXP3Mark,
	TJS_W("%1 不是XP3压缩包，而是无法支持的格式。在应该指定XP3包的地方没有指定，或指定了无法支持的包文件时将产生该错误。请进行确认。"));

TVP_MSG_DECL(TVPMissingPathDelimiterAtLast,
	TJS_W("请在路径名最后制定“>”或者“/”符号。（从吉里吉里2.19 beta14起，文件包路径的符号由“#”变为了“>”）"));

TVP_MSG_DECL(TVPFilenameContainsSharpWarn,
	TJS_W("(注意) '#' 被包括在文件路径 \"%1\" 中。从吉里吉里2.19 beta14起，文件包路径的符号由“#”变为了“>”。")
	TJS_W("如果使用了“#”作为文件包路径符号，请尽快修改为“>”。"));

TVP_MSG_DECL(TVPCannotGetLocalName,
	TJS_W("存储名 %1 无法变换为本地文件名。无法变换包文件和非本地文件的文件。"));

TVP_MSG_DECL(TVPReadError,
	TJS_W("读取错误。存在文件损坏，或设备本身读取失败的可能性。"));

TVP_MSG_DECL(TVPWriteError,
	TJS_W("写入错误"));

TVP_MSG_DECL(TVPSeekError,
	TJS_W("文件内寻址失败。存在文件损坏，或设备本身读取失败的可能性。"));

TVP_MSG_DECL(TVPTruncateError,
	TJS_W("文件切取失败"));

TVP_MSG_DECL(TVPInsufficientMemory,
	TJS_W("内存分配失败。"));

TVP_MSG_DECL(TVPUncompressionFailed,
	TJS_W("文件解压失败。可能是指定了不支持的压缩形式，或文件已损坏。"));

TVP_MSG_DECL(TVPCompressionFailed,
	TJS_W("文件压缩失败。"));

TVP_MSG_DECL(TVPCannotWriteToArchive,
	TJS_W("不能将数据写入文件包中。"));

TVP_MSG_DECL(TVPUnsupportedCipherMode,
	TJS_W("%1 是不支持的加密形式，或文件已损坏。"));

TVP_MSG_DECL(TVPUnsupportedModeString,
	TJS_W("指定了无法识别的模式字符串(%1)"));

TVP_MSG_DECL(TVPUnknownGraphicFormat,
	TJS_W("%1 为未知的图像格式"));

TVP_MSG_DECL(TVPCannotSuggestGraphicExtension,
	TJS_W("无法找到主文件名为 %1 且有适当扩展名的文件。"));

TVP_MSG_DECL(TVPMaskSizeMismatch,
	TJS_W("遮罩图像和主图像大小不同"));

TVP_MSG_DECL(TVPProvinceSizeMismatch,
	TJS_W("领域图像 %1 与主图像大小不同"));

TVP_MSG_DECL(TVPImageLoadError,
	TJS_W("图像读取中发生错误/%1"));

TVP_MSG_DECL(TVPJPEGLoadError,
	TJS_W("JPEG 图像读取中发生错误/%1"));

TVP_MSG_DECL(TVPPNGLoadError,
	TJS_W("PNG 图像读取中发生错误/%1"));

TVP_MSG_DECL(TVPERILoadError,
	TJS_W("ERI 图像读取中发生错误/%1"));

TVP_MSG_DECL(TVPTLGLoadError,
	TJS_W("TLG 图像读取中发生错误/%1"));

TVP_MSG_DECL(TVPInvalidImageSaveType,
	TJS_W("非法的图像存储格式(%1)"));

TVP_MSG_DECL(TVPInvalidOperationFor8BPP,
	TJS_W("执行了对于 8bpp 图像不能执行的操作"));

TVP_MSG_DECL(TVPSpecifyWindow,
	TJS_W("请指定 Window 类型的对象"));

TVP_MSG_DECL(TVPSpecifyLayer,
	TJS_W("请指定 Layer 类型的对象"));

TVP_MSG_DECL(TVPCannotCreateEmptyLayerImage,
	TJS_W("无法将图像的长或者宽指定为 0 以下数值"));

TVP_MSG_DECL(TVPCannotSetPrimaryInvisible,
	TJS_W("主要层无法设置为不可见"));

TVP_MSG_DECL(TVPCannotMovePrimary,
	TJS_W("主要层无法移动"));

TVP_MSG_DECL(TVPCannotSetParentSelf,
	TJS_W("无法成为自身的父对象"));

TVP_MSG_DECL(TVPCannotMoveNextToSelfOrNotSiblings,
	TJS_W("无法移动到自身前后或非父层的前后"));

TVP_MSG_DECL(TVPCannotMovePrimaryOrSiblingless,
	TJS_W("无法前后移动主层或没有兄弟的层"));

TVP_MSG_DECL(TVPCannotMoveToUnderOtherPrimaryLayer,
	TJS_W("无法将层移动到其他的主层下"));

TVP_MSG_DECL(TVPInvalidImagePosition,
	TJS_W("层领域出现了没有图像的领域"));

TVP_MSG_DECL(TVPCannotSetModeToDisabledOrModal,
	TJS_W("无法为已经为模态的或无效/不可见的层设定模式。"));

TVP_MSG_DECL(TVPNotDrawableLayerType,
	TJS_W("本类型的层无法进行描绘、读取图像以及图像的缩放/移动/获取信息。"));

TVP_MSG_DECL(TVPSourceLayerHasNoImage,
	TJS_W("传送源层没有图像"));

TVP_MSG_DECL(TVPUnsupportedLayerType,
	TJS_W("%1 无法在本类型的层中使用"));

TVP_MSG_DECL(TVPNotDrawableFaceType,
	TJS_W("%1 无法在本类型表面上描绘"));

TVP_MSG_DECL(TVPCannotConvertLayerTypeUsingGivenDirection,
	TJS_W("无法进行指定类型的层变换"));

TVP_MSG_DECL(TVPNegativeOpacityNotSupportedOnThisFace,
	TJS_W("本表面无法制定负的透明度"));

TVP_MSG_DECL(TVPSrcRectOutOfBitmap,
	TJS_W("传送源包含位图之外的区域。请指定能够包含正确范围的传送源"));

TVP_MSG_DECL(TVPBoxBlurAreaMustContainCenterPixel,
	TJS_W("矩形模糊的范围必须包含(0,0)。left和right不能同正或同负。（top和bottom也一样）"));

TVP_MSG_DECL(TVPBoxBlurAreaMustBeSmallerThan16Million,
	TJS_W("矩形模糊的范围过大。矩形模糊的范围必须小于1677万像素"));

TVP_MSG_DECL(TVPCannotChangeFocusInProcessingFocus,
	TJS_W("无法在变换焦点时再次进行焦点变换."));

TVP_MSG_DECL(TVPWindowHasNoLayer,
	TJS_W("窗口没有层"));

TVP_MSG_DECL(TVPWindowHasAlreadyPrimaryLayer,
	TJS_W("该窗口已经有了主要层"));

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter,
	TJS_W("事件 %1 需要参数"));

TVP_MSG_DECL(TVPSpecifiedEventNeedsParameter2,
	TJS_W("事件 %1 需要参数 %2 "));

TVP_MSG_DECL(TVPSpecifiedEventNameIsUnknown,
	TJS_W("事件名 %1 为未知事件"));

TVP_MSG_DECL(TVPOutOfRectangle,
	TJS_W("指定到了矩形之外"));

TVP_MSG_DECL(TVPInvalidMethodInUpdating,
	TJS_W("画面更新中无法执行本功能"));

TVP_MSG_DECL(TVPCannotCreateInstance,
	TJS_W("本类无法创建实例"));

TVP_MSG_DECL(TVPUnknownWaveFormat,
	TJS_W("%1 是不支持的声音格式"));

TVP_MSG_DECL(TVPSpecifyMenuItem,
	TJS_W("请指定 MenuItem 类的对象"));

TVP_MSG_DECL(TVPCurrentTransitionMustBeStopping,
	TJS_W("请在现在的渐变停止之后再进行新的渐变。如果对一个层进行多个渐变将会发生本错误。"));

TVP_MSG_DECL(TVPTransHandlerError,
	TJS_W("渐变句柄发生错误 : %1"));

TVP_MSG_DECL(TVPTransAlreadyRegistered,
	TJS_W("渐变类型 %1 已经注册"));

TVP_MSG_DECL(TVPCannotFindTransHander,
	TJS_W("无法找到渐变句柄 %1 "));

TVP_MSG_DECL(TVPSpecifyTransitionSource,
	TJS_W("请指定渐变源"));

TVP_MSG_DECL(TVPLayerCannotHaveImage,
	TJS_W("该层无法拥有图像"));

TVP_MSG_DECL(TVPTransitionSourceAndDestinationMustHaveImage,
	TJS_W("渐变源和渐变对象都必须拥有图像"));

TVP_MSG_DECL(TVPCannotLoadRuleGraphic,
	TJS_W("无法读取规则图像 %1 "));

TVP_MSG_DECL(TVPSpecifyOption,
	TJS_W("请指定选项 %1 "));

TVP_MSG_DECL(TVPTransitionLayerSizeMismatch,
	TJS_W("渐变源(%1)与渐变对象(%2)层大小不同"));

TVP_MSG_DECL(TVPTransitionMutualSource,
	TJS_W("渐变源的渐变源为其自身"));

TVP_MSG_DECL(TVPHoldDestinationAlphaParameterIsNowDeprecated,
	TJS_W("警告 : 方法 %1 的第 %2 个参数所传递的 hda 参数，在吉里吉里２ 2.23 beta 2 以后将被忽略。请使用 Layer.holdAlpha 代替。"));

TVP_MSG_DECL(TVPCannotConnectMultipleWaveSoundBufferAtOnce,
	TJS_W("不能让多个 WaveSoundBuffer 同时使用一个 Filter"));

TVP_MSG_DECL(TVPInvalidWindowSizeMustBeIn64to32768,
	TJS_W("window 必须是 2 的幂，且范围在 64～32768 中"));

TVP_MSG_DECL(TVPInvalidOverlapCountMustBeIn2to32,
	TJS_W("overlap 必须是 2 的幂，且范围在 2～32 中"));

TVP_MSG_DECL(TVPKAGNoLine,
	TJS_W("读取的剧本文件 %1 为空"));

TVP_MSG_DECL(TVPKAGCannotOmmitFirstLabelName,
	TJS_W("剧本文件不能省略最初的段落标签名"));

TVP_MSG_DECL(TVPKAGLabelNotFound,
	TJS_W("剧本文件 %1 内无法找到段落标签 %2 "));

TVP_MSG_DECL(TVPKAGInlineScriptNotEnd,
	TJS_W("未找到[endscript] 或 @endscript "));

TVP_MSG_DECL(TVPKAGSyntaxError,
	TJS_W("KAG标签（Tag）语法错误。请检查'[' 与 ']' 的配对、\" 与 \" 的配对、忘记输入空格、多余的换行、macro……endmacro 的配对、必要的属性是否不足等方面"));

TVP_MSG_DECL(TVPKAGMacroEntityNotAvailable,
	TJS_W("宏实体不能在宏之外使用"));

TVP_MSG_DECL(TVPKAGCallStackUnderflow,
	TJS_W("return 标签没有对应的 call 标签 ( return 标签过多 )"));

TVP_MSG_DECL(TVPKAGReturnLostSync,
	TJS_W("由于剧本文件已经改变，无法找到return所要回到的位置"));

TVP_MSG_DECL(TVPKAGSpecifyKAGParser,
	TJS_W("请指定 KAGParser 类的对象"));

TVP_MSG_DECL(TVPKAGMalformedSaveData,
	TJS_W("存档数据异常。可能存在数据损坏的可能性"));

TVP_MSG_DECL(TVPLabelOrScriptInMacro,
	TJS_W("段落标签和 iscript 不能在宏中使用。"));

TVP_MSG_DECL(TVPUnknownMacroName,
	TJS_W("宏 \"%1\" 未被注册"));

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

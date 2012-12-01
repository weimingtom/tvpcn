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
	TJS_W("脚本中发生例外"));

TVP_MSG_DECL(TVPHardwareExceptionRaised,
	TJS_W("发生了硬件例外"));

TVP_MSG_DECL(TVPMainCDPName,
	TJS_W("脚本编辑器（主要）"));

TVP_MSG_DECL(TVPExceptionCDPName,
	TJS_W("脚本编辑器（例外通知）"));

TVP_MSG_DECL(TVPCannnotLocateUIDLLForFolderSelection,
	TJS_W("在要显示文件夹/文件包选择画面时 ")
		TJS_W("无法找到 krdevui.dll 。\n")
		TJS_W("请将要执行的目录/文件包作为命令行的参数指定"));

TVP_MSG_DECL(TVPInvalidUIDLL,
	TJS_W("krdevui.dll异常或版本错误"));

TVP_MSG_DECL(TVPInvalidBPP,
	TJS_W("无效的色深"));

TVP_MSG_DECL(TVPCannotLoadPlugin,
	TJS_W("无法读取插件 %1 "));

TVP_MSG_DECL(TVPNotValidPlugin,
	TJS_W("%1 不是有效的插件"));

TVP_MSG_DECL(TVPPluginUninitFailed,
	TJS_W("插件释放失败"));

TVP_MSG_DECL(TVPCannnotLinkPluginWhilePluginLinking,
	TJS_W("插件连接中无法连接其他插件"));

TVP_MSG_DECL(TVPNotSusiePlugin,
	TJS_W("异常的 Susie 插件"));

TVP_MSG_DECL(TVPSusiePluginError,
	TJS_W("Susie 中发生错误/错误代码 %1"));

TVP_MSG_DECL(TVPCannotReleasePlugin,
	TJS_W("指定的插件在使用中，无法释放"));

TVP_MSG_DECL(TVPNotLoadedPlugin,
	TJS_W("%1 未被读取"));

TVP_MSG_DECL(TVPCannotAllocateBitmapBits,
	TJS_W("无法分配位图用内存/%1(size=%2)"));

TVP_MSG_DECL(TVPScanLineRangeOver,
	TJS_W("扫描线 %1 超过范围(0—%2)"));

TVP_MSG_DECL(TVPPluginError,
	TJS_W("插件发生了错误/%1"));

TVP_MSG_DECL(TVPInvalidCDDADrive,
	TJS_W("无法从指定的驱动器播放CDDA"));

TVP_MSG_DECL(TVPCDDADriveNotFound,
	TJS_W("未找到可播放 CD-DA 的CD驱动器"));

TVP_MSG_DECL(TVPMCIError,
	TJS_W("发生 MCI 错误 : %1"));

TVP_MSG_DECL(TVPInvalidSMF,
	TJS_W("不是有效的 SMF 文件 : %1"));

TVP_MSG_DECL(TVPMalformedMIDIMessage,
	TJS_W("指定的信息不是格式有效的MIDI信息"));

TVP_MSG_DECL(TVPCannotInitDirectSound,
	TJS_W("DirectSound 无法初始化 : %1"));

TVP_MSG_DECL(TVPCannotCreateDSSecondaryBuffer,
	TJS_W("DirectSound 无法创建第二缓存区 : %1/%2"));

TVP_MSG_DECL(TVPInvalidLoopInformation,
	TJS_W("环回信息 %1 异常"));

TVP_MSG_DECL(TVPNotChildMenuItem,
	TJS_W("指定的目录项目不是该目录项目的子项目"));

TVP_MSG_DECL(TVPCannotInitDirectDraw,
	TJS_W("DirectDraw 无法初始化 : %1"));

TVP_MSG_DECL(TVPCannotFindDisplayMode,
	TJS_W("无法找到适合的画面模式 : %1"));

TVP_MSG_DECL(TVPCannotSwitchToFullScreen,
	TJS_W("无法切换到全屏 : %1"));

TVP_MSG_DECL(TVPInvalidPropertyInFullScreen,
	TJS_W("设定了全屏模式无法修改的属性"));

TVP_MSG_DECL(TVPInvalidMethodInFullScreen,
	TJS_W("调用了全屏模式下无法操作的方法"));

TVP_MSG_DECL(TVPCannotLoadCursor,
	TJS_W("读取鼠标指针 %1 失败"));

TVP_MSG_DECL(TVPCannotLoadKrMovieDLL,
	TJS_W("为了播放视频/Shockwave Flash 需要 krmovie.dll / krflash.dll ")
		TJS_W("文件的支持。无法读取到需要的文件。"));

TVP_MSG_DECL(TVPInvalidKrMovieDLL,
	TJS_W("krmovie.dll/krflash.dll 为异常或不支持的版本"));

TVP_MSG_DECL(TVPErrorInKrMovieDLL,
	TJS_W("krmovie.dll/krflash.dll 内发生了错误/%1"));

TVP_MSG_DECL(TVPWindowAlreadyMissing,
	TJS_W("Window并不存在"));

TVP_MSG_DECL(TVPPrerenderedFontMappingFailed,
	TJS_W("已渲染字体的映射失败 : %1"));

TVP_MSG_DECL_CONST(TVPConfigFailOriginalFileCannotBeRewritten,
	TJS_W("%1 无法被写入。可能该文件正在被使用，或没有写入权限"));

TVP_MSG_DECL(TVPConfigFailTempExeNotErased,
	TJS_W("无法确认 %1 是否退出，因而无法删除。(可以删除本文件)"));

TVP_MSG_DECL_CONST(TVPExecutionFail,
	TJS_W("%1 无法执行"));

TVP_MSG_DECL(TVPPluginUnboundFunctionError,
	TJS_W("插件要求函数 %1 ，但该函数在本体内部存在。")
			TJS_W("请确认插件与本体的版本是否对应。"));

TVP_MSG_DECL(TVPExceptionHadBeenOccured,
	TJS_W(" = (例外发生)"));

TVP_MSG_DECL(TVPConsoleResult,
	TJS_W("控制台 : "));


//---------------------------------------------------------------------------



#endif

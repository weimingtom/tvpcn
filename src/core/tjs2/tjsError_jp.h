//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Japanese localized messages
//---------------------------------------------------------------------------

TJS_MSG_DECL(TJSInternalError, TJS_W("发生了内部错误"))
TJS_MSG_DECL(TJSWarning, TJS_W("警告: "))
TJS_MSG_DECL(TJSWarnEvalOperator, TJS_W("在非全局环境下使用了后置!运算符。(请注意该运算符的行为在 TJS2 version 2.4.1 已经变化)"))
TJS_MSG_DECL(TJSNarrowToWideConversionError, TJS_W("无法转换ANSI字符串到Unicode字符串。数据中含有现在的代码页无法解释的文字。请确认是否制定了正确的数据，存在数据已经破损的可能性。"))
TJS_MSG_DECL(TJSVariantConvertError, TJS_W("无法将 %1 类型变换为 %2 。"))
TJS_MSG_DECL(TJSVariantConvertErrorToObject, TJS_W("无法将 %1 类型变换为 Object 。对于要求 Object 型的参数传递了 Object 型以外的值将会产生此错误"))
TJS_MSG_DECL(TJSIDExpected, TJS_W("请指定识别子"))
TJS_MSG_DECL(TJSSubstitutionInBooleanContext, TJS_W("在要求取得逻辑值的地方使用了 = 运算符(是否是 == 运算符写错了？类似于在赋值之后要和0比较的场合，推荐使用(A=B) != 0 这样的形势)"));
TJS_MSG_DECL(TJSCannotModifyLHS, TJS_W("非法的赋值或非法的表达式操作"))
TJS_MSG_DECL(TJSInsufficientMem, TJS_W("内存不足"))
TJS_MSG_DECL(TJSCannotGetResult, TJS_W("无法从该表达式获得结果"))
TJS_MSG_DECL(TJSNullAccess, TJS_W("访问了 null 对象"))
TJS_MSG_DECL(TJSMemberNotFound, TJS_W("无法找到成员 \"%1\" "))
TJS_MSG_DECL(TJSMemberNotFoundNoNameGiven, TJS_W("无法找到成员"))
TJS_MSG_DECL(TJSNotImplemented, TJS_W("现在调用的机能尚未实现"))
TJS_MSG_DECL(TJSInvalidParam, TJS_W("非法的参数"))
TJS_MSG_DECL(TJSBadParamCount, TJS_W("参数数量错误"))
TJS_MSG_DECL(TJSInvalidType, TJS_W("参数种类错误或调用对象不是函数"))
TJS_MSG_DECL(TJSSpecifyDicOrArray, TJS_W("请指定 Dictionary 或 Array 类的对象"))
TJS_MSG_DECL(TJSSpecifyArray, TJS_W("请指定 Array 类的对象"))
TJS_MSG_DECL(TJSStringDeallocError, TJS_W("无法释放字符串内存"))
TJS_MSG_DECL(TJSStringAllocError, TJS_W("无法分配字符串内存"))
TJS_MSG_DECL(TJSMisplacedBreakContinue, TJS_W("\"break\" 或 \"continue\" 不能在这里存在"))
TJS_MSG_DECL(TJSMisplacedCase, TJS_W("\"case\" 不能在这里存在"))
TJS_MSG_DECL(TJSMisplacedReturn, TJS_W("\"return\" 不能在这里存在"))
TJS_MSG_DECL(TJSStringParseError, TJS_W("字符串常数/正则表达式/八进制值未完结时到达了脚本终端"))
TJS_MSG_DECL(TJSNumberError, TJS_W("无法解释为数值"))
TJS_MSG_DECL(TJSUnclosedComment, TJS_W("主食未完结时到达了脚本终端"))
TJS_MSG_DECL(TJSInvalidChar, TJS_W("非法文字 : \'%1\'"))
TJS_MSG_DECL(TJSExpected, TJS_W("不是 %1 "))
TJS_MSG_DECL(TJSSyntaxError, TJS_W("语法错误(%1)"))
TJS_MSG_DECL(TJSPPError, TJS_W("条件编译式中发生错误"))
TJS_MSG_DECL(TJSCannotGetSuper, TJS_W("不存在超类或超类无法特定"))
TJS_MSG_DECL(TJSInvalidOpecode, TJS_W("非法VM代码"))
TJS_MSG_DECL(TJSRangeError, TJS_W("数值超界"))
TJS_MSG_DECL(TJSAccessDenyed, TJS_W("对读取/写入专用的变量执行了不支持的操作"))
TJS_MSG_DECL(TJSNativeClassCrash, TJS_W("执行上下文错误"))
TJS_MSG_DECL(TJSInvalidObject, TJS_W("对象已经被删除"))
TJS_MSG_DECL(TJSCannotOmit, TJS_W("\"...\" 不能在函数外使用"))
TJS_MSG_DECL(TJSCannotParseDate, TJS_W("非法的日期格式"))
TJS_MSG_DECL(TJSInvalidValueForTimestamp, TJS_W("非法的日期、时间格式"))
TJS_MSG_DECL(TJSExceptionNotFound, TJS_W("由于 \"Exception\" 不存在所以无法创建例外对象"))
TJS_MSG_DECL(TJSInvalidFormatString, TJS_W("非法的格式字符串"))
TJS_MSG_DECL(TJSDivideByZero, TJS_W("除以0错误"))
TJS_MSG_DECL(TJSNotReconstructiveRandomizeData, TJS_W("随机数函数初始化失败(可能是传入了非法的参数)"))
TJS_MSG_DECL(TJSSymbol, TJS_W("ID"))
TJS_MSG_DECL(TJSCallHistoryIsFromOutOfTJS2Script, TJS_W("[TJS脚本管理外]"))
TJS_MSG_DECL(TJSNObjectsWasNotFreed, TJS_W("合计 %1 个的对象未被释放"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_W("\r\n                     "))
#else
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_W("\n                     "))
#endif
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectWasNotFreed,
				 TJS_W("对象 %1 [%2] 未被释放。对象创建时的调用纪录为：\r\n                     %3"))
#else
TJS_MSG_DECL(TJSObjectWasNotFreed,
				 TJS_W("对象 %1 [%2] 未被释放。对象创建时的调用纪录为：\n                     %3"))
#endif
TJS_MSG_DECL(TJSGroupByObjectTypeAndHistory, TJS_W("按照对象类型和对象创建时的记录分类"))
TJS_MSG_DECL(TJSGroupByObjectType, TJS_W("按照对象类型分类"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory,
				 TJS_W("%1 个 : [%2]\r\n                     %3"))
#else
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory,
				 TJS_W("%1 个 : [%2]\n                     %3"))
#endif
TJS_MSG_DECL(TJSObjectCountingMessageTJSGroupByObjectType, TJS_W("%1 個 : [%2]"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_W("%4: 在删除中的对象 %1[%2] 上执行了指令。对象创建时的调用纪录为：\r\n                     %3"))
#else
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_W("%4: 在删除中的对象 %1[%2] 上执行了指令。对象创建时的调用纪录为：\n                     %3"))
#endif
TJS_MSG_DECL(TJSWriteError, TJS_W("发生写入错误"))
TJS_MSG_DECL(TJSReadError, TJS_W("发生读取错误。存在文件损坏或设备读取失败的可能性。"))
TJS_MSG_DECL(TJSSeekError, TJS_W("发生文件寻址错误。存在文件损坏或设备读取失败的可能性。"))
//---------------------------------------------------------------------------

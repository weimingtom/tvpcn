//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Japanese localized messages
//---------------------------------------------------------------------------

TJS_MSG_DECL(TJSInternalError, TJS_W("�������ڲ�����"))
TJS_MSG_DECL(TJSWarning, TJS_W("����: "))
TJS_MSG_DECL(TJSWarnEvalOperator, TJS_W("�ڷ�ȫ�ֻ�����ʹ���˺���!�������(��ע������������Ϊ�� TJS2 version 2.4.1 �Ѿ��仯)"))
TJS_MSG_DECL(TJSNarrowToWideConversionError, TJS_W("�޷�ת��ANSI�ַ�����Unicode�ַ����������к������ڵĴ���ҳ�޷����͵����֡���ȷ���Ƿ��ƶ�����ȷ�����ݣ����������Ѿ�����Ŀ����ԡ�"))
TJS_MSG_DECL(TJSVariantConvertError, TJS_W("�޷��� %1 ���ͱ任Ϊ %2 ��"))
TJS_MSG_DECL(TJSVariantConvertErrorToObject, TJS_W("�޷��� %1 ���ͱ任Ϊ Object ������Ҫ�� Object �͵Ĳ��������� Object �������ֵ��������˴���"))
TJS_MSG_DECL(TJSIDExpected, TJS_W("��ָ��ʶ����"))
TJS_MSG_DECL(TJSSubstitutionInBooleanContext, TJS_W("��Ҫ��ȡ���߼�ֵ�ĵط�ʹ���� = �����(�Ƿ��� == �����д���ˣ��������ڸ�ֵ֮��Ҫ��0�Ƚϵĳ��ϣ��Ƽ�ʹ��(A=B) != 0 ����������)"));
TJS_MSG_DECL(TJSCannotModifyLHS, TJS_W("�Ƿ��ĸ�ֵ��Ƿ��ı��ʽ����"))
TJS_MSG_DECL(TJSInsufficientMem, TJS_W("�ڴ治��"))
TJS_MSG_DECL(TJSCannotGetResult, TJS_W("�޷��Ӹñ��ʽ��ý��"))
TJS_MSG_DECL(TJSNullAccess, TJS_W("������ null ����"))
TJS_MSG_DECL(TJSMemberNotFound, TJS_W("�޷��ҵ���Ա \"%1\" "))
TJS_MSG_DECL(TJSMemberNotFoundNoNameGiven, TJS_W("�޷��ҵ���Ա"))
TJS_MSG_DECL(TJSNotImplemented, TJS_W("���ڵ��õĻ�����δʵ��"))
TJS_MSG_DECL(TJSInvalidParam, TJS_W("�Ƿ��Ĳ���"))
TJS_MSG_DECL(TJSBadParamCount, TJS_W("������������"))
TJS_MSG_DECL(TJSInvalidType, TJS_W("��������������ö����Ǻ���"))
TJS_MSG_DECL(TJSSpecifyDicOrArray, TJS_W("��ָ�� Dictionary �� Array ��Ķ���"))
TJS_MSG_DECL(TJSSpecifyArray, TJS_W("��ָ�� Array ��Ķ���"))
TJS_MSG_DECL(TJSStringDeallocError, TJS_W("�޷��ͷ��ַ����ڴ�"))
TJS_MSG_DECL(TJSStringAllocError, TJS_W("�޷������ַ����ڴ�"))
TJS_MSG_DECL(TJSMisplacedBreakContinue, TJS_W("\"break\" �� \"continue\" �������������"))
TJS_MSG_DECL(TJSMisplacedCase, TJS_W("\"case\" �������������"))
TJS_MSG_DECL(TJSMisplacedReturn, TJS_W("\"return\" �������������"))
TJS_MSG_DECL(TJSStringParseError, TJS_W("�ַ�������/������ʽ/�˽���ֵδ���ʱ�����˽ű��ն�"))
TJS_MSG_DECL(TJSNumberError, TJS_W("�޷�����Ϊ��ֵ"))
TJS_MSG_DECL(TJSUnclosedComment, TJS_W("��ʳδ���ʱ�����˽ű��ն�"))
TJS_MSG_DECL(TJSInvalidChar, TJS_W("�Ƿ����� : \'%1\'"))
TJS_MSG_DECL(TJSExpected, TJS_W("���� %1 "))
TJS_MSG_DECL(TJSSyntaxError, TJS_W("�﷨����(%1)"))
TJS_MSG_DECL(TJSPPError, TJS_W("��������ʽ�з�������"))
TJS_MSG_DECL(TJSCannotGetSuper, TJS_W("�����ڳ�������޷��ض�"))
TJS_MSG_DECL(TJSInvalidOpecode, TJS_W("�Ƿ�VM����"))
TJS_MSG_DECL(TJSRangeError, TJS_W("��ֵ����"))
TJS_MSG_DECL(TJSAccessDenyed, TJS_W("�Զ�ȡ/д��ר�õı���ִ���˲�֧�ֵĲ���"))
TJS_MSG_DECL(TJSNativeClassCrash, TJS_W("ִ�������Ĵ���"))
TJS_MSG_DECL(TJSInvalidObject, TJS_W("�����Ѿ���ɾ��"))
TJS_MSG_DECL(TJSCannotOmit, TJS_W("\"...\" �����ں�����ʹ��"))
TJS_MSG_DECL(TJSCannotParseDate, TJS_W("�Ƿ������ڸ�ʽ"))
TJS_MSG_DECL(TJSInvalidValueForTimestamp, TJS_W("�Ƿ������ڡ�ʱ���ʽ"))
TJS_MSG_DECL(TJSExceptionNotFound, TJS_W("���� \"Exception\" �����������޷������������"))
TJS_MSG_DECL(TJSInvalidFormatString, TJS_W("�Ƿ��ĸ�ʽ�ַ���"))
TJS_MSG_DECL(TJSDivideByZero, TJS_W("����0����"))
TJS_MSG_DECL(TJSNotReconstructiveRandomizeData, TJS_W("�����������ʼ��ʧ��(�����Ǵ����˷Ƿ��Ĳ���)"))
TJS_MSG_DECL(TJSSymbol, TJS_W("ID"))
TJS_MSG_DECL(TJSCallHistoryIsFromOutOfTJS2Script, TJS_W("[TJS�ű�������]"))
TJS_MSG_DECL(TJSNObjectsWasNotFreed, TJS_W("�ϼ� %1 ���Ķ���δ���ͷ�"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_W("\r\n                     "))
#else
TJS_MSG_DECL(TJSObjectCreationHistoryDelimiter, TJS_W("\n                     "))
#endif
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectWasNotFreed,
				 TJS_W("���� %1 [%2] δ���ͷš����󴴽�ʱ�ĵ��ü�¼Ϊ��\r\n                     %3"))
#else
TJS_MSG_DECL(TJSObjectWasNotFreed,
				 TJS_W("���� %1 [%2] δ���ͷš����󴴽�ʱ�ĵ��ü�¼Ϊ��\n                     %3"))
#endif
TJS_MSG_DECL(TJSGroupByObjectTypeAndHistory, TJS_W("���ն������ͺͶ��󴴽�ʱ�ļ�¼����"))
TJS_MSG_DECL(TJSGroupByObjectType, TJS_W("���ն������ͷ���"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory,
				 TJS_W("%1 �� : [%2]\r\n                     %3"))
#else
TJS_MSG_DECL(TJSObjectCountingMessageGroupByObjectTypeAndHistory,
				 TJS_W("%1 �� : [%2]\n                     %3"))
#endif
TJS_MSG_DECL(TJSObjectCountingMessageTJSGroupByObjectType, TJS_W("%1 �� : [%2]"))
#ifdef TJS_TEXT_OUT_CRLF
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_W("%4: ��ɾ���еĶ��� %1[%2] ��ִ����ָ����󴴽�ʱ�ĵ��ü�¼Ϊ��\r\n                     %3"))
#else
TJS_MSG_DECL(TJSWarnRunningCodeOnDeletingObject, TJS_W("%4: ��ɾ���еĶ��� %1[%2] ��ִ����ָ����󴴽�ʱ�ĵ��ü�¼Ϊ��\n                     %3"))
#endif
TJS_MSG_DECL(TJSWriteError, TJS_W("����д�����"))
TJS_MSG_DECL(TJSReadError, TJS_W("������ȡ���󡣴����ļ��𻵻��豸��ȡʧ�ܵĿ����ԡ�"))
TJS_MSG_DECL(TJSSeekError, TJS_W("�����ļ�Ѱַ���󡣴����ļ��𻵻��豸��ȡʧ�ܵĿ����ԡ�"))
//---------------------------------------------------------------------------

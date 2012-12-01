//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
//---------------------------------------------------------------------------


/*
  �v���O�C���ł̗�O����

  �v���O�C���� RTL (Run Time Library) �Ƌg���g���{�̂� RTL �͈قȂ邽�߁A����
  ��O�������ꍇ�͒����Ȃ���΂Ȃ�Ȃ��_�����_������܂��B

  �v���O�C���ŗ�O���đ�����ꍇ�́A��O���z�����Ă��܂�Ȃ�������͂����
  ����B
  �܂�A�ȉ��̂悤�ȃR�[�h�̓_���ł� (���������[�N�������N�����܂�)�B

	try {
		// ��O�𔭐�����悤�ȏ���
		...
	}
	catch(...) {
		// throw; ���Ȃ�
	}

  ��O���đ�����͖̂�肠��܂���B

	try {
		// ��O�𔭐�����悤�ȏ���
		...
	}
	catch(...) {
		// ��O���đ�
		throw;
	}

  �ȉ��̂悤�ɁA��O�̌^�𔻒f���邱�Ƃ͂ł��܂���B

	try {
		// ��O�𔭐�����悤�ȏ���
		...
	}
	catch(const eTJS &e) { // �������� eTJS �� tp_stub �ɖ����̂ŃG���[
		...
	}

  �v���O�C������{�̂ɗ�O�𓊂���ɂ� TVPThrowExceptionMessage ���g�p����
  ���Ƃ��ł��܂��B

	TVPThrowExceptionMessage(TJS_W("��������w�肵�Ă�������"));

  �v���O�C���Ŗ{�̗̂�O��ߑ����A���b�Z�[�W���擾�������ꍇ�� TVPDoTryBlock
  ���g�p���܂��B�{�̂Ŕ���������O���A�v���O�C�����ŕߑ����ċz��������A
  ��O�̃��b�Z�[�W���擾����ɂ͂��̕��@���g�p���邵������܂���B

  ���̃v���O�C���ł́A���� TVPDoTryBlock �̎g�p���@��������܂��B
*/



//---------------------------------------------------------------------------
// �e�X�g�p�R�[���o�b�N�֐�
//---------------------------------------------------------------------------
static void TJS_USERENTRY TryBlock1(void * data)
{
	// try �u���b�N�ł��B
	// ���̊֐����ŗ�O�����������ꍇ�́Acatchblock ���Ăяo����܂��B
	// data �ɂ́ATVPDoTryBlock �Ŏw�肵�� data �����̒l���n����܂��B

	// ���炩�ɗ�O�𔭐�������X�N���v�g�����s
	TVPExecuteScript(TJS_W("var a = 3, b = 0; var c = a \\ b;"), NULL);
}
//---------------------------------------------------------------------------
static bool TJS_USERENTRY CatchBlock1(void * data, const tTVPExceptionDesc & desc)
{
	// catch �u���b�N�ł��B
	// tryblock ���ŗ�O�����������ꍇ�͂��̊֐����Ăяo����܂��B
	// desc.type �� ��O�̌^��\���ATJS �R���̗�O�̏ꍇ�� "eTJS" �A����ȊO�̏ꍇ��
	// "unknown" �ɂȂ��Ă��܂��B
	// desc.message �́A��O�̃��b�Z�[�W�ł��B���b�Z�[�W�������ꍇ�͋󕶎���ƂȂ��Ă��܂��B
	// ���̊֐��� true ��Ԃ��ƁA��O�͍đ�����܂��B
	// false ��Ԃ��ƁA��O�͋z������܂��B
	// data �ɂ́ATVPDoTryBlock �Ŏw�肵�� data �����̒l���n����܂��B

	TVPAddLog(TJS_W("exception type:") + desc.type + TJS_W(", message:") + desc.message);
	return *(bool*)data;
}
//---------------------------------------------------------------------------
static void TJS_USERENTRY FinallyBlock1(void * data)
{
	// finally �u���b�N�ł��B
	// try �u���b�N���ŗ�O���������Ă����Ȃ��Ă��A�ŏI�I�ɂ͎��s�����֐��ł��B
	// data �ɂ́ATVPDoTryBlock �Ŏw�肵�� data �����̒l���n����܂��B
	TVPAddLog(TJS_W("finally executed."));
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// �e�X�g�֐�
//---------------------------------------------------------------------------
class tTestExceptionFunction1 : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * TestExceptionFunction1;
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTestExceptionFunction1::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	// �ŏ��̈��� = ��O���đ����邩
	bool rethrow = false;
	if(numparams >= 1)
		rethrow = (bool)(tjs_int)*param[0];

	TVPDoTryBlock(TryBlock1, CatchBlock1, FinallyBlock1, (void *)&rethrow);
	/*
		TVPDoTryBlock �ɂ́A�S�̈�����n���܂��B
		�������� tryblock �֐��ւ̃|�C���^�A�������� catchblock �֐��ւ�
		�|�C���^�A��O������ finallyblock �֐��ւ̃|�C���^�ł��B
		��l�����́A�e�֐��֓n����� data �����̒l�ł��B
		�e�֐��̐����͏�L�̊e�֐����������������B
	*/

	return TJS_S_OK;
}
//---------------------------------------------------------------------------



/*
 �Q�l

 ���ۂ� TVPDoTryBlock �͈ȉ��̂悤�ɂȂ��Ă��܂��B

void TVPDoTryBlock(
	tTVPTryBlockFunction tryblock,
	tTVPCatchBlockFunction catchblock,
	tTVPFinallyBlockFunction finallyblock,
	void *data)
{
	try
	{
		tryblock(data);
	}
	catch(const eTJS & e)
	{
		if(finallyblock) finallyblock(data);
		tTVPExceptionDesc desc;
		desc.type = TJS_W("eTJS");
		desc.message = e.GetMessage();
		if(catchblock(data, desc)) throw;
		return;
	}
	catch(...)
	{
		if(finallyblock) finallyblock(data);
		tTVPExceptionDesc desc;
		desc.type = TJS_W("unknown");
		if(catchblock(data, desc)) throw;
		return;
	}
	if(finallyblock) finallyblock(data);
}
*/






//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// AverageFunction �̍쐬�Ɠo�^
	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();



	//-----------------------------------------------------------------------
	// 1 �܂��I�u�W�F�N�g���쐬
	TestExceptionFunction1 = new tTestExceptionFunction1();

	// 2 AverageFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(TestExceptionFunction1);

	// 3 ���ł� val �� AverageFunction ��ێ����Ă���̂ŁAAverageFunction ��
	//   Release ����
	TestExceptionFunction1->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("testException1"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);
	//-----------------------------------------------------------------------




	// - global �� Release ����
	global->Release();

	// �����A�o�^����֐�����������ꍇ�� 1 �` 4 ���J��Ԃ�


	// val ���N���A����B
	// ����͕K���s���B�������Ȃ��� val ���ێ����Ă���I�u�W�F�N�g
	// �� Release ���ꂸ�A���Ɏg�� TVPPluginGlobalRefCount �����m�ɂȂ�Ȃ��B
	val.Clear();


	// ���̎��_�ł� TVPPluginGlobalRefCount �̒l��
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// �Ƃ��čT���Ă����BTVPPluginGlobalRefCount �͂��̃v���O�C������
	// �Ǘ�����Ă��� tTJSDispatch �h���I�u�W�F�N�g�̎Q�ƃJ�E���^�̑��v�ŁA
	// ������ɂ͂���Ɠ������A����������Ȃ��Ȃ��ĂȂ��ƂȂ�Ȃ��B
	// �����Ȃ��ĂȂ���΁A�ǂ����ʂ̂Ƃ���Ŋ֐��Ȃǂ��Q�Ƃ���Ă��āA
	// �v���O�C���͉���ł��Ȃ��ƌ������ƂɂȂ�B

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	// �g���g��������A�v���O�C����������悤�Ƃ���Ƃ��ɌĂ΂��֐��B

	// �������炩�̏����Ńv���O�C��������ł��Ȃ��ꍇ��
	// ���̎��_�� E_FAIL ��Ԃ��悤�ɂ���B
	// �����ł́ATVPPluginGlobalRefCount �� GlobalRefCountAtInit ����
	// �傫���Ȃ��Ă���Ύ��s�Ƃ������Ƃɂ���B
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL ���A��ƁAPlugins.unlink ���\�b�h�͋U��Ԃ�

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� average �֐��Ȃǂ��폜����

	// - �܂��ATJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// - global �� DeleteMember ���\�b�h��p���A�I�u�W�F�N�g���폜����
	if(global)
	{
		// TJS ���̂����ɉ������Ă����Ƃ��Ȃǂ�
		// global �� NULL �ɂȂ蓾��̂� global �� NULL �łȂ�
		// ���Ƃ��`�F�b�N����

		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			TJS_W("testException1"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
	}

	// - global �� Release ����
	if(global) global->Release();

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------

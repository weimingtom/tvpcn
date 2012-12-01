//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
//---------------------------------------------------------------------------

/*
	Window.registerMessageReceiver �Ŏ擾�ł����E�B���h�E���b�Z�[�W��
	�R���\�[���ɐ��ꗬ���e�X�g�v���O�����B
*/


/*
	�g���g���� Window �N���X�Ő��������E�B���h�E�� HWND �v���p�e�B��
	���̃E�B���h�E�n���h���𓾂邱�Ƃ��ł��邪�A���������̓s����A
	���̃E�B���h�E�n���h���͂������̃v���p�e�B�̕ύX�⃁�\�b�h�̎��s��
	��蒼�����\��������B
	��������m���邽�߂� Window.registerMessageReceiver �Ń��b�Z�[�W��M�֐���
	�o�^���A TVP_WM_DETACH �� TVP_WM_ATTACH ���Ď����邱�Ƃ��ł���B
*/


//---------------------------------------------------------------------------
// ���b�Z�[�W��M�֐�
//---------------------------------------------------------------------------
static bool __stdcall MyReceiver
	(void *userdata, tTVPWindowMessage *Message)
{
	// ���̊֐��ɂ�(����������Ȃ��Ɋւ�炸)���ɑ�ʂ̃��b�Z�[�W���ʉ߂��邽��
	// �֌W�Ȃ����b�Z�[�W�ɂ��Ă̓p�t�H�[�}���X�����܂�ቺ�����Ȃ��悤�Ȏ������]�܂��

	switch(Message->Msg)
	{
	case TVP_WM_DETACH:
		// �E�B���h�E�n���h������蒼����邩�A���邢��
		// �E�B���h�E���j������钼�O�ɂ��̃��b�Z�[�W������B
		// �g���g���� Window �N���X�̃E�B���h�E�̎q�E�B���h�E�Ƃ���
		// �E�B���h�E��\��t�����ꍇ�͂��̃^�C�~���O�ŃE�B���h�E��
		// ���O���Ȃ���΂Ȃ�Ȃ��B
		// WPARAM �� 1 �Ȃ�΃E�B���h�E���j�������Ƃ��A0 �Ȃ��
		// �E�B���h�E����蒼�����Ƃ��ł���B
		// TVP_WM_DETACH �� TVP_WM_ATTACH �̊Ԃɂ�
		// ���b�Z�[�W���������邱�Ƃ����邪�������ׂ��B
		if(Message->WParam == 1)
			TVPAddLog(TJS_W("TVP_WM_DETACH: closing"));
		else if(Message->WParam == 0)
			TVPAddLog(TJS_W("TVP_WM_DETACH: rebuilding"));
		break;

	case TVP_WM_ATTACH:
		// �E�B���h�E����蒼���ꂽ��ɂ��̃��b�Z�[�W������B
		// LPARAM �ɂ� �V�����E�B���h�E�n���h���������Ă���B
		// �q�E�B���h�E���܂����t����ꍇ�͂��̃^�C�~���O�ł܂����t����
		// ���Ƃ��ł���B
		TVPAddLog(TJS_W("TVP_WM_ATTACH: new window handle ") +
			TJSInt32ToHex(Message->LParam, 8));
		break;

	default:
		// ���̑��̃��b�Z�[�W�B
		TVPAddLog(TJS_W("Message: msg=") + TJSInt32ToHex(Message->Msg, 8) +
			TJS_W(", WParam=") + TJSInt32ToHex(Message->WParam, 8) +
			TJS_W(", LParam=") + TJSInt32ToHex(Message->LParam, 8));
		break;


	}


	return false;
		/* true ��Ԃ��� �g���g���̃E�B���h�E�͂��̃��b�Z�[�W�Ɋ֒m���Ȃ��Ȃ�B
		   TVP_WM_DETACH �� TVP_WM_ATTACH �ւ̉����Ɋւ��Ă͖߂�l�͖�������� */
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// �J�n�֐�
//---------------------------------------------------------------------------
class tWMRDumpStartFunction : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if(numparams < 1) return TJS_E_BADPARAMCOUNT;

		// *param[0] �� Window �N���X�̃I�u�W�F�N�g�ł���K�v������
		iTJSDispatch2 *obj = param[0]->AsObjectNoAddRef();
			// param[0] ���I�u�W�F�N�g������ addref ���Ă���̂�
			// AsObject �ł͂Ȃ� AsObjectNoAddRef ��p����
			// ( AsObjectNoAddRef �͎Q�ƃJ�E���^��������Ȃ��̂�
			//  �I�u�W�F�N�g���g���I����Ă� Release ����K�v�͂Ȃ���
			//  �����Ȃ��Ƃ��u�I�u�W�F�N�g��K�v�Ƃ��Ă�����Ԓ��́v
			//  �I�u�W�F�N�g�����݂������邱�Ƃ��m���ȏꍇ�݂̂Ɏg�p���ׂ� )

		// registerMessageReceiver ���Ă�
		tTJSVariant mode, proc, userdata;
		tTJSVariant *p[3] = {&mode, &proc, &userdata};
		mode = (tTVInteger)(tjs_int)wrmRegister;
		proc = (tTVInteger)reinterpret_cast<tjs_int>(MyReceiver);
		userdata = (tTVInteger)(tjs_int)0;
		obj->FuncCall(0, TJS_W("registerMessageReceiver"), NULL,
			NULL, 3, p, obj);

		// �E�B���h�E�n���h�����擾���Ă݂��肷��
		tTJSVariant val;
		HWND hwnd;
		obj->PropGet(0, TJS_W("HWND"), NULL, &val, obj);
		hwnd = reinterpret_cast<HWND>((tjs_int)(val));

		TVPAddLog(TJS_W("Window handle ") +
			TJSInt32ToHex(reinterpret_cast<tjs_int>(hwnd), 8));

		return TJS_S_OK;
	}
} * WMRDumpStartFunction;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// �I���֐�
//---------------------------------------------------------------------------
class tWMRDumpStopFunction : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if(numparams < 1) return TJS_E_BADPARAMCOUNT;

		// *param[0] �� Window �N���X�̃I�u�W�F�N�g�ł���K�v������
		iTJSDispatch2 *obj = param[0]->AsObjectNoAddRef();

		// registerMessageReceiver ���Ă�
		tTJSVariant mode, proc, userdata;
		tTJSVariant *p[3] = {&mode, &proc, &userdata};
		mode = (tTVInteger)(tjs_int)wrmUnregister;
		proc = (tTVInteger)reinterpret_cast<tjs_int>(MyReceiver);
		userdata = (tTVInteger)(tjs_int)0;
		obj->FuncCall(0, TJS_W("registerMessageReceiver"), NULL,
			NULL, 3, p, obj);

		return TJS_S_OK;
	}
} * WMRDumpStopFunction;
//---------------------------------------------------------------------------








//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// WMRDumpStartFunction, WMRDumpStopFunction �̍쐬�Ɠo�^
	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// 1 �܂��I�u�W�F�N�g���쐬
	WMRDumpStartFunction = new tWMRDumpStartFunction();

	// 2 TestFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(WMRDumpStartFunction);

	// 3 ���ł� val �� TestFunction ��ێ����Ă���̂ŁAWMRDumpStartFunction ��
	//   Release ����
	WMRDumpStartFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("wmrStartDump"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);

	// 1 �܂��I�u�W�F�N�g���쐬
	WMRDumpStopFunction = new tWMRDumpStopFunction();

	// 2 TestFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(WMRDumpStopFunction);

	// 3 ���ł� val �� TestFunction ��ێ����Ă���̂ŁAWMRDumpStopFunction ��
	//   Release ����
	WMRDumpStopFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("wmrStopDump"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);


	// - global �� Release ����
	global->Release();

	// val ���N���A����B
	// ����͕K���s���B�������Ȃ��� val ���ێ����Ă���I�u�W�F�N�g
	// �� Release ���ꂸ�A���Ɏg�� TVPPluginGlobalRefCount �����m�ɂȂ�Ȃ��B
	val.Clear();


	// ���̎��_�ł� TVPPluginGlobalRefCount �̒l��
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	TVPAddLog(TVPPluginGlobalRefCount);
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
	TVPAddLog(TVPPluginGlobalRefCount);
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL ���A��ƁAPlugins.unlink ���\�b�h�͋U��Ԃ�

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^�����֐����폜����

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
			TJS_W("wmrStartDump"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			TJS_W("wmrStopDump"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
			// �o�^�����֐�����������ꍇ�� ������J��Ԃ�
	}

	// - global �� Release ����
	if(global) global->Release();

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------


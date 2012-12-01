//---------------------------------------------------------------------------
/*
	���̃\�[�X�́A�g���g���Ŏg�p�\�Ȋ֐��𑝂₷�v���O�C���̃e�X�g�����˂�
	�v���O�C���̍����̐����B
	���̃\�[�X�t�@�C���̍\���͈ȉ��̒ʂ�B

	�ETJS����g�p�\�ɂ��邽�߂̊֐��Q
	  �֐��Q�� tTJSDispatch �N���X�� FuncCall ���I�[�o�[���C�h���邱�ƂŎ���
	  ����B�����ł͂S�̊֐��A�܂�S�̃N���X���������Ă���B

	  tAverageFunction - �����̕��ς����߂�
	    ��{�I�Ȋ֐��̍\���ATJS����̈�����TJS�ɓn���l�������ɕϊ�������@��
	    ��

	  tSumFunction - �����̍��v�����߂�
	    �^�̔��f�A�����ւ̕ϊ��A��������̕ϊ��Ȃ�

	  tMessageBoxFunction - ���b�Z�[�W�{�b�N�X��\������
	    TJS����̕������ANSI������(�i���[������;�[���I��char*������)�ւ̕ϊ�
	    �Ȃ�

	  tGetUserNameFunction - ���݂̃��[�U�����擾����
	    TJS�֕������Ԃ��Ȃ�


	�E�g���g���Ƃ̃C���^�[�t�F�[�X
	  �g���g���₻�̎��Ӄc�[�����Ăԃv���O�C�����̊֐��Q
	  ��L�̊֐���o�^�E�폜���镔����������Ă���B�܂��A�g���g���ݒ�c�[��
	  �ɃI�v�V�����̐����̏����󂯓n�����@�ɂ��Đ������Ă���B


	�v���O�C���ŗ�O�������s���ۂ̒��ӓ_�ɂ��Ă� exceptest ���Q�Ƃ̂��ƁB
*/


/*
  VC++ 6.0 �ł́A���̌`���ł̃v���O�C���̍���
  1. �V�K�쐬�� Win32 Dynamic-Link Library �̃v���W�F�N�g���u��� DLL
     �v���W�F�N�g�v�Ƃ��č쐬����B
  2. �g���g���̃\�[�X�z�z�p�b�P�[�W�� /base/win32/plugin_kit/ ��
     tp_stub.cpp tp_stub.h ���v���W�F�N�g�t�H���_�ɃR�s�[���邩
     �C���N���[�h�E�p�X��ʂ��Ȃǂ��A
     [�v���W�F�N�g|�v���W�F�N�g�֒ǉ�|�t�@�C��]�Œǉ�����
  3. .def �t�@�C�� (���̃t�@�C���̍Ō�̂ق����Q��) ���쐬���ăv���W�F�N
     �g�ɒǉ�����
  4. ���̃\�[�X�Ȃǂ��Q�l�ɂ��ăv���O����������
     (�g���g������Ă΂�� V2Link �� V2Unlink �͕K�{)
*/


/*
  bcc �͂ǂ��� DLL �쐬���A
  �E�C�����C���֐���
  �E���C�h�����񃊃e����
  �E���C�h�����񃊃e�����̊J�n�A�h���X���(����������ɔz�u������Ă̂�
    �A��)
  �E�Q�o�C�g�n�������܂܂��H
  �Ƃ������������낤�Ɛ���ɂ��̕�������i�[�ł��Ȃ��݂����Ȃ̂�
  ���ӁB�Ƃ肠�����̓C�����C���֐�(class { } ���ɒ��ڏ������֐����܂�) ��
  ���C�h�����񃊃e�������g���̂�������Ζ��͂Ȃ��悤�ł���B
  bcc ���g��Ȃ���΂��܂�C�ɂ��Ȃ��Ă悢
*/




/*
	�쐬���� DLL ���g���ɂ� :


	KAG �̏ꍇ�� loadplugin �^�O�A�g���g��/TJS �̏ꍇ�� Plugins.link ���\�b
	�h���g���B

@loadplugin module=basetest.dll

	���邢��

Plugins.link("basetest.dll");

	����ƁAaverage �Ƃ����֐����g�p�\�ɂȂ�̂ŁAKAG ���� TJS���� TJS �X�N
	���v�g����

@emb exp="average(0, 1, 2)"

	���邢��

var avg = average(0, 1, 2);

	�ȂǂƂ��Ďg���B



	average (�������ׂĂ̕��ς����߂�)
	sum (�����̍��v�����߂�; �������������܂߂Ύ����̌��ʁA����ȊO�͐����̌���)
	messagebox (��������Ƃ�A���b�Z�[�W�{�b�N�X��\������֐�)
	getUserName (���݂̃��[�U����Ԃ��֐�)
	
*/





//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"

#include <lmcons.h>
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// �e�X�g�֐�
//---------------------------------------------------------------------------
class tAverageFunction : public tTJSDispatch
{
	// �g���g���ŗ��p�\�Ȋ֐����ȒP�ɍ쐬����ɂ́A
	// tTJSDispatch ���p�������AFuncCall ����������B

	// ��Ƃ��āA�^����ꂽ�����̕��� (������) ��Ԃ��֐�����������B

	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * AverageFunction;
	// �e�X�g�֐���ێ�
	// iTJSDispatch2 �̔h���� ( tTJSDispatch �� tAverageFunction ������) �͂Ȃ�
	// �ׂ��q�[�v�Ɋm�� ( �܂�Anew �Ŋm�ۂ��� ) �悤�ɂ��ׂ��B
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tAverageFunction::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	// flag       : �Ăяo���t���O
	//              �������Ă��܂�Ȃ�
	// membername : �����o��
	//              �������Ă��܂�Ȃ����A�ꉞ
	if(membername) return TJS_E_MEMBERNOTFOUND;
	//              �ƋL�q���Ă����Ɨǂ�
	// hint       : �����o���q���g
	//              �{����membername�̃n�b�V���l�����A�������Ă��܂�Ȃ�
	// result     : ���ʊi�[��
	//              NULL �̏ꍇ�͌��ʂ��K�v�Ȃ���
	// numparams  : �p�����[�^�̐�
	// param      : �p�����[�^
	// objthis    : �R���e�L�X�g
	//              �N���X���\�b�h�ł͂Ȃ��̂Ŗ������Ă��܂�Ȃ�

	// if(numparams != 3) return TJS_E_BADPARAMCOUNT;
		// �����̐�������Ȃ��ꍇ�� TJS_E_BADPARAMCOUNT ��Ԃ�
		// ���Ƃ��΁A������ 3 �łȂ���΂Ȃ�Ȃ��ꍇ�� ��L�̃R�����g���͂���

	if(numparams == 0) return TJS_E_BADPARAMCOUNT;
		// ���̏ꍇ�͈����� 0 �ł����Ă͂Ȃ�Ȃ��̂ł��̂悤�ɂ���


	// ���ς��v�Z
	tTVReal sum = 0.0;  // tTVReal �� double �� typedef �� tTJSVariant �Ŏg�p
	for(tjs_int i = 0; i < numparams; i++)  // ���ׂĂ̈����ɑ΂���
	{
		sum += (tTVReal)*param[i];
			// *param[i] �Ȃǂ� tTJSVariant �^�B���̌^�͂�����o���A���g�^
			// �ŁA�����A�����A�I�u�W�F�N�g�A�I�N�e�b�g��A������Ȃǂ�\����
			// ���ł��A�e�^�Ƃ̕ϊ��֐�����`����Ă���B
	}
	sum /= (tTVReal)numparams;


	if(result)
	{
		// result �� NULL �ɂȂ肤��̂ŁA�֐��Œl��Ԃ��Ƃ���
		// ���Ȃ炷 result �� NULL �łȂ����`�F�b�N����B
		// result �� NULL �ł����Ă��G���[�ɂ͂��Ȃ��ق����悢
		*result = sum;
	}

	return TJS_S_OK;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// �e�X�g�֐� ����������
//---------------------------------------------------------------------------
class tSumFunction : public tTJSDispatch
{
	// ��Ƃ��āA�^����ꂽ�����̍��v�����߂�֐�
	// (�����̒��Ɏ���������Γ����������A�����łȂ���Γ����͐����ɂ���)

	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * SumFunction;
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tSumFunction::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if(membername) return TJS_E_MEMBERNOTFOUND;

	// ������ 0 �̏ꍇ�̏���
	if(numparams == 0)
	{
		// �����̐���0�Ȃ̂�
		if(result) *result = (tTVInteger)0;
			// tTVInteger �ɃL���X�g���ꂽ������tTJSVariant�^�ɓn����
			// �����͐����ɂȂ�
		return TJS_S_OK;
	}

	// �����̒��Ɏ��������邩�ǂ������`�F�b�N
	bool real_result = false; // �����œ�����Ԃ��ꍇ�� true �ɂ���
	tjs_int i;
	for(i = 0; i < numparams; i++)
	{
		if(param[i]->Type() == tvtReal)  // �����Ȃ��
		{
			real_result = true;
			break;
		}
	}

	// ���v�����߂ē�����Ԃ�
	if(real_result)
	{
		// �����͎����ŕԂ�
		tTVReal sum = 0.0;
		for(i = 0; i < numparams; i++)
		{
			sum += (tTVReal)*param[i];
		}

		if(result) *result = sum;
	}
	else
	{
		// �����͐����ŕԂ�
		tTVInteger sum = 0;
		for(i = 0; i < numparams; i++)
		{
			sum += (tTVInteger)*param[i];
		}

		if(result) *result = sum;
	}

	return TJS_S_OK;
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// �e�X�g�֐� ����ɂ���������
//---------------------------------------------------------------------------
class tMessageBoxFunction : public tTJSDispatch
{
	// ��Ƃ��āA�ȒP�ȃ��b�Z�[�W�{�b�N�X��\������֐�
	// tTJSVariant ���� �i���[������ ( �ӂ��� char * ������ ) �����o����
	// �{�̑��ɗ�O��ʂ��ăG���[�������Ԃ���

	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * MessageBoxFunction;
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tMessageBoxFunction::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{


	if(membername) return TJS_E_MEMBERNOTFOUND;

	if(numparams < 1) return TJS_E_BADPARAMCOUNT;

	// *param[0] ( tTJSVariant �^ ) ����i���[����������o��
	// TJS2 �̓��C�h���� (UNICODE) �ŕ�������Ǘ����Ă���̂�
	// �i���[������ɕϊ�����̂͂�������Ԃ�������B
	// �����ł����i���[������Ƃ͂ӂ��� char * ������̂��ƂŁA
	// ���{��̂΂����� Shift-JIS �R�[�h�ɂȂ�B

	// *param[0] �͂��̎��_�ŕ�����^�łȂ���΂Ȃ�Ȃ���
	// ������^�łȂ������ꍇ�͗�O�𔭐�������
	if(param[0]->Type() != tvtString)
		TVPThrowExceptionMessage(TJS_W("��������w�肵�Ă�������"));
			// TVPThrowExceptionMessage �͗�O�𔭐�������
			// ���̊֐����ĂԂƁA���������ւ͖߂�Ȃ��̂Œ���

	ttstr str(*param[0]); // �������� ttstr �^�ɑ������
	tjs_int narrow_len = str.GetNarrowStrLen(); // �i���[������ϊ���̃T�C�Y
	if(narrow_len == -1)
		TVPThrowExceptionMessage(TJS_W("������̕ϊ��Ɏ��s���܂���"));
			// GetNarrowStrLen �͕����񒆂ɕϊ��ł��Ȃ��������������
			// -1 ��Ԃ�

	char *narrow_str = new char [narrow_len + 1];
			// narrow_len ��null�^�[�~�l�[�^�̕��������܂�ł��Ȃ��̂ŁA
			// ������o�b�t�@���m�ۂ���ꍇ�ɂ� +1 ��Y��Ȃ��悤��
	try
	{
		// ���ǂ��ŗ�O���������邩�킩��Ȃ��̂�
		// �ł��邩���� ��O�ی���s��

		// �������ϊ�
		str.ToNarrowStr(narrow_str, narrow_len);

		// ���b�Z�[�W�{�b�N�X��\��
		MessageBox(TVPGetApplicationWindowHandle(), narrow_str,
			"���b�Z�[�W", MB_OK);

			// TVPGetApplicationWindowHandle �͋g���g���̃E�B���h�E�̂��ׂ�
			// �� owner �ł���E�B���h�E�n���h����Ԃ��B�_�C�A���O�{�b�N�X
			// ��\������Ƃ��Ȃǂɐe�E�B���h�E���K�v�Ȃ�΁A���̃E�B���h�E
			// �n���h����p����Ƃ悢
	}
	catch(...)
	{
		// ��O�����������ꍇ
		delete [] narrow_str; // �m�ۂ����o�b�t�@�����
		throw; // ��O���Ăѓ�����
	}

	delete [] narrow_str; // �m�ۂ����o�b�t�@�����

	if(result) result->Clear(); // result->Clear() ������� void ��Ԃ�

	return TJS_S_OK;
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// �e�X�g�֐� ����ɂ���ɂ���������
//---------------------------------------------------------------------------
class tGetUserNameFunction : public tTJSDispatch
{
	// ��Ƃ��āA���݂̃��[�U����Ԃ��֐�
	// �֐����當�����Ԃ���

	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * GetUserNameFunction;
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tGetUserNameFunction::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	if(membername) return TJS_E_MEMBERNOTFOUND;

	// GetUserName �֐����g�p���āA���݂̃��[�U����Ԃ��B
	// GetUserName �� NT �n OS �̏ꍇ GetUserNameW �Ƃ����֐���
	// �g�p�ł���̂ŁA���� NT �n OS �ł���΂�������g�p����B

	// OS �� NT �n OS ���ǂ����𓾂�
	bool is_nt; // NT �n OS �̏ꍇ�^�ɂ���

	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);
	is_nt = ovi.dwPlatformId == VER_PLATFORM_WIN32_NT;
	
	// GetUserNameW �ւ̊֐��|�C���^�𓾂�
	typedef BOOL (WINAPI *GetUserNameW_t)(LPWSTR lpBuffer, LPDWORD nSize);
	GetUserNameW_t pGetUserNameW = NULL; // UNICODE �� API
	HMODULE advapi_handle = NULL; // ADVAPI32.DLL �̃��W���[���n���h��

	if(is_nt) advapi_handle = LoadLibrary("advapi32.dll");
		// ADVAPI32.DLL ��ǂ݂���

	try
	{
		if(advapi_handle)
		{
			// �֐��|�C���^�𓾂�
			pGetUserNameW = (GetUserNameW_t)
				GetProcAddress(advapi_handle, "GetUserNameW");
			if(!pGetUserNameW)
			{
				FreeLibrary(advapi_handle);
				advapi_handle = NULL;
			}
		}

		// GetUserName ���g�p���Č��݂̃��[�U�����擾
		if(pGetUserNameW)
		{
			// UNICODE (���C�h����)�o�[�W����
			DWORD size;
			size = UNLEN + 1;
			wchar_t un[UNLEN + 1]; // ���C�h������
			un[0] = 0;
			pGetUserNameW(un, &size);

			if(result) *result = un;
				// �[���I�����C�h������͂��̂܂܌��ʂɑ���\
				// (������p�̃o�b�t�@�͐V���ɋg���g�������Ŋm�ۂ����̂�
				//  ����Ɏg�p����������o�b�t�@�͉�����Ă��܂�Ȃ�)
		}
		else
		{
			// ANSI (�i���[����)�o�[�W����
			DWORD size;
			size = UNLEN + 1;
			char un[UNLEN + 1]; // ���C�h������
			un[0] = 0;
			GetUserNameA(un, &size);

			if(result) *result = un; // �[���I���i���[�����������\
									// (�����I�Ƀ��C�h�����ɓ����ŕϊ�)
		}
	}
	catch(...)
	{
		if(advapi_handle) FreeLibrary(advapi_handle);
		throw;
	}

 	if(advapi_handle) FreeLibrary(advapi_handle);

	return TJS_S_OK;
}
//---------------------------------------------------------------------------






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
	AverageFunction = new tAverageFunction();

	// 2 AverageFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(AverageFunction);

	// 3 ���ł� val �� AverageFunction ��ێ����Ă���̂ŁAAverageFunction ��
	//   Release ����
	AverageFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("average"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);
	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------
	// 1 �܂��I�u�W�F�N�g���쐬
	SumFunction = new tSumFunction();

	// 2 SumFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(SumFunction);

	// 3 ���ł� val �� SumFunction ��ێ����Ă���̂ŁASumFunction ��
	//   Release ����
	SumFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("sum"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);
	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------
	// 1 �܂��I�u�W�F�N�g���쐬
	MessageBoxFunction = new tMessageBoxFunction();

	// 2 MessageBoxFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(MessageBoxFunction);

	// 3 ���ł� val �� MessageBoxFunction ��ێ����Ă���̂ŁA
	// MessageBoxFunction �� Release ����
	MessageBoxFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("messagebox"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
		NULL, // �q���g ( �{���̓����o���̃n�b�V���l�����ANULL �ł��悢 )
		&val, // �o�^����l
		global // �R���e�L�X�g ( global �ł悢 )
		);
	//-----------------------------------------------------------------------



	//-----------------------------------------------------------------------
	// 1 �܂��I�u�W�F�N�g���쐬
	GetUserNameFunction = new tGetUserNameFunction();

	// 2 GetUserNameFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(GetUserNameFunction);

	// 3 ���ł� val �� GetUserNameFunction ��ێ����Ă���̂ŁA
	// GetUserNameFunction �� Release ����
	GetUserNameFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("getUserName"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
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
			TJS_W("average"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			TJS_W("sum"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			TJS_W("messagebox"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
		global->DeleteMember(
			0, // �t���O ( 0 �ł悢 )
			TJS_W("getUserName"), // �����o��
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
static tjs_char option_message[] =
L"�e�X�g�J�e�S��:�e�X�g�ݒ荀��1;����̓e�X�g�p�̃I�v�V�����ݒ荀�ڂł��B|"
L"testoption1|select,*default;�f�t�H���g�̒l,1;�I���\�Ȓl1,2;�I���\�Ȓl2\n"
L"�e�X�g�J�e�S��:�e�X�g�ݒ荀��2;����̓e�X�g�p�̃I�v�V�����ݒ荀�ڂł��B|"
L"testoption2|string(64),*�f�t�H���g�̕�����\n";
//---------------------------------------------------------------------------
extern "C" const wchar_t* _stdcall _export GetOptionDesc()
{
	// GetOptionDesc �́A�g���g���ݒ�c�[���ɓn���I�v�V�����̏���Ԃ����߂�
	// �֐��B
	// ��s�ɂ���̃I�v�V�����Ɋւ�����������B
	// ������
	// �J�e�S��:�ݒ薼;����|�I�v�V������|�I�v�V�����Ƃ��Đݒ�\�Ȓl
	// (��؂�L���ɒ���)
	// �u�I�v�V�������v�ɂ� '-' (�n�C�t��) �͂��Ȃ��B
	// �u�I�v�V�����Ƃ��Đݒ�\�Ȓl�v�͈ȉ��̏����ł���B
	// �^�C�v,�l�̒l;�ȒP�Ȑ���[,�l;�l�̊ȒP�Ȑ��� ...]
	// �u�^�C�v�v�͌��o�[�W�����ł� select �� string �̂ݎg�p�\�B

	// �u�^�C�v�v�� select ���w�肷��ƁA�R���{�{�b�N�X����l��I�����邱�Ƃ�
	// �ł���悤�ɂȂ�B
	// �u�l;�l�̊ȒP�Ȑ����v�̐擪�� * ������Ƃ��ꂪ�f�t�H���g�̃I�v�V����
	// �ł��邱�Ƃ������B�f�t�H���g�̃I�v�V�����͕K����͖����Ă͂Ȃ�Ȃ��B
	// �u�l�v�͎��ۂɋg���g���{�̂ւ̃I�v�V�����Ƃ��ēn�������̂��A
	// �u�l�̊ȒP�Ȑ����v�͐l�Ԃ��ǂ�ŗ����ł�����̂������B

	// �u�^�C�v�v�� string ���w�肷��ƁA�G�f�B�b�g�{�b�N�X�Ń��[�U���C�ӂ̕�
	// �������͂ł���悤�ɂȂ�B
	// �u�^�C�v�v�� string ���w�肷��ꍇ�́Astring(n),*�f�t�H���g������ ��
	// �`�� n �ɂ͍ő啶�������L�q����B0 �ŕ����������͂Ȃ��Ȃ邪�A�ł����
	// �����������ق����悢�B
	// �u�f�t�H���g������v�ɂ̓f�t�H���g�̕�������L�q����Bstring(n) �̂���
	// �� , (�J���}) �� * (�A�X�e���X�N) ���ȗ��ł��Ȃ��B

	// ��� /msg/win32/option_desc_ja.txt �ɂ�����B


	return option_message;

	// �e�I�v�V��������A�Ō�̉��s�L����Y���Ƒ�ςȂ��ƂɂȂ�B


	// �g���g���ݒ�Ȃǂ́A���̃v���O�C���ɐݒ�\�ȍ��ڂ����邩�ǂ�����
	// �v���O�C��DLL��
	// �擪2KB�ȓ��� "(--has-option--)" �Ƃ��������񂪂��邩�ǂ���
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// (�Q�d���p���͊܂߂�)
	// �Ŕ��f���邽�߁A�擪2KB�ȓ��ɂ���������Ȃ���΂Ȃ�Ȃ��B
	// bcc �̏ꍇ�̓����J�I�v�V���� /GC �� /GC"(--has-option--)"
	// �ƋL�q���AVC++ �̏ꍇ�������J�I�v�V���� /COMMENT ��
	// /COMMENT:"(--has-option--)" �ƋL�q����B
	// BCB �� IDE ����� �v���W�F�N�g�I�v�V�����́u�����J(�ڍ�)�v��
	// �u�C���[�W�̃R�����g�v�� "(--has-option--)" �Ə����B
	// (�Q�d���p���ň͂܂Ȃ��ƃG���[�ɂȂ邩������Ȃ�)

	// UPX �Ȃǂň��k����Ƃ����̕�����������Ă��܂��̂ŁA
	// �w�b�_�̖��g�p�̈� (0x80�`0x1ff �� 00 �Ŗ��܂��Ă��鏊�Ȃ�) ��
	// ����������Ȃǂ���B

	// �{�̂ɓn���ꂽ�I�v�V������ TVPGetCommandLine �֐��Ŏ擾�ł���B
	// tTJSVariant val;
	// if(TVPGetCommandLine(TJS_W("-testoption1"), &val))
	// {
	//      // ������ �I�v�V���� "-testoption1" ���n����Ă����Ƃ���
	//      // �����������Bval �ɂ͂��̒l�������Ă���B
	//      // /environ/win32/wuvorbis/WuVorbisMainUnit.cpp ���Q��
	// }
	// TVPGetcommandLine �̎��s�͌��o�[�W�����ł͍����Ȃ̂łł����
	// ���̊֐��̌Ăяo���̌��ʂ̓L���b�V�����������悢�B

	// �g���g���ݒ�́A�v���O�C�����u�g���g���{�̂Ɠ����t�H���_�v��
	// �u�g���g���{�̂̂Ɠ����t�H���_�ɂ��� plugin �Ƃ����t�H���_�v����
	// �����������Ȃ��B�]���Đݒ�\�ȃI�v�V�������܂ރv���O�C����
	// Releaser �ȂǂŃA�[�J�C�u���Ɋ܂߂Ă͂Ȃ�Ȃ�(���Ƃ���Releaser��
	// �v���O�C���𒆂Ɋ܂߂邱�Ƃ͐�������Ă��Ȃ�)�B

	// ���� GetOptionDesc �֐����ł� tp_stub �ɋL�q����Ă���悤��
	// �e�g���g���{�̓��̊֐��͗��p�ł��Ȃ��B
}
//---------------------------------------------------------------------------
/*
	V2Link �� V2Unlink �� DLL ����G�N�X�|�[�g����Ă���K�v������B
	�]���āA.def �t�@�C�����쐬��

EXPORTS
	V2Link
	V2Unlink

	�ƋL�q���ăv���W�F�N�g�ɒǉ�����B

	GetOptionsDesc ���g���ꍇ�͂�����ǉ�����B
*/
//---------------------------------------------------------------------------


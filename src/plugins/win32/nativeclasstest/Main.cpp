//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"

//---------------------------------------------------------------------------



/*
	�v���O�C�����Ńl�C�e�B�u�������ꂽ�N���X��񋟂��A�g���g�����Ŏg�p�ł���悤��
	�����ł��B

	�l�C�e�B�u�N���X�� tTJSNativeInstance ���p�������N���X��ɍ쐬���A���̃l�C
	�e�B�u�N���X�Ƃ̃C���^�[�t�F�[�X�� tTJSNativeClassForPlugin ���x�[�X�ɍ쐬��
	�܂��B

	�uTJS2 ���t�@�����X�v�́u�g�ݍ��݂̎�����v�́u��{�I�Ȏg�����v�ɂ����Ɠ���
	�N���X�������ł͍쐬���܂��B�������A�v���O�C���Ŏ�������s����ATJS2 ���t�@
	�����X�ɂ����Ƃ͎኱�����̎d�����قȂ邱�Ƃɒ��ӂ��Ă��������B
*/



//---------------------------------------------------------------------------
// �e�X�g�N���X
//---------------------------------------------------------------------------
/*
	�e�I�u�W�F�N�g (iTJSDispatch2 �C���^�[�t�F�[�X) �ɂ̓l�C�e�B�u�C���X�^���X��
	�Ă΂��AiTJSNativeInstance �^�̃I�u�W�F�N�g��o�^���邱�Ƃ��ł��A�����
	�I�u�W�F�N�g������o�����Ƃ��ł��܂��B
	�܂��A�l�C�e�B�u�C���X�^���X�̎����ł��B�l�C�e�B�u�C���X�^���X����������ɂ�
	tTJSNativeInstance ����N���X�𓱏o���܂��BtTJSNativeInstance ��
	iTJSNativeInstance �̊�{�I�ȓ�����������Ă��܂��B
*/
class NI_Test : public tTJSNativeInstance // �l�C�e�B�u�C���X�^���X
{
public:
	NI_Test()
	{
		// �R���X�g���N�^
		/*
			NI_Test �̃R���X�g���N�^�ł��BC++ �N���X�Ƃ��Ă̏������� ��q��
			Construct ���������ōς܂��Ă����AConstruct �ł̏������͍ŏ����̕�
			�ɂ��邱�Ƃ��������߂��܂��B���̗�ł́A�f�[�^�����o�� Value �ɏ���
			�l�Ƃ��� 0 ��ݒ肵�Ă��܂��B
		*/
		Value = 0;
	}

	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		// TJS2 �I�u�W�F�N�g���쐬�����Ƃ��ɌĂ΂��
		/*
			TJS2 �� new ���Z�q�� TJS2 �I�u�W�F�N�g���쐬�����Ƃ��ɌĂ΂�܂��B
			numparams �� param ������ new ���Z�q�ɓn���ꂽ������\���Ă��܂��B
			tjs_obj �����́A�쐬����� TJS �I�u�W�F�N�g�ł��B
			���̗�ł́A����������� (����ɂ��ꂪ void �Ŗ������)�A����� Value
			�̏����l�Ƃ��Đݒ肵�Ă��܂��B
		*/

		// ����������΂���������l�Ƃ��� Value �ɓ����
		if(numparams >= 1 && param[0]->Type() != tvtVoid)
			Value = (tjs_int)*param[0];

		return S_OK;
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// �I�u�W�F�N�g�������������Ƃ��ɌĂ΂��
		/*
			�I�u�W�F�N�g�������������Ƃ��ɌĂ΂�郁�\�b�h�ł��B�����ɏI������
			�������Ɨǂ��ł��傤�B���̗�ł͉������܂���B
		*/
	}

	/*
		�f�[�^�����o�𑀍삷�邽�߂̌��J���\�b�h�Q�ł��B��q����l�C�e�B�u�N���X
		���ŁA�����𗘗p����R�[�h�������܂��B
	*/
	void SetValue(tjs_int n) { Value = n; }
	tjs_int GetValue() const { return Value; }

	tjs_int GetSquare() const { return Value*Value; }
	void Add(tjs_int n) { Value += n; }
	void Print() const { TVPAddLog(ttstr(Value)); }

private:
	/*
		�f�[�^�����o�ł��B�l�C�e�B�u�C���X�^���X�ɂ́A�K�v�ȃf�[�^�����o�����R��
		�������Ƃ��ł��܂��B
	*/
	tjs_int Value; // �l
};
//---------------------------------------------------------------------------
/*
	����� NI_Test �̃I�u�W�F�N�g���쐬���ĕԂ������̊֐��ł��B
	��q�� TJSCreateNativeClassForPlugin �̈����Ƃ��ēn���܂��B
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Test()
{
	return new NI_Test();
}
//---------------------------------------------------------------------------
/*
	TJS2 �̃l�C�e�B�u�N���X�͈�ӂ� ID �ŋ�ʂ���Ă���K�v������܂��B
	����͌�q�� TJS_BEGIN_NATIVE_MEMBERS �}�N���Ŏ����I�Ɏ擾����܂����A
	���� ID ���i�[����ϐ����ƁA���̕ϐ��������Ő錾���܂��B
	�����l�ɂ͖����� ID ��\�� -1 ���w�肵�Ă��������B
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_Test
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 �p�́u�N���X�v���쐬���ĕԂ��֐��ł��B
*/
static iTJSDispatch2 * Create_NC_Test()
{
	/*
		�܂��A�N���X�̃x�[�X�ƂȂ�N���X�I�u�W�F�N�g���쐬���܂��B
		����ɂ� TJSCreateNativeClassForPlugin ��p���܂��B
		TJSCreateNativeClassForPlugin �̑�P�����̓N���X���A��Q������
		�l�C�e�B�u�C���X�^���X��Ԃ��֐����w�肵�܂��B
		�쐬�����I�u�W�F�N�g���ꎞ�I�Ɋi�[���郍�[�J���ϐ��̖��O��
		classobj �ł���K�v������܂��B
	*/
	tTJSNativeClassForPlugin * classobj =
		TJSCreateNativeClassForPlugin(TJS_W("Test"), Create_NI_Test);


	/*
		TJS_BEGIN_NATIVE_MEMBERS �}�N���ł��B�����ɂ� TJS2 ���Ŏg�p����N���X��
		���w�肵�܂��B
		���̃}�N���� TJS_END_NATIVE_MEMBERS �}�N���ŋ��܂ꂽ�ꏊ�ɁA�N���X��
		�����o�ƂȂ�ׂ����\�b�h��v���p�e�B�̋L�q�����܂��B
	*/
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Test)

		/*
			��� finalize ���\�b�h��錾���܂��Bfinalize �ɑ������鏈����
			tTJSNativeInstance::Invalidate ���I�[�o�[���C�h���邱�Ƃł������ł�
			�܂��̂ŁA�ʏ�͋�̃��\�b�h�ŏ\���ł��B
		*/
		TJS_DECL_EMPTY_FINALIZE_METHOD

		/*
			(TJS��) �R���X�g���N�^��錾���܂��BTJS �ŃN���X�������Ƃ��A
			�N���X���ŃN���X�Ɠ����̃��\�b�h��錾���Ă��镔���ɑ������܂��B

			TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL �}�N���̂P�Ԗڂ̈����̓l�C�e�B�u
			�C���X�^���X�Ɋ��蓖�Ă�ϐ����ŁA�Q��ʖڂ̈����͂��̕ϐ��̌^���ł��B
			���̗�ł̂��̃u���b�N���ł� NI_Test * _this �Ƃ����ϐ������p�\�ŁA
			�l�C�e�B�u�C���X�^���X�ɃA�N�Z�X���邱�Ƃ��ł��܂��B
			�}�N���̂R�Ԗڂ̈����́ATJS ���Ŏg�p����N���X�����w�肵�܂��B
			TJS_END_NATIVE_CONSTRUCTOR_DECL �}�N���̈��������l�ł��B
			�������A�R���X�g���N�^�ɑ������鏈���� tTJSNativeInstance::Construct
			���I�[�o�[���C�h���鎖�Ŏ����ł���̂ŁA�����ł͉��������� S_OK ���
			���܂��B
		*/
		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Test,
			/*TJS class name*/Test)
		{
			// NI_Test::Construct �ɂ����e���L�q�ł���̂�
			// �����ł͉������Ȃ�
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Test)

		/*
			print ���\�b�h��錾���܂��B���\�b�h����
			TJS_BEGIN_NATIVE_METHOD_DECL �� TJS_END_NATIVE_METHOD_DECL �̗��}�N
			���ɓ������̂��w�肷��K�v������܂��B���̃}�N�����Ŏg�p�\�ȕϐ���
			tjs_int numparams �� tTJSVariant **param �������āA���ꂼ��A�n����
			�������̐��ƈ����������Ă��܂��B���̃��\�b�h�ł͂����͎g�p���Ă���
			����B20�`21�s�ڂ́A�I�u�W�F�N�g����l�C�e�B�u�C���X�^���X�����o��
			���߂̃}�N���ł��B���̗�ł� _this �Ƃ��� NI_Test * �^�̕ϐ��Ƀl�C
			�e�B�u�C���X�^���X�����o���A�Ƃ����Ӗ��ɂȂ�܂��B�ȍ~�A_this �Ƃ�
			���ϐ��Ńl�C�e�B�u�C���X�^���X�ɃA�N�Z�X�ł��܂��B23�s�ڂŁA����
			�l�C�e�B�u�C���X�^���X�� Print ���\�b�h���Ăяo���Ă��܂��B
		*/
		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/print) // print ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
				/*var. type*/NI_Test);

			_this->Print();

			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/print)


		/*
			add ���\�b�h��錾���܂��B�����ł� numparams �� param ���g�p���Ă��܂��B
		*/
		TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/add) // add ���\�b�h
		{
			TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
				/*var. type*/NI_Test);

			if(numparams < 1) return TJS_E_BADPARAMCOUNT;

			_this->Add((tjs_int)*param[0]);

			return TJS_S_OK;
		}
		TJS_END_NATIVE_METHOD_DECL(/*func. name*/add)

		/*
			value �v���p�e�B��錾���܂��BTJS_BEGIN_NATIVE_PROP_DECL ��
			TJS_END_NATIVE_PROP_DECL �̗��}�N���ɂ́A���\�b�h�̐錾�Ɠ������A
			�v���p�e�B�����w�肵�܂��B

			TJS_BEGIN_NATIVE_PROP_GETTER �� TJS_END_NATIVE_PROP_GETTER �}�N����
			�͂܂ꂽ�ꏊ�ɂ́A�Q�b�^�[���L�q���邱�Ƃ��ł��܂��B�Q�b�^�[���ł�
			tTJSVariant �^�ł��� *result �ɒl��ݒ肷��悤�ɋL�q���܂��B
			���l�ɁATJS_BEGIN_NATIVE_PROP_SETTER �� TJS_END_NATIVE_PROP_SETTER
			�}�N���ň͂܂ꂽ�ꏊ�ɂ̓Z�b�^�[���L�q���邱�Ƃ��ł��܂��B
			�Z�b�^�[���ł� tTJSVariant �^�ł��� *param �ɐݒ肳���ׂ��l���i�[
			����Ă���̂ŁA������g���ď��������܂��B
		*/
		TJS_BEGIN_NATIVE_PROP_DECL(value) // value �v���p�e�B
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
					/*var. type*/NI_Test);
				*result = (tTVInteger)_this->GetValue();
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_BEGIN_NATIVE_PROP_SETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
					/*var. type*/NI_Test);
				_this->SetValue((tjs_int)*param);
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(value)

		/*
			�����ł͓ǂݏo����p�v���p�e�B��錾���Ă��܂��B�Z�b�^�[�̑����
			TJS_DENY_NATIVE_PROP_SETTER ���������Ƃɂ��A�ǂݏo����p�v���p�e�B
			����邱�Ƃ��ł��܂��B
		*/
		TJS_BEGIN_NATIVE_PROP_DECL(square) // square �ǂݏo����p�v���p�e�B
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this,
					/*var. type*/NI_Test);

				*result = (tTVInteger)_this->GetSquare();

				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_DENY_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(square)

	TJS_END_NATIVE_MEMBERS

	/*
		���̊֐��� classobj ��Ԃ��܂��B
	*/
	return classobj;
}
//---------------------------------------------------------------------------
/*
	TJS_NATIVE_CLASSID_NAME �͈ꉞ undef ���Ă������ق����悢�ł��傤
*/
#undef TJS_NATIVE_CLASSID_NAME
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

	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();


	//-----------------------------------------------------------------------
	// 1 �܂��N���X�I�u�W�F�N�g���쐬
	iTJSDispatch2 * tjsclass = Create_NC_Test();

	// 2 tjsclass �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(tjsclass);

	// 3 ���ł� val �� tjsclass ��ێ����Ă���̂ŁAtjsclass ��
	//   Release ����
	tjsclass->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("Test"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
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


	/*
		�������A�N���X�̏ꍇ�A�����Ɂu�I�u�W�F�N�g���g�p���ł���v�Ƃ������Ƃ�
		�m�邷�ׂ�����܂���B��{�I�ɂ́APlugins.unlink �ɂ��v���O�C���̉����
		�댯�ł���ƍl���Ă������� (�������� Plugins.link �Ń����N������A�Ō��
		�Ńv���O�C������������A�v���O�����I���Ɠ����Ɏ����I�ɉ��������̂��g)�B
	*/

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� Test �N���X�Ȃǂ��폜����

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
			TJS_W("Test"), // �����o��
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


//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include "PassThroughDrawDevice.h"

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class NI_PluggedDrawDevice : public tTJSNativeInstance // �l�C�e�B�u�C���X�^���X
{
	iTVPDrawDevice * Device;
public:
	NI_PluggedDrawDevice()
	{
		// �R���X�g���N�^
		Device = new tTVPPassThroughDrawDevice();
	}

	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
	{
		// TJS2 �I�u�W�F�N�g���쐬�����Ƃ��ɌĂ΂��

		return S_OK;
	}

	void TJS_INTF_METHOD Invalidate()
	{
		// �I�u�W�F�N�g�������������Ƃ��ɌĂ΂��
	}
};
//---------------------------------------------------------------------------
/*
	����� NI_PluggedDrawDevice �̃I�u�W�F�N�g���쐬���ĕԂ������̊֐��ł��B
	��q�� TJSCreateNativeClassForPlugin �̈����Ƃ��ēn���܂��B
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_PluggedDrawDevice()
{
	return new NI_PluggedDrawDevice();
}
//---------------------------------------------------------------------------
/*
	TJS2 �̃l�C�e�B�u�N���X�͈�ӂ� ID �ŋ�ʂ���Ă���K�v������܂��B
	����͌�q�� TJS_BEGIN_NATIVE_MEMBERS �}�N���Ŏ����I�Ɏ擾����܂����A
	���� ID ���i�[����ϐ����ƁA���̕ϐ��������Ő錾���܂��B
	�����l�ɂ͖����� ID ��\�� -1 ���w�肵�Ă��������B
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_PluggedDrawDevice
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 �p�́u�N���X�v���쐬���ĕԂ��֐��ł��B
*/
static iTJSDispatch2 * Create_NC_PluggedDrawDevice()
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
		TJSCreateNativeClassForPlugin(TJS_W("PluggedDrawDevice"), Create_NI_PluggedDrawDevice);


	/*
		TJS_BEGIN_NATIVE_MEMBERS �}�N���ł��B�����ɂ� TJS2 ���Ŏg�p����N���X��
		���w�肵�܂��B
		���̃}�N���� TJS_END_NATIVE_MEMBERS �}�N���ŋ��܂ꂽ�ꏊ�ɁA�N���X��
		�����o�ƂȂ�ׂ����\�b�h��v���p�e�B�̋L�q�����܂��B
	*/
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/PluggedDrawDevice)

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
			���̗�ł̂��̃u���b�N���ł� NI_PluggedDrawDevice * _this �Ƃ����ϐ������p�\�ŁA
			�l�C�e�B�u�C���X�^���X�ɃA�N�Z�X���邱�Ƃ��ł��܂��B
			�}�N���̂R�Ԗڂ̈����́ATJS ���Ŏg�p����N���X�����w�肵�܂��B
			TJS_END_NATIVE_CONSTRUCTOR_DECL �}�N���̈��������l�ł��B
			�������A�R���X�g���N�^�ɑ������鏈���� tTJSNativeInstance::Construct
			���I�[�o�[���C�h���鎖�Ŏ����ł���̂ŁA�����ł͉��������� S_OK ���
			���܂��B
		*/
		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_PluggedDrawDevice,
			/*TJS class name*/PluggedDrawDevice)
		{
			// NI_PluggedDrawDevice::Construct �ɂ����e���L�q�ł���̂�
			// �����ł͉������Ȃ�
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/PluggedDrawDevice)

		/*
			print ���\�b�h��錾���܂��B���\�b�h����
			TJS_BEGIN_NATIVE_METHOD_DECL �� TJS_END_NATIVE_METHOD_DECL �̗��}�N
			���ɓ������̂��w�肷��K�v������܂��B���̃}�N�����Ŏg�p�\�ȕϐ���
			tjs_int numparams �� tTJSVariant **param �������āA���ꂼ��A�n����
			�������̐��ƈ����������Ă��܂��B���̃��\�b�h�ł͂����͎g�p���Ă���
			����B20�`21�s�ڂ́A�I�u�W�F�N�g����l�C�e�B�u�C���X�^���X�����o��
			���߂̃}�N���ł��B���̗�ł� _this �Ƃ��� NI_PluggedDrawDevice * �^�̕ϐ��Ƀl�C
			�e�B�u�C���X�^���X�����o���A�Ƃ����Ӗ��ɂȂ�܂��B�ȍ~�A_this �Ƃ�
			���ϐ��Ńl�C�e�B�u�C���X�^���X�ɃA�N�Z�X�ł��܂��B23�s�ڂŁA����
			�l�C�e�B�u�C���X�^���X�� Print ���\�b�h���Ăяo���Ă��܂��B
		*/

		TJS_BEGIN_NATIVE_PROP_DECL(interface)
		{
			TJS_BEGIN_NATIVE_PROP_GETTER
			{
				TJS_GET_NATIVE_INSTANCE(/*var. name*/_this, /*var. type*/tTJSNI_PassThroughDrawDevice);
				*result = reinterpret_cast<tjs_int64>(_this->GetDevice());
				return TJS_S_OK;
			}
			TJS_END_NATIVE_PROP_GETTER

			TJS_DENY_NATIVE_PROP_SETTER
		}
		TJS_END_NATIVE_PROP_DECL(interface)



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
	iTJSDispatch2 * tjsclass = Create_NC_PluggedDrawDevice();

	// 2 tjsclass �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(tjsclass);

	// 3 ���ł� val �� tjsclass ��ێ����Ă���̂ŁAtjsclass ��
	//   Release ����
	tjsclass->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("PluggedDrawDevice"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
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

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� PluggedDrawDevice �N���X�Ȃǂ��폜����

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
			TJS_W("PluggedDrawDevice"), // �����o��
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


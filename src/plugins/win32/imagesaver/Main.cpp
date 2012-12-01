//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// BMP �ł̉摜�ۑ�
//---------------------------------------------------------------------------
void SaveAsBMP(const ttstr & name, tjs_int width, tjs_int height,
	const tjs_uint8 * bufferptr, tjs_int bufferpitch)
{
	// BMP �ŉ摜��ۑ�����

	// �X�g���[�����J��
	IStream * out = TVPCreateIStream(name, TJS_BS_WRITE);
	if(!out)
		TVPThrowExceptionMessage((ttstr(TJS_W("cannot open : ")) +
			ttstr(name)).c_str());
	tjs_uint8 * buf = NULL;

	try
	{
		// �g���g�������̉摜�L���b�V�����N���A
		// ������s��Ȃ��ƁA�摜���t�@�C���ɏ�������ł�
		// �g���g�������̃L���b�V�����Q�Ƃ��Ă��܂�
		TVPClearGraphicCache();


		// �w�b�_������
		tjs_uint pitch = width*4;
		tjs_uint srcpitch = bufferpitch < 0 ? -bufferpitch : bufferpitch;

		BITMAPFILEHEADER bfh;
		bfh.bfType = 'B' + ('M' << 8);
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
			pitch * height;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		unsigned long written;
		out->Write(&bfh, sizeof(bfh), &written);
		if(written != sizeof(bfh))
			TVPThrowExceptionMessage((ttstr(TJS_W("write failed : ")) +
				ttstr(name)).c_str());

		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(bih);
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = 0;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		out->Write(&bih, sizeof(bih), &written);
		if(written != sizeof(bih))
			TVPThrowExceptionMessage((ttstr(TJS_W("write failed : ")) +
				ttstr(name)).c_str());


		// �r�b�g�}�b�v���o��
		buf = new tjs_uint8 [pitch];
		bufferptr += bufferpitch * (height-1);

		for(tjs_int y = 0; y < height; y++)
		{
			memcpy(buf, bufferptr, srcpitch);
			out->Write(buf, pitch, &written);
			if(written != pitch)
				TVPThrowExceptionMessage((ttstr(TJS_W("write failed : ")) +
					ttstr(name)).c_str());
			bufferptr -= bufferpitch; // �㉺���t�ɕۑ�����̂�
		}

	}
	catch(...)
	{
		out->Release();
		if(buf) delete [] buf;
		throw;
	}

	out->Release();
	if(buf) delete [] buf;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// ���C���̉摜��ۑ����邽�߂̊֐� ( TJS �֐� )
//---------------------------------------------------------------------------
class tSaveLayerImageFunction : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if(membername) return TJS_E_MEMBERNOTFOUND;

		// ���� : ���C��, �t�@�C����, �摜�`�� [, �摜�ŗL�I�v�V���� ...]
		if(numparams < 3) return TJS_E_BADPARAMCOUNT;

		const tjs_uint8 * bufferptr;
		tjs_int bufferpitch;
		tjs_int width;
		tjs_int height;

		// �摜�̃T�C�Y�A�s�N�Z���o�b�t�@�ւ̃|�C���^�A�s�b�`�𓾂�
		iTJSDispatch2 * layerobj = param[0]->AsObjectNoAddRef();

		tTJSVariant val;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageWidth"), NULL, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageWidth failed."));
		width = val;

		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageHeight"), NULL, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageHeight failed."));
		height = val;

		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBuffer"), NULL, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBuffer failed."));
		bufferptr = (const tjs_uint8 *)(tjs_int)val;

		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferPitch"), NULL, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferPitch failed."));
		bufferpitch = val;

		// �摜�`�����`�F�b�N
		ttstr format = *param[2];
		if(format == TJS_W("bmp"))
		{
			// BMP �ŕۑ�
			SaveAsBMP(*param[1], width, height, bufferptr, bufferpitch);
		}
		else
		{
			TVPThrowExceptionMessage(TJS_W("Not supported format."));
		}

		// �߂�l�� void
		if(result) result->Clear();

		// �߂�
		return TJS_S_OK;
	}
} * SaveLayerImageFunction;
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

	// SaveLayerImageFunction �̍쐬�Ɠo�^
	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// 1 �܂��I�u�W�F�N�g���쐬
	SaveLayerImageFunction = new tSaveLayerImageFunction();

	// 2 SaveLayerImageFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(SaveLayerImageFunction);

	// 3 ���ł� val �� SaveLayerImageFunction ��ێ����Ă���̂ŁA
	// SaveLayerImageFunction �� Release ����
	SaveLayerImageFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("saveLayerImage"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
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

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� saveLayerImage �֐����폜����

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
			TJS_W("saveLayerImage"), // �����o��
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




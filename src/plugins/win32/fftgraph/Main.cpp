//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// �f�[�^
//---------------------------------------------------------------------------
// ���̃v���O�C���ł͕����̃X�y�A�i�𓯎��ɕ\�����邱�Ƃ͍l���Ȃ�
tjs_int SampleBufferLen = 0; // SampleBuffer �T�C�Y
tjs_int16 *SampleBuffer = NULL; // ���� PCM �o�b�t�@
tjs_int FFTLen = 0; // FFT��
float *FFTData = NULL; // FFT�f�[�^
float *WindowData = NULL; // ���֐��f�[�^
int *fft_ip = NULL; // FFT���[�N
float *fft_w = NULL; // FFT���[�N
tjs_int BandCount = 0; // �ш搔
float BandCut = 0; // ����g�J�b�g�I�t
	// (�ΐ��ڐ���ɂ�����) ����g��� 1/BandCut ���̒���g�̈��\�����Ȃ�
tjs_int *BandData = NULL; // �ш悲�Ƃ̐����f�[�^
tjs_int *BandPeakData = NULL; // �ш悲�Ƃ̃s�[�N�f�[�^
tjs_int *BandPeakCount = NULL; // �Ō�Ƀs�[�N�f�[�^���X�V���ꂽ������̃J�E���g
tjs_int *BandStart = NULL; // �ш悲�Ƃ̊J�n���g���ʒu
tjs_int *BandEnd = NULL; // �ш悲�Ƃ̏I�����g���ʒu
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// visualization �p�̃T���v���f�[�^���擾
//---------------------------------------------------------------------------
static void GetVisBuffer(iTJSDispatch2 * obj, tjs_int numsamples,
	tjs_int channels, tjs_int aheadsamples)
{
	if(SampleBuffer == NULL || SampleBufferLen != channels * numsamples)
	{
		if(SampleBuffer) delete [] SampleBuffer;
		SampleBuffer = new tjs_int16[SampleBufferLen = channels * numsamples];
	}

	tTJSVariant val[4];
	tTJSVariant *pval[4] = { val, val +1, val +2, val +3 };

	val[0] = (tjs_int64)(tjs_int)SampleBuffer;
	val[1] = (tjs_int64)numsamples;
	val[2] = (tjs_int64)channels;
	val[3] = (tjs_int64)aheadsamples;

	tTJSVariant res;

	static tjs_uint32 getVisBuffer_hint = 0;
	if(TJS_SUCCEEDED(obj->FuncCall(0, L"getVisBuffer", &getVisBuffer_hint,
		&res, 4, pval, obj)))
	{
		tjs_int ret = res;
		if(ret < numsamples)
		{
			ZeroMemory(SampleBuffer, sizeof(tjs_int16) * SampleBufferLen);
		}
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// FFT
//---------------------------------------------------------------------------
extern "C"
{
	void ddst(int n, int isgn, float *a, int *ip, float *w);
}
void DoFFT()
{
	// SampleBuffer �ɍ������U�t�[���G�ϊ� (���ۂ̓T�C���ϊ�) ���s��
	// FFTData �Ɋi�[����
	tjs_int len = SampleBufferLen;
	tjs_int len2 = len / 2;

	if(FFTLen != len)
	{
		FFTLen = len;

		// �o�b�t�@�̊m��
		if(FFTData) delete [] FFTData, FFTData = NULL;
		if(WindowData) delete [] WindowData, WindowData = NULL;
		if(fft_ip) delete [] fft_ip, fft_ip = NULL;
		if(fft_w) delete[] fft_w, fft_w = NULL;

		FFTData = new float[len];
		WindowData = new float[len];
		fft_ip = new int[(int)(2.0 + sqrt(len * (1.0 / 2.0)))];
		fft_w = new float[(int)(len * (5.0 / 4.0))];

		fft_ip[0] = 0;

		// ���֐��̍쐬
		float mul = (4.0/32768.0/len);
		for(tjs_int i = 0; i < len; i++)
		{
			WindowData[i] = sin(3.14159265358979 * (i + 0.5) / len)  * mul;
				// (sin��)
		}
	}

	// FFTData �ɑ��֐���K�p���Ȃ���f�[�^��]��
	for(tjs_int i = 0; i < len2; i++)
	{
		FFTData[i] = SampleBuffer[i] * WindowData[i];
		FFTData[i + len2] = SampleBuffer[i + len2] * WindowData[len2 - i - 1];
	}

	// �������U�T�C���ϊ�
	ddst(len, -1, FFTData, fft_ip, fft_w);

	// ���������̃J�b�g
	FFTData[0] = FFTData[1] = 0;

}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// �ш悲�Ƃ̎��g�������𓾂�
//---------------------------------------------------------------------------
void GetBandSpectrum(int count, float cut, float mindb, int maxval, int falldown,
	int peakholdtime, int peakfalldown)
{
	if(count != BandCount || BandCut != cut)
	{
		BandCount = count;
		BandCut = cut;

		// �o�b�t�@�̊m��
		if(BandData) delete [] BandData, BandData = NULL;
		if(BandPeakData) delete [] BandPeakData, BandPeakData = NULL;
		if(BandPeakCount) delete [] BandPeakCount, BandPeakCount = NULL;
		if(BandStart) delete [] BandStart, BandStart = NULL;
		if(BandEnd) delete [] BandEnd, BandEnd = NULL;

		BandData = new tjs_int[count];
		BandPeakData = new tjs_int[count];
		BandPeakCount = new tjs_int[count];
		tjs_int i;
		for(i = 0; i < count; i ++)
			BandData[i] = BandPeakData[i] = BandPeakCount[i] = 0;
		BandStart = new tjs_int[count];
		BandEnd = new tjs_int[count];

		// �ш悲�Ƃ̎��g���J�n�ʒu�Ǝ��g���I���ʒu�̌v�Z
		for(i = 0; i < count; i++)
		{
			float j = (float)(i  ) * ((cut-1) / cut) / (float)count + (1.0 / cut);
			float k = (float)(i+1) * ((cut-1) / cut) / (float)count + (1.0 / cut);
			int start = pow(FFTLen, j);
			int end = pow(FFTLen, k);
			if(start == end) end++;
			if(end > FFTLen) end = FFTLen;
			BandStart[i] = start;
			BandEnd[i] = end;
		}
	}

	// �ш悲�Ƃɐ����̃s�[�N�𓾂Ă���� dB �ɂ��Ċi�[
	float max_mindb_1 = maxval * 1.0 / mindb;
	for(tjs_int i = 0; i < count; i++)
	{
		float max = 0;
		for(tjs_int j = BandStart[i]; j < BandEnd[i]; j++)
		{
			float v = FFTData[j];
			if(v < 0) v = -v;
			if(max < v) max = v;
		}


		float db;
		if(max != 0) db = log10(max*max) * 10; else db = -1000000;
		if(db > 0) db = 0;
		if(db < mindb) db = mindb;

		tjs_int vi = maxval - max_mindb_1 * db;

		BandData[i] -= falldown;
		if(BandData[i] < 0) BandData[i] = 0;
		if(BandData[i] < vi) BandData[i] = vi;
		if(BandData[i] > maxval) BandData[i] = maxval;

		if(BandPeakCount[i] == peakholdtime)
		{
			BandPeakData[i] -= peakfalldown;
			if(BandPeakData[i] < 0) BandPeakData[i] = 0;
		}
		else
		{
			BandPeakCount[i] ++;
		}

		if(BandPeakData[i] < vi)
		{
			BandPeakData[i] = vi;
			BandPeakCount[i] = 0;
		}
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// �X�y�A�i��`�悷�邽�߂̊֐�
//---------------------------------------------------------------------------
struct FFTGraphParam
{
	iTJSDispatch2 * sb;
	tjs_uint8 * laybuf;
	tjs_int laybufpitch;
	tjs_int laywidth;
	tjs_int layheight;
	tjs_int left;
	tjs_int top;
	tjs_int width;
	tjs_int height;
	iTJSDispatch2 * options;
};
//---------------------------------------------------------------------------
static tjs_int GetValueFromOptions(iTJSDispatch2 *options, const tjs_char *name,
	tjs_uint32 *hint, tjs_int defvalue)
{
	tTJSVariant val;
	if(!options) return defvalue;

	if(TJS_FAILED(options->PropGet(TJS_MEMBERMUSTEXIST, name, hint, &val, options)))
	{
		return defvalue;
	}

	return val;
}
//---------------------------------------------------------------------------
void DrawBarGraph(const FFTGraphParam & param, tjs_int count, tjs_int thickness,
	tjs_uint32 oncolor, tjs_uint32 offcolor, tjs_uint32 bgcolor, tjs_uint32 peakcolor)
{
	tjs_int th = 1000 / (param.height / thickness);

	GetBandSpectrum(count, 8, -70, 1000-th*2, 40, 40, 10);

	tjs_uint8 *lbstart = param.laybuf +
		(param.top + param.height - 1) * param.laybufpitch + param.left * sizeof(tjs_uint32);

	tjs_int bw = param.width / count;
	for(int i = 0; i < count; i++)
	{
		tjs_uint8 *lb = lbstart + i * bw * sizeof(tjs_uint32);

		int y = 0;
		int v = 0;
		bool peakdrawn = false;
		while(true)
		{
			for(int x = 0; x < bw; x++) *((tjs_uint32*)lb + x) = bgcolor;
			lb -= param.laybufpitch;

			if(!peakdrawn && BandPeakData[i] <= v && peakcolor != offcolor)
			{
				peakdrawn = true;
				for(tjs_int yy = 1; yy < thickness; yy++)
				{
					*(tjs_uint32*)lb = bgcolor;
					for(int x = 1; x < bw; x++) *((tjs_uint32*)lb + x) = peakcolor;
					lb -= param.laybufpitch;
				}
			}
			else if(BandData[i] >= v)
			{
				for(tjs_int yy = 1; yy < thickness; yy++)
				{
					*(tjs_uint32*)lb = bgcolor;
					for(int x = 1; x < bw; x++) *((tjs_uint32*)lb + x) = oncolor;
					lb -= param.laybufpitch;
				}
			}
			else
			{
				for(tjs_int yy = 1; yy < thickness; yy++)
				{
					*(tjs_uint32*)lb = bgcolor;
					for(int x = 1; x < bw; x++) *((tjs_uint32*)lb + x) = offcolor;
					lb -= param.laybufpitch;
				}
			}


			y += thickness;
			v += th;

			if(y + thickness > param.height) break;
		}
	}

}
//---------------------------------------------------------------------------
void DrawFFTGraph(const FFTGraphParam & param)
{
	GetVisBuffer(param.sb, 2048, 1, 0);
	DoFFT();

	tjs_int type = 0;
	static tjs_uint32 type_hint = 0;
	type = GetValueFromOptions(param.options, L"type", &type_hint, type);

	switch(type)
{
	case 0: // winamp, fire��
	  {
#if 1
		const static tjs_uint32 firecolors[16] =
			{
				0xff20ff00, 0xff40ff00, 0xff60ff00, 0xff80ff00,
				0xffa0ff00, 0xffc0ff00, 0xffe0ff00, 0xffffff00,
				0xffffe000, 0xffffc000, 0xffffa000, 0xffff8000,
				0xffff6000, 0xffff4000, 0xffff2000, 0xffffff00
			};
		const tjs_uint32 peak_color = 0xff808080;
#else
			/* �������񃂁[�h */
		const static tjs_uint32 firecolors[16] =
			{
				0xff11DDFF, 0xff11CCEE, 0xff11BBDD, 0xff11AACC,
				0xff1199CC, 0xff1188BB, 0xff1177AA, 0xff0066AA,
				0xff005599, 0xff004499, 0xff004488, 0xff003388,
				0xff002277, 0xff001166, 0xff000066, 0xff000055
			};
		const tjs_uint32 peak_color = 0xff11eeff;
#endif


		GetBandSpectrum(param.width, 3.7, -70, param.height - 1,
			(param.height+15) / 16, 30, (param.height+31) / 32);

		tjs_uint8 *lbstart = param.laybuf +
			(param.top + param.height - 1) * param.laybufpitch +
			param.left * sizeof(tjs_uint32);

		for(int x = 0; x < param.width; x++)
		{
			tjs_uint8 *lb = lbstart + x * sizeof(tjs_uint32);

			int y;
			for(y = 0; y < BandData[x]; y ++)
			{
				int c = 15 - BandData[x] + y;
				*(tjs_uint32*)lb = c < 0 ? 0xff00ff00 : firecolors[c];
				lb -= param.laybufpitch;
			}
			for(; y < param.height; y++)
			{
				*(tjs_uint32*)lb = 0;
				lb -= param.laybufpitch;
			}

			*(tjs_uint32*)
				(lbstart + x * sizeof(tjs_uint32) - BandPeakData[x] * param.laybufpitch)
					= peak_color;
		}
	  }
	  break;
	case 1: // LCD ��
	  {
		tjs_int division = 16;
		tjs_int thick = 2;
		tjs_int oncolor = 0xff000000;
		tjs_int offcolor = 0xffb0b0b0;
		tjs_int bgcolor = 0xffc0c0c0;
		tjs_int peakcolor = 0xff707070;

		static tjs_uint32 hints[6];

		division = GetValueFromOptions(param.options, L"division", hints + 0, division);
		thick = GetValueFromOptions(param.options, L"thick", hints + 1, thick);
		oncolor = GetValueFromOptions(param.options, L"oncolor", hints + 2, oncolor);
		offcolor = GetValueFromOptions(param.options, L"offcolor", hints + 3, offcolor);
		bgcolor = GetValueFromOptions(param.options, L"bgcolor", hints + 4, bgcolor);
		peakcolor = GetValueFromOptions(param.options, L"peakcolor", hints + 5, peakcolor);

		DrawBarGraph(param, division, thick, oncolor, offcolor, bgcolor, peakcolor);
	  }
	  break;
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// �X�y�A�i��`�悷�邽�߂� TJS �֐�
//---------------------------------------------------------------------------
class tDrawFFTGraphFunction : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if(membername) return TJS_E_MEMBERNOTFOUND;

		// ���� : ���C��, �T�E���h�o�b�t�@, ���[�ʒu, ��[�ʒu, ��, ����,
		//        [, �I�v�V����]

		if(numparams < 6) return TJS_E_BADPARAMCOUNT;

		iTJSDispatch2 * layerobj = param[0]->AsObjectNoAddRef();
		iTJSDispatch2 * sbobj = param[1]->AsObjectNoAddRef();

		tjs_uint8 * bufferptr;
		tjs_int bufferpitch;
		tjs_int width;
		tjs_int height;

		// �摜�̃T�C�Y�A�s�N�Z���o�b�t�@�ւ̃|�C���^�A�s�b�`�𓾂�

		tTJSVariant val;
		static tjs_uint32 imageWidth_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageWidth"), &imageWidth_hint, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageWidth failed."));
		width = val;

		static tjs_uint32 imageHeight_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageHeight"), &imageHeight_hint, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageHeight failed."));
		height = val;

		static tjs_uint32 mainImageBufferForWrite_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferForWrite"), &mainImageBufferForWrite_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferForWrite failed."));
		bufferptr = (tjs_uint8 *)(tjs_int)val;

		static tjs_uint32 mainImageBufferPitch_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferPitch"), &mainImageBufferPitch_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferPitch failed."));
		bufferpitch = val;
			/*
				note:
				PropGet ���� "hint" (�q���g) �����ɂ� tjs_uint32 �^�̕ϐ��ւ̃|�C���^��n���B
				TJS2 �́A�����o������������Ƃ��ɁA�����r���ŎZ�o���ꂽ���l�� hint �Ɋi�[���ĕԂ��B
				�Q��ڈȍ~�́A�Ă� hint �ɓn���ꂽ�l��p���Č������s�����߁A�����������ɂȂ�B
				���ۂ̓q���g�̓����o���̃n�b�V���ŁA���������o���Ȃ�Γ������l�ɂȂ邪�A
				�����q���g�̒l������Ƃ����ē��������o���ł���Ƃ͌���Ȃ��B
				�q���g�̒l���s���ȏꍇ�͐������l�Ɏ����I�ɏC������邪�A�����l�Ƃ��Ă� 0 ��
				�w�肷�邱�Ƃ����������B
			*/


		// �p�����[�^������
		FFTGraphParam fftparam;
		fftparam.sb = sbobj;
		fftparam.laybuf = bufferptr;
		fftparam.laybufpitch = bufferpitch;
		fftparam.laywidth = width;
		fftparam.layheight = height;
		fftparam.left = *param[2];
		fftparam.top = *param[3];
		fftparam.width = *param[4];
		fftparam.height = *param[5];
		if(numparams >= 7)
			fftparam.options = param[6]->AsObjectNoAddRef();
		else
			fftparam.options = NULL;

		// �X�y�A�i��`��
		DrawFFTGraph(fftparam);

		// �̈���A�b�v�f�[�g
		{
			tTJSVariant val[4];
			tTJSVariant *pval[4] = { val, val +1, val +2, val +3 };

			val[0] = (tjs_int64)fftparam.left;
			val[1] = (tjs_int64)fftparam.top;
			val[2] = (tjs_int64)fftparam.width;
			val[3] = (tjs_int64)fftparam.height;
			static tjs_uint32 update_hint = 0;
			layerobj->FuncCall(0, L"update", &update_hint, NULL, 4, pval, layerobj);
		}


		// �߂�
		return TJS_S_OK;
	}
} * DrawFFTGraphFunction;
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

	// TestFunction �̍쐬�Ɠo�^
	tTJSVariant val;

	// TJS �̃O���[�o���I�u�W�F�N�g���擾����
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// 1 �܂��I�u�W�F�N�g���쐬
	DrawFFTGraphFunction = new tDrawFFTGraphFunction();

	// 2 DrawFFTGraphFunction �� tTJSVariant �^�ɕϊ�
	val = tTJSVariant(DrawFFTGraphFunction);

	// 3 ���ł� val �� DrawFFTGraphFunction ��ێ����Ă���̂ŁA
	// DrawFFTGraphFunction �� Release ����
	DrawFFTGraphFunction->Release();


	// 4 global �� PropSet ���\�b�h��p���A�I�u�W�F�N�g��o�^����
	global->PropSet(
		TJS_MEMBERENSURE, // �����o���Ȃ������ꍇ�ɂ͍쐬����悤�ɂ���t���O
		TJS_W("drawFFTGraph"), // �����o�� ( ���Ȃ炸 TJS_W( ) �ň͂� )
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

	// TJS �̃O���[�o���I�u�W�F�N�g�ɓo�^���� drawFFTGraph �֐����폜����

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
			TJS_W("drawFFTGraph"), // �����o��
			NULL, // �q���g
			global // �R���e�L�X�g
			);
			// �o�^�����֐�����������ꍇ�� ������J��Ԃ�
	}

	// - global �� Release ����
	if(global) global->Release();

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	// ���̑��̏���
	if(SampleBuffer) delete [] SampleBuffer, SampleBuffer = NULL;
	if(FFTData) delete [] FFTData, FFTData = NULL;
	if(WindowData) delete [] WindowData, WindowData = NULL;
	if(fft_ip) delete [] fft_ip, fft_ip = NULL;
	if(fft_w) delete[] fft_w, fft_w = NULL;
	if(BandData) delete [] BandData, BandData = NULL;
	if(BandPeakData) delete [] BandPeakData, BandPeakData = NULL;
	if(BandPeakCount) delete [] BandPeakCount, BandPeakCount = NULL;
	if(BandStart) delete [] BandStart, BandStart = NULL;
	if(BandEnd) delete [] BandEnd, BandEnd = NULL;

	return S_OK;
}
//---------------------------------------------------------------------------




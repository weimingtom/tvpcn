//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
#include "wave.h"
#include "common.h"

//---------------------------------------------------------------------------
/*
	'�g' �g�����W�V����
	���X�^�X�N���[���ɂ��g�����W�V����
*/
//---------------------------------------------------------------------------
class tTVPWaveTransHandler : public iTVPDivisibleTransHandler
{
	//	'�g' �g�����W�V�����n���h���N���X�̎���

	tjs_int RefCount; // �Q�ƃJ�E���^
		/*
			iTVPDivisibleTransHandler �� �Q�ƃJ�E���^�ɂ��Ǘ����s��
		*/

protected:
	tjs_uint64 StartTick; // �g�����W�V�������J�n���� tick count
	tjs_uint64 HalfTime; // �g�����W�V�����ɗv���鎞�� / 2
	tjs_uint64 Time; // �g�����W�V�����ɗv���鎞��
	tTVPLayerType LayerType; // ���C���^�C�v
	tjs_int Width; // ��������摜�̕�
	tjs_int Height; // ��������摜�̍���
	tjs_int MaxH; // �ő�U��
	double MaxOmega; // �ő�p���x
	tjs_int CurH; // ���݂̐U��
	double CurOmega; // ���݂̊p���x
	double CurRadStart; // �p�J�n�ʒu
	tjs_int64 CurTime; // ���݂� tick count
	tjs_int BlendRatio; // �u�����h��
	tjs_uint32 BGColor1; // �w�i�F���̂P
	tjs_uint32 BGColor2; // �w�i�F���̂Q
	tjs_uint32 CurBGColor; // ���݂̔w�i�F
	tjs_int WaveType; // 0 = �ŏ��ƍŌ� 1 = �ŏ� 2 = �Ō� ���g���ׂ���
	bool First; // ��ԍŏ��̌Ăяo�����ǂ���

public:
	tTVPWaveTransHandler(tjs_uint64 time, tTVPLayerType layertype,
		tjs_int width, tjs_int height,
		tjs_int maxh, double maxomega,
		tjs_uint32 bgcolor1, tjs_uint32 bgcolor2, tjs_int wavetype)
	{
		RefCount = 1;

		LayerType = layertype;
		Width = width;
		Height = height;
		Time = time;
		HalfTime = time / 2;
		MaxH = maxh;
		MaxOmega = maxomega;
		BGColor1 = bgcolor1;
		BGColor2 = bgcolor2;
		WaveType = wavetype;

		First = true;
	}

	virtual ~tTVPWaveTransHandler()
	{
	}

	tjs_error TJS_INTF_METHOD AddRef()
	{
		// iTVPBaseTransHandler �� AddRef
		// �Q�ƃJ�E���^���C���N�������g
		RefCount ++;
		return TJS_S_OK;
	}

	tjs_error TJS_INTF_METHOD Release()
	{
		// iTVPBaseTransHandler �� Release
		// �Q�ƃJ�E���^���f�N�������g���A0 �ɂȂ�Ȃ�� delete this
		if(RefCount == 1)
			delete this;
		else
			RefCount--;
		return TJS_S_OK;
	}


	tjs_error TJS_INTF_METHOD SetOption(
			/*in*/iTVPSimpleOptionProvider *options // option provider
		)
	{
		// iTVPBaseTransHandler �� SetOption
		// �Ƃ��ɂ�邱�ƂȂ�
		return TJS_S_OK;
	}

	tjs_error TJS_INTF_METHOD StartProcess(tjs_uint64 tick);

	tjs_error TJS_INTF_METHOD EndProcess();

	tjs_error TJS_INTF_METHOD Process(
			tTVPDivisibleData *data);

	tjs_error TJS_INTF_METHOD MakeFinalImage(
			iTVPScanLineProvider ** dest,
			iTVPScanLineProvider * src1,
			iTVPScanLineProvider * src2)
	{
		*dest = src2; // ��ɍŏI�摜�� src2
		return TJS_S_OK;
	}
};
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPWaveTransHandler::StartProcess(tjs_uint64 tick)
{
	// �g�����W�V�����̉�ʍX�V��񂲂ƂɌĂ΂��

	// �g�����W�V�����̉�ʍX�V���ɂ��A�܂��ŏ��� StartProcess ���Ă΂��
	// ���̂��� Process ��������Ă΂�� ( �̈�𕪊��������Ă���ꍇ )
	// �Ō�� EndProcess ���Ă΂��

	if(First)
	{
		// �ŏ��̎��s
		First = false;
		StartTick = tick;
	}

	// �摜���Z�ɕK�v�Ȋe�p�����[�^���v�Z
	tjs_int64 t = CurTime = (tick - StartTick);
	if(CurTime > Time) CurTime = Time;
	if(t >= HalfTime) t = Time - t;
	if(t < 0) t = 0;

	double tt = sin((3.14159265358979/2.0) * t / (tjs_int64)HalfTime);

	// CurH, CurOmega, CurRadStart
	CurH = tt * MaxH;
	switch(WaveType)
	{
	case 0: // �ŏ��ƍŌオ�g���ׂ���
		CurOmega = MaxOmega * tt;
		break;
	case 1: // �ŏ����g���ׂ���
		CurOmega = MaxOmega * ((tjs_int64)Time - CurTime) / (tjs_int64)Time;
		break;
	case 2: // �Ōオ�g���ׂ���
		CurOmega = MaxOmega * CurTime / (tjs_int64)Time;
		break;
	}
	CurRadStart = -CurOmega * (Height / 2);

	// BlendRatio
	BlendRatio = CurTime * 255 / Time;
	if(BlendRatio > 255) BlendRatio = 255;

	// �w�i�F�̃u�����h
	CurBGColor = Blend(BGColor1, BGColor2, BlendRatio);

	return TJS_S_TRUE;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPWaveTransHandler::EndProcess()
{
	// �g�����W�V�����̉�ʍX�V��񕪂��I��邲�ƂɌĂ΂��

	if(BlendRatio == 255) return TJS_S_FALSE; // �g�����W�V�����I��

	return TJS_S_TRUE;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPWaveTransHandler::Process(
			tTVPDivisibleData *data)
{
	// �g�����W�V�����̊e�̈悲�ƂɌĂ΂��
	// �g���g���͉�ʂ��X�V����Ƃ��ɂ������̗̈�ɕ������Ȃ��珈�����s���̂�
	// ���̃��\�b�h�͒ʏ�A��ʍX�V���ɂ�������Ă΂��

	// data �ɂ͗̈��摜�Ɋւ����񂪓����Ă���

	// �����p�����[�^���v�Z
	double rad = data->Top * CurOmega + CurRadStart; // �p�x

	// ���C�����Ƃɏ���
	tjs_int n;
	for(n = 0; n < data->Height; n++, rad += CurOmega)
	{
		// �Y���ʒu
		tjs_int d = (tjs_int)(sin(rad) * CurH);

		// �]��
		tjs_int l, r;

		// �����ł��ׂ����Ƃ́Adata->Src1 �� data->Src2 �� (0, data->Top + n) ����
		// �n�܂�P���C���� BlendRatio �ɂ���ău�����h���A(d, data->Top + n) ��
		// �]������B�͂ݏo�ĕ`�悳��Ȃ������� CurBGColor �œh��Ԃ��B
		// �������A���E�� data->Left �� data->Width �ɂ���ăN���b�s���O�����B
		// �܂��Adata->Dest �ɓ]������Ƃ��́A���̃I�t�Z�b�g�� (data->Left, data->Top)
		// �ł͂Ȃ���(data->DestLeft, data->DestTop) �ɂȂ�̂ŕ␳����B

		// �X�L�������C��
		tjs_uint32 *dest;
		const tjs_uint32 *src1;
		const tjs_uint32 *src2;
		if(TJS_FAILED(data->Dest->GetScanLineForWrite(data->DestTop + n, (void**)&dest)))
			return TJS_E_FAIL;
		if(TJS_FAILED(data->Src1->GetScanLine(data->Top + n, (const void**)&src1)))
			return TJS_E_FAIL;
		if(TJS_FAILED(data->Src2->GetScanLine(data->Top + n, (const void**)&src2)))
			return TJS_E_FAIL;

		// �����̂���镔���ɔw�i�F��]��
		if(d > 0)
		{
			l = 0;
			r = d;
			if(Clip(l, r, data->Left, data->Left + data->Width))
				TVPFillARGB(dest + l + data->DestLeft - data->Left, r - l, CurBGColor);
		}

		// ���[�̂���镔���ɔw�i�F��]��
		if(d < 0)
		{
			l = d + Width;
			r = Width;
			if(Clip(l, r, data->Left, data->Left + data->Width))
				TVPFillARGB(dest + l + data->DestLeft - data->Left, r - l, CurBGColor);
		}

		// �u�����h���Ȃ���]��
		// TVPConstAlphaBlend_SD(dest, src1, src2, len, opa)
		// �� dest �� src1 �� src2 �� opa �Ŏw�肵��������ō������ē]������
		l = d;
		r = Width + d;
		if(Clip(l, r, data->Left, data->Left + data->Width))
		{
			if(LayerType == ltAlpha)
				TVPConstAlphaBlend_SD_d(dest + l + data->DestLeft - data->Left,
					src1 + l - d, src2 + l - d, r - l, BlendRatio);
			else if(LayerType == ltAddAlpha)
				TVPConstAlphaBlend_SD_a(dest + l + data->DestLeft - data->Left,
					src1 + l - d, src2 + l - d, r - l, BlendRatio);
			else
				TVPConstAlphaBlend_SD(dest + l + data->DestLeft - data->Left,
					src1 + l - d, src2 + l - d, r - l, BlendRatio);
				/*
					�]���悪���������Ă���ꍇ�̓u�����h�A���S���Y�����Ⴄ�̂�
					���ӂ���K�v������B
					_d �̃T�t�B�b�N�X�����u�����h�֐��͂��ׂĒʏ�̃��u�����h�ŁA
					���l���l�������u�����h���s���B���l�� _a �̃T�t�B�b�N�X������
					�u�����h�֐��͉��Z���u�����h�ł���B_a �� _d �T�t�B�b�N�X����
					���Ȃ��u�����h�֐��ɔ�ׂĒᑬ�B_d �� _a �T�t�B�b�N�X��������
					���u�����h�֐��̓��l�͈���Ȃ� ( ��Ɋ��S�ɕs�����ł���ƈ����� )�B
				*/
		}
	}


	return TJS_S_OK;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class tTVPWaveTransHandlerProvider : public iTVPTransHandlerProvider
{
	tjs_uint RefCount; // �Q�ƃJ�E���^
public:
	tTVPWaveTransHandlerProvider() { RefCount = 1; }
	~tTVPWaveTransHandlerProvider() {; }

	tjs_error TJS_INTF_METHOD AddRef()
	{
		// iTVPBaseTransHandler �� AddRef
		// �Q�ƃJ�E���^���C���N�������g
		RefCount ++;
		return TJS_S_OK;
	}

	tjs_error TJS_INTF_METHOD Release()
	{
		// iTVPBaseTransHandler �� Release
		// �Q�ƃJ�E���^���f�N�������g���A0 �ɂȂ�Ȃ�� delete this
		if(RefCount == 1)
			delete this;
		else
			RefCount--;
		return TJS_S_OK;
	}

	tjs_error TJS_INTF_METHOD GetName(
			/*out*/const tjs_char ** name)
	{
		// ���̃g�����W�V�����̖��O��Ԃ�
		if(name) *name = TJS_W("wave");
		return TJS_S_OK;
	}


	tjs_error TJS_INTF_METHOD StartTransition(
			/*in*/iTVPSimpleOptionProvider *options, // option provider
			/*in*/iTVPSimpleImageProvider *imagepro, // image provider
			/*in*/tTVPLayerType layertype, // destination layer type
			/*in*/tjs_uint src1w, tjs_uint src1h, // source 1 size
			/*in*/tjs_uint src2w, tjs_uint src2h, // source 2 size
			/*out*/tTVPTransType *type, // transition type
			/*out*/tTVPTransUpdateType * updatetype, // update typwe
			/*out*/iTVPBaseTransHandler ** handler // transition handler
			)
	{
		if(type) *type = ttExchange; // transition type : exchange
		if(updatetype) *updatetype = tutDivisible;
			// update type : divisible
		if(!handler) return TJS_E_FAIL;
		if(!options) return TJS_E_FAIL;

		if(src1w != src2w || src1h != src2h)
			return TJS_E_FAIL; // src1 �� src2 �̃T�C�Y����v���Ă��Ȃ��Ƒʖ�


		// �I�v�V�����𓾂�
		tTJSVariant tmp;
		tjs_uint64 time;
		tjs_int maxh = 50;
		double maxomega = 0.2;
		tjs_uint32 bgcolor1 = 0;
		tjs_uint32 bgcolor2 = 0;
		tjs_int wavetype = 0;

		if(TJS_FAILED(options->GetValue(TJS_W("time"), &tmp)))
			return TJS_E_FAIL; // time �������w�肳��Ă��Ȃ�
		if(tmp.Type() == tvtVoid) return TJS_E_FAIL;
		time = (tjs_int64)tmp;
		if(time < 2) time = 2; // ���܂菬���Ȑ��l���w�肷��Ɩ�肪�N����̂�

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("maxh"), &tmp)))
			if(tmp.Type() != tvtVoid) maxh = (tjs_int)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("maxomega"), &tmp)))
			if(tmp.Type() != tvtVoid) maxomega = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("bgcolor1"), &tmp)))
			if(tmp.Type() != tvtVoid) bgcolor1 = (tjs_int)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("bgcolor2"), &tmp)))
			if(tmp.Type() != tvtVoid) bgcolor2 = (tjs_int)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("wavetype"), &tmp)))
			if(tmp.Type() != tvtVoid) wavetype = (tjs_int)tmp;


		// �I�u�W�F�N�g���쐬
		*handler = new tTVPWaveTransHandler(time, layertype,
			src1w, src1h, maxh, maxomega,
			bgcolor1, bgcolor2, wavetype);

		return TJS_S_OK;
	}

} static * WaveTransHandlerProvider;
//---------------------------------------------------------------------------
void RegisterWaveTransHandlerProvider()
{
	// TVPAddTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^����
	WaveTransHandlerProvider = new tTVPWaveTransHandlerProvider();
	TVPAddTransHandlerProvider(WaveTransHandlerProvider);
}
//---------------------------------------------------------------------------
void UnregisterWaveTransHandlerProvider()
{
	// TVPRemoveTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^��������
	TVPRemoveTransHandlerProvider(WaveTransHandlerProvider);
	WaveTransHandlerProvider->Release();
}
//---------------------------------------------------------------------------

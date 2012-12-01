//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
#include "mosaic.h"
#include "common.h"

#include <stdio.h>

//---------------------------------------------------------------------------
/*
	'���U�C�N' �g�����W�V����
	��`���U�C�N�ɂ��g�����W�V����
	���̃g�����W�V�����͓]���悪���������Ă����(�v����Ƀg�����W�V�������s��
	���C���� type �� ltOpaque �ȊO�̏ꍇ)�A����ɓ��ߏ��������ł��Ȃ��̂�
	����
*/
//---------------------------------------------------------------------------
class tTVPMosaicTransHandler : public iTVPDivisibleTransHandler
{
	//	'���U�C�N' �g�����W�V�����n���h���N���X�̎���

	tjs_int RefCount; // �Q�ƃJ�E���^
		/*
			iTVPDivisibleTransHandler �� �Q�ƃJ�E���^�ɂ��Ǘ����s��
		*/

protected:
	tjs_uint64 StartTick; // �g�����W�V�������J�n���� tick count
	tjs_uint64 Time; // �g�����W�V�����ɗv���鎞��
	tjs_uint64 HalfTime; // �g�����W�V�����ɗv���鎞�� / 2
	tjs_uint64 CurTime; // ���݂̎���
	tjs_int Width; // ��������摜�̕�
	tjs_int Height; // ��������摜�̍���
	tjs_int CurOfsX; // �u���b�N�I�t�Z�b�g X
	tjs_int CurOfsY; // �u���b�N�I�t�Z�b�g Y
	tjs_int MaxBlockSize; // �ő�̃u���b�N��
	tjs_int CurBlockSize; // ���݂̃u���b�N��
	tjs_int BlendRatio; // �u�����h��
	bool First; // ��ԍŏ��̌Ăяo�����ǂ���

public:
	tTVPMosaicTransHandler(tjs_uint64 time, tjs_int width, tjs_int height,
		tjs_int maxblocksize)
	{
		RefCount = 1;

		Width = width;
		Height = height;
		Time = time;
		HalfTime = time / 2;
		MaxBlockSize = maxblocksize;

		First = true;
	}

	virtual ~tTVPMosaicTransHandler()
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
tjs_error TJS_INTF_METHOD tTVPMosaicTransHandler::StartProcess(tjs_uint64 tick)
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

	// �摜���Z�ɕK�v�ȃp�����[�^���v�Z
	tjs_int64 t = CurTime = (tick - StartTick);
	if(CurTime > Time) CurTime = Time;
	if(t >= HalfTime) t = Time - t;
	if(t < 0) t = 0;
	CurBlockSize = (MaxBlockSize-2) * t / HalfTime + 2;

	// BlendRatio
	BlendRatio = CurTime * 255 / Time;
	if(BlendRatio > 255) BlendRatio = 255;

	// ���S�̃u���b�N��{���ɒ��S�Ɏ����Ă�����悤�� CurOfsX �� CurOfsY �𒲐�
	int x = Width / 2;
	int y = Height / 2;
	x /= CurBlockSize;
	y /= CurBlockSize;
	x *= CurBlockSize;
	y *= CurBlockSize;
	CurOfsX = (Width - CurBlockSize) / 2 - x;
	CurOfsY = (Height - CurBlockSize) / 2 - y;
	if(CurOfsX > 0) CurOfsX -= CurBlockSize;
	if(CurOfsY > 0) CurOfsY -= CurBlockSize;

	return TJS_S_TRUE;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPMosaicTransHandler::EndProcess()
{
	// �g�����W�V�����̉�ʍX�V��񕪂��I��邲�ƂɌĂ΂��

	if(BlendRatio == 255) return TJS_S_FALSE; // �g�����W�V�����I��

	return TJS_S_TRUE;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPMosaicTransHandler::Process(
			tTVPDivisibleData *data)
{
	// �g�����W�V�����̊e�̈悲�ƂɌĂ΂��
	// �g���g���͉�ʂ��X�V����Ƃ��ɂ������̗̈�ɕ������Ȃ��珈�����s���̂�
	// ���̃��\�b�h�͒ʏ�A��ʍX�V���ɂ�������Ă΂��

	// data �ɂ͗̈��摜�Ɋւ����񂪓����Ă���

	// data->Left, data->Top, data->Width, data->Height �Ŏ�������`��
	// �̂ݓ]������K�v������B�����ōs�������͈ȉ��̒ʂ�B
	// 1: ���̓]����`�Ɋ܂܂�郂�U�C�N�̃u���b�N�͈̔͂𔻒肷��
	// 2: �܂��Ӌ��̃u���b�N�ɑ΂��ē]����`�Ƃ̐ϋ�`�𓾂Ă����ɐF��h��Ԃ�
	// 3: �c��̃u���b�N�͂͂ݏo���ɂ��Ē��ӂ���K�v���Ȃ��̂ŐS�����Ȃ��F��h��Ԃ�


	// �ϐ��̏���
	tjs_uint8 *dest;
	tjs_int destpitch;
	const tjs_uint8 *src1;
	tjs_int src1pitch;
	const tjs_uint8 *src2;
	tjs_int src2pitch;
	if(TJS_FAILED(data->Dest->GetScanLineForWrite(0, (void**)&dest)))
		return TJS_E_FAIL;
	if(TJS_FAILED(data->Src1->GetScanLine(0, (const void**)&src1)))
		return TJS_E_FAIL;
	if(TJS_FAILED(data->Src2->GetScanLine(0, (const void**)&src2)))
		return TJS_E_FAIL;
	if(TJS_FAILED(data->Dest->GetPitchBytes(&destpitch)))
		return TJS_E_FAIL;
	if(TJS_FAILED(data->Src1->GetPitchBytes(&src1pitch)))
		return TJS_E_FAIL;
	if(TJS_FAILED(data->Src2->GetPitchBytes(&src2pitch)))
		return TJS_E_FAIL;

	tjs_int destxofs = data->DestLeft - data->Left;
	tjs_int destyofs = data->DestTop - data->Top;

	// 1: ���̓]����`�Ɋ܂܂�郂�U�C�N�̃u���b�N�͈̔͂𔻒肷��
	int startx, starty;
	int endx, endy;
	int bs = CurBlockSize;

	startx = (data->Left - CurOfsX) / bs;
	starty = (data->Top - CurOfsY) / bs;
	endx = (data->Left + data->Width - 1 - CurOfsX) / bs;
	endy = (data->Top + data->Height - 1 - CurOfsY) / bs;

	// �h��Ԃ��}�N��
#define FILL_LINE(dp, xlen, ylen, d) { \
			tjs_uint8 *__destp = (tjs_uint8*)(dp); \
			int __count = ylen; \
			tjs_uint32 color = (d); \
			switch(xlen)                   \
			{                              \
			case 2:                        \
				do           \
				{                          \
					((tjs_uint32*)__destp)[0] =       \
					((tjs_uint32*)__destp)[1] = color;      \
					__destp += destpitch;  \
				} while(--__count);                          \
				break;                     \
			case 3:                        \
				do           \
				{                          \
					((tjs_uint32*)__destp)[0] =       \
					((tjs_uint32*)__destp)[1] =       \
					((tjs_uint32*)__destp)[2] = color;      \
					__destp += destpitch;  \
				} while(--__count);                          \
				break;                     \
			case 4:                        \
				do           \
				{                          \
					((tjs_uint32*)__destp)[0] =       \
					((tjs_uint32*)__destp)[1] =       \
					((tjs_uint32*)__destp)[2] =       \
					((tjs_uint32*)__destp)[3] = color;      \
					__destp += destpitch;  \
				} while(--__count);                          \
				break;                     \
			case 5:                        \
				do           \
				{                          \
					((tjs_uint32*)__destp)[0] =       \
					((tjs_uint32*)__destp)[1] =       \
					((tjs_uint32*)__destp)[2] =       \
					((tjs_uint32*)__destp)[3] =       \
					((tjs_uint32*)__destp)[4] = color;      \
					__destp += destpitch;  \
				} while(--__count);                          \
				break;                     \
			case 6:                        \
				do           \
				{                          \
					((tjs_uint32*)__destp)[0] =       \
					((tjs_uint32*)__destp)[1] =       \
					((tjs_uint32*)__destp)[2] =       \
					((tjs_uint32*)__destp)[3] =       \
					((tjs_uint32*)__destp)[4] =       \
					((tjs_uint32*)__destp)[5] = color;      \
					__destp += destpitch;  \
				} while(--__count);                          \
				break;                     \
			case 7:                        \
				do           \
				{                          \
					((tjs_uint32*)__destp)[0] =       \
					((tjs_uint32*)__destp)[1] =       \
					((tjs_uint32*)__destp)[2] =       \
					((tjs_uint32*)__destp)[3] =       \
					((tjs_uint32*)__destp)[4] =       \
					((tjs_uint32*)__destp)[5] =       \
					((tjs_uint32*)__destp)[6] = color;      \
					__destp += destpitch;  \
				} while(--__count);                          \
				break;                     \
			default:                       \
				while(__count--) \
				{ \
					TVPFillARGB((tjs_uint32*)__destp, (xlen), color); \
					__destp += destpitch; \
				} \
			} \
		}


	// ���ӂ��Ȃ���̓h��Ԃ��}�N��
	int bs2 = bs >> 1;
#define FILL_ONE(x, y) { \
		tjs_int l = (x) * bs + CurOfsX; \
		tjs_int t = (y) * bs + CurOfsY; \
		tjs_int r = l + bs; \
		tjs_int b = t + bs; \
		tjs_int mx = l + bs2, my = t + bs2; \
		if(Clip(l, r, data->Left, data->Left + data->Width) && \
			Clip(t, b, data->Top, data->Top + data->Height)) \
		{ \
			if(mx < 0) mx = 0; \
			if(my < 0) my = 0; \
			if(mx >= Width) mx = Width - 1; \
			if(my >= Height) my = Height - 1; \
			tjs_uint32 d = Blend( \
				*((const tjs_uint32 *)(src1 + my*src1pitch) + mx), \
				*((const tjs_uint32 *)(src2 + my*src2pitch) + mx), \
				BlendRatio); \
			tjs_uint8 *destp = (tjs_uint8*) \
				((tjs_uint32*)(dest + (t + destyofs)*destpitch) + l + destxofs); \
			tjs_int xlen = r - l; \
			tjs_int ylen = b - t; \
			FILL_LINE(destp, xlen, ylen, d); \
		} \
	}
		/* �{���͓]�����u���b�N�͈͓̔��ɂ���s�N�Z���̐F�̕��ς������Y�킾����
		   �d���Ȃ�̂ł��Ȃ� */

	// 2: �܂��Ӌ��̃u���b�N�ɑ΂��ē]����`�Ƃ̐ϋ�`�𓾂Ă����ɐF��h��Ԃ�
	// 3: �c��̃u���b�N�͂͂ݏo���ɂ��Ē��ӂ���K�v���Ȃ��̂ŐS�����Ȃ��F��h��Ԃ�

	// ��ԏ�̍s
	int y = starty;
	for(int x = startx; x <= endx; x++)
	{
		FILL_ONE(x, y);
	}
	y++;

	// �Ȃ��قǂ̍s
	for(; y < endy; y++)
	{
		// ���[
		FILL_ONE(startx, y);

		// �Ȃ��ق�
		tjs_int x = startx + 1;
		tjs_int l = x * bs + CurOfsX;
		tjs_int t = y * bs + CurOfsY;
		const tjs_uint32 *src1p =
			(const tjs_uint32*)(src1 + (t + bs2)*src1pitch) + (l + bs2);
		const tjs_uint32 *src2p =
			(const tjs_uint32*)(src2 + (t + bs2)*src2pitch) + (l + bs2);
		tjs_uint32 *destp =
			((tjs_uint32*)(dest + (t + destyofs)*destpitch) + l + destxofs);

		for(; x < endx; x++)
		{
			// �����̓h��Ԃ���(�͂ݏo�Ă��邩�ǂ�����)�m�[�`�F�b�N�ł�����
			tjs_uint32 d = Blend(*src1p, *src2p, BlendRatio);
			FILL_LINE(destp, bs, bs, d);

			src1p += bs;
			src2p += bs;
			destp += bs;
		}

		// �E�[
		FILL_ONE(endx, y);
	}


	// ��ԉ��̍s
	if(y <= endy)
	{
		for(int x = startx; x <= endx; x++)
		{
			FILL_ONE(x, y);
		}
	}

#undef FILL_ONE
#undef FILL_LINE

	return TJS_S_OK;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class tTVPMosaicTransHandlerProvider : public iTVPTransHandlerProvider
{
	tjs_uint RefCount; // �Q�ƃJ�E���^
public:
	tTVPMosaicTransHandlerProvider() { RefCount = 1; }
	~tTVPMosaicTransHandlerProvider() {; }

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
		if(name) *name = TJS_W("mosaic");
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
		tjs_int maxblocksize = 30;

		if(TJS_FAILED(options->GetValue(TJS_W("time"), &tmp)))
			return TJS_E_FAIL; // time �������w�肳��Ă��Ȃ�
		if(tmp.Type() == tvtVoid) return TJS_E_FAIL;
		time = (tjs_int64)tmp;
		if(time < 2) time = 2; // ���܂菬���Ȑ��l���w�肷��Ɩ�肪�N����̂�

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("maxsize"), &tmp)))
			if(tmp.Type() != tvtVoid) maxblocksize = (tjs_int)tmp;

		// �I�u�W�F�N�g���쐬
		*handler = new tTVPMosaicTransHandler(time, src1w, src1h, maxblocksize);

		return TJS_S_OK;
	}

} static * MosaicTransHandlerProvider;
//---------------------------------------------------------------------------
void RegisterMosaicTransHandlerProvider()
{
	// TVPAddTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^����
	MosaicTransHandlerProvider = new tTVPMosaicTransHandlerProvider();
	TVPAddTransHandlerProvider(MosaicTransHandlerProvider);
}
//---------------------------------------------------------------------------
void UnregisterMosaicTransHandlerProvider()
{
	// TVPRemoveTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^��������
	TVPRemoveTransHandlerProvider(MosaicTransHandlerProvider);
	MosaicTransHandlerProvider->Release();
}
//---------------------------------------------------------------------------

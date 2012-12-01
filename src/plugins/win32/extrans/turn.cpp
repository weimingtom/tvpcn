//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
#include "turn.h"
#include "turntrans_table.h"
#include "common.h"

#include <stdio.h>

//---------------------------------------------------------------------------
/*
	'turn' �g�����W�V����
	�����`�̏����ȃ^�C�����Ђ�����Ԃ��悤�ɂ��Đ؂�ւ��g�����W�V����
	���낢�낪��΂��Ă݂������܂�����]���Ă��镵�͋C���o�Ă��Ȃ�

	���̃g�����W�V�����͓]���悪���������Ă����(�v����Ƀg�����W�V�������s��
	���C���� type �� ltCoverRect �ȊO�̏ꍇ)�A����ɓ��ߏ��������ł��Ȃ��̂�
	����
*/
//---------------------------------------------------------------------------
// �e�J��
static const tjs_int gloss[64] =
	{
	   0,   0,   0,   0,  16,  48,  80, 128,
	 192, 128,  80,  48,  16,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   0,   0,   0,   0,   0,   0,   0,   0,
	   };
#define TURN_WIDTH_FACTOR 2
	// 1 ��ݒ肷��� 2 ��ݒ肵���Ƃ�������x�ɉ�]����u���b�N�̐��������Ȃ�
//---------------------------------------------------------------------------
class tTVPTurnTransHandler : public iTVPDivisibleTransHandler
{
	//	'���U�C�N' �g�����W�V�����n���h���N���X�̎���

	tjs_int RefCount; // �Q�ƃJ�E���^
		/*
			iTVPDivisibleTransHandler �� �Q�ƃJ�E���^�ɂ��Ǘ����s��
		*/

protected:
	tjs_uint64 StartTick; // �g�����W�V�������J�n���� tick count
	tjs_uint64 Time; // �g�����W�V�����ɗv���鎞��
	tjs_uint64 CurTime; // ���݂̎���
	tjs_int Width; // ��������摜�̕�
	tjs_int Height; // ��������摜�̍���
	tjs_int BGColor; // �w�i�F
	tjs_int Phase; // �A�j���[�V�����̃t�F�[�Y
	bool First; // ��ԍŏ��̌Ăяo�����ǂ���

public:
	tTVPTurnTransHandler(tjs_uint64 time, tjs_int width, tjs_int height, tjs_uint32 bgcolor)
	{
		RefCount = 1;

		Width = width;
		Height = height;
		Time = time;
		BGColor = bgcolor;

		First = true;
	}

	virtual ~tTVPTurnTransHandler()
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
tjs_error TJS_INTF_METHOD tTVPTurnTransHandler::StartProcess(tjs_uint64 tick)
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

	// ���������]���n�߁A�Ō�ɉE�オ��]���I����܂ŏ���������


	CurTime = (tick - StartTick);
	if(CurTime > Time) CurTime = Time;
	int xcount = (Width-1) / 64 + 1;
	int ycount = (Height-1) / 64 + 1;
	Phase = CurTime * (64 + (xcount +  ycount) *TURN_WIDTH_FACTOR) / Time -
		ycount *TURN_WIDTH_FACTOR;


	return TJS_S_TRUE;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPTurnTransHandler::EndProcess()
{
	// �g�����W�V�����̉�ʍX�V��񕪂��I��邲�ƂɌĂ΂��

	if(CurTime == Time) return TJS_S_FALSE; // �g�����W�V�����I��

	return TJS_S_TRUE;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTVPTurnTransHandler::Process(
			tTVPDivisibleData *data)
{
	// �g�����W�V�����̊e�̈悲�ƂɌĂ΂��
	// �g���g���͉�ʂ��X�V����Ƃ��ɂ������̗̈�ɕ������Ȃ��珈�����s���̂�
	// ���̃��\�b�h�͒ʏ�A��ʍX�V���ɂ�������Ă΂��

	// data �ɂ͗̈��摜�Ɋւ����񂪓����Ă���

	// data->Left, data->Top, data->Width, data->Height �Ŏ�������`��
	// �̂ݓ]������K�v������B�����ōs�������� '���U�C�N' �̃g�����W�V����
	// �̏����Ɏ��Ă��āA�ȉ��̒ʂ�B
	// 1: ���̓]����`�Ɋ܂܂��u���b�N�͈̔͂𔻒肷��
	// 2: ��ʈ�ԉ��̃u���b�N�̓A�N�Z�X�I�[�o�[�����ɋC�����ē]������
	// 3: ���͈̔͂̍��[�ƉE�[�̃u���b�N�́A�㉺�̃N���b�s���O�ɉ����A
	//    ���E�̃N���b�s���O���s���Ȃ���]������
	// 4: ����ȊO�̃u���b�N�͏㉺�̃N���b�s���O�݂̂��s���Ȃ���]������
	// ���Ȃ݂ɋg���g���͒ʏ� 8 ���C�����Ƃ̉��ɍג����̈���ォ�珇��
	// �w�肵�Ă���B
	// �u���b�N�T�C�Y�� 64x64 �Œ�B

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

	startx = data->Left / 64;
	starty = data->Top / 64;
	endx = (data->Left + data->Width - 1) / 64;
	endy = (data->Top + data->Height - 1) / 64;

	// 2: ��ʈ�ԉ��̃u���b�N�̓A�N�Z�X�I�[�o�[�����ɋC�����ē]������
	// 3: ���͈̔͂̍��[�ƉE�[�̃u���b�N�́A�㉺�̃N���b�s���O�ɉ����A
	//    ���E�̃N���b�s���O���s���Ȃ���]������
	// 4: ����ȊO�̃u���b�N�͏㉺�̃N���b�s���O�݂̂��s���Ȃ���]������

	for(int y = starty; y <= endy; y++)
	{
		// �����悤�Ȃ̂�������o�Ă��ĉ������ǁA����

		for(int x = startx; x <= endx; x++)
		{
			tjs_int phase = Phase - (x - y) * TURN_WIDTH_FACTOR;
			if(phase < 0) phase = 0;
			if(phase > 63) phase = 63;
			tjs_int gl = gloss[phase];
			if(y * 64 + 64 >= Height || x == startx || x == endx)
			{
				// �������A�N�Z�X�I�[�o�[�����̉\��������
				// ���邢�� ���[ �E�[�̃u���b�N
				tjs_int l = (x) * 64;
				tjs_int t = (y) * 64;
				tjs_int r = l + 64;
				tjs_int b = t + 64;
				if(Clip(l, r, data->Left, data->Left + data->Width) &&
					Clip(t, b, data->Top, data->Top + data->Height))
				{
					// l, t, r, b �͊��ɃN���b�v���ꂽ�̈��\���Ă���

					// phase ������

					if(phase == 0)
					{
						// ���S�� src1
						tjs_uint8 * dp = dest + (t + destyofs) * destpitch +
							(l + destxofs) * sizeof(tjs_uint32);
						const tjs_uint8 * sp = src1 + t * src1pitch +
							l * sizeof(tjs_uint32);
						tjs_int count = b - t;
						tjs_int len = (r - l) * sizeof(tjs_uint32);
						while(count--)
							memcpy(dp, sp, len), dp += destpitch, sp += src1pitch;
					}
					else if(phase == 63)
					{
						// ���S�� src2
						tjs_uint8 * dp = dest + (t + destyofs) * destpitch +
							(l + destxofs) * sizeof(tjs_uint32);
						const tjs_uint8 * sp = src2 + t * src2pitch +
							l * sizeof(tjs_uint32);
						tjs_int count = b - t;
						tjs_int len = (r - l) * sizeof(tjs_uint32);
						while(count--)
							memcpy(dp, sp, len), dp += destpitch, sp += src2pitch;
					}
					else
					{
						// �]���p�����[�^�ƃ\�[�X������
						const tTurnTransParams *params = TurnTransParams[phase];
						const tjs_uint8 * src;
						tjs_int srcpitch;
						if(phase < 32)  
						{
							src = src1;
							srcpitch = src1pitch;
						}
						else
						{
							src = src2;
							srcpitch = src2pitch;
						}

						tjs_int line = t - y * 64;  // �J�n���C�� ( 0 .. 63 )
						tjs_int start = l - x * 64; // ���[�̐؂����镔�� ( 0 .. 63 )
						tjs_int end = r - x * 64; // �E�[

						params += line;

						src += x * 64 * sizeof(tjs_uint32);

						tjs_int count = b - t;
						tjs_uint8 *dp =
							(tjs_uint8*)
							((tjs_uint32*)(dest + (t + destyofs) * destpitch)
								+ x * 64 + destxofs);
						while(count --)
						{
							tjs_int fl, fr;

							// ���̔w�i
							fl = 0;
							fr = params->start;
							if(Clip(fl, fr, start, end))
							{
								// fl-fr ��w�i�F�œh��Ԃ�
								TVPFillARGB((tjs_uint32*)dp + fl, fr - fl, BGColor);
							}

							// �E�̔w�i
							fl = params->start + params->len;
							fr = 64;
							if(Clip(fl, fr, start, end))
							{
								// fl-fr ��w�i�F�œh��Ԃ�
								TVPFillARGB((tjs_uint32*)dp + fl, fr - fl, BGColor);
							}

							// �ό`�]��
							fl = params->start;
							fr = params->start + params->len;
							if(Clip(fl, fr, start, end))
							{
								tjs_int sx = params->sx;
								tjs_int sy = params->sy;
								sx += params->stepx * (fl - params->start);
								sy += params->stepy * (fl - params->start);
								if(gl)
								{
									for(; fl < fr; fl++)
									{
										tjs_int yy = y * 64 + (sy >> 16);
										if(yy >= Height)
											((tjs_uint32*)dp)[fl] = BGColor;
										else
											((tjs_uint32*)dp)[fl] = Blend(
												*(const tjs_uint32*)
													(src + (sx >> 16) * sizeof(tjs_uint32) +
													yy * srcpitch),
													0xffffff, gl);
										sx += params->stepx;
										sy += params->stepy;
									}
								}
								else
								{
									for(; fl < fr; fl++)
									{
										tjs_int yy = y * 64 + (sy >> 16);
										if(yy >= Height)
											((tjs_uint32*)dp)[fl] = BGColor;
										else
											((tjs_uint32*)dp)[fl] =
												*(const tjs_uint32*)
													(src + (sx >> 16) * sizeof(tjs_uint32) +
													yy * srcpitch);
										sx += params->stepx;
										sy += params->stepy;
									}
								}
							}
							dp += destpitch;
							params ++;
						}
					}
				}
			}
			else
			{
				// �E�[�A���[�A�A�N�Z�X�I�[�o�[�����ɂ͒��ӂ����ɓ]��
				tjs_int l = (x) * 64;
				tjs_int t = (y) * 64;
				tjs_int r = l + 64;
				tjs_int b = t + 64;
				if(Clip(t, b, data->Top, data->Top + data->Height))
				{
					// l, t, r, b �͊��ɃN���b�v���ꂽ�̈��\���Ă���

					// phase ������

					if(phase == 0)
					{
						// ���S�� src1
						tjs_uint8 * dp = dest + (t + destyofs) * destpitch +
							(l + destxofs) * sizeof(tjs_uint32);
						const tjs_uint8 * sp = src1 + t * src1pitch +
							l * sizeof(tjs_uint32);
						tjs_int count = b - t;
						tjs_int len = (r - l) * sizeof(tjs_uint32);
						while(count--)
							memcpy(dp, sp, len), dp += destpitch, sp += src1pitch;
					}
					else if(phase == 63)
					{
						// ���S�� src2
						tjs_uint8 * dp = dest + (t + destyofs) * destpitch +
							(l + destxofs) * sizeof(tjs_uint32);
						const tjs_uint8 * sp = src2 + t * src2pitch +
							l * sizeof(tjs_uint32);
						tjs_int count = b - t;
						tjs_int len = (r - l) * sizeof(tjs_uint32);
						while(count--)
							memcpy(dp, sp, len), dp += destpitch, sp += src2pitch;
					}
					else
					{
						// �]���p�����[�^�ƃ\�[�X������
						const tTurnTransParams *params = TurnTransParams[phase];
						const tjs_uint8 * src;
						tjs_int srcpitch;
						if(phase < 32)  
						{
							src = src1;
							srcpitch = src1pitch;
						}
						else
						{
							src = src2;
							srcpitch = src2pitch;
						}

						tjs_int line = t - y * 64;  // �J�n���C�� ( 0 .. 63 )

						params += line;

						src += l * sizeof(tjs_uint32) + y * 64 * srcpitch;

						tjs_int count = b - t;
						tjs_uint8 *dp =
							(tjs_uint8*)
							((tjs_uint32*)(dest + (t + destyofs) * destpitch)
								+ l + destxofs);
						while(count --)
						{
							tjs_int fl, fr;

							// ���̔w�i
							// 0-params->start ��w�i�F�œh��Ԃ�
							TVPFillARGB((tjs_uint32*)dp + 0, params->start, BGColor);

							// �E�̔w�i
							fl = params->start + params->len;
							// fl-64 ��w�i�F�œh��Ԃ�
							TVPFillARGB((tjs_uint32*)dp + fl, 64 - fl, BGColor);

							// �ό`�]��
							fl = params->start;
							fr = params->start + params->len;
							tjs_int sx = params->sx;
							tjs_int sy = params->sy;
							if(gl)
							{
								for(; fl < fr; fl++)
								{
									((tjs_uint32*)dp)[fl] = Blend(
										*(const tjs_uint32*)
											(src + (sx >> 16) * sizeof(tjs_uint32) +
											(sy >> 16) * srcpitch),
											0xffffff, gl);
									sx += params->stepx;
									sy += params->stepy;
								}
							}
							else
							{
								TVPLinTransCopy((tjs_uint32*)dp + fl, fr - fl,
									(const tjs_uint32*)src, sx, sy,
									params->stepx, params->stepy, srcpitch);
/*
								for(; fl < fr; fl++)
								{
									((tjs_uint32*)dp)[fl] =
										*(const tjs_uint32*)
											(src + (sx >> 16) * sizeof(tjs_uint32) +
											(sy >> 16) * srcpitch);
									sx += params->stepx;
									sy += params->stepy;
								}
*/
							}
							dp += destpitch;
							params ++;
						}
					}
				}
			}
		}
	}

	return TJS_S_OK;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
class tTVPTurnTransHandlerProvider : public iTVPTransHandlerProvider
{
	tjs_uint RefCount; // �Q�ƃJ�E���^
public:
	tTVPTurnTransHandlerProvider() { RefCount = 1; }
	~tTVPTurnTransHandlerProvider() {; }

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
		if(name) *name = TJS_W("turn");
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
		tjs_uint32 bgcolor = 0;

		if(TJS_FAILED(options->GetValue(TJS_W("time"), &tmp)))
			return TJS_E_FAIL; // time �������w�肳��Ă��Ȃ�
		if(tmp.Type() == tvtVoid) return TJS_E_FAIL;
		time = (tjs_int64)tmp;
		if(time < 2) time = 2; // ���܂菬���Ȑ��l���w�肷��Ɩ�肪�N����̂�

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("bgcolor"), &tmp)))
		if(tmp.Type() != tvtVoid) bgcolor = (tjs_int)tmp;

		// �I�u�W�F�N�g���쐬
		*handler = new tTVPTurnTransHandler(time, src1w, src1h, bgcolor);

		return TJS_S_OK;
	}

} static * TurnTransHandlerProvider;
//---------------------------------------------------------------------------
void RegisterTurnTransHandlerProvider()
{
	// TVPAddTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^����
	TurnTransHandlerProvider = new tTVPTurnTransHandlerProvider();
	TVPAddTransHandlerProvider(TurnTransHandlerProvider);
}
//---------------------------------------------------------------------------
void UnregisterTurnTransHandlerProvider()
{
	// TVPRemoveTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^��������
	TVPRemoveTransHandlerProvider(TurnTransHandlerProvider);
	TurnTransHandlerProvider->Release();
}
//---------------------------------------------------------------------------



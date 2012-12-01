//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
#include "rotatebase.h"
#include "rotatetrans.h"
#include "common.h"

#include <stdio.h>

//---------------------------------------------------------------------------
/*
	�؂�ւ����A�؂�ւ���̉摜���N���N���񂷌n�̃g�����W�V�����̎������B
	�����ɋL�q���Ă��邷�ׂẴg�����W�V�����n���h���� tTVPBaseRotateTransHandler
	���p�����Ă���B
*/
//---------------------------------------------------------------------------
class tTVPRotateZoomTransHandler : public tTVPBaseRotateTransHandler
{
	// ��ʂ̒��S�ŉ�]���Ȃ���Y�[���C���A���邢�̓Y�[���A�E�g����g�����W�V����
	// (KAG�ł�)����ʂ���ɉ�]���ăY�[���C���A���邢�̓Y�[���A�E�g����

	double Factor; // �����Y�[���g�嗦
	double TargetFactor; // �ŏI�Y�[���g�嗦
	double Accel; // �����x�I�ȓ������s�킹�邩�ǂ��� ( 0 = �s��Ȃ� )
	double Twist; // ������]�ʒu
	double TwistAccel; // ��]�ɉ����x�I�ȓ������s�킹�邩�ǂ���
	tjs_int CenterX; // ��]���S X �ʒu
	tjs_int CenterY; // ��]���S Y �ʒu
	bool FixSrc1; // src1 ���Œ肷�邩


public:
	tTVPRotateZoomTransHandler(tjs_uint64 time,
		tjs_int width, tjs_int height,
			double factor,
			double targetfactor,
			double accel,
			double twist,
			double twistaccel,
			tjs_int centerx,
			tjs_int centery,
			bool fixsrc1)
			: tTVPBaseRotateTransHandler(time, width, height, 0)
	{
		Factor = factor;
		TargetFactor = targetfactor;
		Accel = accel;
		Twist = twist;
		TwistAccel = twistaccel;
		CenterX = centerx;
		CenterY = centery;
		FixSrc1 = fixsrc1;
	}


	void CalcPosition()
	{
		// src1, src2 �̉�ʈʒu��ݒ肷��

		// src1
		// src1 �͏�ɉ�ʑS�̌Œ�
		tPoint points[3];

		points[0].x = 0; points[0].y = 0;
		points[1].x = Width - 1; points[1].y = 0;
		points[2].x = 0; points[2].y = Height - 1;

		AddSource(points, FixSrc1 ? 1 : 2);

		// src2
		float zm = (float)(tjs_int)CurTime / (float)(tjs_int)Time;
		float tm = zm;
		if(Accel < 0)
		{
			// �㌷ ( �ŏ��������������A���X�ɒx���Ȃ� )
			zm = 1.0 - zm;
			zm = pow(zm, -Accel);
			zm = 1.0 - zm;
		}
		else if(Accel > 0)
		{
			// ���� ( �ŏ��͓������x���A���X�ɑ����Ȃ� )
			zm = pow(zm, Accel);
		}

		tjs_int scx = Width/2;
		tjs_int scy = Height/2;
		tjs_int cx = (scx - CenterX) * zm + CenterX;
		tjs_int cy = (scy - CenterY) * zm + CenterY;

		if(TwistAccel < 0)
		{
			// �㌷ ( �ŏ��������������A���X�ɒx���Ȃ� )
			tm = 1.0 - tm;
			tm = pow(tm, -TwistAccel);
			tm = 1.0 - tm;
		}
		else if(TwistAccel > 0)
		{
			// ���� ( �ŏ��͓������x���A���X�ɑ����Ȃ� )
			tm = pow(tm, TwistAccel);
		}

		float rad = CurTime == Time ? 0 : 2 * 3.14159265368979 * Twist * tm;
		zm = (TargetFactor - Factor) * zm + Factor;
		float s = sin(rad) * zm;
		float c = cos(rad) * zm;

		points[0].x = -cx *  c + -cy * s + scx;
		points[0].y = -cx * -s + -cy * c + scy;
		points[1].x = (Width-1-cx) *  c + -cy * s + scx;
		points[1].y = (Width-1-cx) * -s + -cy * c + scy;
		points[2].x = -cx *  c + (Height-1-cy) * s + scx;
		points[2].y = -cx * -s + (Height-1-cy) * c + scy;

		AddSource(points, FixSrc1 ? 2 : 1);
	}

};
//---------------------------------------------------------------------------
class tTVPRotateZoomTransHandlerProvider : public iTVPTransHandlerProvider
{
	tjs_uint RefCount; // �Q�ƃJ�E���^
public:
	tTVPRotateZoomTransHandlerProvider() { RefCount = 1; }
	~tTVPRotateZoomTransHandlerProvider() {; }

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
		if(name) *name = TJS_W("rotatezoom");
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
		double factor = 1;
		double accel = 0;
		double twist = 2;
		double twistaccel = -2;
		tjs_int centerx = src1w / 2;
		tjs_int centery = src1h / 2;

		if(TJS_FAILED(options->GetValue(TJS_W("time"), &tmp)))
			return TJS_E_FAIL; // time �������w�肳��Ă��Ȃ�
		if(tmp.Type() == tvtVoid) return TJS_E_FAIL;
		time = (tjs_int64)tmp;
		if(time < 2) time = 2; // ���܂菬���Ȑ��l���w�肷��Ɩ�肪�N����̂�

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("factor"), &tmp)))
			if(tmp.Type() != tvtVoid) factor = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("accel"), &tmp)))
			if(tmp.Type() != tvtVoid) accel = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("twist"), &tmp)))
			if(tmp.Type() != tvtVoid) twist = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("twistaccel"), &tmp)))
			if(tmp.Type() != tvtVoid) twistaccel = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("centerx"), &tmp)))
			if(tmp.Type() != tvtVoid) centerx = (tjs_int)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("centery"), &tmp)))
			if(tmp.Type() != tvtVoid) centery = (tjs_int)tmp;

		// �I�u�W�F�N�g���쐬
		*handler = new tTVPRotateZoomTransHandler(time,
			src1w, src1h,
			factor, 1, accel, twist, twistaccel, centerx, centery, true);

		return TJS_S_OK;
	}

} static * RotateZoomTransHandlerProvider;
//---------------------------------------------------------------------------
class tTVPRotateVanishTransHandlerProvider : public iTVPTransHandlerProvider
{
	tjs_uint RefCount; // �Q�ƃJ�E���^
public:
	tTVPRotateVanishTransHandlerProvider() { RefCount = 1; }
	~tTVPRotateVanishTransHandlerProvider() {; }

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
		if(name) *name = TJS_W("rotatevanish");
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
		double accel = 2;
		double twist = 2;
		double twistaccel = 2;
		tjs_int centerx = src1w / 2;
		tjs_int centery = src1h / 2;

		if(TJS_FAILED(options->GetValue(TJS_W("time"), &tmp)))
			return TJS_E_FAIL; // time �������w�肳��Ă��Ȃ�
		if(tmp.Type() == tvtVoid) return TJS_E_FAIL;
		time = (tjs_int64)tmp;
		if(time < 2) time = 2; // ���܂菬���Ȑ��l���w�肷��Ɩ�肪�N����̂�

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("accel"), &tmp)))
			if(tmp.Type() != tvtVoid) accel = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("twist"), &tmp)))
			if(tmp.Type() != tvtVoid) twist = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("twistaccel"), &tmp)))
			if(tmp.Type() != tvtVoid) twistaccel = (double)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("centerx"), &tmp)))
			if(tmp.Type() != tvtVoid) centerx = (tjs_int)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("centery"), &tmp)))
			if(tmp.Type() != tvtVoid) centery = (tjs_int)tmp;

		// �I�u�W�F�N�g���쐬
		*handler = new tTVPRotateZoomTransHandler(time,
			src1w, src1h,
			1, 0, accel, twist, twistaccel, centerx, centery, false);

		return TJS_S_OK;
	}

} static * RotateVanishTransHandlerProvider;
//---------------------------------------------------------------------------
class tTVPRotateSwapTransHandler : public tTVPBaseRotateTransHandler
{
	// �\��ʂ���]���Ȃ��牓������A����ʂ���]���Ȃ���ߊ��g�����W�V����
	float Twist; // ��]��

public:
	tTVPRotateSwapTransHandler(tjs_uint64 time,
		tjs_int width, tjs_int height,
			tjs_uint32 bgcolor, double twist)
			: tTVPBaseRotateTransHandler(time, width, height, bgcolor)
	{
		Twist = twist * 3.14159265368979 * 2;
	}


	void CalcPosition()
	{
		// src1, src2 �̉�ʈʒu��ݒ肷��
		tPoint points[3];
		float rad;
		tjs_int cx, cy;
		tjs_int scx = Width/2;
		tjs_int scy = Height/2;
		float zm = (float)(tjs_int)CurTime / (float)(tjs_int)Time;
		float tm, s, c;

		tjs_int cnt = 2;

		switch(CurTime >= Time / 2 ? 0:1)
		{
			while(true)
			{
			case 0:
				// src1
				tm = zm * zm;
				cx = ( - scx ) * tm + scx + sin(tm * 3.14159265368979)*scx*1.5;
				cy = ( - scy ) * tm + scy;
				rad = tm * Twist;
				tm = 1.0 - tm;
				s = sin(rad) * tm;
				c = cos(rad) * tm;

				points[0].x = -scx *  c + -scy * s + cx;
				points[0].y = (-scx * -s + -scy * c) * tm + cy;
				points[1].x = (Width-1-scx) *  c + -scy * s + cx;
				points[1].y = ((Width-1-scx) * -s + -scy * c) * tm + cy;
				points[2].x = -scx *  c + (Height-1-scy) * s + cx;
				points[2].y = (-scx * -s + (Height-1-scy) * c) * tm + cy;
				AddSource(points, 1);
				cnt--;
				if(cnt == 0) break;

			case 1:
				// src2
				tm = 1.0 - (1.0 - zm) * (1.0 - zm);
				cx = (scx - (Width  - 1)) * tm + (Width  - 1) - sin(tm * 3.14159265368979) * scx*1.5;
				cy = (scy - (Height - 1)) * tm + (Height - 1);
				rad = (-1.0 + tm) * Twist;
				s = sin(rad) * tm;
				c = cos(rad) * tm;

				points[0].x = -scx *  c + -scy * s + cx;
				points[0].y = (-scx * -s + -scy * c) * tm + cy;
				points[1].x = (Width-1-scx) *  c + -scy * s + cx;
				points[1].y = ((Width-1-scx) * -s + -scy * c) * tm + cy;
				points[2].x = -scx *  c + (Height-1-scy) * s + cx;
				points[2].y = (-scx * -s + (Height-1-scy) * c) * tm + cy;
				AddSource(points, 2);
				cnt--;
				if(cnt == 0) break;
			}
		}
	}

};
//---------------------------------------------------------------------------
class tTVPRotateSwapTransHandlerProvider : public iTVPTransHandlerProvider
{
	tjs_uint RefCount; // �Q�ƃJ�E���^
public:
	tTVPRotateSwapTransHandlerProvider() { RefCount = 1; }
	~tTVPRotateSwapTransHandlerProvider() {; }

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
		if(name) *name = TJS_W("rotateswap");
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
		double twist = 1;

		if(TJS_FAILED(options->GetValue(TJS_W("time"), &tmp)))
			return TJS_E_FAIL; // time �������w�肳��Ă��Ȃ�
		if(tmp.Type() == tvtVoid) return TJS_E_FAIL;
		time = (tjs_int64)tmp;
		if(time < 2) time = 2; // ���܂菬���Ȑ��l���w�肷��Ɩ�肪�N����̂�

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("bgcolor"), &tmp)))
			if(tmp.Type() != tvtVoid) bgcolor = (tjs_int)tmp;

		if(TJS_SUCCEEDED(options->GetValue(TJS_W("twist"), &tmp)))
			if(tmp.Type() != tvtVoid) twist = (double)tmp;

		// �I�u�W�F�N�g���쐬
		*handler = new tTVPRotateSwapTransHandler(time,
			src1w, src1h, bgcolor, twist);

		return TJS_S_OK;
	}

} static * RotateSwapTransHandlerProvider;
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void RegisterRotateTransHandlerProvider()
{
	// TVPAddTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^����
	RotateZoomTransHandlerProvider = new tTVPRotateZoomTransHandlerProvider();
	TVPAddTransHandlerProvider(RotateZoomTransHandlerProvider);

	RotateVanishTransHandlerProvider = new tTVPRotateVanishTransHandlerProvider();
	TVPAddTransHandlerProvider(RotateVanishTransHandlerProvider);

	RotateSwapTransHandlerProvider = new tTVPRotateSwapTransHandlerProvider();
	TVPAddTransHandlerProvider(RotateSwapTransHandlerProvider);
}
//---------------------------------------------------------------------------
void UnregisterRotateTransHandlerProvider()
{
	// TVPRemoveTransHandlerProvider ���g���ăg�����W�V�����n���h���v���o�C�_��
	// �o�^��������
	TVPRemoveTransHandlerProvider(RotateZoomTransHandlerProvider);
	RotateZoomTransHandlerProvider->Release();

	TVPRemoveTransHandlerProvider(RotateVanishTransHandlerProvider);
	RotateVanishTransHandlerProvider->Release();

	TVPRemoveTransHandlerProvider(RotateSwapTransHandlerProvider);
	RotateSwapTransHandlerProvider->Release();
}
//---------------------------------------------------------------------------


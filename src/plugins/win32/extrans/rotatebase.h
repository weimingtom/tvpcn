//---------------------------------------------------------------------------

#ifndef rotatebaseH
#define rotatebaseH

#include "tp_stub.h"


//---------------------------------------------------------------------------
struct tPoint
{
	tjs_int x;
	tjs_int y;
};
//---------------------------------------------------------------------------
struct tRotateDrawLine
{
	tjs_int start; // destination start
	tjs_int sx; // source start x
	tjs_int sy; // source start y
	tjs_int stepx; // source step x
	tjs_int stepy; // source step y
};
//---------------------------------------------------------------------------
struct tRotateDrawRegionData
{
	tjs_int left; // left position of destination x
	tjs_int right; // right position of destination x
	tjs_int type; // 0 = bgcolor, 1 = src1, 2 = src2
};
//---------------------------------------------------------------------------
struct tRotateDrawData
{
	int count;
	tRotateDrawLine src1;
	tRotateDrawLine src2;
	tRotateDrawRegionData region[5];
};
//---------------------------------------------------------------------------
class tTVPBaseRotateTransHandler : public iTVPDivisibleTransHandler
{
	//	��]���s���g�����W�V�����n���h�����N���X�̎���

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

	tRotateDrawData * DrawData; // �`��p�f�[�^

public:
	tTVPBaseRotateTransHandler(tjs_uint64 time,
		tjs_int width, tjs_int height, tjs_uint32 bgcolor);

	virtual ~tTVPBaseRotateTransHandler();

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

protected:

	void AddLine(tjs_int line, tjs_int left, tjs_int right, tjs_int type);
	void AddSource(const tPoint *points, tjs_int type);

	virtual void CalcPosition() = 0;
		// ��`�̈ʒu���v�Z����
		// ���ʃN���X�Ŏ������邱��
};
//---------------------------------------------------------------------------
#endif

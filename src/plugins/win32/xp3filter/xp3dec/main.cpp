//---------------------------------------------------------------------------
/*
	���̃\�[�X�ł́Axp3enc.dll ���g���� Releaser �ňÍ������� XP3 �t�@�C����
	������悤�ɂ���g���g���p�v���O�C����������܂��B

	�v���O�C���̃t�@�C�����́A�g���q�� tpm �Ȃ�ΔC�ӂ̕��ł��܂��܂���B
	�g���g���͊g���q�� tpm �̃t�@�C����������� startup.tjs ��ǂݍ�������
	����O�A���Ȃ킿 XP3 �t�@�C�����ŏ��ɊJ�����O�ɓǂݍ��ނ̂ŁA�g���q��
	tpm �ł���K�v������܂��B
	�g���q�� tpm �Ȃ�΃t�@�C������(�v���O�C���Ԃ̈ˑ������Ȃ����)�Ȃ�ł�
	���܂��܂��񂪁A�����ł͕֋X��Axp3dec.tpm �Ƃ��܂��B

	xp3enc.dll �ƈقȂ�Axp3dec.tpm �͋g���g���{�̂̃v���O�C���Ƃ��ē��삵��
	���B���̂��߁A�g���g���{�̗p�̃v���O�C���̍�@�ɏ]���č쐬����Ȃ���΂�
	��Ȃ�܂��񂪁Atp_stub.h ����ė��p�\�ɂȂ�A�g���g���{�̂̊e�@�\��
	���p���邱�Ƃ��ł��܂��B�������ATVPSetXP3ArchiveExtractionFilter �͕���
	�X���b�h���瓯���ɌĂяo�����\��������܂����Atp_stub.h ����ė��p
	�\�ɂȂ�֐��͂قƂ�ǂ����C���X���b�h����̌Ăяo���݂̂ɑΉ����Ă���
	�̂Œ��ӂ��Ă��������B

	�Í������Ɋւ��鏔���ӂɂ��āA xp3enc.dll �̃T���v���̃\�[�X main.cpp
	���Q�Ƃ��Ă��������B
*/





//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
	// tp_stub.h �ɂ̓C���N���[�h�E�p�X���w�肵�Ă����܂��B
	// tp_stub.cpp ����� tp_stub.h �̃o�[�W�����͋g���g���{�̂�
	// �������̕��ł��邱�Ƃ��D�܂����ł��B
	// (�������{�̑��̃C���^�[�t�F�[�X���ς��Ȃ���΁A�Â�
	// xp3dec.dll �ł��p�����ė��p�ł��܂�)
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void TVP_tTVPXP3ArchiveExtractionFilter_CONVENTION
	TVPXP3ArchiveExtractionFilter(tTVPXP3ExtractionFilterInfo *info)
{
	// TVPXP3ArchiveExtractionFilter �֐��͖{�̑�����Ăяo�����
	// �R�[���o�b�N�֐��ł��B
	// ����������A����� tTVPXP3ExtractionFilterInfo �\���̂ւ̃|�C���^
	// �ł��B

	// TVPXP3ArchiveExtractionFilter �́A��q�� V2Link �֐�����
	// TVPSetXP3ArchiveExtractionFilter �ɂ��ݒ肳��܂��B

	// �����ł͒P���ɁAxp3enc.dll �̃T���v���ō쐬���ꂽ XP3 �A�[�J�C�u��
	// �������ׂ��A�f�[�^�����ׂ� FileHash �̍ŉ��ʃo�C�g�� XOR
	// ���邱�Ƃɂ��܂��B

	// ���̊֐��͕����̃X���b�h���瓯���ɌĂяo�����\��������̂�
	// ���ӂ��Ă��������B

	/*
		tTVPXP3ExtractionFilterInfo �̃����o�͈ȉ��̒ʂ�
		* SizeOfSelf        : �������g�̍\���̂̃T�C�Y
		* Offset            : "Buffer" �����o���w�������f�[�^���A
		*                   : �A�[�J�C�u�Ɋi�[����Ă��邻�̃t�@�C���̐擪�����
		*                   : �ǂ̃I�t�Z�b�g�ʒu����̂��̂��A��\��
		* Buffer            : �f�[�^�{��
		* BufferSize        : "Buffer" �����o�̎w�������f�[�^�̃T�C�Y(�o�C�g�P��)
		* FileHash          : �t�@�C���̈Í���������Ԃł̃t�@�C�����e��32bit�n�b�V��
	*/

	// �ꉞ�\���̂̃T�C�Y���`�F�b�N����
	if(info->SizeOfSelf != sizeof(tTVPXP3ExtractionFilterInfo))
	{
		// �\���̂̃T�C�Y���Ⴄ�ꍇ�͂����ŃG���[�ɂ��������悢
		TVPThrowExceptionMessage(TJS_W("Incompatible tTVPXP3ExtractionFilterInfo size"));
			// TVPThrowExceptionMessage �͗�O���b�Z�[�W�𓊂���֐�
			// ���̊֐��͖߂�Ȃ� ( �����ƌĂяo�����������̂ڂ����ʒu��
			// ��O���⑫����邽�� )
	}

	// ����
	tjs_uint i;
	for(i = 0; i < info->BufferSize; i++)
		((unsigned char *)info->Buffer)[i] ^= info->FileHash;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
//#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// V2Link �́A�g���g���{�̂ƃ����N�����Ƃ��ɌĂяo�����֐��ł�
	
	// �X�^�u�̏�����(�K���L�q����)
	TVPInitImportStub(exporter);

	// TVPSetXP3ArchiveExtractionFilter �֐���
	// TVPXP3ArchiveExtractionFilter ���w�肵�A�R�[���o�b�N�֐���ݒ肷��
	TVPSetXP3ArchiveExtractionFilter(TVPXP3ArchiveExtractionFilter);

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall V2Unlink()
{
	// V2Unlink �́A�g���g���{�̂���؂藣�����Ƃ��ɌĂяo�����֐��ł�

	// �ꉞ TVPSetXP3ArchiveExtractionFilter �� NULL ��n���A
	// �R�[���o�b�N�֐��̓o�^����������
	TVPSetXP3ArchiveExtractionFilter(NULL);

	// �X�^�u�̎g�p�I��(�K���L�q����)
	TVPUninitImportStub();

	return S_OK; 
}
//---------------------------------------------------------------------------




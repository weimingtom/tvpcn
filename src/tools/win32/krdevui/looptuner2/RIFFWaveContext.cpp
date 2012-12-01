//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RIFFWaveContext.h"

//---------------------------------------------------------------------------
const GUID __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT =
		{ 0x00000003 , 0x0000, 0x0010, { 0x80,0x00,0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }};
const GUID __KSDATAFORMAT_SUBTYPE_PCM =
		{ 0x00000001 , 0x0000, 0x0010, { 0x80,0x00,0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }};
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
static void ConvertFloatTo16bits(short int *output, const float *input, int channels, int count)
{
	// �F�r��Ǥ� 32bit float �Τߤ��Q����

	// float PCM �� +1.0  0  -1.0 �ι���ˤ���
	// ������Υǩ`���ϥ���åפ���
	int total = channels * count;
	while(total--)
	{
		float t = *(input ++) * 32768.0;
		if(t > 0)
		{
			int i = (int)(t + 0.5);
			if(i > 32767) i = 32767;
			*(output++) = (short int)i;
		}
		else
		{
			int i = (int)(t - 0.5);
			if(i < -32768) i = -32768;
			*(output++) = (short int)i;
		}
	}

}
//---------------------------------------------------------------------------
static void ConvertIntTo16bits(short int *output, const void *input, int bytespersample,
	int validbits, int channels, int count)
{
	// ������ int16 �ʤɤΥ������ΛQ�ޤä��ͤ�Ĥ����٤�

	// ������F�� PCM �� 16bit �ˉ�Q����
	int total = channels * count;

	if(bytespersample == 1)
	{
		// �i���z�ߕr�Υ����å��Ǥ��Ǥ� 8bit �����Υǩ`���� �Є��ӥå����� 8 �ӥå�
		// �Ǥ��뤳�Ȥ����^����Ƥ���Τǅg�� 16bit �˒�����������ˤ���
		const char *p = (const char *)input;
		while(total--) *(output++) = (short int)( ((int)*(p++)-0x80) * 0x100);
	}
	else if(bytespersample == 2)
	{
		// �Є��ӥå������¤������� �ޥ�������ȡ��ȥ��
		short int mask =  ~( (1 << (16 - validbits)) - 1);
		const short int *p = (const short int *)input;
		while(total--) *(output++) = (short int)(*(p++) & mask);
	}
	else if(bytespersample == 3)
	{
		long int mask = ~( (1 << (24 - validbits)) - 1);
		const unsigned char *p = (const unsigned char *)input;
		while(total--)
		{
			int t = p[0] + (p[1] << 8) + (p[2] << 16);
			p += 3;
			t |= -(t&0x800000); // ���Œ���
			t &= mask; // �ޥ���
			t >>= 8;
			*(output++) = (short int)t;
		}
	}
	else if(bytespersample == 4)
	{
		long int mask = ~( (1 << (32 - validbits)) - 1);
		const unsigned __int32 *p = (const unsigned __int32 *)input;
		while(total--)
		{
			*(output++) = (short int)((*(p++) & mask) >> 16);
		}
	}

}
//---------------------------------------------------------------------------
static void ConvertTo16bits(short int *output, const void *input, int bytespersample,
	bool isfloat, int validbits, int channels, int count)
{
	// ָ�����줿�ե��`�ޥåȤ� 16bit �ե��`�ޥåȤˉ�Q����
	// channels �� -6 �Έ��Ϥ��؄e��
	// FL FC FR BL BR LF �ȁK��Ǥ���ǩ`�� (AC3��ͬ�����) ��
	// FL FR FC LF BL BR (WAVEFORMATEXTENSIBLE ���ڴ��������) �ˁK���椨��
	int cns = channels;
	if(cns<0) cns = -cns;

	if(isfloat)
		ConvertFloatTo16bits(output, (const float *)input, cns, count);
	else
		ConvertIntTo16bits(output, input, bytespersample, validbits, cns, count);

	if(channels == -6)
	{
		// �����ͥ������椨
		short int *op = output;
		for(int i = 0; i < count; i++)
		{
			short int fc = op[1];
			short int bl = op[3];
			short int br = op[4];
			op[1] = op[2];
			op[2] = fc;
			op[3] = op[5];
			op[4] = bl;
			op[5] = br;
			op += 6;
		}
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
__fastcall TRIFFWaveContext::TRIFFWaveContext()
{
	FInputStream = NULL;
}
//---------------------------------------------------------------------------
__fastcall TRIFFWaveContext::~TRIFFWaveContext()
{
	if(FInputStream) delete FInputStream;
}
//---------------------------------------------------------------------------
bool __fastcall TRIFFWaveContext::Start(AnsiString filename)
{
	try
	{
		FInputStream = new TFileStream(filename, fmOpenRead | fmShareDenyWrite);
	}
	catch(...)
	{
		return false;
	}

	// �إå����i��
	BYTE buf[8];
	FInputStream->Position=0;
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "RIFF", 4)) return false; // RIFF ���ʤ�
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "WAVEfmt ", 4)) return false; // WAVE �Ǥʤ��� fmt ����󥯤��������ʤ�

	DWORD fmtlen;
	FInputStream->ReadBuffer(&fmtlen,4);

	FInputStream->ReadBuffer(&Format, sizeof(WAVEFORMATEX));
	if(Format.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE &&
		Format.Format.wFormatTag != WAVE_FORMAT_PCM &&
		Format.Format.wFormatTag != WAVE_FORMAT_IEEE_FLOAT) return false; // ����Ǥ��ʤ��ե��`�ޥå�

	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		FInputStream->ReadBuffer(
			(char*)&Format + sizeof(Format.Format),
			sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX));
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_PCM)
	{
		Format.Samples.wValidBitsPerSample = Format.Format.wBitsPerSample;
		Format.SubFormat = __KSDATAFORMAT_SUBTYPE_PCM;
		Format.dwChannelMask = 0;
	}
	else if(Format.Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
	{
		Format.Samples.wValidBitsPerSample = Format.Format.wBitsPerSample;
		Format.SubFormat = __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		Format.dwChannelMask = 0;
	}

	// ���N�����å�
	if(Format.Format.wBitsPerSample & 0x07) return false; // 8 �α����ǤϤʤ�
	if(Format.Format.wBitsPerSample > 32) return false; // 32bit ���󤭤��������ϒQ���ʤ�
	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
		Format.Samples.wValidBitsPerSample < 8) return false; // �Є��ӥåȤ� 8 δ���ϒQ���ʤ�
	if(Format.Format.wBitsPerSample < Format.Samples.wValidBitsPerSample)
		return false; // �Є��ӥå������g�H�Υӥå������󤭤�
	if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 16))
		IsFloat = true;
	else if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_PCM, 16))
		IsFloat = false;
	else
		return false; // δ֪�Υ��֥ե��`�ޥå�

	if(IsFloat && Format.Format.wBitsPerSample != Format.Samples.wValidBitsPerSample)
		return false; // float ���Є��ӥå�������{�ӥå����ȵȤ����ʤ�
	if(IsFloat && Format.Format.wBitsPerSample != 32)
		return false; // float �Υӥå����� 32 �Ǥʤ�


	FGranuleSize = Format.Format.wBitsPerSample / 8 * Format.Format.nChannels;

	// data ����󥯤�̽��
	FInputStream->Position= fmtlen + 0x14;

	while(1)
	{
		FInputStream->ReadBuffer(buf,4); // data ?
		FInputStream->ReadBuffer(&FDataSize,4); // �ǩ`��������
		if(memcmp(buf,"data",4)==0) break;
		FInputStream->Position=FInputStream->Position+FDataSize;

		if(FInputStream->Position >= FInputStream->Size) return false;
	}

	FDataStart=FInputStream->Position;

	return true;
}
//---------------------------------------------------------------------------
int __fastcall TRIFFWaveContext::Read(__int16 * dest, int destgranules)
{
	int samplesize = FGranuleSize;
	destgranules *= samplesize; // data (in bytes) to read
	int bytestowrite;
	int remain = FDataStart + FDataSize - FInputStream->Position;
	int readsamples;

	char *buf = new char[destgranules];

	try
	{
		bytestowrite = (destgranules>remain) ? remain : destgranules;
		bytestowrite = FInputStream->Read(buf, bytestowrite);

		readsamples = bytestowrite / samplesize;

		// convert to the destination format
		ConvertTo16bits(dest, buf, Format.Format.wBitsPerSample / 8, IsFloat,
			Format.Samples.wValidBitsPerSample, Format.Format.nChannels, readsamples);
	}
	catch(...)
	{
		delete [] buf;
		throw;
	}
	delete [] buf;

	return readsamples;
}
//---------------------------------------------------------------------------

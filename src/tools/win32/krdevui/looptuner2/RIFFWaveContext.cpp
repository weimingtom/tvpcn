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
	// 現時点では 32bit float のみを変換する

	// float PCM は +1.0  0  -1.0 の範囲にある
	// 範囲外のデータはクリップする
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
	// 本当は int16 などのサイズの決まった型をつかうべき

	// 整数表現の PCM を 16bit に変換する
	int total = channels * count;

	if(bytespersample == 1)
	{
		// 読み込み時のチェックですでに 8bit 入力のデータは 有効ビット数が 8 ビット
		// であることが保証されているので単に 16bit に拡張するだけにする
		const char *p = (const char *)input;
		while(total--) *(output++) = (short int)( ((int)*(p++)-0x80) * 0x100);
	}
	else if(bytespersample == 2)
	{
		// 有効ビット数以下の数値は マスクして取り去る
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
			t |= -(t&0x800000); // 符号拡張
			t &= mask; // マスク
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
	// 指定されたフォーマットを 16bit フォーマットに変換する
	// channels が -6 の場合は特別に
	// FL FC FR BL BR LF と並んでいるデータ (AC3と同じ順序) を
	// FL FR FC LF BL BR (WAVEFORMATEXTENSIBLE の期待する順序) に並び替える
	int cns = channels;
	if(cns<0) cns = -cns;

	if(isfloat)
		ConvertFloatTo16bits(output, (const float *)input, cns, count);
	else
		ConvertIntTo16bits(output, input, bytespersample, validbits, cns, count);

	if(channels == -6)
	{
		// チャンネルの入れ替え
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

	// ヘッダを読む
	BYTE buf[8];
	FInputStream->Position=0;
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "RIFF", 4)) return false; // RIFF がない
	FInputStream->ReadBuffer(buf, 8);
	if(memcmp(buf, "WAVEfmt ", 4)) return false; // WAVE でないか fmt チャンクがすぐこない

	DWORD fmtlen;
	FInputStream->ReadBuffer(&fmtlen,4);

	FInputStream->ReadBuffer(&Format, sizeof(WAVEFORMATEX));
	if(Format.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE &&
		Format.Format.wFormatTag != WAVE_FORMAT_PCM &&
		Format.Format.wFormatTag != WAVE_FORMAT_IEEE_FLOAT) return false; // 対応できないフォーマット

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

	// 各種チェック
	if(Format.Format.wBitsPerSample & 0x07) return false; // 8 の倍数ではない
	if(Format.Format.wBitsPerSample > 32) return false; // 32bit より大きいサイズは扱えない
	if(Format.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
		Format.Samples.wValidBitsPerSample < 8) return false; // 有効ビットが 8 未満は扱えない
	if(Format.Format.wBitsPerSample < Format.Samples.wValidBitsPerSample)
		return false; // 有効ビット数が実際のビット数より大きい
	if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 16))
		IsFloat = true;
	else if(!memcmp(&Format.SubFormat, &__KSDATAFORMAT_SUBTYPE_PCM, 16))
		IsFloat = false;
	else
		return false; // 未知のサブフォーマット

	if(IsFloat && Format.Format.wBitsPerSample != Format.Samples.wValidBitsPerSample)
		return false; // float の有効ビット数が格納ビット数と等しくない
	if(IsFloat && Format.Format.wBitsPerSample != 32)
		return false; // float のビット数が 32 でない


	FGranuleSize = Format.Format.wBitsPerSample / 8 * Format.Format.nChannels;

	// data チャンクを探す
	FInputStream->Position= fmtlen + 0x14;

	while(1)
	{
		FInputStream->ReadBuffer(buf,4); // data ?
		FInputStream->ReadBuffer(&FDataSize,4); // データサイズ
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

//---------------------------------------------------------------------------
/*
	Risa [�ꤵ]      alias ���Ｊ��3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief Phase Vocoder �Όgװ
//---------------------------------------------------------------------------

/*
	Phase Vocoder (�ե��`�� �ܥ��`�� ; λ��ܥ��`��)�Όgװ

	�ο��Y��:

		http://www.panix.com/~jens/pvoc-dolson.par
			Phase Vocoder �Υ���`�ȥꥢ�롣���ߥ�`�������ˤ�狼��褦�ˡ�
			������Ƥ��ꡢ��ѧ���եե��ɥ�`��

		http://www.dspdimension.com/
			�o��(���`�ץ󥽩`���ǤϤʤ�)�� Time Stretcher/Pitch Shifter��
			DIRAC�䡢���N���르�ꥺ����h����
			Pitch Shifter ���h���Ĥʥ��`�����`�ɤʤɡ�

		http://soundlab.cs.princeton.edu/software/rt_pvc/
			real-time phase vocoder analysis/synthesis library + visualization
			���`�����ꡣ
*/

#include "tjsCommHead.h"

#include <math.h>
#include "PhaseVocoderDSP.h"
#include <string.h>

#include "tjsUtils.h"

#include "tvpgl_ia32_intf.h"
extern tjs_uint32 TVPCPUType;

extern "C"
{
#ifdef _WIN32
#define TVP_CDECL __cdecl
#endif
void TVP_CDECL sse__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi(tRisaPhaseVocoderDSP *_this, int ch);
void TVP_CDECL def__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi(tRisaPhaseVocoderDSP *_this, int ch);
void TVP_CDECL sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj(float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len);
void TVP_CDECL def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj(float * dest, const float * const * src,
					float * win, int numch, size_t srcofs, size_t len);
void TVP_CDECL sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj(float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len);
void TVP_CDECL def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj(float * dest[], const float * src,
					float * win, int numch, size_t destofs, size_t len);
}

//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::tRisaPhaseVocoderDSP(
				unsigned int framesize,
				unsigned int frequency, unsigned int channels) :
					InputBuffer(framesize * 4 * channels),
					OutputBuffer(framesize * 4 * channels)
		// InputBuffer ����ͤǤ�
		// channels * (framesize + (framesize/oversamp)) ��Ҫ�ǡ�
		// OutputBuffer ����ͤǤ�
		// channels * (framesize + (framesize/oversamp)*MAX_TIME_SCALE) ��Ҫ
{
	// �ե��`��ɤγ��ڻ�
	FFTWorkIp = NULL;
	FFTWorkW = NULL;
	InputWindow = NULL;
	OutputWindow = NULL;
	AnalWork = NULL;
	SynthWork = NULL;
	LastAnalPhase = NULL;
	LastSynthPhase = NULL;

	FrameSize = framesize;
	OverSampling = 8;
	Frequency = frequency;
	Channels = channels;
	InputHopSize = OutputHopSize = FrameSize / OverSampling;

	TimeScale = 1.0;
	FrequencyScale = 1.0;
	RebuildParams = true; // �ؤ����ؤ˥ѥ��`�����٘��B����褦����

	LastSynthPhaseAdjustCounter = 0;

	try
	{
		// ��`���ʤɤδ_��
		AnalWork  = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		SynthWork = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			AnalWork[ch] = NULL, SynthWork[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			AnalWork[ch]  = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize), 4);
			SynthWork[ch] = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize), 4);
		}

		LastAnalPhase = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastAnalPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastAnalPhase[ch] = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
			memset(LastAnalPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 �ǥ��ꥢ
		}

		LastSynthPhase = (float **)TJSAlignedAlloc(sizeof(float *) * Channels, 4);
		for(unsigned int ch = 0; ch < Channels; ch++)
			LastSynthPhase[ch] = NULL;
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			LastSynthPhase[ch] = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
			memset(LastSynthPhase[ch], 0, FrameSize/2 * sizeof(float)); // 0 �ǥ��ꥢ
		}

		FFTWorkIp = (int *)TJSAlignedAlloc(sizeof(int) * (static_cast<int>(2+sqrt((double)FrameSize/4))), 4);
		FFTWorkIp[0] = FFTWorkIp[1] = 0;
		FFTWorkW = (float *)TJSAlignedAlloc(sizeof(float) * (FrameSize/2), 4);
		InputWindow = (float *)TJSAlignedAlloc(sizeof(float) * FrameSize, 4);
		OutputWindow = (float *)TJSAlignedAlloc(sizeof(float) * FrameSize, 4);
	}
	catch(...)
	{
		Clear();
		throw;
	}

	// ������Хåե������ݤ򥯥ꥢ
	float *bufp1;
	size_t buflen1;
	float *bufp2;
	size_t buflen2;

	InputBuffer.GetWritePointer(InputBuffer.GetSize(),
							bufp1, buflen1, bufp2, buflen2);
	if(bufp1) memset(bufp1, 0, sizeof(float)*buflen1);
	if(bufp2) memset(bufp2, 0, sizeof(float)*buflen2);

	OutputBuffer.GetWritePointer(OutputBuffer.GetSize(),
							bufp1, buflen1, bufp2, buflen2);
	if(bufp1) memset(bufp1, 0, sizeof(float)*buflen1);
	if(bufp2) memset(bufp2, 0, sizeof(float)*buflen2);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::~tRisaPhaseVocoderDSP()
{
	Clear();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetTimeScale(float v)
{
	if(TimeScale != v)
	{
		TimeScale = v;
		RebuildParams = true;
		InputHopSize = OutputHopSize = FrameSize / OverSampling;
		OutputHopSize = static_cast<unsigned int>(InputHopSize * TimeScale) & ~1;
			// �� ż���˥��饤��(��Ҫ)
			// �}���� re,im, re,im, ... �����Ф���FFT�ˤ��ͬ����(�}�����΂�����2��)
			// PCM����ץ�ˉ�Q����뤿�ᡢPCM����ץ�⣲�����ĤǒQ��ʤ��Ȥʤ�ʤ�.
			// ���Όg�H�� OutputHopSize �ˏ��ä� ExactTimeScale ��Ӌ�㤵���.
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetFrequencyScale(float v)
{
	if(FrequencyScale != v)
	{
		FrequencyScale = v;
		RebuildParams = true;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::SetOverSampling(unsigned int v)
{
	if(v == 0)
	{
		// TimeScale �ˏ��äƂ����O��
		// ������铂��όg�H�Υꥹ�˥󥰤ˤ��Q�����줿�����Ǥ��ꡢ
		// Փ��Ĥʸ����Ϥʤ���
		if(TimeScale <= 0.2) v = 2;
		else if(TimeScale <= 1.2) v = 4;
		else v = 8;
	}

	if(OverSampling != v)
	{
		OverSampling = v;
		InputHopSize = OutputHopSize = FrameSize / OverSampling;
		OutputHopSize = static_cast<unsigned int>(InputHopSize * TimeScale) & ~1;
		// ������OutputHopSize��Ӌ��ˤĤ��Ƥ� tRisaPhaseVocoderDSP::SetTimeScale
		// ����դΤ���
		RebuildParams = true;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tRisaPhaseVocoderDSP::Clear()
{
	// ȫ�ƤΥХåե��ʤɤ��Ť���
	if(AnalWork)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(AnalWork[ch]), AnalWork[ch] = NULL;
		TJSAlignedDealloc(AnalWork), AnalWork = NULL;
	}
	if(SynthWork)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(SynthWork[ch]), SynthWork[ch] = NULL;
		TJSAlignedDealloc(SynthWork), SynthWork = NULL;
	}
	if(LastAnalPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(LastAnalPhase[ch]), LastAnalPhase[ch] = NULL;
		TJSAlignedDealloc(LastAnalPhase), LastAnalPhase = NULL;
	}
	if(LastSynthPhase)
	{
		for(unsigned int ch = 0; ch < Channels; ch++)
			TJSAlignedDealloc(LastSynthPhase[ch]), LastSynthPhase[ch] = NULL;
		TJSAlignedDealloc(LastSynthPhase), LastSynthPhase = NULL;
	}
	TJSAlignedDealloc(FFTWorkIp), FFTWorkIp = NULL;
	TJSAlignedDealloc(FFTWorkW), FFTWorkW = NULL;
	TJSAlignedDealloc(InputWindow), InputWindow = NULL;
	TJSAlignedDealloc(OutputWindow), OutputWindow = NULL;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetInputFreeSize()
{
	return InputBuffer.GetFreeSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaPhaseVocoderDSP::GetInputBuffer(
	size_t numsamplegranules,
	float * & p1, size_t & p1size,
	float * & p2, size_t & p2size)
{
	size_t numsamples = numsamplegranules * Channels;

	if(InputBuffer.GetFreeSize() < numsamples) return false; // ʮ�֤ʿդ��������ʤ�

	InputBuffer.GetWritePointer(numsamples, p1, p1size, p2, p2size);

	p1size /= Channels;
	p2size /= Channels;

	InputBuffer.AdvanceWritePos(numsamples);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
size_t tRisaPhaseVocoderDSP::GetOutputReadySize()
{
	return OutputBuffer.GetDataSize() / Channels;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool tRisaPhaseVocoderDSP::GetOutputBuffer(
	size_t numsamplegranules,
	const float * & p1, size_t & p1size,
	const float * & p2, size_t & p2size)
{
	size_t numsamples = numsamplegranules * Channels;

	if(OutputBuffer.GetDataSize() < numsamples) return false; // ʮ�֤ʜʂ�g�ߥ���ץ뤬�ʤ�

	OutputBuffer.GetReadPointer(numsamples, p1, p1size, p2, p2size);

	p1size /= Channels;
	p2size /= Channels;

	OutputBuffer.AdvanceReadPos(numsamples);

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tRisaPhaseVocoderDSP::tStatus tRisaPhaseVocoderDSP::Process()
{
	bool use_sse =
			(TVPCPUType & TVP_CPU_HAS_MMX) &&
			(TVPCPUType & TVP_CPU_HAS_SSE) &&
			(TVPCPUType & TVP_CPU_HAS_CMOV);


	// �ѥ��`������Ӌ��α�Ҫ��������Ϥ���Ӌ��򤹤�
	if(RebuildParams)
	{
		// ���v����Ӌ��(�����Ǥ�Vorbis I ��)
		float recovery_of_loss_of_vorbis_window = 2.0;
				//         1            1         2
				//  2  =  ��  1dx  /   ��   vorbis (x) dx
				//         0            0
				// where vobis = vorbis I window function
		float output_volume =
			TimeScale / FrameSize  / sqrt(FrequencyScale) / OverSampling * 2 *
											recovery_of_loss_of_vorbis_window;
		for(unsigned int i = 0; i < FrameSize; i++)
		{
			double x = ((double)i+0.5)/FrameSize;
			double window = sin(M_PI/2*sin(M_PI*x)*sin(M_PI*x));
			InputWindow[i]  = (float)(window);
			OutputWindow[i] = (float)(window *output_volume);
		}

		// ���Τۤ��Υѥ��`������Ӌ��
		OverSamplingRadian = (float)((2.0*M_PI)/OverSampling);
		OverSamplingRadianRecp = (float)(1.0/OverSamplingRadian);
		FrequencyPerFilterBand = (float)((double)Frequency/FrameSize);
		FrequencyPerFilterBandRecp = (float)(1.0/FrequencyPerFilterBand);
		ExactTimeScale = (float)OutputHopSize / InputHopSize;

		// �ե饰�򵹤�
		RebuildParams = false;
	}

	// �����Хåե��ڤΥǩ`����ʮ�֤���
	if(InputBuffer.GetDataSize() < FrameSize * Channels)
		return psInputNotEnough; // ���ʤ�

	// �����Хåե��οդ���ʮ�֤���
	if(OutputBuffer.GetFreeSize() < FrameSize * Channels)
		return psOutputFull; // ���ʤ�

	// ���줫������z�⤦�Ȥ��� OutputBuffer ���I�������� OutputHopSize ����ץ�
	// ����˥�`��� 0 ������ (���`�Щ`��åוr�ˤϤ߳������֤ʤΤ�)
	{
		float *p1, *p2;
		size_t p1len, p2len;

		OutputBuffer.GetWritePointer(OutputHopSize*Channels,
				p1, p1len, p2, p2len, (FrameSize - OutputHopSize)*Channels);
		memset(p1, 0, p1len * sizeof(float));
		if(p2) memset(p2, 0, p2len * sizeof(float));
	}

	// ���v�����m�ä��Ĥġ������Хåե����� AnalWork ���i���z��
	{
		const float *p1, *p2;
		size_t p1len, p2len;
		InputBuffer.GetReadPointer(FrameSize*Channels, p1, p1len, p2, p2len);
		p1len /= Channels;
		p2len /= Channels;
		(use_sse?
		sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj:
		def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj)
			(AnalWork, p1, InputWindow, Channels, 0, p1len);

		if(p2)
			(use_sse?
			sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj:
			def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj)
				(AnalWork, p2, InputWindow + p1len, Channels, p1len, p2len);
	}

	// �����ͥ뤴�Ȥ˄I��
	for(unsigned int ch = 0; ch < Channels; ch++)
	{
		//------------------------------------------------
		// ����
		//------------------------------------------------

		// ����θ��ֲ��֤�g�Ф���
			(use_sse?
			sse__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi:
			def__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi)
				(this, ch);
	}

	// ���v�����m�ä��Ĥġ�SynthWork ��������Хåե��˕����z��
	{
		float *p1, *p2;
		size_t p1len, p2len;

		OutputBuffer.GetWritePointer(FrameSize*Channels, p1, p1len, p2, p2len);
		p1len /= Channels;
		p2len /= Channels;
		(use_sse?
		sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj:
		def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj)
			(p1, SynthWork, OutputWindow, Channels, 0, p1len);
		if(p2)
			(use_sse?
			sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj:
			def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj)
				(p2, SynthWork, OutputWindow + p1len, Channels, p1len, p2len);
	}

	// LastSynthPhase �����{�����뤫
	LastSynthPhaseAdjustCounter += LastSynthPhaseAdjustIncrement;
	if(LastSynthPhaseAdjustCounter >= LastSynthPhaseAdjustInterval)
	{
		// LastSynthPhase �����{�����륫����Ȥˤʤä�
		LastSynthPhaseAdjustCounter = 0;

		// �������Ф��{���� LastSynthPhase �� unwrapping �Ǥ��롣
		// LastSynthPhase ��λ��β�۷e�����ΤǴ󤭤������ˤʤäƤ�������
		// �m�����g���Ǥ���� unwrapping ���ʤ��ȡ�������(�������󤭤�����)����
		// ������k�����������˺ϳɤ������ʤ��ʤäƤ��ޤ���
		// �����������Ȥ��������Ф褤���ᡢ���ؤ��� unwrapping ���Ф���Ҫ�Ϥʤ���
		// �����Ǥ� LastSynthPhaseAdjustInterval/LastSynthPhaseAdjustIncrement �ؤ��Ȥ��{�����Ф���
		for(unsigned int ch = 0; ch < Channels; ch++)
		{
			unsigned int framesize_d2 = FrameSize / 2;
			for(unsigned int i = 0; i < framesize_d2; i++)
			{
				long int n = static_cast<long int>(LastSynthPhase[ch][i] / (2.0*M_PI));
				LastSynthPhase[ch][i] -= n * (2.0*M_PI);
			}
		}
	}

	// ������Хåե��Υݥ��󥿤��M���
	OutputBuffer.AdvanceWritePos(OutputHopSize * Channels);
	InputBuffer.AdvanceReadPos(InputHopSize * Channels);

	// ���Ʃ`���� = no error
	return psNoError;
}
//---------------------------------------------------------------------------

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
#ifndef RisaPhaseVocoderH
#define RisaPhaseVocoderH

#include "RingBuffer.h"

//---------------------------------------------------------------------------
//! @brief Phase Vocoder DSP ���饹
//---------------------------------------------------------------------------
class tRisaPhaseVocoderDSP
{
protected:
	float ** AnalWork; //!< ����(Analyze)�åХåե�(FrameSize��) ��ǰ��Ц��ʤ��褦��
	float ** SynthWork; //!< �ϳ������I�Хåե�(FrameSize)
	float ** LastAnalPhase; //!< ǰ�ؽ����r�θ��ե��륿�Х�ɤ�λ�� (�������ͥ뤴�Ȥ�FrameSize/2��)
	float ** LastSynthPhase; //!< ǰ�غϳɕr�θ��ե��륿�Х�ɤ�λ�� (�������ͥ뤴�Ȥ�FrameSize/2��)

	int * FFTWorkIp; //!< rdft �˶ɤ� ip �ѥ��`��
	float * FFTWorkW; //!< rdft �˶ɤ� w �ѥ��`��
	float * InputWindow; //!< �����÷��v��
	float * OutputWindow; //!< �����÷��v��

	unsigned int FrameSize; //!< FFT������
	unsigned int OverSampling; //!< ���`�Щ`����ץ�󥰂S��
	unsigned int Frequency; //!< PCM ����ץ���ܲ���
	unsigned int Channels; //!< PCM �����ͥ���
	unsigned int InputHopSize; //!< FrameSize/OverSampling
	unsigned int OutputHopSize; //!< InputHopSize * TimeScale (SetTimeScale�r����Ӌ�㤵���)

	float	TimeScale; //!< �r�g�S����Υ����`��(����/����)
	float	FrequencyScale; //!< �ܲ�������Υ����`��(����/����)

	// ���¡�RebuildParams ����Εr���٘��B�����ѥ��`��
	// �����ˤ����������Ǥϡ�InputWindow �� OutputWindow ���٘��B�����
	float OverSamplingRadian; //!< (2.0*M_PI)/OverSampling
	float OverSamplingRadianRecp; //!< OverSamplingRadian ������
	float FrequencyPerFilterBand; //!< Frequency/FrameSize
	float FrequencyPerFilterBandRecp; //!< FrequencyPerFilterBand ������
	float ExactTimeScale; //!< ���ܤ�TimeScale = OutputHopSize / InputHopSize
	// �٘��B�����ѥ��`���������ޤ�

	tRisaRingBuffer<float> InputBuffer; //!< �����å�󥰥Хåե�
	tRisaRingBuffer<float> OutputBuffer; //!< �����å�󥰥Хåե�

	bool	RebuildParams; //!< �ڲ��Ĥʥѥ��`���ʤɤ��٘��B���ʤ���Фʤ�ʤ��Ȥ�����

	unsigned long LastSynthPhaseAdjustCounter; //!< LastSynthPhase ���a���������ڤ�Ϥ��뤿��Υ�����
	const static unsigned long LastSynthPhaseAdjustIncrement = 0x03e8a444; //!< LastSynthPhaseAdjustCounter�˼��㤹�낎
	const static unsigned long LastSynthPhaseAdjustInterval  = 0xfa2911fe; //!< LastSynthPhase ���a����������


public:
	//! @brief Process ���������Ʃ`����
	enum tStatus
	{
		psNoError, //!< ���}�ʤ�
		psInputNotEnough, //!< �������⤦�ʤ� (GetInputBuffer�ǵä��ݥ��󥿤˕����Ƥ�����ԇ�Ф���)
		psOutputFull //!< �����Хåե������äѤ� (GetOutputBuffer�ǵä��ݥ��󥿤����i�߳����Ƥ�����ԇ�Ф���)
	};

public:
	//! @brief		���󥹥ȥ饯��
	//! @param		framesize		�ե�`�ॵ����(2���ہ\, 16)
	//! @param		frequency		����PCM�Υ���ץ�󥰥�`��
	//! @param		channels		����PCM�Υ����ͥ���
	//! @note		���S�äǤ�framesize=4096,oversamp=16���餤���褯��
	//! @note		�ܥ����äǤ�framesize=256,oversamp=8���餤���褤��
	tRisaPhaseVocoderDSP(unsigned int framesize,
					unsigned int frequency, unsigned int channels);

	//! @brief		�ǥ��ȥ饯��
	~tRisaPhaseVocoderDSP();

	float GetTimeScale() const { return TimeScale; } //!< �r�g�S����Υ����`���ä�

	//! @brief		�r�g�S����Υ����`����O������
	//! @param		v     �����`��
	void SetTimeScale(float v);

	float GetFrequencyScale() const { return FrequencyScale; } //!< �ܲ����S����Υ����`���ä�

	//! @brief		�ܲ����S����Υ����`����O������
	//! @param		v     �����`��
	void SetFrequencyScale(float v);

	//! @brief		���`�Щ`����ץ�󥰂S����ȡ�ä���
	//! @return		���`�Щ`����ץ�󥰂S��
	unsigned int GetOverSampling() const { return OverSampling; }

	//! @brief		���`�Щ`����ץ�󥰂S�����O������
	//! @param		v		�S�� ( 0 = �r�g�S����Υ����`��ˏ��ä��ԄӵĤ��O�� )
	void SetOverSampling(unsigned int v);

	unsigned int GetInputHopSize() const { return InputHopSize; } //!< InputHopSize��ä�
	unsigned int GetOutputHopSize() const { return OutputHopSize; } //!< OutputHopSize ��ä�

private:
	//! @brief		���ꥢ
	void Clear();

public:
	//! @brief		�����Хåե��οդ�����ץ륰��˥�`������ä�
	//! @return		�����Хåե��οդ�����ץ륰��˥�`����
	size_t GetInputFreeSize();

	//! @brief		�����Хåե��Ε����z�ߥݥ��󥿤�ä�
	//! @param		numsamplegranules �����z�ߤ�������ץ륰��˥�`����
	//! @param		p1		�֥�å�1�����^�ؤΥݥ��󥿤��{���뤿��Ή���
	//! @param		p1size	p1�α��֥�å��Υ���ץ륰��˥�`����
	//! @param		p2		�֥�å�2�����^�ؤΥݥ��󥿤��{���뤿��Ή���(NULL������ä�)
	//! @param		p2size	p2�α��֥�å��Υ���ץ륰��˥�`����(0������ä�)
	//! @return		�դ����������ʤ���ЂΡ��դ���������ꡢ�ݥ��󥿤�����������
	//! @note		p1 �� p2 �Τ褦�ˣ��ĤΥݥ��󥿤Ȥ��Υ��������������Τϡ�
	//!				���ΥХåե����g�H�ϥ�󥰥Хåե��ǡ���󥰥Хåե��ڲ��Υ�˥��ʥХåե�
	//!				�νK�ˤ�ޤ��������Ԥ����뤿�ᡣ�ޤ����ʤ����Ϥ�p2��NULL�ˤʤ뤬���ޤ���
	//!				���Ϥ� p1 �Τ��Ȥ� p2 �˾A���ƕ����z�ޤʤ���Фʤ�ʤ���
	bool GetInputBuffer(size_t numsamplegranules,
		float * & p1, size_t & p1size,
		float * & p2, size_t & p2size);

	//! @brief		�����Хåե��Μʂ�g�ߥ���ץ륰��˥�`������ä�
	//! @return		�����Хåե��Μʂ�g�ߥ���ץ륰��˥�`����
	size_t GetOutputReadySize();

	//! @brief		�����Хåե����i���z�ߥݥ��󥿤�ä�
	//! @param		numsamplegranules �i���z�ߤ�������ץ륰��˥�`����
	//! @param		p1		�֥�å�1�����^�ؤΥݥ��󥿤��{���뤿��Ή���
	//! @param		p1size	p1�α��֥�å��Υ���ץ륰��˥�`����
	//! @param		p2		�֥�å�2�����^�ؤΥݥ��󥿤��{���뤿��Ή���(NULL������ä�)
	//! @param		p2size	p2�α��֥�å��Υ���ץ륰��˥�`����(0������ä�)
	//! @return		�ʂ䤵�줿����ץ뤬���ʤ���ЂΡ�����ץ뤬��ꡢ�ݥ��󥿤�����������
	//! @note		p1 �� p2 �Τ褦�ˣ��ĤΥݥ��󥿤Ȥ��Υ��������������Τϡ�
	//!				���ΥХåե����g�H�ϥ�󥰥Хåե��ǡ���󥰥Хåե��ڲ��Υ�˥��ʥХåե�
	//!				�νK�ˤ�ޤ��������Ԥ����뤿�ᡣ�ޤ����ʤ����Ϥ�p2��NULL�ˤʤ뤬���ޤ���
	//!				���Ϥ� p1 �Τ��Ȥ� p2 ��A�����i�߳����ʤ���Фʤ�ʤ���
	bool GetOutputBuffer(size_t numsamplegranules,
		const float * & p1, size_t & p1size,
		const float * & p2, size_t & p2size);

	//! @brief		�I���1���ƥå��Ф�
	//! @return		�I��Y�����enum
	tStatus Process();

	//! @brief		����θ��ֲ��֤�I����
	//! @param		ch			�I����Ф������ͥ�
	//! @note		�����β��֤ϸ�CPU���Ȥ����m������뤿�ᡢ
	//!				�gװ�� opt_default �ǥ��쥯�ȥ��¤ʤɤ��ä���롣
	//!				(PhaseVocoderDSP.cpp�ڤˤϤ���Όgװ�Ϥʤ�)
	void ProcessCore(int ch);
};
//---------------------------------------------------------------------------

#endif

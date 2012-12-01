
/*****************************************************************************
                          E R I N A - L i b r a r y
                                                        ��K���� 2001/5/23
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(CXMIO_H_INCLUDE)
#define	CXMIO_H_INCLUDE

#if	!defined(ERIMATRIX_H_INCLUDE)
#include "erimatrix.h"
#endif

#if	!defined(CMPERI_H_INCLUDE)
#include "cmperi.h"
#endif


/*****************************************************************************
                                �������
 *****************************************************************************/

struct	MIO_INFO_HEADER
{
	DWORD	dwVersion ;
	DWORD	fdwTransformation ;
	DWORD	dwArchitecture ;
	DWORD	dwChannelCount ;
	DWORD	dwSamplesPerSec ;
	DWORD	dwBlocksetCount ;
	DWORD	dwSubbandDegree ;
	DWORD	dwAllSampleCount ;
	DWORD	dwLappedDegree ;
	DWORD	dwBitsPerSample ;
} ;

struct	MIO_DATA_HEADER
{
	BYTE	bytVersion ;
	BYTE	bytFlags ;
	BYTE	bytReserved1 ;
	BYTE	bytReserved2 ;
	DWORD	dwSampleCount ;
} ;

#define	MIO_LEAD_BLOCK	0x01


/*****************************************************************************
                            �����R�s���֥�������
 *****************************************************************************/

class	MIOEncoder
{
protected:
	MIO_INFO_HEADER		m_mioih ;				// �������إå�

	unsigned int		m_nBufLength ;			// �Хåե��L������ץ�����
	void *				m_ptrBuffer1 ;			// ��քI��Хåե�
	void *				m_ptrBuffer2 ;			// �K���椨�Хåե�
	SBYTE *				m_ptrBuffer3 ;			// ���󥿩`��`���åХåե�
	REAL32 *			m_ptrSamplingBuf ;		// ����ץ���åХåե�
	REAL32 *			m_ptrInternalBuf ;		// ���g�Хåե�
	REAL32 *			m_ptrWorkBuf ;			// DCT �����å�`�����ꥢ
	REAL32 *			m_ptrWeightTable ;		// ���ܲ����ɷ֤��ؤߥƩ`�֥�
	REAL32 *			m_ptrLastDCT ;			// ֱǰ�֥�å��� DCT �S��

	double				m_rLowWeight ;			// ���ܲ��ɷ֤��ؤ�
	double				m_rMiddleWeight ;		// ���ܲ��ɷ֤��ؤ�
	double				m_rPowerScale ;			// ���ӻ��λ��ʥӥå���
	int					m_nOddWeight ;			// �֥�å��ጝ�߂S��

	SWORD *				m_ptrNextDstBuf ;		// �����Хåե����ɥ쥹
	REAL32 *			m_ptrLastDCTBuf ;		// ���}�����åХåե�
	unsigned int		m_nSubbandDegree ;		// ���ФΥ�����
	unsigned int		m_nDegreeNum ;
	int					m_nFrequencyWidth[7] ;	// ���ܲ������η�
	int					m_nFrequencyPoint[7] ;	// ���ܲ�����������λ��
	SIN_COS *			m_pRevolveParam ;

public:
	// ���B�v��
	MIOEncoder( void ) ;
	// �����v��
	virtual ~MIOEncoder( void ) ;

	// ���ڻ����ѥ��`�����O����
	int Initialize( const MIO_INFO_HEADER & infhdr ) ;
	// �K�ˣ�����ν�Ťʤɣ�
	void Delete( void ) ;
	// ������R�s
	int EncodeSound
		( RLHEncodeContext & context,
			const MIO_DATA_HEADER & datahdr, const void * ptrWaveBuf ) ;
	// �R�s���ץ������O��
	void SetCompressionParameter
		( double rLowWeight = 16.0,
			double rMiddleWeight = 8.0,
			double rPowerScale = 0.5,
			int nOddWeight = 1 ) ;

protected:
	// 8�ӥåȤ�PCM��R�s
	int EncodeSoundPCM8
		( RLHEncodeContext & context,
			const MIO_DATA_HEADER & datahdr, const void * ptrWaveBuf ) ;
	// 16�ӥåȤ�PCM��R�s
	int EncodeSoundPCM16
		( RLHEncodeContext & context,
			const MIO_DATA_HEADER & datahdr, const void * ptrWaveBuf ) ;

protected:
	// ���Х������Ή���˰餦�ѥ��`������Ӌ��
	void InitializeWithDegree( unsigned int nSubbandDegree ) ;
	// ָ������ץ��Ф�����������
	double EvaluateVolume( const REAL32 * ptrWave, int nCount ) ;
	// �ֽ⥳�`�ɤ�ȡ�ä���
	int GetDivisionCode( void ) ;
	// 16�ӥåȤηǿ���R�s
	int EncodeSoundDCT
		( RLHEncodeContext & context,
			const MIO_DATA_HEADER & datahdr, const void * ptrWaveBuf ) ;
	// LOT ��Q��ʩ��
	void PerformLOT
		( RLHEncodeContext & context,
			REAL32 * ptrSamples, REAL32 rPowerScale ) ;
	// ͨ���Υ֥�å�����Ż����Ƴ�������
	int EncodeInternalBlock
		( RLHEncodeContext & context,
			REAL32 * ptrSamples, REAL32 rPowerScale ) ;
	// ��`�ɥ֥�å�����Ż����Ƴ�������
	int EncodeLeadBlock
		( RLHEncodeContext & context,
			REAL32 * ptrSamples, REAL32 rPowerScale ) ;
	// �ݥ��ȥ֥�å�����Ż����Ƴ�������
	int EncodePostBlock
		( RLHEncodeContext & context, REAL32 rPowerScale ) ;
	// ���ӻ�
	void Quantumize
		( PINT ptrQuantumized, const REAL32 * ptrSource,
			int nDegreeNum, REAL32 rPowerScale,
			DWORD * ptrWeightCode, int * ptrCoefficient ) ;

} ;


/*****************************************************************************
                            ����չ�_���֥�������
 *****************************************************************************/

class	MIODecoder
{
protected:
	MIO_INFO_HEADER		m_mioih ;				// �������إå�

	unsigned int		m_nBufLength ;			// �Хåե��L������ץ�����
	void *				m_ptrBuffer1 ;			// ��քI��Хåե�
	void *				m_ptrBuffer2 ;			// �K���椨�Хåե�
	SBYTE *				m_ptrBuffer3 ;			// ���󥿩`��`���åХåե�
	PBYTE				m_ptrDivisionTable ;	// �ֽ⥳�`�ɥƩ`�֥�
	SDWORD *			m_ptrWeightCode ;		// ���ӻ��S���Ʃ`�֥�
	PINT				m_ptrCoefficient ;		//
	REAL32 *			m_ptrMatrixBuf ;		// ���������åХåե�
	REAL32 *			m_ptrInternalBuf ;		// ���g�Хåե�
	REAL32 *			m_ptrWorkBuf ;			// DCT �����å�`�����ꥢ
	REAL32 *			m_ptrWorkBuf2 ;
	REAL32 *			m_ptrWeightTable ;		// ���ܲ����ɷ֤��ؤߥƩ`�֥�
	REAL32 *			m_ptrLastDCT ;			// ֱǰ�� DCT �S��

	PBYTE				m_ptrNextDivision ;		// �Τηֽ⥳�`��
	SDWORD *			m_ptrNextWeight ;		// �Τ����ӻ��S��
	PINT				m_ptrNextCoefficient ;	//
	PINT				m_ptrNextSource ;		// �Τ������ź�
	REAL32 *			m_ptrLastDCTBuf ;		// ���}�����åХåե�
	unsigned int		m_nSubbandDegree ;		// ���ФΥ�����
	unsigned int		m_nDegreeNum ;
	SIN_COS *			m_pRevolveParam ;
	int					m_nFrequencyPoint[7] ;	// ���ܲ�����������λ��

public:
	// ���B�v��
	MIODecoder( void ) ;
	// �����v��
	virtual ~MIODecoder( void ) ;

	// ���ڻ����ѥ��`�����O����
	int Initialize( const MIO_INFO_HEADER & infhdr ) ;
	// �K�ˣ�����ν�Ťʤɣ�
	void Delete( void ) ;
	// ������R�s
	int DecodeSound
		( RLHDecodeContext & context,
			const MIO_DATA_HEADER & datahdr, void * ptrWaveBuf ) ;

protected:
	// 8�ӥåȤ�PCM��չ�_
	int DecodeSoundPCM8
		( RLHDecodeContext & context,
			const MIO_DATA_HEADER & datahdr, void * ptrWaveBuf ) ;
	// 16�ӥåȤ�PCM��չ�_
	int DecodeSoundPCM16
		( RLHDecodeContext & context,
			const MIO_DATA_HEADER & datahdr, void * ptrWaveBuf ) ;

protected:
	// ���Х������Ή���˰餦�ѥ��`������Ӌ��
	void InitializeWithDegree( unsigned int nSubbandDegree ) ;
	// 16�ӥåȤηǿ���չ�_
	int DecodeSoundDCT
		( RLHDecodeContext & context,
			const MIO_DATA_HEADER & datahdr, void * ptrWaveBuf ) ;
	// ͨ���Υ֥�å���ͺŤ���
	int DecodeInternalBlock
		( RLHDecodeContext & context,
			SWORD * ptrDst, unsigned int nSamples ) ;
	// ��`�ɥ֥�å���ͺŤ���
	int DecodeLeadBlock
		( RLHDecodeContext & context ) ;
	// �ݥ��ȥ֥�å���ͺŤ���
	int DecodePostBlock
		( RLHDecodeContext & context,
			SWORD * ptrDst, unsigned int nSamples ) ;
	// ���ӻ�
	void IQuantumize
		( REAL32 * ptrDestination,
			const INT * ptrQuantumized, int nDegreeNum,
			SDWORD nWeightCode, int nCoefficient ) ;
	// �ͺ�
	int DecodeSamples
		( RLHDecodeContext & context,
			INT * ptrQuantumized, unsigned int nCount ) ;

} ;


#endif

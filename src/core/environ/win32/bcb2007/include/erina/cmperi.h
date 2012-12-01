
/*****************************************************************************
                         E R I N A - L i b r a r y
                                                      ��K���� 2001/05/23
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(CMPERI_H_INCLUDE)
#define	CMPERI_H_INCLUDE

#include "experi.h"


/*****************************************************************************
                               �ե����륪�֥�������
 *****************************************************************************/

class	EWriteFileObj
{
public:
	// ���B�v��
	EWriteFileObj( void ) ;
	// �����v��
	virtual ~EWriteFileObj( void ) ;
	// �ǩ`�����������
	virtual DWORD Write( const void * ptrBuf, DWORD dwByte ) = 0 ;
	// �ե�������L����ȡ�ä���
	virtual DWORD GetLength( void ) = 0 ;
	// �ե�����ݥ��󥿤�ȡ�ä���
	virtual DWORD GetPointer( void ) = 0 ;
	// �ե�����ݥ��󥿤��ƄӤ���
	virtual void Seek( DWORD dwPointer ) = 0 ;

} ;

class	EWriteFile	: public	EWriteFileObj
{
protected:
	HANDLE	m_hFile ;
	bool	m_flagToClose ;

public:
	EWriteFile( void ) ;
	EWriteFile( HANDLE hFile ) ;
	virtual ~EWriteFile( void ) ;

	bool Open( const char * pszFile ) ;
	void Attach( HANDLE hFile ) ;
	void Close( void ) ;

	virtual DWORD Write( const void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;

class	ERIWriteFile	: public	EWriteFileObj
{
protected:
	EWriteFileObj *	m_pFile ;
	struct	RECORD_INFO
	{
		RECORD_INFO *	pParent ;
		DWORD			dwOffset ;
		DWORD			dwLimit ;
	} ;
	RECORD_INFO *	m_pRecInf ;

public:
	enum	FileID
	{
		fidImage,
		fidSound,
		fidMovie
	} ;

public:
	// ���B�v��
	ERIWriteFile( void ) ;
	// �����v��
	virtual ~ERIWriteFile( void ) ;
	// �{������ե�������_��
	bool Open( EWriteFileObj * pFile, FileID fidType ) ;
	// �{������ե�������]����
	void Close( void ) ;
	// �إå��쥳�`�ɤ��������
	bool WriteHeader
		( const ERI_FILE_HEADER & efh, const ERI_INFO_HEADER & eih ) ;
	// �쥳�`�ɤ��_��
	bool DescendRecord( const UINT64 * pRecID ) ;
	// �쥳�`�ɤ��]����
	void AscendRecord( void ) ;

	virtual DWORD Write( const void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;


/*****************************************************************************
                   ����󥰥�����ޥ���ƥ�����
 *****************************************************************************/

class	RLHEncodeContext
{
protected:
	int		m_nIntBufCount ;	// ���g�����Хåե�����e����Ƥ���ӥå���
	DWORD	m_dwIntBuffer ;		// ���g�����Хåե�
	ULONG	m_nBufferingSize ;	// �Хåե���󥰤���Х�����
	ULONG	m_nBufCount ;		// �Хåե�����e����Ƥ���Х�����
	PBYTE	m_ptrBuffer ;		// �����Хåե������^�ؤΥݥ���

	struct	MTF_TABLE
	{
		BYTE	iplt[0x100] ;

		inline void Initialize( void ) ;
		inline int Search( BYTE p ) ;
	} ;
	MTF_TABLE *	m_pMTFTable[0x101] ;	// MTF�Ʃ`�֥�

	ERINA_HUFFMAN_TREE *	m_pLastHuffmanTree ;
	ERINA_HUFFMAN_TREE *	m_pHuffmanTree[0x101] ;

public:
	// ���B�v��
	RLHEncodeContext( ULONG nBufferingSize ) ;
	// �����v��
	virtual ~RLHEncodeContext( void ) ;

	// ��ӥåȳ�������
	int OutNBits( DWORD dwData, int n ) ;
	// ����ޥ��`�ɤ˷��Ż������H�Υӥå�����Ӌ��
	static inline int TryOutACode( int num ) ;
	// ����ޥ��`�ɤ��������
	int OutACode( int num ) ;
	// �Хåե������ݤ�������ƿդˤ���
	int Flushout( void ) ;
	// �Хåե������ݤ��������
	virtual ULONG WriteNextData( const BYTE * ptrBuffer, ULONG nBytes ) = 0 ;

	// ����󥰥�����޷��Ť˷��Ż������H�Υ��������ӥå�������Ӌ��
	static int TryEncodeGammaCodes( const INT * ptrSrc, int nCount ) ;
	// ����󥰥�����޷��Ť˷��Ż����Ƴ�������
	int EncodeGammaCodes( const INT * ptrSrc, int nCount ) ;

	// RL-MTF-G ���Ťη��Ż��Μʂ�򤹤�
	void PrepareToEncodeRLMTFGCode( void ) ;
	// RL-MTF-G ���Ť˷��Ż����Ƴ�������
	int EncodeRLMTFGCodes( PBYTE ptrSrc, int nCount ) ;

	// ERINA ���Ťη��Ż��Μʂ�򤹤�
	void PrepareToEncodeERINACode( void ) ;
	// �ϥեޥ���Ťǳ�������
	int OutAHuffmanCode( ERINA_HUFFMAN_TREE * tree, int num ) ;
	// �L����ϥեޥ���Ťǳ�������
	int OutLengthHuffman( ERINA_HUFFMAN_TREE * tree, int length ) ;
	// ERINA ���Ť˷��Ż����Ƴ�������
	int EncodeERINACodes( const INT * ptrSrc, int nCount ) ;
	// ERINA ���Ť˷��Ż����Ƴ�������
	int EncodeERINACodesSBZL( const SBYTE * ptrSrc, int nCount ) ;
	// ERINA ���ţ�256ɫ�ã��˷��Ż����Ƴ�������
	int EncodeERINACodes256( const BYTE * ptrSrc, int nCount ) ;

} ;

//
// MTF_TABLE ����ڻ�����
//////////////////////////////////////////////////////////////////////////////
inline void RLHEncodeContext::MTF_TABLE::Initialize( void )
{
	for ( int i = 0; i < 0x100; i ++ )
		iplt[i] = (BYTE) i ;
}

//
// �Ʃ`�֥���Ф�������ơ�Move To Front ��g��
//////////////////////////////////////////////////////////////////////////////
inline int RLHEncodeContext::MTF_TABLE::Search( BYTE p )
{
	BYTE	lplt, cplt ;
	int		index = 0 ;
	lplt = p ;
	for ( ; ; )
	{
		cplt = iplt[index] ;
		iplt[index ++] = lplt ;
		if ( cplt == p )
			break ;
		lplt = cplt ;
	}
	return	index ;
}

//
// ����ޥ��`�ɤ˷��Ż������H�Υӥå�����Ӌ��
//////////////////////////////////////////////////////////////////////////////
inline int RLHEncodeContext::TryOutACode( int num )
{
	int	b = 0, c = 1 ;
	num -- ;
	while ( num >= c )
	{
		num -= c ;
		c <<= 1 ;
		b ++ ;
	}
	return	b * 2 + 1 ;
}


/*****************************************************************************
                               �R�s���֥�������
 *****************************************************************************/

class	ERIEncoder
{
protected:
	ERI_INFO_HEADER		m_EriInfHdr ;			// �������إå�

	UINT				m_nBlockSize ;			// �֥�å��󥰥�����
	ULONG				m_nBlockArea ;			// �֥�å���e
	ULONG				m_nBlockSamples ;		// �֥�å��Υ���ץ���
	UINT				m_nChannelCount ;		// ����ͥ���
	ULONG				m_nWidthBlocks ;		// ����η����֥�å�����
	ULONG				m_nHeightBlocks ;		// ����θߤ����֥�å�����

	UINT				m_nSrcBytesPerPixel ;	// 1�ԥ�����ΥХ�����

	PINT				m_ptrColumnBuf ;		// �ХХåե�
	PINT				m_ptrLineBuf ;			// �ХХåե�
	PINT				m_ptrBuffer1 ;			// �ڲ��Хåե�
	PINT				m_ptrBuffer2 ;
	int					m_nChannelDepth ;		// ����ͥ�Υӥå����

	PINT				m_pArrangeTable[4] ;	// ����ץ�󥰥Ʃ`�֥�

	PBYTE				m_ptrOperations ;		// ���ڥ�`�����Ʃ`�֥�
	PINT				m_ptrDstBuffer ;		// �ǥ����ƥ��ͩ`�����Хåե�

	ERINA_HUFFMAN_TREE *	m_pHuffmanTree ;	// �ϥեޥ�ľ

public:
	typedef	void (ERIEncoder::*PFUNC_COLOR_OPRATION)( void ) ;
	typedef	void (ERIEncoder::*PFUNC_SAMPLING)
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	typedef	void (ERIEncoder::*PFUNC_SAMPLINGII)
		( REAL32 * ptrBuffer[], PBYTE ptrSrc,
			LONG nLineBytes, int nWidth, int nHeight ) ;
	typedef	void (ERIEncoder::*PFUNC_BLOCK_SCALING)( void ) ;

protected:
	UINT				m_nBlocksetCount ;		// 1�֥�å����åȤΥ֥�å���
	PFUNC_BLOCK_SCALING	m_pfnBlockScaling ;		// �֥�å������`����v��

	REAL32 *			m_ptrBuffer3[4][4] ;	// �ڲ��Хåե�
	REAL32 *			m_ptrBuffer4[16] ;
	REAL32 *			m_ptrBuffer5[16] ;
	REAL32 *			m_ptrWorkDCT1 ;			// DCT ��Q�����I�I��
	REAL32 *			m_ptrWorkDCT2 ;
	INT					m_nDCDiffBuffer[16] ;	// DC �ɷֲ���åХåե�

	PINT				m_ptrTableDC ;			// ֱ���ɷ�

public:
	struct	PARAMETER
	{
		REAL32			m_rYScaleDC ;			// �x��ֱ���ɷ֤����ӻ��S��
		REAL32			m_rCScaleDC ;			// ɫ��ֱ���ɷ֤����ӻ��S��
		REAL32			m_rYScaleLow ;			// �x�ȵ��ܲ��ɷ֤����ӻ��S��
		REAL32			m_rCScaleLow ;			// ɫ����ܲ��ɷ֤����ӻ��S��
		REAL32			m_rYScaleHigh ;			// �x�ȸ��ܲ��ɷ֤����ӻ��S��
		REAL32			m_rCScaleHigh ;			// ɫ����ܲ��ɷ֤����ӻ��S��
		int				m_nLowThreshold ;		// �x�ȵ��ܲ��ɷ֤�铂�
		int				m_nHighThreshold ;		// �x�ȸ��ܲ��ɷ֤�铂�

		PARAMETER( void ) ;
	} ;
protected:
	PARAMETER			m_parameter ;			// �R�s�ѥ��`��

	static PFUNC_COLOR_OPRATION	m_pfnColorOperation[0x10] ;

public:
	// ���B�v��
	ERIEncoder( void ) ;
	// �����v��
	virtual ~ERIEncoder( void ) ;

	// ���ڻ����ѥ��`�����O����
	int Initialize( const ERI_INFO_HEADER & infhdr ) ;
	// �K�ˣ�����ν�Ťʤɣ�
	void Delete( void ) ;
	// �����R�s
	int EncodeImage
		( const RASTER_IMAGE_INFO & srcimginf,
			RLHEncodeContext & context, DWORD fdwFlags = 0x0021 ) ;
	// �R�s���ץ������O��
	void SetCompressionParameter( const PARAMETER & parameter ) ;

protected:
	// ����ץ�󥰥Ʃ`�֥�γ��ڻ�
	void InitializeSamplingTable( void ) ;
	// �ե륫��`����ΈR�s
	int EncodeTrueColorImage
		( const RASTER_IMAGE_INFO & imginf,
			RLHEncodeContext & context, DWORD fdwFlags ) ;
	// �ե륫��`����ΈR�s
	int EncodeTrueColorImageII
		( const RASTER_IMAGE_INFO & imginf,
			RLHEncodeContext & context, DWORD fdwFlags ) ;
	// �ѥ�åȻ���ΈR�s
	int EncodePaletteImage
		( const RASTER_IMAGE_INFO & imginf, RLHEncodeContext & context ) ;

	// ��քI��
	void DifferentialOperation( LONG nAllBlockLines, PINT * ptrNextLineBuf ) ;
	// ���ڥ�`����󥳩`�ɤ�ȡ��
	DWORD DecideOperationCode
		( int nCompressMode, LONG nAllBlockLines, PINT * ptrNextLineBuf ) ;

	// ����ͥ�Υӥå���Ȥ�ȡ�ä���
	virtual int GetChannelDepth
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;
	// ָ���Υӥå���Ȥ˥ǩ`������Ҏ������
	void NormalizeWithDepth( PINT ptrDst, PINT ptrSrc, int nCount ) ;
	// ����򥵥�ץ�󥰤����v���ؤΥݥ��󥿤�ȡ�ä���
	virtual PFUNC_SAMPLING GetSamplingFunc
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// ����`���ڥ�`������v��Ⱥ
	void ColorOperation0000( void ) ;
	void ColorOperation0101( void ) ;
	void ColorOperation0110( void ) ;
	void ColorOperation0111( void ) ;
	void ColorOperation1001( void ) ;
	void ColorOperation1010( void ) ;
	void ColorOperation1011( void ) ;
	void ColorOperation1101( void ) ;
	void ColorOperation1110( void ) ;
	void ColorOperation1111( void ) ;

	// ���쥤����Υ���ץ��
	void SamplingGray8
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB����(15�ӥå�)�Υ���ץ��
	void SamplingRGB16
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB����Υ���ץ��
	void SamplingRGB24
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGBA����Υ���ץ��
	void SamplingRGBA32
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;

protected:
	// ����ץ�󥰥Ʃ`�֥�γ��ڻ�
	void InitializeZigZagTable( void ) ;
	// �ե륫��`����ΈR�s
	int EncodeTrueColorImageDCT
		( const RASTER_IMAGE_INFO & imginf,
			RLHEncodeContext & context, DWORD fdwFlags ) ;

	// ����򥵥�ץ�󥰤����v���ؤΥݥ��󥿤�ȡ�ä���
	virtual PFUNC_SAMPLINGII GetSamplingFuncII
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// RGB����Υ���ץ��
	void SamplingRGB24II
		( REAL32 * ptrBuffer[], PBYTE ptrSrc,
			LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGBA����Υ���ץ��
	void SamplingRGBA32II
		( REAL32 * ptrBuffer[], PBYTE ptrSrc,
			LONG nLineBytes, int nWidth, int nHeight ) ;
	// ����I���ָ�������O��
	void FillOddArea
		( REAL32 * ptrBuffer, int nWidth, int nHeight, REAL32 rFill ) ;

	// RGB -> YUV ��Q�v��
	void ConvertRGBtoYUV( void ) ;

	// 4:4:4 �����`���
	void BlockScaling444( void ) ;
	// 4:2:2 �����`���
	void BlockScaling422( void ) ;
	// 4:1:1 �����`���
	void BlockScaling411( void ) ;

	// DCT ��Q��ʩ��
	void PerformDCT( void ) ;

	// ���ӻ���ʩ��
	void ArrangeAndQuantumize( BYTE bytCoefficient[] ) ;

protected:
	// չ�_�M��״�r֪ͨ�v��
	virtual int OnEncodedBlock( LONG line, LONG column ) ;
	virtual int OnEncodedLine( LONG line ) ;

} ;

#define	ERI_ENCODE_INDEPENDENT_BLOCK	0x0001
#define	ERI_MAKE_COMPRESS_MODE(x)		(((x) & 0x03) << 4)
#define	ERI_GET_COMPRESS_MODE(x)		(((x) & 0x30) >> 4)



/*****************************************************************************
                     �ե����륹�ȥ�`�ॳ��ƥ�����
 *****************************************************************************/

class	EFileEncodeContext	: public	RLHEncodeContext
{
public:
	// ���B�v��
	EFileEncodeContext( EWriteFileObj * pFileObj, ULONG nBufferingSize ) ;
	// �����v��
	virtual ~EFileEncodeContext( void ) ;

protected:
	EWriteFileObj *	m_pFileObj ;

public:
	// �ΤΥǩ`�����������
	virtual ULONG WriteNextData( const BYTE * ptrBuffer, ULONG nBytes ) ;
	// �ե����륪�֥������Ȥ��O������
	void AttachFileObject( EWriteFileObj * pFileObj ) ;

} ;


#endif

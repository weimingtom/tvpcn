
/*****************************************************************************
                         E R I N A - L i b r a r y
                                                      ��K���� 2001/05/23
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(EXPERI_H_INCLUDED)
#define	EXPERI_H_INCLUDED


/*****************************************************************************
                       �饤�֥����ڻ��K���v��
 *****************************************************************************/

#if	defined(_M_IX86) && !defined(ERI_INTEL_X86)
#define	ERI_INTEL_X86
#endif

extern	"C"
{
	void eriInitializeLibrary( void ) ;
	void eriCloseLibrary( void ) ;
	void eriInitializeTask( void ) ;
	void eriCloseTask( void ) ;
#if	defined(ERI_INTEL_X86)
	void eriEnableMMX( int fForceEnable = 0 ) ;
	void eriDisableMMX( int fForceDisable = 0 ) ;
	void eriInitializeFPU( void ) ;
#endif

} ;

#if	defined(ERI_INTEL_X86)
#define	ERI_USE_MMX_PENTIUM	0x0002
#define	ERI_USE_XMM_P3		0x0008
extern	"C"	DWORD	ERI_EnabledProcessorType ;
#endif


/*****************************************************************************
                         ���ꥢ���`�����
 *****************************************************************************/

extern	PVOID eriAllocateMemory( DWORD dwBytes ) ;
extern	void eriFreeMemory( PVOID ptrMem ) ;


/*****************************************************************************
                                �������
 *****************************************************************************/

struct	ERI_FILE_HEADER
{
	DWORD	dwVersion ;
	DWORD	dwContainedFlag ;
	DWORD	dwKeyFrameCount ;
	DWORD	dwFrameCount ;
	DWORD	dwAllFrameTime ;
} ;

struct	ERI_INFO_HEADER
{
	DWORD	dwVersion ;
	DWORD	fdwTransformation ;
	DWORD	dwArchitecture ;
	DWORD	fdwFormatType ;
	SDWORD	nImageWidth ;
	SDWORD	nImageHeight ;
	DWORD	dwBitsPerPixel ;
	DWORD	dwClippedPixel ;
	DWORD	dwSamplingFlags ;
	SDWORD	dwQuantumizedBits[2] ;
	DWORD	dwAllottedBits[2] ;
	DWORD	dwBlockingDegree ;
	DWORD	dwLappedBlock ;
	DWORD	dwFrameTransform ;
	DWORD	dwFrameDegree ;
} ;

struct	RASTER_IMAGE_INFO
{
	DWORD	fdwFormatType ;
	PBYTE	ptrImageArray ;
	SDWORD	nImageWidth ;
	SDWORD	nImageHeight ;
	DWORD	dwBitsPerPixel ;
	SDWORD	BytesPerLine ;
} ;

#if	!defined(__ENTISGLS_H__)
union	ENTIS_PALETTE
{
	struct
	{
		BYTE	Blue ;
		BYTE	Green ;
		BYTE	Red ;
		BYTE	Reserved ;
	}			rgb ;
	struct
	{
		BYTE	Blue ;
		BYTE	Green ;
		BYTE	Red ;
		BYTE	Alpha ;
	}			rgba ;
} ;
#endif

#define	EFH_CONTAIN_IMAGE		0x00000001
#define	EFH_CONTAIN_ALPHA		0x00000002
#define	EFH_CONTAIN_PALETTE		0x00000010
#define	EFH_CONTAIN_WAVE		0x00000100
#define	EFH_CONTAIN_SEQUENCE	0x00000200

#define	ERI_RGB_IMAGE			0x00000001
#define	ERI_RGBA_IMAGE			0x04000001
#define	ERI_GRAY_IMAGE			0x00000002
#define	ERI_TYPE_MASK			0x00FFFFFF
#define	ERI_WITH_PALETTE		0x01000000
#define	ERI_USE_CLIPPING		0x02000000
#define	ERI_WITH_ALPHA			0x04000000

#define	CVTYPE_LOSSLESS_ERI		0x03020000
#define	CVTYPE_DCT_ERI			0x00000001
#define	CVTYPE_LOT_ERI			0x00000005

#define	ERI_ARITHMETIC_CODE		32
#define	ERI_RUNLENGTH_GAMMA		0xFFFFFFFF
#define	ERI_RUNLENGTH_HUFFMAN	0xFFFFFFFC

#define	ERISF_YUV_4_4_4			0x00040404
#define	ERISF_YUV_4_2_2			0x00040202
#define	ERISF_YUV_4_1_1			0x00040101



/*****************************************************************************
                            ���˥�`��������v��
 *****************************************************************************/

extern	"C"
{
	int eriAdditionOfFrame
		( const RASTER_IMAGE_INFO & riiDst,
			const RASTER_IMAGE_INFO & riiSrc ) ;
	int eriSubtractionOfFrame
		( const RASTER_IMAGE_INFO & riiDst,
			const RASTER_IMAGE_INFO & riiSrc ) ;
	int eriCopyImage
		( const RASTER_IMAGE_INFO & riiDst,
			const RASTER_IMAGE_INFO & riiSrc ) ;

} ;


/*****************************************************************************
                                 �軭�v��
 *****************************************************************************/

extern	"C"
{
	void eriDrawImageToDC
		( HDC hDC, int x, int y, const RASTER_IMAGE_INFO & rii,
			const ENTIS_PALETTE * ppalette = NULL, SIZE * psize = NULL ) ;

} ;


/*****************************************************************************
                             �ե����륪�֥�������
 *****************************************************************************/

class	EFileObject
{
public:
	// ���B�v��
	EFileObject( void ) ;
	// �����v��
	virtual ~EFileObject( void ) ;
	// �ǩ`�����i���z��
	virtual DWORD Read( void * ptrBuf, DWORD dwByte ) = 0 ;
	// �ե�������L����ȡ�ä���
	virtual DWORD GetLength( void ) = 0 ;
	// �ե�����ݥ��󥿤�ȡ�ä���
	virtual DWORD GetPointer( void ) = 0 ;
	// �ե�����ݥ��󥿤��ƄӤ���
	virtual void Seek( DWORD dwPointer ) = 0 ;

} ;

class	EReadFile	: public	EFileObject
{
protected:
	HANDLE	m_hFile ;
	bool	m_flagToClose ;

public:
	EReadFile( void ) ;
	EReadFile( HANDLE hFile ) ;
	virtual ~EReadFile( void ) ;

	bool Open( const char * pszFile ) ;
	void Attach( HANDLE hFile ) ;
	void Close( void ) ;

	virtual DWORD Read( void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;

class	EMemFile	: public	EFileObject
{
protected:
	const BYTE *	m_ptrMemory ;
	DWORD			m_dwLength ;
	DWORD			m_dwPointer ;

public:
	EMemFile( const BYTE * ptrMem, DWORD dwLength ) ;
	virtual ~EMemFile( void ) ;
	virtual DWORD Read( void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;


/*****************************************************************************
                         �ϥեޥ�ĥ�`������
 *****************************************************************************/

#define	ERINA_CODE_FLAG			0x80000000U
#define	ERINA_HUFFMAN_ESCAPE	0x7FFFFFFF
#define	ERINA_HUFFMAN_NULL		0x8000U
#define	ERINA_HUFFMAN_MAX		0x4000
#define	ERINA_HUFFMAN_ROOT		0x200

struct	ERINA_HUFFMAN_NODE
{
	WORD	m_weight ;
	WORD	m_parent ;
	DWORD	m_child_code ;
} ;

struct	ERINA_HUFFMAN_TREE
{
	ERINA_HUFFMAN_NODE	m_hnTree[0x201] ;
	int					m_iSymLookup[0x100] ;
	int					m_iEscape ;
	int					m_iTreePointer ;

	// �ĥ�`�γ��ڻ�
	void Initialize( void ) ;
	// �k���l�Ȥ򥤥󥯥����
	void IncreaseOccuedCount( int iEntry ) ;
	// �H���ؤߤ���Ӌ�㤹��
	void RecountOccuredCount( int iParent ) ;
	// �ĥ�`����Ҏ��
	void Normalize( int iEntry ) ;
	// �¤����ե�`����ȥ�����ɤ���׷��
	void AddNewEntry( int nNewCode ) ;
	// �����F����2�֤�1�ˤ���ľ���٘���
	void HalfAndRebuild( void ) ;

} ;


/*****************************************************************************
                     ����󥰥�����ޥ���ƥ�����
 *****************************************************************************/

class	RLHDecodeContext
{
protected:
	int		m_flgZero ;			// ����ե饰
	ULONG	m_nLength ;			// ����󥰥�
	int		m_nIntBufCount ;	// ���g�����Хåե�����e����Ƥ���ӥå���
	DWORD	m_dwIntBuffer ;		// ���g�����Хåե�
	ULONG	m_nBufferingSize ;	// �Хåե���󥰤���Х�����
	ULONG	m_nBufCount ;		// �Хåե��βФ�Х�����
	PBYTE	m_ptrBuffer ;		// �����Хåե������^�ؤΥݥ���
	PBYTE	m_ptrNextBuf ;		// �Τ��i���z��٤������Хåե��ؤΥݥ���

	struct	ARITHCODE_SYMBOL
	{
		WORD	wOccured ;					// ����ܥ�γ��F����
		SWORD	wSymbol ;					// ����ܥ�
	} ;
	struct	STATISTICAL_MODEL
	{
		DWORD				dwTotalSymbolCount ;	// ȫ����ܥ��� < 10000H
		ARITHCODE_SYMBOL	SymbolTable[1] ;		// �yӋ��ǥ�
	} ;
	DWORD	m_dwCodeRegister ;		// ���`�ɥ쥸����
	DWORD	m_dwAugendRegister ;	// ���`������ɥ쥸����
	DWORD	m_dwCodeBuffer ;		// �ӥåȥХåե�
	int		m_nVirtualPostBuf ;		// ����Хåե�������
	int		m_nSymbolBitCount ;		// ���ŤΥӥå���
	int		m_nSymbolSortCount ;	// ���Ťξt��
	int		m_maskSymbolBit ;		// ���ŤΥӥåȥޥ���
	STATISTICAL_MODEL *	m_pLastStatisticalModel ;		// ����νyӋ��ǥ�
	STATISTICAL_MODEL *	m_pStatisticalTable[0x100] ;	// �yӋ��ǥ�

	// MTF �Ʃ`�֥�
	struct	MTF_TABLE
	{
		BYTE	iplt[0x100] ;

		inline void Initialize( void ) ;
		inline BYTE MoveToFront( int index ) ;
	} ;
	MTF_TABLE *	m_pMTFTable[0x101] ;

	// �ͺ��v��
	typedef	ULONG (RLHDecodeContext::*PFUNC_DECODE_SYMBOLS)
									( PINT ptrDst, ULONG nCount ) ;
	PFUNC_DECODE_SYMBOLS	m_pfnDecodeSymbols ;

	// �ϥեޥ�ľ
	ERINA_HUFFMAN_TREE *	m_pLastHuffmanTree ;
	ERINA_HUFFMAN_TREE *	m_pHuffmanTree[0x101] ;

public:
	// ���B�v��
	RLHDecodeContext( ULONG nBufferingSize ) ;
	// �����v��
	virtual ~RLHDecodeContext( void ) ;

	// �ΤΥǩ`�����i���z��
	virtual ULONG ReadNextData( PBYTE ptrBuffer, ULONG nBytes ) = 0 ;

	// ����ե饰���i���z��ǡ�����ƥ����Ȥ���ڻ�����
	void Initialize( void ) ;
	// �Хåե����դΕr���ΤΥǩ`�����i���z��
	int PrefetchBuffer( void ) ;
	// 1�ӥå�ȡ�ä��루 0 �֤�1�򷵤� ��
	INT GetABit( void ) ;
	// n�ӥå�ȡ�ä���
	UINT GetNBits( int n ) ;
	// ����ޥ��`�ɤ�ȡ�ä���
	INT GetACode( void ) ;

	// �Хåե���ե�å��夹��
	void FlushBuffer( void ) ;
	// ���g���ŤΏͺŤΜʂ䤹��
	void PrepareToDecodeArithmeticCode( int nBitCount ) ;
	// RL-MTF-G ���ŤΏͺŤΜʂ�򤹤�
	void PrepareToDecodeRLMTFGCode( void ) ;

	// չ�_�����ǩ`����ȡ�ä���
	inline ULONG DecodeSymbols( PINT ptrDst, ULONG nCount ) ;
	ULONG DecodeGammaCodes( PINT ptrDst, ULONG nCount ) ;
	ULONG DecodeArithmeticCodes( PINT ptrDst, ULONG nCount ) ;
	ULONG DecodeRLMTFGCodes( PBYTE ptrDst, ULONG nCount ) ;

	// ERINA ���ŤΏͺŤΜʂ�򤹤�
	void PrepareToDecodeERINACode( void ) ;
	// �ϥեޥ���Ť�ȡ�ä���
	int GetAHuffmanCode( ERINA_HUFFMAN_TREE * tree ) ;
	// �L���Υϥեޥ���Ť�ȡ�ä���
	int GetLengthHuffman( ERINA_HUFFMAN_TREE * tree ) ;
	// ERINA ���Ť�ͺŤ���
	ULONG DecodeERINACodes( PINT ptrDst, ULONG nCount ) ;
	// ERINA ���Ť�ͺŤ���
	ULONG DecodeERINACodesSBZL( SBYTE * ptrDst, ULONG nCount ) ;
	// ERINA ���ţ�256ɫ�ã���ͺŤ���
	ULONG DecodeERINACodes256( PBYTE ptrDst, ULONG nCount ) ;

} ;

//
// MTF_TABLE ����ڻ�����
//////////////////////////////////////////////////////////////////////////////
inline void RLHDecodeContext::MTF_TABLE::Initialize( void )
{
	for ( int i = 0; i < 0x100; i ++ )
		iplt[i] = (BYTE) i ;
}

//
// Move To Front ��g��
//////////////////////////////////////////////////////////////////////////////
inline BYTE RLHDecodeContext::MTF_TABLE::MoveToFront( int index )
{
	BYTE	pplt = iplt[index] ;
	while ( index > 0 )
	{
		iplt[index] = iplt[index - 1] ;
		index -- ;
	}
	return	(iplt[0] = pplt) ;
}

//
// չ�_�����ǩ`����ȡ�ä���
//////////////////////////////////////////////////////////////////////////////
inline ULONG RLHDecodeContext::DecodeSymbols( PINT ptrDst, ULONG nCount )
{
	return	(this->*m_pfnDecodeSymbols)( ptrDst, nCount ) ;
}


/*****************************************************************************
                               չ�_���֥�������
 *****************************************************************************/

class	ERIDecoder
{
protected:
	ERI_INFO_HEADER		m_EriInfHdr ;			// �������إå�

	UINT				m_nBlockSize ;			// �֥�å��󥰥�����
	ULONG				m_nBlockArea ;			// �֥�å���e
	ULONG				m_nBlockSamples ;		// �֥�å��Υ���ץ���
	UINT				m_nChannelCount ;		// ����ͥ���
	ULONG				m_nWidthBlocks ;		// ����η����֥�å�����
	ULONG				m_nHeightBlocks ;		// ����θߤ����֥�å�����

	UINT				m_nDstBytesPerPixel ;	// 1�ԥ�����ΥХ�����

	PBYTE				m_ptrOperations ;		// ���ڥ�`�����Ʃ`�֥�
	PINT				m_ptrColumnBuf ;		// �ХХåե�
	PINT				m_ptrLineBuf ;			// �ХХåե�
	PINT				m_ptrBuffer ;			// �ڲ��Хåե�
	PINT				m_ptrSubBuf ;			// �ڲ��Хåե�

	INT					m_fEnhancedMode ;		// ������`��
	PINT				m_pArrangeTable[4] ;	// ����ץ�󥰥Ʃ`�֥�

	ERINA_HUFFMAN_TREE *	m_pHuffmanTree ;	// �ϥեޥ�ľ

public:
	typedef	void (ERIDecoder::*PFUNC_COLOR_OPRATION)( void ) ;
	typedef	void (ERIDecoder::*PFUNC_RESTORE)
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	struct		OPERATION_CODE_II
	{
		int		ColorOperation ;
		int		ArrangeCode ;
	} ;
	typedef	void (ERIDecoder::*PFUNC_RESTOREII)
		( PBYTE ptrDst, LONG nLineBytes,
			REAL32 * ptrSrc[], int nWidth, int nHeight ) ;
	typedef	void (ERIDecoder::*PFUNC_BLOCK_SCALING)( void ) ;

protected:
	UINT				m_nBlocksetCount ;		// 1�֥�å����åȤΥ֥�å���
	PFUNC_BLOCK_SCALING	m_pfnBlockScaling ;		// �֥�å������`����v��

	REAL32 *			m_ptrBuffer2[4][4] ;	// �ڲ��Хåե�
	REAL32 *			m_ptrBuffer3[16] ;
	REAL32 *			m_ptrBuffer4[16] ;
	REAL32 *			m_ptrWorkDCT1 ;			// DCT ��Q�����I�I��
	REAL32 *			m_ptrWorkDCT2 ;
	INT					m_nDCDiffBuffer[16] ;	// DC �ɷֲ���åХåե�

	PINT				m_ptrTableDC ;			// ֱ���ɷ�

	REAL32				m_rMatrixScale ;		// DCT ���ЂS��
	REAL32				m_rYScaleDC ;			// ֱ���ɷ֤������ӻ��S��
	REAL32				m_rCScaleDC ;

protected:
	static PFUNC_COLOR_OPRATION	m_pfnColorOperation[0x10] ;
	static const OPERATION_CODE_II	m_opcCodeTable[0x40] ;

public:
	// ���B�v��
	ERIDecoder( void ) ;
	// �����v��
	virtual ~ERIDecoder( void ) ;

	// ���ڻ����ѥ��`�����O����
	int Initialize( const ERI_INFO_HEADER & infhdr ) ;
	// �K�ˣ�����ν�Ťʤɣ�
	void Delete( void ) ;
	// �����չ�_
	int DecodeImage
		( const RASTER_IMAGE_INFO & dstimginf,
			RLHDecodeContext & context, bool fTopDown ) ;

protected:
	// ����ץ�󥰥Ʃ`�֥�γ��ڻ�
	void InitializeArrangeTable( void ) ;
	void InitializeArrangeTableII( void ) ;
	// �ե륫��`�����չ�_
	int DecodeTrueColorImage
		( const RASTER_IMAGE_INFO & dstimginf, RLHDecodeContext & context ) ;
	// �ե륫��`���񒈏��ե��`�ޥåȤ�չ�_
	int DecodeTrueColorImageII
		( const RASTER_IMAGE_INFO & dstimginf, RLHDecodeContext & context ) ;
	// �ѥ�åȻ����չ�_
	int DecodePaletteImage
		( const RASTER_IMAGE_INFO & dstimginf, RLHDecodeContext & context ) ;

	// ���ڥ�`�����g��
	void PerformOperation
		( DWORD dwOpCode, LONG nAllBlockLines, PINT * ptrNextLineBuf ) ;

	// չ�_����򥹥ȥ������v���ؤΥݥ��󥿤�ȡ�ä���
	virtual PFUNC_RESTORE GetRestoreFunc
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

	// ���쥤�����չ�_
	void RestoreGray8
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB����(15�ӥå�)��չ�_
	void RestoreRGB16
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB�����չ�_
	void RestoreRGB24
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGBA�����չ�_
	void RestoreRGBA32
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;

protected:
	// ����ץ�󥰥Ʃ`�֥�γ��ڻ�
	void InitializeZigZagTable( void ) ;
	// �ե륫��`����ΈR�s
	int DecodeTrueColorImageDCT
		( const RASTER_IMAGE_INFO & imginf, RLHDecodeContext & context ) ;

	// չ�_����򥹥ȥ������v���ؤΥݥ��󥿤�ȡ�ä���
	virtual PFUNC_RESTOREII GetRestoreFuncII
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// �g��������������BYTE�ͤ��
	BYTE RoundR32ToByte( REAL32 r ) ;
	// RGB�����չ�_
	void RestoreRGB24II
		( PBYTE ptrDst, LONG nLineBytes,
			REAL32 * ptrSrc[], int nWidth, int nHeight ) ;
	// RGBA�����չ�_
	void RestoreRGBA32II
		( PBYTE ptrDst, LONG nLineBytes,
			REAL32 * ptrSrc[], int nWidth, int nHeight ) ;

	// YUV -> RGB ��Q�v��
	void ConvertYUVtoRGB( void ) ;

	// 4:4:4 �����`���
	void BlockScaling444( void ) ;
	// 4:2:2 �����`���
	void BlockScaling422( void ) ;
	// 4:1:1 �����`���
	void BlockScaling411( void ) ;

	// IDCT ��Q��ʩ��
	void PerformIDCT( void ) ;

	// �����ӻ���ʩ��
	void ArrangeAndIQuantumize( BYTE bytCoefficient[] ) ;

protected:
	// չ�_�M��״�r֪ͨ�v��
	virtual int OnDecodedBlock( LONG line, LONG column ) ;
	virtual int OnDecodedLine( LONG line ) ;

} ;


/*****************************************************************************
                     �ե����륹�ȥ�`�ॳ��ƥ�����
 *****************************************************************************/

class	EFileDecodeContext	: public	RLHDecodeContext
{
public:
	// ���B�v��
	EFileDecodeContext( EFileObject * pFileObj, ULONG nBufferingSize ) ;
	// �����v��
	virtual ~EFileDecodeContext( void ) ;

protected:
	EFileObject *	m_pFileObj ;

public:
	// �ΤΥǩ`�����i���z��
	virtual ULONG ReadNextData( PBYTE ptrBuffer, ULONG nBytes ) ;
	// �ե����륪�֥������Ȥ��O������
	void AttachFileObject( EFileObject * pFileObj ) ;

} ;


#endif

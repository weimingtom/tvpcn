
/*****************************************************************************
                         E R I N A - L i b r a r y
                                                      最終更新 2001/05/23
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(EXPERI_H_INCLUDED)
#define	EXPERI_H_INCLUDED


/*****************************************************************************
                       ライブラリ初期化終了関数
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
                         メモリアロケーション
 *****************************************************************************/

extern	PVOID eriAllocateMemory( DWORD dwBytes ) ;
extern	void eriFreeMemory( PVOID ptrMem ) ;


/*****************************************************************************
                                画像情報
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
                            アニメーション用関数
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
                                 描画関数
 *****************************************************************************/

extern	"C"
{
	void eriDrawImageToDC
		( HDC hDC, int x, int y, const RASTER_IMAGE_INFO & rii,
			const ENTIS_PALETTE * ppalette = NULL, SIZE * psize = NULL ) ;

} ;


/*****************************************************************************
                             ファイルオブジェクト
 *****************************************************************************/

class	EFileObject
{
public:
	// 構築関数
	EFileObject( void ) ;
	// 消滅関数
	virtual ~EFileObject( void ) ;
	// データを読み込む
	virtual DWORD Read( void * ptrBuf, DWORD dwByte ) = 0 ;
	// ファイルの長さを取得する
	virtual DWORD GetLength( void ) = 0 ;
	// ファイルポインタを取得する
	virtual DWORD GetPointer( void ) = 0 ;
	// ファイルポインタを移動する
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
                         ハフマンツリー構造体
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

	// ツリーの初期化
	void Initialize( void ) ;
	// 発生頻度をインクリメント
	void IncreaseOccuedCount( int iEntry ) ;
	// 親の重みを再計算する
	void RecountOccuredCount( int iParent ) ;
	// ツリーの正規化
	void Normalize( int iEntry ) ;
	// 新しいフリーエントリを作成して追加
	void AddNewEntry( int nNewCode ) ;
	// 各出現数を2分の1にして木を再構成
	void HalfAndRebuild( void ) ;

} ;


/*****************************************************************************
                     ランレングスガンマコンテキスト
 *****************************************************************************/

class	RLHDecodeContext
{
protected:
	int		m_flgZero ;			// ゼロフラグ
	ULONG	m_nLength ;			// ランレングス
	int		m_nIntBufCount ;	// 中間入力バッファに蓄積されているビット数
	DWORD	m_dwIntBuffer ;		// 中間入力バッファ
	ULONG	m_nBufferingSize ;	// バッファリングするバイト数
	ULONG	m_nBufCount ;		// バッファの残りバイト数
	PBYTE	m_ptrBuffer ;		// 入力バッファの先頭へのポインタ
	PBYTE	m_ptrNextBuf ;		// 次に読み込むべき入力バッファへのポインタ

	struct	ARITHCODE_SYMBOL
	{
		WORD	wOccured ;					// シンボルの出現回数
		SWORD	wSymbol ;					// シンボル
	} ;
	struct	STATISTICAL_MODEL
	{
		DWORD				dwTotalSymbolCount ;	// 全シンボル数 < 10000H
		ARITHCODE_SYMBOL	SymbolTable[1] ;		// 統計モデル
	} ;
	DWORD	m_dwCodeRegister ;		// コードレジスタ
	DWORD	m_dwAugendRegister ;	// オージェンドレジスタ
	DWORD	m_dwCodeBuffer ;		// ビットバッファ
	int		m_nVirtualPostBuf ;		// 仮想バッファカウンタ
	int		m_nSymbolBitCount ;		// 符号のビット数
	int		m_nSymbolSortCount ;	// 符号の総数
	int		m_maskSymbolBit ;		// 符号のビットマスク
	STATISTICAL_MODEL *	m_pLastStatisticalModel ;		// 最後の統計モデル
	STATISTICAL_MODEL *	m_pStatisticalTable[0x100] ;	// 統計モデル

	// MTF テーブル
	struct	MTF_TABLE
	{
		BYTE	iplt[0x100] ;

		inline void Initialize( void ) ;
		inline BYTE MoveToFront( int index ) ;
	} ;
	MTF_TABLE *	m_pMTFTable[0x101] ;

	// 復号関数
	typedef	ULONG (RLHDecodeContext::*PFUNC_DECODE_SYMBOLS)
									( PINT ptrDst, ULONG nCount ) ;
	PFUNC_DECODE_SYMBOLS	m_pfnDecodeSymbols ;

	// ハフマン木
	ERINA_HUFFMAN_TREE *	m_pLastHuffmanTree ;
	ERINA_HUFFMAN_TREE *	m_pHuffmanTree[0x101] ;

public:
	// 構築関数
	RLHDecodeContext( ULONG nBufferingSize ) ;
	// 消滅関数
	virtual ~RLHDecodeContext( void ) ;

	// 次のデータを読み込む
	virtual ULONG ReadNextData( PBYTE ptrBuffer, ULONG nBytes ) = 0 ;

	// ゼロフラグを読み込んで、コンテキストを初期化する
	void Initialize( void ) ;
	// バッファが空の時、次のデータを読み込む
	int PrefetchBuffer( void ) ;
	// 1ビット取得する（ 0 又は1を返す ）
	INT GetABit( void ) ;
	// nビット取得する
	UINT GetNBits( int n ) ;
	// ガンマコードを取得する
	INT GetACode( void ) ;

	// バッファをフラッシュする
	void FlushBuffer( void ) ;
	// 算術符号の復号の準備する
	void PrepareToDecodeArithmeticCode( int nBitCount ) ;
	// RL-MTF-G 符号の復号の準備をする
	void PrepareToDecodeRLMTFGCode( void ) ;

	// 展開したデータを取得する
	inline ULONG DecodeSymbols( PINT ptrDst, ULONG nCount ) ;
	ULONG DecodeGammaCodes( PINT ptrDst, ULONG nCount ) ;
	ULONG DecodeArithmeticCodes( PINT ptrDst, ULONG nCount ) ;
	ULONG DecodeRLMTFGCodes( PBYTE ptrDst, ULONG nCount ) ;

	// ERINA 符号の復号の準備をする
	void PrepareToDecodeERINACode( void ) ;
	// ハフマン符号を取得する
	int GetAHuffmanCode( ERINA_HUFFMAN_TREE * tree ) ;
	// 長さのハフマン符号を取得する
	int GetLengthHuffman( ERINA_HUFFMAN_TREE * tree ) ;
	// ERINA 符号を復号する
	ULONG DecodeERINACodes( PINT ptrDst, ULONG nCount ) ;
	// ERINA 符号を復号する
	ULONG DecodeERINACodesSBZL( SBYTE * ptrDst, ULONG nCount ) ;
	// ERINA 符号（256色用）を復号する
	ULONG DecodeERINACodes256( PBYTE ptrDst, ULONG nCount ) ;

} ;

//
// MTF_TABLE を初期化する
//////////////////////////////////////////////////////////////////////////////
inline void RLHDecodeContext::MTF_TABLE::Initialize( void )
{
	for ( int i = 0; i < 0x100; i ++ )
		iplt[i] = (BYTE) i ;
}

//
// Move To Front を実行
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
// 展開したデータを取得する
//////////////////////////////////////////////////////////////////////////////
inline ULONG RLHDecodeContext::DecodeSymbols( PINT ptrDst, ULONG nCount )
{
	return	(this->*m_pfnDecodeSymbols)( ptrDst, nCount ) ;
}


/*****************************************************************************
                               展開オブジェクト
 *****************************************************************************/

class	ERIDecoder
{
protected:
	ERI_INFO_HEADER		m_EriInfHdr ;			// 画像情報ヘッダ

	UINT				m_nBlockSize ;			// ブロッキングサイズ
	ULONG				m_nBlockArea ;			// ブロック面積
	ULONG				m_nBlockSamples ;		// ブロックのサンプル数
	UINT				m_nChannelCount ;		// チャネル数
	ULONG				m_nWidthBlocks ;		// 画像の幅（ブロック数）
	ULONG				m_nHeightBlocks ;		// 画像の高さ（ブロック数）

	UINT				m_nDstBytesPerPixel ;	// 1ピクセルのバイト数

	PBYTE				m_ptrOperations ;		// オペレーションテーブル
	PINT				m_ptrColumnBuf ;		// 列バッファ
	PINT				m_ptrLineBuf ;			// 行バッファ
	PINT				m_ptrBuffer ;			// 内部バッファ
	PINT				m_ptrSubBuf ;			// 内部バッファ

	INT					m_fEnhancedMode ;		// 拡張モード
	PINT				m_pArrangeTable[4] ;	// サンプリングテーブル

	ERINA_HUFFMAN_TREE *	m_pHuffmanTree ;	// ハフマン木

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
	UINT				m_nBlocksetCount ;		// 1ブロックセットのブロック数
	PFUNC_BLOCK_SCALING	m_pfnBlockScaling ;		// ブロックスケーリング関数

	REAL32 *			m_ptrBuffer2[4][4] ;	// 内部バッファ
	REAL32 *			m_ptrBuffer3[16] ;
	REAL32 *			m_ptrBuffer4[16] ;
	REAL32 *			m_ptrWorkDCT1 ;			// DCT 変換用作業領域
	REAL32 *			m_ptrWorkDCT2 ;
	INT					m_nDCDiffBuffer[16] ;	// DC 成分差分用バッファ

	PINT				m_ptrTableDC ;			// 直流成分

	REAL32				m_rMatrixScale ;		// DCT 行列係数
	REAL32				m_rYScaleDC ;			// 直流成分の逆量子化係数
	REAL32				m_rCScaleDC ;

protected:
	static PFUNC_COLOR_OPRATION	m_pfnColorOperation[0x10] ;
	static const OPERATION_CODE_II	m_opcCodeTable[0x40] ;

public:
	// 構築関数
	ERIDecoder( void ) ;
	// 消滅関数
	virtual ~ERIDecoder( void ) ;

	// 初期化（パラメータの設定）
	int Initialize( const ERI_INFO_HEADER & infhdr ) ;
	// 終了（メモリの解放など）
	void Delete( void ) ;
	// 画像を展開
	int DecodeImage
		( const RASTER_IMAGE_INFO & dstimginf,
			RLHDecodeContext & context, bool fTopDown ) ;

protected:
	// サンプリングテーブルの初期化
	void InitializeArrangeTable( void ) ;
	void InitializeArrangeTableII( void ) ;
	// フルカラー画像の展開
	int DecodeTrueColorImage
		( const RASTER_IMAGE_INFO & dstimginf, RLHDecodeContext & context ) ;
	// フルカラー画像拡張フォーマットの展開
	int DecodeTrueColorImageII
		( const RASTER_IMAGE_INFO & dstimginf, RLHDecodeContext & context ) ;
	// パレット画像の展開
	int DecodePaletteImage
		( const RASTER_IMAGE_INFO & dstimginf, RLHDecodeContext & context ) ;

	// オペレーション実行
	void PerformOperation
		( DWORD dwOpCode, LONG nAllBlockLines, PINT * ptrNextLineBuf ) ;

	// 展開画像をストアする関数へのポインタを取得する
	virtual PFUNC_RESTORE GetRestoreFunc
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// カラーオペレーション関数群
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

	// グレイ画像の展開
	void RestoreGray8
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB画像(15ビット)の展開
	void RestoreRGB16
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB画像の展開
	void RestoreRGB24
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGBA画像の展開
	void RestoreRGBA32
		( PBYTE ptrDst, LONG nLineBytes, int nWidth, int nHeight ) ;

protected:
	// サンプリングテーブルの初期化
	void InitializeZigZagTable( void ) ;
	// フルカラー画像の圧縮
	int DecodeTrueColorImageDCT
		( const RASTER_IMAGE_INFO & imginf, RLHDecodeContext & context ) ;

	// 展開画像をストアする関数へのポインタを取得する
	virtual PFUNC_RESTOREII GetRestoreFuncII
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// 実数を整数に丸めてBYTE型に飽和
	BYTE RoundR32ToByte( REAL32 r ) ;
	// RGB画像の展開
	void RestoreRGB24II
		( PBYTE ptrDst, LONG nLineBytes,
			REAL32 * ptrSrc[], int nWidth, int nHeight ) ;
	// RGBA画像の展開
	void RestoreRGBA32II
		( PBYTE ptrDst, LONG nLineBytes,
			REAL32 * ptrSrc[], int nWidth, int nHeight ) ;

	// YUV -> RGB 変換関数
	void ConvertYUVtoRGB( void ) ;

	// 4:4:4 スケーリング
	void BlockScaling444( void ) ;
	// 4:2:2 スケーリング
	void BlockScaling422( void ) ;
	// 4:1:1 スケーリング
	void BlockScaling411( void ) ;

	// IDCT 変換を施す
	void PerformIDCT( void ) ;

	// 逆量子化を施す
	void ArrangeAndIQuantumize( BYTE bytCoefficient[] ) ;

protected:
	// 展開進行状況通知関数
	virtual int OnDecodedBlock( LONG line, LONG column ) ;
	virtual int OnDecodedLine( LONG line ) ;

} ;


/*****************************************************************************
                     ファイルストリームコンテキスト
 *****************************************************************************/

class	EFileDecodeContext	: public	RLHDecodeContext
{
public:
	// 構築関数
	EFileDecodeContext( EFileObject * pFileObj, ULONG nBufferingSize ) ;
	// 消滅関数
	virtual ~EFileDecodeContext( void ) ;

protected:
	EFileObject *	m_pFileObj ;

public:
	// 次のデータを読み込む
	virtual ULONG ReadNextData( PBYTE ptrBuffer, ULONG nBytes ) ;
	// ファイルオブジェクトを設定する
	void AttachFileObject( EFileObject * pFileObj ) ;

} ;


#endif

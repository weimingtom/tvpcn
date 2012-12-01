
/*****************************************************************************
                         E R I N A - L i b r a r y
                                                      ��K���� 2001/05/29
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(ERINALIB_H_INCLUDE)
#define	ERINALIB_H_INCLUDE

#include "experi.h"
#include "cmperi.h"
#include "cxmio.h"



/*****************************************************************************
                        ERI �ե����륤�󥿩`�ե��`��
 *****************************************************************************/

class	ERIFile	: public	EFileObject
{
protected:
	EFileObject *	m_pFile ;
	struct	RECORD_HEADER
	{
		UINT64	ui64ID ;
		UINT64	ui64Length ;
	} ;
	struct	RECORD_INFO
	{
		RECORD_HEADER	header ;
		RECORD_INFO *	pParent ;
		DWORD			dwOffset ;
		DWORD			dwLimit ;
	} ;
	RECORD_INFO *	m_pRecInf ;

public:
	// �����Х��֥�������
	class	EStringObj
	{
	protected:
		char *	m_ptrString ;
		int		m_nLength ;
	public:
		// ���B�v��
		EStringObj( void ) ;
		EStringObj( const char * pszString ) ;
		EStringObj( const wchar_t * pwszString ) ;
		// �����v��
		~EStringObj( void ) ;
		// �����в���
		operator const char * ( void ) const
			{	return	m_ptrString ;	}
		const char * CharPtr( void ) const
			{	return	m_ptrString ;	}
		int GetLength( void ) const
			{	return	m_nLength ;		}
		// ����
		const EStringObj & operator = ( const EStringObj & string ) ;
	} ;

	// �i���z�ߥޥ���
	enum	ReadMask
	{
		rmFileHeader	= 0x00000001,
		rmPreviewInfo	= 0x00000002,
		rmImageInfo		= 0x00000004,
		rmSoundInfo		= 0x00000008,
		rmCopyright		= 0x00000010,
		rmDescription	= 0x00000020,
		rmPaletteTable	= 0x00000040
	} ;
	DWORD			m_fdwReadMask ;
	// �ե�����إå�
	ERI_FILE_HEADER	m_FileHeader ;
	// �ץ�ӥ�`�������إå�
	ERI_INFO_HEADER	m_PreviewInfo ;
	// �������إå�
	ERI_INFO_HEADER	m_InfoHeader ;
	// �������إå�
	MIO_INFO_HEADER	m_MIOInfHdr ;
	// �ѥ�åȥƩ`�֥�
	ENTIS_PALETTE	m_PaletteTable[0x100] ;
	// ���������
	EStringObj		m_strCopyright ;
	// ������
	EStringObj		m_strDescription ;

public:
	// ���B�v��
	ERIFile( void ) ;
	// �����v��
	virtual ~ERIFile( void ) ;

	// �ե�����Υ��`�ץ󷽷�
	enum	OpenType
	{
		otOpenRoot,			// ��`�ȥ쥳�`�ɤ��_������
		otReadHeader,		// ���إå��쥳�`�ɤ��i���z��ǂ�����^
		otOpenStream,		// �إå����i���z�ߥ��ȥ�`��쥳�`�ɤ��_��
		otOpenImageData		// ����ǩ`���쥳�`�ɤ��_��
	} ;
	// �{������ե�������_��
	bool Open( EFileObject * pFile, OpenType type = otOpenImageData ) ;
	// �ե�������]����
	void Close( void ) ;

	// �쥳�`�ɤ��_��
	bool DescendRecord( const UINT64 * pRecID = NULL ) ;
	// �쥳�`�ɤ��]����
	void AscendRecord( void ) ;
	// �쥳�`��ID��ȡ�ä���
	UINT64 GetRecordID( void ) const ;

	virtual DWORD Read( void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;


/*****************************************************************************
                    MIO�ե����륹�ȥ�`���������֥�������
 *****************************************************************************/

class	MIODynamicPlayer
{
public:
	// ���B�v��
	MIODynamicPlayer( void ) ;
	// �����v��
	~MIODynamicPlayer( void ) ;

protected:
	//
	// �쥳�`�����i�ߥ��֥�������
	class	EPreloadBuffer	: public	EMemFile
	{
	public:
		BYTE *			m_ptrBuffer ;
		ULONG			m_nKeySample ;
		MIO_DATA_HEADER	m_miodh ;
	public:
		// ���B�v��
		EPreloadBuffer( DWORD dwLength ) ;
		// �����v��
		virtual ~EPreloadBuffer( void ) ;
	} ;
	//
	// ���`�ե�`��ݥ���Ș�����
	struct	KEY_POINT
	{
		ULONG	nKeySample ;
		DWORD	dwRecOffset ;
	} ;
	//
	// EPreloadBuffer ���Х��֥�������
	class	EPreloadQueue
	{
	public:
		EPreloadBuffer **	m_queue ;
		unsigned int		m_length ;
		unsigned int		m_limit ;
	public:
		// ���B�v��
		EPreloadQueue( void ) ;
		// �����v��
		~EPreloadQueue( void ) ;
		// ���Ф������L�����O��
		void SetLimit( unsigned int nLimit ) ;
		// ���Ф���ȥ
		void RemoveAll( void ) ;
	} ;
	//
	// KEY_POINT ���Х��֥�������
	class	EKeyPointList
	{
	public:
		KEY_POINT *		m_list ;
		unsigned int	m_count ;
		unsigned int	m_limit ;
	public:
		// ���B�v��
		EKeyPointList( void ) ;
		// �����v��
		~EKeyPointList( void ) ;
		// ���Ф�����L�����O��
		void SetLimit( unsigned int nLimit ) ;
		// ���Ф���ȥ
		void RemoveAll( void ) ;
	} ;
	//
	// ����å��å�å��`��
	enum	ThreadMessage
	{
		tmSeekSound	= WM_USER,
		tmQuit
	} ;

	// ERI���˥�`�����ե�����
	ERIFile					m_erif ;
	// չ�_���֥�������
	EFileDecodeContext *	m_pmioc ;
	MIODecoder				m_miod ;
	// ���i�ߥ���å�
	HANDLE					m_hThread ;
	DWORD					m_idThread ;
	// ���i�ߥ���`
	EPreloadQueue			m_queueSound ;
	HANDLE					m_hQueueNotEmpty ;	// ���i�ߥ���`���դǤʤ�
	HANDLE					m_hQueueSpace ;		// ���i�ߥ���`���i���z�߿���
	// �������`���å��`�ݥ��������
	EKeyPointList			m_listKeySample ;
	// �����ĄI��
	CRITICAL_SECTION		m_cs ;

public:
	// MIO �ե�������_��
	bool Open( EFileObject * pFile, unsigned int nPreloadSize = 0 ) ;
	// MIO �ե�������]����
	void Close( void ) ;

	// ָ������ץ���ƄӤ�������Υ֥�å��Υǩ`����ȡ�ä���
	virtual void * GetWaveBufferFrom
		( ULONG nSample, DWORD & dwBytes, DWORD & dwOffsetBytes ) ;
	// �Τ������ǩ`�������ȥ�`������^�Ǥ��뤫��
	virtual bool IsNextDataRewound( void ) ;
	// �Τ������ǩ`����ȡ��
	virtual void * GetNextWaveBuffer( DWORD & dwBytes ) ;
	// �����Хåե��_��
	virtual void * AllocateWaveBuffer( DWORD dwBytes ) ;
	// �����ǩ`���Ɨ��S��
	virtual void DeleteWaveBuffer( void * ptrWaveBuf ) ;

public:
	// ����ͥ�����ȡ�ä���
	DWORD GetChannelCount( void ) const ;
	// ����ץ���ܲ�����ȡ�ä���
	DWORD GetFrequency( void ) const ;
	// ����ץ�󥰥ӥåȷֽ��ܤ�ȡ�ä���
	DWORD GetBitsPerSample( void ) const ;
	// ȫ����L��������ץ�������ȡ�ä���
	DWORD GetTotalSampleCount( void ) const ;

protected:
	// ���i�ߥХåե���ȡ�ä���
	EPreloadBuffer * GetPreloadBuffer( void ) ;
	// ���i�ߥХåե���׷�Ӥ���
	void AddPreloadBuffer( EPreloadBuffer * pBuffer ) ;

protected:
	// ����å��v��
	static DWORD WINAPI ThreadProc( LPVOID parameter ) ;
	DWORD LoadingThreadProc( void ) ;
	// ���`�ե�`��ݥ���Ȥ�׷�Ӥ���
	void AddKeySample( KEY_POINT key ) ;
	// ָ���Υ��`�ե�`����������
	KEY_POINT * SearchKeySample( unsigned int nKeySample ) ;
	// ָ���Υ���ץ�򺬤�֥�å����i���z��
	void SeekKeySample( ULONG nSample, unsigned int & nCurrentSample ) ;

public:
	// �����I������ƥ����륻�������
	void Lock( void ) ;
	void Unlock( void ) ;

} ;


/*****************************************************************************
                    ERI���˥�`�����ե����륪�֥�������
 *****************************************************************************/

class	ERIAnimation
{
public:
	// ���B�v��
	ERIAnimation( void ) ;
	// �����v��
	~ERIAnimation( void ) ;

protected:
	//
	// �쥳�`�����i�ߥ��֥�������
	class	EPreloadBuffer	: public	EMemFile
	{
	public:
		BYTE *	m_ptrBuffer ;
		ULONG	m_iFrameIndex ;
		UINT64	m_ui64RecType ;
	public:
		// ���B�v��
		EPreloadBuffer( DWORD dwLength ) ;
		// �����v��
		virtual ~EPreloadBuffer( void ) ;
	} ;
	//
	// ���`�ե�`��ݥ���Ș�����
	struct	KEY_POINT
	{
		ULONG	iKeyFrame ;
		DWORD	dwRecOffset ;
	} ;
	//
	// EPreloadBuffer ���Х��֥�������
	class	EPreloadQueue
	{
	public:
		EPreloadBuffer **	m_queue ;
		unsigned int		m_length ;
		unsigned int		m_limit ;
	public:
		// ���B�v��
		EPreloadQueue( void ) ;
		// �����v��
		~EPreloadQueue( void ) ;
		// ���Ф������L�����O��
		void SetLimit( unsigned int nLimit ) ;
		// ���Ф���ȥ
		void RemoveAll( void ) ;
	} ;
	//
	// KEY_POINT ���Х��֥�������
	class	EKeyPointList
	{
	public:
		KEY_POINT *		m_list ;
		unsigned int	m_count ;
		unsigned int	m_limit ;
	public:
		// ���B�v��
		EKeyPointList( void ) ;
		// �����v��
		~EKeyPointList( void ) ;
		// ���Ф�����L�����O��
		void SetLimit( unsigned int nLimit ) ;
		// ���Ф���ȥ
		void RemoveAll( void ) ;
	} ;
	//
	// ����å��å�å��`��
	enum	ThreadMessage
	{
		tmSeekFrame	= WM_USER,
		tmSeekSound,
		tmQuit
	} ;

	// ERI���˥�`�����ե�����
	bool					m_fTopDown ;
	bool					m_fWaveOutput ;
	bool					m_fWaveStreaming ;
	ERIFile					m_erif ;
	// չ�_���֥�������
	EFileDecodeContext *	m_peric ;
	ERIDecoder				m_erid ;
	EFileDecodeContext *	m_pmioc ;
	MIODecoder				m_miod ;
	// ����Хåե�
	unsigned int			m_iCurrentFrame ;
	RASTER_IMAGE_INFO *		m_pDstImage ;
	ENTIS_PALETTE *			m_pPaletteTable ;
	RASTER_IMAGE_INFO *		m_pWorkImage ;
	// ���i�ߥ���å�
	HANDLE					m_hThread ;
	DWORD					m_idThread ;
	// �������i�ߥ���`
	EPreloadQueue			m_queueImage ;
	HANDLE					m_hQueueNotEmpty ;	// ���i�ߥ���`���դǤʤ�
	HANDLE					m_hQueueSpace ;		// ���i�ߥ���`���i���z�߿���
	// �ե�`�ॷ�`���å��`�ե�`��ݥ��������
	EKeyPointList			m_listKeyFrame ;
	EKeyPointList			m_listKeyWave ;
	// �����ĄI��
	CRITICAL_SECTION		m_cs ;

protected:
	// ����չ�_�����Хåե�Ҫ��
	virtual RASTER_IMAGE_INFO * CreateImageBuffer
		( DWORD format, SDWORD width, SDWORD height,
					DWORD bpp, ENTIS_PALETTE ** ppaltbl ) ;
	// ����չ�_�����Хåե���ȥ
	virtual void DeleteImageBuffer
		( RASTER_IMAGE_INFO * prii, ENTIS_PALETTE * paltbl ) ;
	// ��������Ҫ��
	virtual bool RequestWaveOut
		( DWORD channels, DWORD frequency, DWORD bps ) ;
	// ���������K��
	virtual void CloseWaveOut( void ) ;
	// �����ǩ`������
	virtual void PushWaveBuffer( void * ptrWaveBuf, DWORD dwBytes ) ;
	// �����Хåե��_��
	virtual void * AllocateWaveBuffer( DWORD dwBytes ) ;
	// �����ǩ`���Ɨ��S��
	virtual void DeleteWaveBuffer( void * ptrWaveBuf ) ;
public:
	// �������ȥ�`�ߥ��_ʼ
	virtual void BeginWaveStreaming( void ) ;
	// �������ȥ�`�ߥ󥰽K��
	virtual void EndWaveStreaming( void ) ;

public:
	// ���˥�`�����ե�������_��
	bool Open( EFileObject * pFile,
			unsigned int nPreloadSize = 0, bool fTopDown = false ) ;
	// ���˥�`�����ե�������]����
	void Close( void ) ;

	// ���^�ե�`����Ƅ�
	bool SeekToBegin( void ) ;
	// �ΤΥե�`����Ƅ�
	bool SeekToNextFrame( int nSkipFrame = 0 ) ;
	// ָ���Υե�`����Ƅ�
	bool SeekToFrame( unsigned int iFrameIndex ) ;
	// ָ���Υե�`��ϥ��`�ե�`�फ��
	bool IsKeyFrame( unsigned int iFrameIndex ) ;

protected:
	// �ΤΥե�`���չ�_����
	bool DecodeNextFrame( void ) ;
	// ���i�ߥХåե���ȡ�ä���
	EPreloadBuffer * GetPreloadBuffer( void ) ;
	// ���i�ߥХåե���׷�Ӥ���
	void AddPreloadBuffer( EPreloadBuffer * pBuffer ) ;

public:
	// �����ȥե�`��Υ���ǥå�����ȡ�ä���
	unsigned int CurrentIndex( void ) const ;
	// �����ȥե�`��λ����ȡ��
	const RASTER_IMAGE_INFO * GetImageInfo( void ) const ;
	// �ѥ�åȥƩ`�֥�ȡ��
	const ENTIS_PALETTE * GetPaletteEntries( void ) const ;
	// ���`�ե�`���ȡ��
	unsigned int GetKeyFrameCount( void ) const ;
	// ȫ�ե�`������ȡ��
	unsigned int GetAllFrameCount( void ) const ;
	// ȫ���˥�`�����r�g��ȡ��
	unsigned int GetTotalTime( void ) const ;
	// �ե�`�෬�Ť���r�g�؉�Q
	unsigned int FrameIndexToTime( unsigned int iFrameIndex ) const ;
	// �r�g����ե�`�෬�Ť؉�Q
	unsigned int TimeToFrameIndex( unsigned int nMilliSec ) const ;

protected:
	// ����å��v��
	static DWORD WINAPI ThreadProc( LPVOID parameter ) ;
	DWORD LoadingThreadProc( void ) ;
	// ���`�ե�`��ݥ���Ȥ�׷�Ӥ���
	void AddKeyPoint( EKeyPointList & list, KEY_POINT key ) ;
	// ָ���Υ��`�ե�`����������
	KEY_POINT * SearchKeyPoint
		( EKeyPointList & list, unsigned int iKeyFrame ) ;
	// ָ���Υե�`��˥��`������
	void SeekKeyPoint
		( EKeyPointList & list,
			unsigned int iFrame, unsigned int & iCurtrentFrame ) ;
	// ָ���������ǩ`���ޤǥ��`�����ƥ��ȥ�`�ߥ󥰳�������
	void SeekKeyWave
		( EKeyPointList & list, unsigned int iFrame ) ;

public:
	// �����I������ƥ����륻�������
	void Lock( void ) ;
	void Unlock( void ) ;

} ;


/*****************************************************************************
                  ERI���˥�`�����ե�����������֥�������
 *****************************************************************************/

class	ERIAnimationWriter
{
public:
	// ���B�v��
	ERIAnimationWriter( void ) ;
	// �����v��
	~ERIAnimationWriter( void ) ;

protected:
	//
	// �����Хåե����֥�������
	class	EOutputBuffer
	{
	public:
		EOutputBuffer *	m_ptrNext ;
		DWORD			m_dwBytes ;
		PVOID			m_ptrBuffer ;
	public:
		// ���B�v��
		EOutputBuffer( const BYTE * ptrBuffer, ULONG nBytes ) ;
		// �����v��
		~EOutputBuffer( void ) ;
	} ;
	//
	// �R�s����ƥ�����
	class	EEncodeContext	: public	RLHEncodeContext
	{
	public:
		EOutputBuffer *	m_pFirstBuf ;
		EOutputBuffer *	m_pLastBuf ;
		DWORD			m_dwTotalBytes ;
	public:
		// ���B�v��
		EEncodeContext( void ) ;
		// �����v��
		virtual ~EEncodeContext( void ) ;
		// �ǩ`����ȥ
		void Delete( void ) ;
		// �ΤΥǩ`�����������
		virtual ULONG WriteNextData( const BYTE * ptrBuffer, ULONG nBytes ) ;
	} ;
	//
	// ����åɥ�å��`��
	enum	ThreadMessage
	{
		tmEncodeImage	= WM_USER,
		tmQuit
	} ;

	// �����������Ʃ`����
	enum	WriterStatus
	{
		wsNotOpened,
		wsOpened,
		wsWritingHeader,
		wsWritingStream
	} ;
	WriterStatus			m_wsStatus ;		// ���Ʃ`����
	// �ե����륪�֥�������
	ERIWriteFile			m_eriwf ;			// �ե����륪�֥�������
	// �إå����
	DWORD					m_dwHeaderBytes ;	// �إå��쥳�`�ɤΥХ�����
	ERI_FILE_HEADER			m_efh ;				// �ե�����إå�
	ERI_INFO_HEADER			m_prvwih ;			// �ץ�ӥ�`�������إå�
	ERI_INFO_HEADER			m_eih ;				// �������إå�
	MIO_INFO_HEADER			m_mih ;				// �������إå�
	// �ե�`�෬��
	DWORD					m_dwKeyFrame ;		// ���`�ե�`��
	DWORD					m_dwKeyWave ;		// ���`�����`��
	DWORD					m_dwFrameCount ;	// �����g�ߥե�`��t��
	DWORD					m_dwWaveCount ;		// �����g�ߥ����`�־t��
	// �����������
	DWORD					m_dwMioHeaderPos ;		// �������إå���λ��
	DWORD					m_dwOutputWaveSamples ;	// �����g�ߥ���ץ���
	// �R�s���֥�������
	EEncodeContext			m_eric1 ;
	EEncodeContext			m_eric2 ;
	ERIEncoder				m_erie1 ;
	ERIEncoder				m_erie2 ;
	EFileEncodeContext *	m_pmioc ;
	MIOEncoder				m_mioe ;
	// ��քI���åХåե�
	RASTER_IMAGE_INFO *		m_priiLast ;
	RASTER_IMAGE_INFO *		m_priiBuf ;
	// ����R�s����å�
	bool					m_fDualEncoding ;
	bool					m_fCompressSuccessed ;
	HANDLE					m_hCompressed ;
	HANDLE					m_hThread ;
	DWORD					m_idThread ;
	// ����ΈR�s�ѥ��`��
	ERIEncoder::PARAMETER	m_eriep ;
	// �����ΈR�s�ѥ��`��
	double					m_rLowWeight ;
	double					m_rMiddleWeight ;
	double					m_rPowerScale ;
	int						m_nOddWeight ;

public:
	// �ե�������_��
	bool Open( EWriteFileObj * pFile, ERIWriteFile::FileID fidType ) ;
	// �ե�������]����
	void Close( void ) ;

public:
	// �ե�����إå����_��
	bool BeginFileHeader( DWORD dwKeyFrame, DWORD dwKeyWave ) ;
	// �ץ�ӥ�`�������إå����������
	bool WritePreviewInfo( const ERI_INFO_HEADER & eih ) ;
	// �������إå����������
	bool WriteEriInfoHeader( const ERI_INFO_HEADER & eih ) ;
	// �������إå����������
	bool WriteMioInfoHeader( const MIO_INFO_HEADER & mih ) ;
	// �����������������
	bool WriteCopyright( const void * ptrCopyright, DWORD dwBytes ) ;
	// �����Ȥ��������
	bool WriteDescription( const void * ptrDescription, DWORD dwBytes ) ;
	// �ե�����إå����]����
	void EndFileHeader( void ) ;

public:
	// ����ΈR�s�ѥ��`�����O������
	void SetImageCompressionParameter
			( const ERIEncoder::PARAMETER & eriep ) ;
	// �����ΈR�s�ѥ��`�����O������
	void SetSoundCompressionParameter
		( double rLowWeight = 16.0,
			double rMiddleWeight = 8.0,
			double rPowerScale = 0.5,
			int nOddWeight = 1 ) ;

public:
	// ���ȥ�`����_ʼ����
	bool BeginStream( void ) ;
	// �ѥ�åȥƩ`�֥���������
	bool WritePaletteTable
		( const ENTIS_PALETTE * paltbl, unsigned int nLength ) ;
	// �ץ�ӥ�`������������
	bool WritePreviewData( const RASTER_IMAGE_INFO & rii, DWORD fdwFlags ) ;
	// �����ǩ`�����������
	bool WriteWaveData( const void * ptrWaveBuf, DWORD dwSampleCount ) ;
	// ����ǩ`�����������
	bool WriteImageData( const RASTER_IMAGE_INFO & rii, DWORD fdwFlags ) ;
	// ���ȥ�`����]����
	bool EndStream( DWORD dwTotalTime ) ;

protected:
	// ����Хåե�������
	RASTER_IMAGE_INFO * CreateImageBuffer( const ERI_INFO_HEADER & eih ) ;
	// ����Хåե�����ȥ
	void DeleteImageBuffer( RASTER_IMAGE_INFO * prii ) ;

public:
	// �������줿�����ö����ȡ�ä���
	DWORD GetWrittenFrameCount( void ) const ;
	// �ޥ������åɈR�s���Є���
	void EnableDualEncoding( bool fDualEncoding ) ;

protected:
	// ����å��v��
	static DWORD WINAPI ThreadProc( LPVOID parameter ) ;
	DWORD EncodingThreadProc( void ) ;

} ;


#endif

/****************************************************************************/
/*! @file
@brief Windows Media Reader

Windows Mediaファイルを�iみ�zむ
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/22
@note
*****************************************************************************/

#include "CWMReader.h"
#include "CWMAllocator.h"
#include "CWMBuffer.h"

//## CWMOutput
//----------------------------------------------------------------------------
//! @brief	  	CWMOutput destructor
//----------------------------------------------------------------------------
CWMOutput::~CWMOutput()
{}
//----------------------------------------------------------------------------
//! @brief	  	勣箔されたインタ�`フェイスを卦す
//! @param		riid : インタ�`フェイスのIID
//! @param		ppv : インタ�`フェイスを卦すポインタ�`へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CWMOutput::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	return CUnknown::NonDelegatingQueryInterface(riid, ppv);
}
//----------------------------------------------------------------------------
//! @brief	  	IWMSyncReaderを函誼する
//! @return		IWMSyncReaderへのポインタ
//----------------------------------------------------------------------------
IWMSyncReader *CWMOutput::WMReader()
{
	return Reader()->Reader();
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムの�O協を佩う
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::SetStream(void)
{
    WMT_STREAM_SELECTION	wmtSS = WMT_ON;
	HRESULT hr = S_OK;
	if( IsEnable() )
	{
		if( FAILED( hr = WMReader()->SetStreamsSelected( 1, &m_StreamNum, &wmtSS ) ) )
			return( hr );

		if( FAILED(hr = WMReader()->SetReadStreamSamples( m_StreamNum, TRUE ) ) )
			return( hr );
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	サポ�`トしているメディアタイプを函誼する
//!
//! �R�sフォ�`マットのみ竃薦するようになっている。
//! @param		iPosition : メディアタイプの會方
//! @param		pmt : メディアタイプ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::GetMediaType( int iPosition, CMediaType *pmt )
{
	if( iPosition != 0 ) return VFW_S_NO_MORE_ITEMS;

	HRESULT	hr;
	CComPtr<IWMProfile>	readerProfile;
	if( FAILED( hr = WMReader()->QueryInterface( IID_IWMProfile, (void **)&readerProfile ) ) )
        return( hr );

	CComPtr<IWMStreamConfig>	streamConfig;
	if( FAILED(hr = readerProfile->GetStreamByNumber( m_StreamNum, &streamConfig )) )
		return hr;

	CComPtr<IWMMediaProps> pMediaProps;
	if( FAILED(hr = streamConfig.QueryInterface( &pMediaProps ) ) )
		return hr;

	DWORD	mediaTypeSize;
	if( FAILED( hr = pMediaProps->GetMediaType( NULL, &mediaTypeSize ) ) )
		return hr;

	std::vector<BYTE>	buff(mediaTypeSize,0);
	WM_MEDIA_TYPE	*pMediaType = reinterpret_cast<WM_MEDIA_TYPE*>( &( buff.at(0) ) );
	if( FAILED( hr = pMediaProps->GetMediaType( pMediaType, &mediaTypeSize ) ) )
		return hr;

	GUID	type = pMediaType->subtype;
	if( type == WMMEDIASUBTYPE_MP43 || type == WMMEDIASUBTYPE_MP4S || type == WMMEDIASUBTYPE_MPEG2_VIDEO ||
		type == WMMEDIASUBTYPE_MSS1 || type == WMMEDIASUBTYPE_MSS2 || type == WMMEDIASUBTYPE_WMVP ||
		type == WMMEDIASUBTYPE_WMAudio_Lossless || type == WMMEDIASUBTYPE_WMAudioV2 || type == WMMEDIASUBTYPE_WMAudioV7 ||
		type == WMMEDIASUBTYPE_WMAudioV8 || type == WMMEDIASUBTYPE_WMAudioV9 || type == WMMEDIASUBTYPE_WMSP1 ||
		type == WMMEDIASUBTYPE_WMV1 || type == WMMEDIASUBTYPE_WMV2 || type == WMMEDIASUBTYPE_WMV3 )
	{
		pmt->Set( *(reinterpret_cast<AM_MEDIA_TYPE*>(pMediaType)) );
		return hr;
	}
	return VFW_S_NO_MORE_ITEMS;
}
//----------------------------------------------------------------------------
//! @brief	  	メディアタイプを�O協する
//! @param		pmt : メディアタイプ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::SetMediaType( const CMediaType *pmt )
{
	CMediaType	mt;
	if( GetMediaType(0, &mt ) == S_OK )
	{
		if( mt.majortype == pmt->majortype &&
			mt.subtype == pmt->subtype &&
			mt.bFixedSizeSamples == pmt->bFixedSizeSamples &&
			mt.bTemporalCompression == pmt->bTemporalCompression &&
//			mt.lSampleSize == pmt->lSampleSize &&
			mt.formattype == pmt->formattype )
		{
			return S_OK;
		}
	}
	return VFW_E_INVALIDMEDIATYPE;
}
//----------------------------------------------------------------------------
//! @brief	  	駅勣なバッファサイズを函誼する
//! @param		buffers : バッファの方
//! @param		bufsize : バッファのサイズ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::GetNeedBufferSize( long &buffers, long &bufsize )
{
	HRESULT		hr;
	DWORD max;
	if( FAILED(hr = WMReader()->GetMaxStreamSampleSize( m_StreamNum, &max ) ) )
		return hr;

//	buffers = 2;
	buffers = 7;
	bufsize = max;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	バッファサイズの�Q協を宥岑する
//! @param		buffers : バッファの方
//! @param		bufsize : バッファのサイズ
//! @param		aling : アライメント
//! @param		prefix : プリフィックス
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::SetDecidedBufferSize( long buffers, long bufsize, long aling, long prefix )
{
	// 蒙に採もしない
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	肝のサンプルを誼る
//! @param		pSample : サンプルを卦すポインタのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::GetNextSample( IMediaSample **pSample )
{
	HRESULT hr;
	if( m_StreamNum == 0 || pSample == NULL )
		return S_FALSE;	// このストリ�`ムはない

	INSSBuffer	*pWMSample = NULL;
	QWORD	cnsSampleTime;
	QWORD	cnsDuration;
	DWORD	dwFlags;

	if( FAILED(hr = WMReader()->GetNextSample( m_StreamNum, &pWMSample, &cnsSampleTime, &cnsDuration, &dwFlags, NULL, NULL )) )
	{
		if( hr == NS_E_NO_MORE_SAMPLES ) return S_FALSE;
		return hr;
	}

	REFERENCE_TIME	startTime = (REFERENCE_TIME)cnsSampleTime;
	REFERENCE_TIME	endTime = (REFERENCE_TIME)(cnsSampleTime + cnsDuration);
	IMediaSample *pOutSample = reinterpret_cast<CWMBuffer*>(pWMSample)->GetSample();
	pOutSample->AddRef();
	pWMSample->Release();
	pOutSample->SetMediaTime(&startTime, &endTime);
#if 0
	if( startTime < Reader()->m_StartTime )
		pOutSample->SetPreroll(TRUE);
	else
		pOutSample->SetPreroll(FALSE);
#endif
	startTime -= Reader()->m_StartTime;
	endTime -= Reader()->m_StartTime;
	pOutSample->SetTime(&startTime, &endTime);
	pOutSample->SetSyncPoint(dwFlags & WM_SF_CLEANPOINT);
	*pSample = pOutSample;

	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	アロケ�`タ�`を�O協する
//! @param		alloc : アロケ�`タ�`
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMOutput::SetAllocator( IMemAllocator *alloc )
{
	HRESULT		hr;

	if( alloc == NULL )
		return E_INVALIDARG;

	CComPtr<IWMSyncReader2>	reader2;
	if( FAILED(hr = WMReader()->QueryInterface( &reader2 ) ) )
		return hr;

	CWMAllocator *wmAlloc = new CWMAllocator( alloc );
	CComPtr<IWMReaderAllocatorEx>	pWMRAE;
	pWMRAE = wmAlloc;

	hr = reader2->SetAllocateForStream( m_StreamNum, pWMRAE );
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	メディアタイプがDXVAかどうか�_�Jする
//! 
//! 登協圭隈はWMF SDK Helpの "Enabling DirectX Video Acceleration" に��峰されているもの
//! @param		pmt : メディアタイプ
//! @return		DXVAかどうか
//----------------------------------------------------------------------------
bool CWMOutput::IsDXVASubtype( const AM_MEDIA_TYPE *pmt )
{
	GUID guidDXVA = { 0x00000000, 0xa0c7, 0x11d3, { 0xb9,0x84,0x00,0xc0,0x4f,0x2e,0x73,0xc5 } };
	unsigned long const *plguid;
	unsigned long const *plguidDXVA;
	plguid = (unsigned long const *)&pmt->subtype;
	plguidDXVA = (unsigned long *)&guidDXVA;

	if( ( plguid[1] == plguidDXVA[1] ) && ( plguid[2] == plguidDXVA[2] ) && ( plguid[3] == plguidDXVA[3] ) )
		return true;
	return false;
}


//## CWMReader
#pragma warning(disable: 4355)	// コンストラクタのベ�`スメンバ兜豚晒�rにthisを聞うとワ�`ニングが竃るのでそれを雙峭
//----------------------------------------------------------------------------
//! @brief	  	CWMReader constructor
//----------------------------------------------------------------------------
CWMReader::CWMReader()
: m_AudioOut(this), m_VideoOut(this), m_StartTime(0), m_EndTime(0), m_Rate(1.0)
{
	m_AudioOut.AddRef();
	m_VideoOut.AddRef();
}
#pragma warning(default: 4355)
//----------------------------------------------------------------------------
//! @brief	  	CWMReader destructor
//----------------------------------------------------------------------------
CWMReader::~CWMReader()
{
	if( m_HeaderInfo.p )
		m_HeaderInfo.Release();

	if( m_WMReader.p )
		m_WMReader.Release();
}
//----------------------------------------------------------------------------
//! @brief	  	竃薦方を函誼する ( 啌�颪頒�蕗なら2つなど )
//! @return		竃薦方
//----------------------------------------------------------------------------
int CWMReader::GetNumberOfOutput(void)
{
	return (m_AudioOut.IsEnable() != 0 ? 1 : 0) + (m_VideoOut.IsEnable() != 0 ? 1 : 0);
}
//----------------------------------------------------------------------------
//! @brief	  	竃薦ストリ�`ムを誼る
//! @param		num : 竃薦ストリ�`ム桑催
//! @return		竃薦ストリ�`ム
//----------------------------------------------------------------------------
IOutputStream* CWMReader::GetOutputStream( int num )
{
	if( num >= 2 || num < 0 )
		return NULL;

	if( m_VideoOut.IsEnable() )
	{
		if( num == 0 )
			return &m_VideoOut;
		else if( num == 1 && m_AudioOut.IsEnable() )
			return &m_AudioOut;
		else
			return NULL;
	}
	else if( m_AudioOut.IsEnable() )
	{
		if( num == 0 )
			return &m_AudioOut;
		else
			return NULL;
	}
	else
		return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムを�_く
//! @param		stream : ストリ�`ム
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::OpenStream( IStream *stream )
{
	HRESULT		hr = S_OK;
	if( stream == NULL )
		return E_INVALIDARG;

	if( m_WMReader.p == NULL )
	{	// DLLからWMCreateSyncReaderを�iみ�zんで聞喘する
		if( m_WmvDll.IsLoaded() == false )
			m_WmvDll.Load("wmvcore.dll");
		if( m_WmvDll.IsLoaded() == false )
			return m_WmvDll.GetLastError();

		typedef HRESULT (*FuncWMCreateSyncReader)( IUnknown *pUnkCert, DWORD dwRights, IWMSyncReader **ppSyncReader );
		FuncWMCreateSyncReader pWMCreateSyncReader = (FuncWMCreateSyncReader)m_WmvDll.GetProcAddress("WMCreateSyncReader");
		if( pWMCreateSyncReader == NULL )
			return m_WmvDll.GetLastError();
		if( FAILED(hr = pWMCreateSyncReader(  NULL, 0, &m_WMReader ) ))
			return hr;
	}

	if( FAILED( hr = Reader()->OpenStream( stream ) ) )
		return hr;

	CComPtr<IWMProfile>	pProfile;
	if( FAILED(hr = Reader()->QueryInterface( &pProfile ) ) )
		return hr;

	if( FAILED(hr = GetStreamNumbers( pProfile ) ) )
		return hr;

	if( FAILED(hr = Reader()->QueryInterface( &m_HeaderInfo ) ) )
		return hr;

	if( FAILED(hr = SetStream() ) )
		return hr;

//	hr = SetRange();
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏�_兵�rにコ�`ルされる
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::OnStart()
{
	HRESULT hr;
	if( FAILED(hr = SetStream() ) )
		return hr;
	if( FAILED(hr = SetRange() ) )
		return hr;

	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムが�]じられる�rにコ�`ルされる
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::Close(void)
{
	Reader()->Close();
	m_AudioOut.SetStreamNumber( 0 );
	m_VideoOut.SetStreamNumber( 0 );
	m_StartTime = 0;
	m_EndTime = 0;
	m_Rate = 1.0;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	�_兵�r�gを�O協する
//! @param		start : �_兵�r�g
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::SetStartTime( QWORD start )
{
	m_StartTime = start;
	return SetRange();
}
//----------------------------------------------------------------------------
//! @brief	  	唯峭�r�gを�O協する
//! @param		end : 唯峭�r�g
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::SetEndTime( QWORD end )
{
	m_EndTime = end;
	return SetRange();
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏レ�`トを�O協する
//! @param		rate : 壅伏レ�`ト
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::SetRate( double rate )
{
	m_Rate = rate;
	// 蒙に聞わない
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏���譴淋O協を郡啌する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::SetRange(void)
{
	QWORD		duration;
	if( m_EndTime != 0 )
		duration = m_EndTime - m_StartTime;
	else
		duration = GetDuration() - m_StartTime;
	return Reader()->SetRange( m_StartTime, duration );
}
//----------------------------------------------------------------------------
//! @brief	  	竃薦するストリ�`ムの�O協を佩う
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::SetStream(void)
{
	HRESULT hr;
	if( FAILED(hr = m_VideoOut.SetStream() ) )
		return( hr );
	hr = m_AudioOut.SetStream();
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	メディアの光�N�､鯣ゝ辰垢�
//! @param		name : 勣箔するものの兆念
//! @return		勣箔した��
//----------------------------------------------------------------------------
QWORD CWMReader::GetAttributeQWORD( LPCWSTR name )
{
	HRESULT	hr;
	WORD	stream = 0;
	QWORD	result = 0;
	WORD	length = sizeof(QWORD);
	WMT_ATTR_DATATYPE	attrType;
	hr = HeaderInf()->GetAttributeByName( &stream, name, &attrType, (BYTE*)&result, &length );
	if( FAILED( hr ) || attrType != WMT_TYPE_QWORD || length != sizeof(QWORD) )
	{
		result = 0;
	}
	return result;
}
//----------------------------------------------------------------------------
//! @brief	  	メディアの光�N�､鯣ゝ辰垢�
//! @param		name : 勣箔するものの兆念
//! @return		勣箔した��
//----------------------------------------------------------------------------
DWORD CWMReader::GetAttributeDWORD( LPCWSTR name )
{
	HRESULT	hr;
	WORD	stream = 0;
	DWORD	result = 0;
	WORD	length = sizeof(DWORD);
	WMT_ATTR_DATATYPE	attrType;
	hr = HeaderInf()->GetAttributeByName( &stream, name, &attrType, (BYTE*)&result, &length );
	if( FAILED( hr ) || attrType != WMT_TYPE_DWORD || length != sizeof(DWORD) )
	{
		result = 0;
	}
	return result;
}
//----------------------------------------------------------------------------
//! @brief	  	メディアの�r�g嫌を函誼する
//! @return		�r�g嫌
//----------------------------------------------------------------------------
QWORD CWMReader::GetDuration(void)
{
	return GetAttributeQWORD(g_wszWMDuration);
}
//----------------------------------------------------------------------------
//! @brief	  	メディアのフレ�`ム方を函誼する
//! @return		フレ�`ム方
//----------------------------------------------------------------------------
QWORD CWMReader::GetNumberOfFrame(void)
{
	return GetAttributeQWORD(g_wszWMNumberOfFrames);
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの嫌を函誼する
//! @return		ビデオの嫌
//----------------------------------------------------------------------------
DWORD CWMReader::GetVideoWidth(void)
{
	return GetAttributeDWORD(g_wszWMVideoWidth);
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの互さを函誼する
//! @return		ビデオの互さ
//----------------------------------------------------------------------------
DWORD CWMReader::GetVideoHeight(void)
{
	return GetAttributeDWORD(g_wszWMVideoHeight);
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオのフレ�`ムレ�`トを函誼する
//! @return		ビデオのフレ�`ムレ�`ト
//----------------------------------------------------------------------------
DWORD CWMReader::GetVideoFrameRate(void)
{
	return GetAttributeDWORD(g_wszWMVideoFrameRate);
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ム桑催を函誼する
//!
//! m_AudioOut.m_StreamNumとm_VideoOut.m_StreamNumへ�､鰓O協する;
//! ストリ�`ム桑催は1-63の�､鬚箸�
//! @param		pProfile : プロファイル
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CWMReader::GetStreamNumbers( IWMProfile* pProfile )
{
	HRESULT		hr = S_OK;
	GUID		streamType;

	if ( NULL == pProfile )
		return( E_INVALIDARG );

	DWORD	numOfStreams = 0;
	if( FAILED(hr = pProfile->GetStreamCount( &numOfStreams ) ) )
		return( hr );

	m_AudioOut.SetStreamNumber( 0 );
	m_VideoOut.SetStreamNumber( 0 );

	for( DWORD i = 0; i < numOfStreams; i++ )
	{
		CComPtr<IWMStreamConfig>	pStream;
		if( FAILED(hr = pProfile->GetStream( i, &pStream ) ) )
			break;

		WORD streamNumber = 0;
		if( FAILED(hr = pStream->GetStreamNumber( &streamNumber ) ) )
			break;

		if( FAILED(hr = pStream->GetStreamType( &streamType ) ) )
			break;

		// 匯桑兜めに��つかったストリ�`ムを聞う
		if( WMMEDIATYPE_Audio == streamType && m_AudioOut.GetStreamNumber() == 0 )
			m_AudioOut.SetStreamNumber( streamNumber );
		else if( WMMEDIATYPE_Video == streamType && m_VideoOut.GetStreamNumber() == 0 )
			m_VideoOut.SetStreamNumber( streamNumber );
	}
	return hr;
}




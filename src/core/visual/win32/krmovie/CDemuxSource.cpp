/****************************************************************************/
/*! @file
@brief Demux source filter

デマルチプレクサソ�`スフィルタを�g廾する
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/22
@note
*****************************************************************************/

#include <assert.h>
#include <streams.h>
#include "CDemuxSource.h"
#include "CDemuxOutputPin.h"

// {68D40D07-25DB-4c49-AAAA-327DF25387AA}
static const GUID CLSID_DemuxSource = { 0x68d40d07, 0x25db, 0x4c49, { 0xaa, 0xaa, 0x32, 0x7d, 0xf2, 0x53, 0x87, 0xaa } };


//----------------------------------------------------------------------------
//! @brief	  	CDemuxSource constructor
//! @param		lpunk : 鹿撹された侭嗤宀オブジェクトへのポインタ。
//! @param		phr : HRESULT �､悗離櫂ぅ鵐拭�
//! @param		reader : このフィルタのリ�`ダ�`
//! @param		clsid : このフィルタのクラスID
//----------------------------------------------------------------------------
CDemuxSource::CDemuxSource( LPUNKNOWN lpunk, HRESULT *phr, IDemuxReader *reader, CLSID clsid )
: CSource( NAME("Demux Source"), lpunk, clsid, phr ), m_DemuxReader(reader)//, m_cRef(0)
{
	m_rtStop = _I64_MAX / 2;
	m_rtDuration = m_rtStop;
	m_dRateSeeking = 1.0;
	m_dwSeekingCaps = AM_SEEKING_CanSeekForwards | AM_SEEKING_CanSeekBackwards | AM_SEEKING_CanSeekAbsolute
		| AM_SEEKING_CanGetStopPos | AM_SEEKING_CanGetDuration;
}
//----------------------------------------------------------------------------
//! @brief	  	CDemuxSource destructor
//----------------------------------------------------------------------------
CDemuxSource::~CDemuxSource()
{
	delete m_DemuxReader;
}
//----------------------------------------------------------------------------
//! @brief	  	勣箔されたインタ�`フェイスを卦す
//! @param		riid : インタ�`フェイスのIID
//! @param		ppv : インタ�`フェイスを卦すポインタ�`へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CDemuxSource::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	if( riid == IID_IMediaSeeking ) {
		return GetInterface( static_cast<IMediaSeeking*>( this ), ppv );
	} else if( riid == IID_IAMFilterMiscFlags ) {
		return GetInterface( static_cast<IAMFilterMiscFlags*>( this ), ppv );
	} else {
		return CSource::NonDelegatingQueryInterface(riid, ppv);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏したいストリ�`ムを�_く
//! @param		stream : ム�`ビ�`へのストリ�`ム
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::OpenStream( IStream *stream )
{
	CAutoLock lock(&m_crtFilterLock);

	HRESULT hr;
	if( (hr = Reader()->OpenStream(stream)) != S_OK )
		return hr;

	m_rtDuration = Reader()->GetDuration();
	m_rtStop = m_rtDuration;

	ClearPins();
	for( int i = 0; i < Reader()->GetNumberOfOutput(); ++i )
	{
		IOutputStream	*outStream = Reader()->GetOutputStream(i);
		assert( outStream );
		CDemuxOutputPin *pin = new CDemuxOutputPin( NAME("Demux Source"), this, &hr, L"Demux output pin", this, outStream, &m_crtFilterLock );
//		pin->AddRef();
		// AddPinはnewした粁�Aでコ�`ルされるので、苧幣議に弖紗する駅勣はない
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏レ�`トが�筝�された�rにコ�`ルされる
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ChangeRate(void)
{
	{	// クリティカル セクション ロックのスコ�`プ。
		CAutoLock cAutoLock(&m_crtFilterLock);
		if( m_dRateSeeking <= 0 ) {
			m_dRateSeeking = 1.0;  // �m俳な�､縫螢札奪箸垢襦�
			return E_FAIL;
		}
	}
	UpdateFromSeek();
	return Reader()->SetRate( m_dRateSeeking );
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏が�_兵された�rにコ�`ルされる
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ChangeStart(void)
{
	UpdateFromSeek();
	if( m_rtStart > m_rtDuration  )
		m_rtStart = m_rtDuration;	// 来め

	return Reader()->SetStartTime( m_rtStart );
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏が唯峭された�rにコ�`ルされる
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ChangeStop(void)
{
	UpdateFromSeek();
	return Reader()->SetEndTime(m_rtStop);
}
//----------------------------------------------------------------------------
//! @brief	  	隠隔しているピンをすべて��茅する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
void CDemuxSource::ClearPins()
{
	while( GetPinCount() > 0 )
		delete GetPin(GetPinCount()-1);
}
//----------------------------------------------------------------------------
//! @brief	  	フィルタをポ�`ズする
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
STDMETHODIMP CDemuxSource::Pause()
{
	{
		CAutoLock lock(&m_crtFilterLock);
		if( m_State == State_Stopped )
		{
			HRESULT	hr = Reader()->OnStart();
			if( FAILED(hr) )
				return hr;
			NewSegment();
		}
	}
	return CSource::Pause();
}
//----------------------------------------------------------------------------
//! @brief	  	シ�`クからの�筝�を郡啌する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
void CDemuxSource::UpdateFromSeek()
{
	DeliverBeginFlush();
	Stop();
	DeliverEndFlush();
}
//----------------------------------------------------------------------------
//! @brief	  	仟たに壅伏が�_兵されたことを宥岑する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::NewSegment()
{
	return DeliverNewSegment(m_rtStart, m_rtStop, m_dRateSeeking);
}
//----------------------------------------------------------------------------
//! @brief	  	仟たに壅伏が�_兵されたことをダウンストリ�`ムへ宥岑する
//! @param		tStart : �_兵�r�g
//! @param		tStop : 唯峭�r�g
//! @param		dRate : 壅伏レ�`ト
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	int cPins = GetPinCount();
	for( int c = 0; c < cPins; c++ )
	{
		CBaseOutputPin *pPin = reinterpret_cast<CBaseOutputPin*>(GetPin(c));
		if( pPin->IsConnected() ) {
			HRESULT hr = pPin->DeliverNewSegment(tStart,tStop,dRate);
			if (FAILED(hr)) {
				return hr;
			}
		}
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムが�K極に�_したことをダウンストリ�`ムへ宥岑する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::DeliverEndOfStream(void)
{
	int cPins = GetPinCount();
	for( int c = 0; c < cPins; c++ )
	{
		CBaseOutputPin *pPin = reinterpret_cast<CBaseOutputPin*>(GetPin(c));
		if( pPin->IsConnected() ) {
			HRESULT hr = pPin->DeliverEndOfStream();
			if (FAILED(hr)) {
				return hr;
			}
		}
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	フラッシュ�I尖の�K阻をダウンストリ�`ムへ勣箔する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::DeliverEndFlush(void)
{
	int cPins = GetPinCount();
	for( int c = 0; c < cPins; c++ )
	{
		CBaseOutputPin *pPin = reinterpret_cast<CBaseOutputPin*>(GetPin(c));
		if( pPin->IsConnected() ) {
			HRESULT hr = pPin->DeliverEndFlush();
			if (FAILED(hr)) {
				return hr;
			}
		}
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	フラッシュ�I尖の�_兵をダウンストリ�`ムへ勣箔する
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::DeliverBeginFlush(void)
{
	int cPins = GetPinCount();
	for( int c = 0; c < cPins; c++ )
	{
		CBaseOutputPin *pPin = reinterpret_cast<CBaseOutputPin*>(GetPin(c));
		if( pPin->IsConnected() ) {
			HRESULT hr = pPin->DeliverBeginFlush();
			if (FAILED(hr)) {
				return hr;
			}
		}
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	フィルタタイプを函誼する
//! @return		フィルタタイプ
//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CDemuxSource::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓俊�Aされているピンの方を函誼する
//! @return		俊�A方
//----------------------------------------------------------------------------
ULONG CDemuxSource::GetNumberOfConnection()
{
	ULONG	result = 0;
	int cPins = GetPinCount();
	for( int c = 0; c < cPins; c++ )
	{
		CBaseOutputPin *pPin = reinterpret_cast<CBaseOutputPin*>(GetPin(c));
		if( pPin->IsConnected() ) {
			++result;
		}
	}
	return result;
}
//----------------------------------------------------------------------------
//! @brief	  	タイムフォ�`マットがサポ�`トされているかどうか�_�Jする
//! @param		pFormat : タイムフォ�`マット
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::IsFormatSupported(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	// only seeking in time (REFERENCE_TIME units) is supported
	return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムの��枠タイム フォ�`マットを函誼する
//! @param		pFormat : タイムフォ�`マット
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::QueryPreferredFormat(GUID *pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	タイムフォ�`マットを�O協する
//! @param		pFormat : タイムフォ�`マット
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::SetTimeFormat(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	// nothing to set; just check that it's TIME_FORMAT_TIME
	return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : E_INVALIDARG;
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したタイム フォ�`マットが�F壓聞われているフォ�`マットかどうかを�_�Jする
//! @param		pFormat : タイムフォ�`マット
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::IsUsingTimeFormat(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓のタイム フォ�`マットを函誼する
//! @param		pFormat : タイムフォ�`マット
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetTimeFormat(GUID *pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムの�r�g嫌を函誼する
//! @param		pDuration : �Lさ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetDuration(LONGLONG *pDuration)
{
	CheckPointer(pDuration, E_POINTER);
	CAutoLock lock(&m_crtFilterLock);
	*pDuration = m_rtDuration;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムの唯峭�r�gを函誼する
//! @param		pStop : 唯峭�r�g
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetStopPosition(LONGLONG *pStop)
{
	CheckPointer(pStop, E_POINTER);
	CAutoLock lock(&m_crtFilterLock);
	*pStop = m_rtStop;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ストリ�`ムの�F壓�r�gを函誼する
//! @param		pCurrent : �F壓�r�g
//! @return		隆サポ�`ト
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetCurrentPosition(LONGLONG *pCurrent)
{
	// GetCurrentPosition is typically supported only in renderers and
	// not in source filters.
	return E_NOTIMPL;
}
//----------------------------------------------------------------------------
//! @brief	  	シ�`ク嬬薦を函誼する
//! @param		pCapabilities : シ�`ク嬬薦
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetCapabilities( DWORD * pCapabilities )
{
	CheckPointer(pCapabilities, E_POINTER);
	*pCapabilities = m_dwSeekingCaps;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	峺協したシ�`ク嬬薦をストリ�`ムが隔っているかどうかを��い栽わせる
//! @param		pCapabilities : シ�`ク嬬薦
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::CheckCapabilities( DWORD * pCapabilities )
{
	CheckPointer(pCapabilities, E_POINTER);
	// make sure all requested capabilities are in our mask
	return (~m_dwSeekingCaps & *pCapabilities) ? S_FALSE : S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	1 つのタイム フォ�`マットから�eのタイム フォ�`マットに���Qする
//! @param		pTarget : ���Qされたタイムを鞭け函る�篳�へのポインタ
//! @param		pTargetFormat : タ�`ゲット フォ�`マットのタイム フォ�`マット GUID へのポインタ。NULL の��栽は、�F壓のフォ�`マットが聞われる
//! @param		Source : ���Qするタイム��
//! @param		pSourceFormat : ���Qするフォ�`マットのタイム フォ�`マット GUID へのポインタ。NULL の��栽は、�F壓のフォ�`マットが聞われる
//! @return		エラ�`コ�`ド
//! @note	TIME_FORMAT_MEDIA_TIME參翌はサポ�`トしていない
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ConvertTimeFormat( LONGLONG * pTarget, const GUID * pTargetFormat, LONGLONG Source, const GUID * pSourceFormat )
{
	CheckPointer(pTarget, E_POINTER);
	// format guids can be null to indicate current format

	// since we only support TIME_FORMAT_MEDIA_TIME, we don't really
	// offer any conversions.
	if(pTargetFormat == 0 || *pTargetFormat == TIME_FORMAT_MEDIA_TIME)
	{
		if(pSourceFormat == 0 || *pSourceFormat == TIME_FORMAT_MEDIA_TIME)
		{
			*pTarget = Source;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓了崔と唯峭了崔を�O協する
//! @param		pCurrent : �F壓了崔を峺協する�篳�へのポインタ、�F壓のタイム フォ�`マットの�g了
//! @param		CurrentFlags : 了崔を峺協するためのフラグのビットごとの�Mみ栽わせ
//! @param		pStop : �K阻タイムを峺協する�篳�へのポインタ、�F壓のタイム フォ�`マットの�g了
//! @param		StopFlags : 了崔を峺協するためのフラグのビットごとの�Mみ栽わせ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::SetPositions( LONGLONG * pCurrent, DWORD CurrentFlags, LONGLONG * pStop,  DWORD StopFlags )
{
	DWORD StopPosBits = StopFlags & AM_SEEKING_PositioningBitsMask;
	DWORD StartPosBits = CurrentFlags & AM_SEEKING_PositioningBitsMask;

	if(StopFlags) {
		CheckPointer(pStop, E_POINTER);
		// accept only relative, incremental, or absolute positioning
		if(StopPosBits != StopFlags) {
			return E_INVALIDARG;
		}
	}

	if(CurrentFlags) {
		CheckPointer(pCurrent, E_POINTER);
		if(StartPosBits != AM_SEEKING_AbsolutePositioning &&
			StartPosBits != AM_SEEKING_RelativePositioning) {
			return E_INVALIDARG;
		}
	}

	{	// scope for autolock
		CAutoLock lock(&m_crtFilterLock);

		// set start position
		if(StartPosBits == AM_SEEKING_AbsolutePositioning) {
			m_rtStart = *pCurrent;
		} else if(StartPosBits == AM_SEEKING_RelativePositioning) {
			m_rtStart += *pCurrent;
		}

		// set stop position
		if(StopPosBits == AM_SEEKING_AbsolutePositioning) {
			m_rtStop = *pStop;
		} else if(StopPosBits == AM_SEEKING_IncrementalPositioning) {
			m_rtStop = m_rtStart + *pStop;
		} else if(StopPosBits == AM_SEEKING_RelativePositioning) {
			m_rtStop = m_rtStop + *pStop;
		}
	}

	HRESULT hr = S_OK;
	if(SUCCEEDED(hr) && StopPosBits) {
		hr = ChangeStop();
	}
	if(StartPosBits) {
		hr = ChangeStart();
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	�F壓の了崔と唯峭了崔を函誼する
//! @param		pCurrent : �_兵了崔を鞭け函る�篳�へのポインタ
//! @param		pStop : 唯峭了崔を鞭け函る�篳�へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetPositions( LONGLONG * pCurrent, LONGLONG * pStop )
{
	if(pCurrent) {
		*pCurrent = m_rtStart;
	}
	if(pStop) {
		*pStop = m_rtStop;
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	シ�`クが嗤�燭淵織ぅ爐旅��譴鯣ゝ辰垢�
//! @param		pEarliest : シ�`クが嗤�燭遍遒瞑腓ぅ織ぅ爐鯤椶曳，��篳�へのポインタ
//! @param		pLatest : シ�`クが嗤�燭遍遒矍Wいタイムを鞭け函る�篳�へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetAvailable( LONGLONG * pEarliest, LONGLONG * pLatest )
{
	if(pEarliest) {
		*pEarliest = 0;
	}
	if(pLatest) {
		CAutoLock lock(&m_crtFilterLock);
		*pLatest = m_rtDuration;
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏レ�`トを�O協する
//! @param		dRate : 壅伏レ�`ト
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::SetRate( double dRate)
{
	{
		CAutoLock lock(&m_crtFilterLock);
		m_dRateSeeking = dRate;
	}
	return ChangeRate();
}
//----------------------------------------------------------------------------
//! @brief	  	壅伏レ�`トを函誼する
//! @param		dRate : 壅伏レ�`ト
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetRate( double * pdRate)
{
	CheckPointer(pdRate, E_POINTER);
	CAutoLock lock(&m_crtFilterLock);
	*pdRate = m_dRateSeeking;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	�_兵了崔の念にキュ�`に秘るデ�`タの楚を函誼する
//! @param		pPreroll : プリロ�`ル タイムを鞭け函る�篳�へのポインタ
//! @return		エラ�`コ�`ド
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetPreroll(LONGLONG *pPreroll)
{
	CheckPointer(pPreroll, E_POINTER);
	*pPreroll = 0;
	return S_OK;
}

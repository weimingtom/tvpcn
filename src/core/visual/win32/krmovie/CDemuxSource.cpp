/****************************************************************************/
/*! @file
@brief Demux source filter

�ǥޥ���ץ쥯�����`���ե��륿��gװ����
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
//! @param		lpunk : ���ɤ��줿�����ߥ��֥������ȤؤΥݥ��󥿡�
//! @param		phr : HRESULT ���ؤΥݥ��󥿡�
//! @param		reader : ���Υե��륿�Υ�`���`
//! @param		clsid : ���Υե��륿�Υ��饹ID
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
//! @brief	  	Ҫ�󤵤줿���󥿩`�ե������򷵤�
//! @param		riid : ���󥿩`�ե�������IID
//! @param		ppv : ���󥿩`�ե������򷵤��ݥ��󥿩`�ؤΥݥ���
//! @return		����`���`��
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
//! @brief	  	�������������ȥ�`����_��
//! @param		stream : ��`�ө`�ؤΥ��ȥ�`��
//! @return		����`���`��
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
		// AddPin��new�������A�ǥ��`�뤵���Τǡ���ʾ�Ĥ�׷�Ӥ����Ҫ�Ϥʤ�
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	������`�Ȥ�������줿�r�˥��`�뤵���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ChangeRate(void)
{
	{	// ����ƥ����� ��������� ��å��Υ����`�ס�
		CAutoLock cAutoLock(&m_crtFilterLock);
		if( m_dRateSeeking <= 0 ) {
			m_dRateSeeking = 1.0;  // �m�Фʂ��˥ꥻ�åȤ��롣
			return E_FAIL;
		}
	}
	UpdateFromSeek();
	return Reader()->SetRate( m_dRateSeeking );
}
//----------------------------------------------------------------------------
//! @brief	  	�������_ʼ���줿�r�˥��`�뤵���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ChangeStart(void)
{
	UpdateFromSeek();
	if( m_rtStart > m_rtDuration  )
		m_rtStart = m_rtDuration;	// ���

	return Reader()->SetStartTime( m_rtStart );
}
//----------------------------------------------------------------------------
//! @brief	  	������ֹͣ���줿�r�˥��`�뤵���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::ChangeStop(void)
{
	UpdateFromSeek();
	return Reader()->SetEndTime(m_rtStop);
}
//----------------------------------------------------------------------------
//! @brief	  	���֤��Ƥ���ԥ�򤹤٤���������
//! @return		����`���`��
//----------------------------------------------------------------------------
void CDemuxSource::ClearPins()
{
	while( GetPinCount() > 0 )
		delete GetPin(GetPinCount()-1);
}
//----------------------------------------------------------------------------
//! @brief	  	�ե��륿��ݩ`������
//! @return		����`���`��
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
//! @brief	  	���`������Ή����ӳ����
//! @return		����`���`��
//----------------------------------------------------------------------------
void CDemuxSource::UpdateFromSeek()
{
	DeliverBeginFlush();
	Stop();
	DeliverEndFlush();
}
//----------------------------------------------------------------------------
//! @brief	  	�¤����������_ʼ���줿���Ȥ�֪ͨ����
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::NewSegment()
{
	return DeliverNewSegment(m_rtStart, m_rtStop, m_dRateSeeking);
}
//----------------------------------------------------------------------------
//! @brief	  	�¤����������_ʼ���줿���Ȥ�����󥹥ȥ�`���֪ͨ����
//! @param		tStart : �_ʼ�r�g
//! @param		tStop : ֹͣ�r�g
//! @param		dRate : ������`��
//! @return		����`���`��
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
//! @brief	  	���ȥ�`�ब�K�ˤ��_�������Ȥ�����󥹥ȥ�`���֪ͨ����
//! @return		����`���`��
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
//! @brief	  	�ե�å���I��νK�ˤ�����󥹥ȥ�`���Ҫ�󤹤�
//! @return		����`���`��
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
//! @brief	  	�ե�å���I����_ʼ������󥹥ȥ�`���Ҫ�󤹤�
//! @return		����`���`��
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
//! @brief	  	�ե��륿�����פ�ȡ�ä���
//! @return		�ե��륿������
//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CDemuxSource::GetMiscFlags(void)
{
	return AM_FILTER_MISC_FLAGS_IS_SOURCE;
}
//----------------------------------------------------------------------------
//! @brief	  	�F�ڽӾA����Ƥ���ԥ������ȡ�ä���
//! @return		�ӾA��
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
//! @brief	  	������ե��`�ޥåȤ����ݩ`�Ȥ���Ƥ��뤫�ɤ����_�J����
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::IsFormatSupported(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	// only seeking in time (REFERENCE_TIME units) is supported
	return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`��΃��ȥ����� �ե��`�ޥåȤ�ȡ�ä���
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::QueryPreferredFormat(GUID *pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	������ե��`�ޥåȤ��O������
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::SetTimeFormat(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	// nothing to set; just check that it's TIME_FORMAT_TIME
	return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : E_INVALIDARG;
}
//----------------------------------------------------------------------------
//! @brief	  	ָ������������ �ե��`�ޥåȤ��F��ʹ���Ƥ���ե��`�ޥåȤ��ɤ�����_�J����
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::IsUsingTimeFormat(const GUID * pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	return *pFormat == TIME_FORMAT_MEDIA_TIME ? S_OK : S_FALSE;
}
//----------------------------------------------------------------------------
//! @brief	  	�F�ڤΥ����� �ե��`�ޥåȤ�ȡ�ä���
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetTimeFormat(GUID *pFormat)
{
	CheckPointer(pFormat, E_POINTER);
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`��Εr�g����ȡ�ä���
//! @param		pDuration : �L��
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetDuration(LONGLONG *pDuration)
{
	CheckPointer(pDuration, E_POINTER);
	CAutoLock lock(&m_crtFilterLock);
	*pDuration = m_rtDuration;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`���ֹͣ�r�g��ȡ�ä���
//! @param		pStop : ֹͣ�r�g
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetStopPosition(LONGLONG *pStop)
{
	CheckPointer(pStop, E_POINTER);
	CAutoLock lock(&m_crtFilterLock);
	*pStop = m_rtStop;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`��άF�ڕr�g��ȡ�ä���
//! @param		pCurrent : �F�ڕr�g
//! @return		δ���ݩ`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetCurrentPosition(LONGLONG *pCurrent)
{
	// GetCurrentPosition is typically supported only in renderers and
	// not in source filters.
	return E_NOTIMPL;
}
//----------------------------------------------------------------------------
//! @brief	  	���`��������ȡ�ä���
//! @param		pCapabilities : ���`������
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetCapabilities( DWORD * pCapabilities )
{
	CheckPointer(pCapabilities, E_POINTER);
	*pCapabilities = m_dwSeekingCaps;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ָ���������`�������򥹥ȥ�`�ब�֤äƤ��뤫�ɤ����򆖤��Ϥ碌��
//! @param		pCapabilities : ���`������
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::CheckCapabilities( DWORD * pCapabilities )
{
	CheckPointer(pCapabilities, E_POINTER);
	// make sure all requested capabilities are in our mask
	return (~m_dwSeekingCaps & *pCapabilities) ? S_FALSE : S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	1 �ĤΥ����� �ե��`�ޥåȤ���e�Υ����� �ե��`�ޥåȤˉ�Q����
//! @param		pTarget : ��Q���줿��������ܤ�ȡ������ؤΥݥ���
//! @param		pTargetFormat : ���`���å� �ե��`�ޥåȤΥ����� �ե��`�ޥå� GUID �ؤΥݥ��󥿡�NULL �Έ��Ϥϡ��F�ڤΥե��`�ޥåȤ�ʹ����
//! @param		Source : ��Q���륿������
//! @param		pSourceFormat : ��Q����ե��`�ޥåȤΥ����� �ե��`�ޥå� GUID �ؤΥݥ��󥿡�NULL �Έ��Ϥϡ��F�ڤΥե��`�ޥåȤ�ʹ����
//! @return		����`���`��
//! @note	TIME_FORMAT_MEDIA_TIME����ϥ��ݩ`�Ȥ��Ƥ��ʤ�
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
//! @brief	  	�F��λ�ä�ֹͣλ�ä��O������
//! @param		pCurrent : �F��λ�ä�ָ����������ؤΥݥ��󥿡��F�ڤΥ����� �ե��`�ޥåȤ΅gλ
//! @param		CurrentFlags : λ�ä�ָ�����뤿��Υե饰�ΥӥåȤ��ȤνM�ߺϤ碌
//! @param		pStop : �K�˥������ָ����������ؤΥݥ��󥿡��F�ڤΥ����� �ե��`�ޥåȤ΅gλ
//! @param		StopFlags : λ�ä�ָ�����뤿��Υե饰�ΥӥåȤ��ȤνM�ߺϤ碌
//! @return		����`���`��
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
//! @brief	  	�F�ڤ�λ�ä�ֹͣλ�ä�ȡ�ä���
//! @param		pCurrent : �_ʼλ�ä��ܤ�ȡ������ؤΥݥ���
//! @param		pStop : ֹͣλ�ä��ܤ�ȡ������ؤΥݥ���
//! @return		����`���`��
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
//! @brief	  	���`�����Є��ʥ�����ι����ȡ�ä���
//! @param		pEarliest : ���`�����Є�������礤��������ܤ�ȡ������ؤΥݥ���
//! @param		pLatest : ���`�����Є�������W����������ܤ�ȡ������ؤΥݥ���
//! @return		����`���`��
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
//! @brief	  	������`�Ȥ��O������
//! @param		dRate : ������`��
//! @return		����`���`��
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
//! @brief	  	������`�Ȥ�ȡ�ä���
//! @param		dRate : ������`��
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetRate( double * pdRate)
{
	CheckPointer(pdRate, E_POINTER);
	CAutoLock lock(&m_crtFilterLock);
	*pdRate = m_dRateSeeking;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	�_ʼλ�ä�ǰ�˥���`�����ǩ`��������ȡ�ä���
//! @param		pPreroll : �ץ��`�� ��������ܤ�ȡ������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT CDemuxSource::GetPreroll(LONGLONG *pPreroll)
{
	CheckPointer(pPreroll, E_POINTER);
	*pPreroll = 0;
	return S_OK;
}

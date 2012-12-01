/****************************************************************************/
/*! @file
@brief IMediaSeeking Proxy

IMediaSeeking�κ��ӳ���������롣
�ԥ󤬥��`���ե��륿��IMediaSeeking��ʹ�ä��������Ϥʤɤ�ʹ��
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/22
@note
*****************************************************************************/

#include "CMediaSeekingProxy.h"

//#define ENABLE_SET

//----------------------------------------------------------------------------
//! @brief	  	CMediaSeekingProxy constructor
//! @param		delegate : �I���ί�Τ��뤿��Υ��饹
//----------------------------------------------------------------------------
CMediaSeekingProxy::CMediaSeekingProxy(IMediaSeeking* delegate)
: m_Delegate(delegate)
{}
//----------------------------------------------------------------------------
//! @brief	  	CMediaSeekingProxy destructor
//----------------------------------------------------------------------------
CMediaSeekingProxy::~CMediaSeekingProxy()
{}
//----------------------------------------------------------------------------
//! @brief	  	�I���ί�Τ��륷�`�����󥿩`�ե��������O������
//! @param		seeker : �I���ί�Τ��뤿��Υ��饹
//----------------------------------------------------------------------------
void CMediaSeekingProxy::SetSeeker( IMediaSeeking *seeker )
{
//	if( m_Delegate.p != NULL )
//		Release();
	m_Delegate = seeker;
}
//----------------------------------------------------------------------------
//! @brief	  	Ҫ�󤵤줿���󥿩`�ե������򷵤�
//! @param		riid : ���󥿩`�ե�������IID
//! @param		ppvObj : ���󥿩`�ե������򷵤��ݥ��󥿩`�ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CMediaSeekingProxy::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	if( IID_IMediaSeeking == riid )
		return GetInterface( static_cast<IMediaSeeking*>(this), ppvObj );
	return E_NOINTERFACE;
}
//----------------------------------------------------------------------------
//! @brief	  	���󥿩`�ե������β��ե�����Ȥ� 1 ���ĉ��䤹
//! @return		�¤������ե�����Ȃ��򷵤�
//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMediaSeekingProxy::AddRef()
{
	return MediaSeeking()->AddRef();
}
//----------------------------------------------------------------------------
//! @brief	  	���󥿩`�ե������β��ե�����Ȥ� 1 ���Ĝp�٤�����
//! @return		�¤������ե�����Ȃ��򷵤�
//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMediaSeekingProxy::Release()
{
	return MediaSeeking()->Release();
}
//----------------------------------------------------------------------------
//! @brief	  	���`��������ȡ�ä���
//! @param		pCapabilities : ���`������
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetCapabilities(DWORD *pCapabilities)
{
	return MediaSeeking()->GetCapabilities(pCapabilities);
}
//----------------------------------------------------------------------------
//! @brief	  	ָ���������`�������򥹥ȥ�`�ब�֤äƤ��뤫�ɤ����򆖤��Ϥ碌��
//! @param		pCapabilities : ���`������
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::CheckCapabilities(DWORD *pCapabilities)
{
	return MediaSeeking()->CheckCapabilities(pCapabilities);
}
//----------------------------------------------------------------------------
//! @brief	  	������ե��`�ޥåȤ����ݩ`�Ȥ���Ƥ��뤫�ɤ����_�J����
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::IsFormatSupported(const GUID *pFormat)
{
	return MediaSeeking()->IsFormatSupported(pFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`��΃��ȥ����� �ե��`�ޥåȤ�ȡ�ä���
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::QueryPreferredFormat(GUID *pFormat)
{
	return MediaSeeking()->QueryPreferredFormat(pFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	������ե��`�ޥåȤ��O������
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::SetTimeFormat(const GUID *pFormat)
{
#ifdef ENABLE_SET
	return MediaSeeking()->SetTimeFormat(pFormat);
#else
	return S_OK;
#endif
}
//----------------------------------------------------------------------------
//! @brief	  	�F�ڤΥ����� �ե��`�ޥåȤ�ȡ�ä���
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetTimeFormat( GUID *pFormat)
{
	return MediaSeeking()->GetTimeFormat(pFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`��Εr�g����ȡ�ä���
//! @param		pDuration : �L��
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetDuration(LONGLONG *pDuration)
{
	return MediaSeeking()->GetDuration(pDuration);
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`���ֹͣ�r�g��ȡ�ä���
//! @param		pStop : ֹͣ�r�g
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetStopPosition(LONGLONG *pStop)
{
	return MediaSeeking()->GetStopPosition(pStop);
}
//----------------------------------------------------------------------------
//! @brief	  	���ȥ�`��άF�ڕr�g��ȡ�ä���
//! @param		pCurrent : �F�ڕr�g
//! @return		δ���ݩ`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetCurrentPosition(LONGLONG *pCurrent)
{
	return MediaSeeking()->GetCurrentPosition(pCurrent);
}
//----------------------------------------------------------------------------
//! @brief	  	1 �ĤΥ����� �ե��`�ޥåȤ���e�Υ����� �ե��`�ޥåȤˉ�Q����
//! @param		pTarget : ��Q���줿��������ܤ�ȡ������ؤΥݥ���
//! @param		pTargetFormat : ���`���å� �ե��`�ޥåȤΥ����� �ե��`�ޥå� GUID �ؤΥݥ��󥿡�NULL �Έ��Ϥϡ��F�ڤΥե��`�ޥåȤ�ʹ����
//! @param		Source : ��Q���륿������
//! @param		pSourceFormat : ��Q����ե��`�ޥåȤΥ����� �ե��`�ޥå� GUID �ؤΥݥ��󥿡�NULL �Έ��Ϥϡ��F�ڤΥե��`�ޥåȤ�ʹ����
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat)
{
	return MediaSeeking()->ConvertTimeFormat(pTarget,pTargetFormat,Source,pSourceFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	�F��λ�ä�ֹͣλ�ä��O������
//! @param		pCurrent : �F��λ�ä�ָ����������ؤΥݥ��󥿡��F�ڤΥ����� �ե��`�ޥåȤ΅gλ
//! @param		CurrentFlags : λ�ä�ָ�����뤿��Υե饰�ΥӥåȤ��ȤνM�ߺϤ碌
//! @param		pStop : �K�˥������ָ����������ؤΥݥ��󥿡��F�ڤΥ����� �ե��`�ޥåȤ΅gλ
//! @param		StopFlags : λ�ä�ָ�����뤿��Υե饰�ΥӥåȤ��ȤνM�ߺϤ碌
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags)
{
#ifdef ENABLE_SET
	return MediaSeeking()->SetPositions(pCurrent,dwCurrentFlags,pStop,dwStopFlags);
#else
	return S_OK;
#endif
}
//----------------------------------------------------------------------------
//! @brief	  	�F�ڤ�λ�ä�ֹͣλ�ä�ȡ�ä���
//! @param		pCurrent : �_ʼλ�ä��ܤ�ȡ������ؤΥݥ���
//! @param		pStop : ֹͣλ�ä��ܤ�ȡ������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop)
{
	return MediaSeeking()->GetPositions(pCurrent,pStop);
}
//----------------------------------------------------------------------------
//! @brief	  	���`�����Є��ʥ�����ι����ȡ�ä���
//! @param		pEarliest : ���`�����Є�������礤��������ܤ�ȡ������ؤΥݥ���
//! @param		pLatest : ���`�����Є�������W����������ܤ�ȡ������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest)
{
	return MediaSeeking()->GetAvailable(pEarliest,pLatest);
}
//----------------------------------------------------------------------------
//! @brief	  	������`�Ȥ��O������
//! @param		dRate : ������`��
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::SetRate(double dRate)
{
#ifdef ENABLE_SET
	return MediaSeeking()->SetRate(dRate);
#else
	return S_OK;
#endif
}
//----------------------------------------------------------------------------
//! @brief	  	������`�Ȥ�ȡ�ä���
//! @param		dRate : ������`��
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetRate(double *dRate)
{
	return MediaSeeking()->GetRate(dRate);
}
//----------------------------------------------------------------------------
//! @brief	  	�_ʼλ�ä�ǰ�˥���`�����ǩ`��������ȡ�ä���
//! @param		pPreroll : �ץ��`�� ��������ܤ�ȡ������ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetPreroll(LONGLONG *pllPreroll)
{
	return MediaSeeking()->GetPreroll(pllPreroll);
}
//----------------------------------------------------------------------------
//! @brief	  	ָ������������ �ե��`�ޥåȤ��F��ʹ���Ƥ���ե��`�ޥåȤ��ɤ�����_�J����
//! @param		pFormat : ������ե��`�ޥå�
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::IsUsingTimeFormat(const GUID *pFormat)
{
	return MediaSeeking()->IsUsingTimeFormat(pFormat);
}


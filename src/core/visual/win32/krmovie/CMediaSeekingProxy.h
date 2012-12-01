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


#ifndef __MEDIA_SEEKING_PROXY_H__
#define __MEDIA_SEEKING_PROXY_H__

#include <streams.h>
#include <assert.h>
#include <atlcomcli.h>

//! IMediaSeeking�ؤβ�����g�˥��åץ��ȥ�`��ضɤ��ƄI������Ϥϡ�CPosPassThru��ʹ�ä��뤬��
//! ���`���ե��륿�Υԥ��IMediaSeeking��gװ�����Ҫ��������ϡ����`���ե��륿��IMediaSeeking
//! ���󥿩`�ե���������ӳ����¤ˤʤ뤳�Ȥ��त��˼���롣
//! CMediaSeekingProxy�Ϥ��Τ褦����;��ʹ������Υ��饹��
class CMediaSeekingProxy : public IMediaSeeking
{
private:
//	CComPtr<IMediaSeeking>	m_Delegate;
	IMediaSeeking*			m_Delegate;	// CComPtr �ǳ֤Ĥ������֤Ĥ��Ȥˤʤä��_�ŤǤ��ʤ��ʤ뤳�Ȥ�����
	//----------------------------------------------------------------------------
	//! @brief	  	IMediaSeeking��ȡ�ä���
	//! @return		IMediaSeeking���󥿩`�ե�����
	//----------------------------------------------------------------------------
	IMediaSeeking *MediaSeeking()
	{
		assert( m_Delegate );
		return m_Delegate;
	}

	// delegate���O�����ʤ����ɤ䥳�ԩ`���ֹ����
	CMediaSeekingProxy();
	CMediaSeekingProxy(const CMediaSeekingProxy& copy);
	void operator =(const CMediaSeekingProxy& lhs);

public:
	CMediaSeekingProxy(IMediaSeeking* delegate);
	virtual ~CMediaSeekingProxy(void);
	void SetSeeker( IMediaSeeking *seeker );

	//Methods of IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppvObj );
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//IMediaSeeking Interface
	virtual STDMETHODIMP GetCapabilities(DWORD *pCapabilities);
	virtual STDMETHODIMP CheckCapabilities(DWORD *pCapabilities);
	virtual STDMETHODIMP IsFormatSupported(const GUID *pFormat);
	virtual STDMETHODIMP QueryPreferredFormat(GUID *pFormat);
	virtual STDMETHODIMP SetTimeFormat(const GUID *pFormat);
	virtual STDMETHODIMP GetTimeFormat( GUID *pFormat);
	virtual STDMETHODIMP GetDuration(LONGLONG *pDuration);
	virtual STDMETHODIMP GetStopPosition(LONGLONG *pStop);
	virtual STDMETHODIMP GetCurrentPosition(LONGLONG *pCurrent);
	virtual STDMETHODIMP ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat);
	virtual STDMETHODIMP SetPositions(LONGLONG *pCurrent,DWORD dwCurrentFlags,LONGLONG *pStop,DWORD dwStopFlags);
	virtual STDMETHODIMP GetPositions(LONGLONG *pCurrent, LONGLONG *pStop);
	virtual STDMETHODIMP GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest);
	virtual STDMETHODIMP SetRate(double dRate);
	virtual STDMETHODIMP GetRate(double *dRate);
	virtual STDMETHODIMP GetPreroll(LONGLONG *pllPreroll);
	virtual STDMETHODIMP IsUsingTimeFormat(const GUID *pFormat);
};

#endif	// __MEDIA_SEEKING_PROXY_H__

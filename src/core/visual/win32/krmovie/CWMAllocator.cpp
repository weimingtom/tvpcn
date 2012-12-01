/****************************************************************************/
/*! @file
@brief DirectShow allocator wrapper for Windows media format

DirectShow�Υ����`���`���åפ���Windows Media Format SDK�äΥ����`���`
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/10/28
@note
*****************************************************************************/

#include "CWMAllocator.h"
#include "CWMBuffer.h"
#include <assert.h>


//----------------------------------------------------------------------------
//! @brief	  	�ǥ��ȥ饯��
//----------------------------------------------------------------------------
CWMAllocator::~CWMAllocator()
{
}

//----------------------------------------------------------------------------
//! @brief	  	Ҫ�󤵤줿���󥿩`�ե������򷵤�
//! @param		riid : ���󥿩`�ե�������IID
//! @param		ppv : ���󥿩`�ե������򷵤��ݥ��󥿩`�ؤΥݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMAllocator::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	if( IID_IWMReaderAllocatorEx == riid ) {
		return GetInterface( static_cast<IWMReaderAllocatorEx*>(this), ppv );
	} else if( IID_IMemAllocator == riid ) {
		return GetInterface( static_cast<IMemAllocator*>(m_MemAlloc), ppv );
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	�Хåե���_������ ( ���ȥ�`�फ��Ҫ�󤵤줿���Ϥˤ��Υ᥽�åɤ����Ф�� )
//! @param		wStreamNum : ���ȥ�`�෬��
//! @param		cbBuffer : Ҫ�󤵤줿�Хåե�������
//! @param		ppBuffer : �Хåե��򷵤��ݥ��󥿤Υݥ���
//! @param		dwFlags : ��;�ե饰
//! @param		cnsSampleTime : ����ץ륿����
//! @param		cnsSampleDuration : ����ץ��ʾ�r�g
//! @param		pvContext : IWMReader::Start���`��r�˶ɤ��줿�ݥ���
//! @return		����`���`��
//----------------------------------------------------------------------------
STDMETHODIMP CWMAllocator::AllocateForStreamEx( WORD wStreamNum, DWORD cbBuffer, INSSBuffer **ppBuffer,
	DWORD dwFlags, QWORD cnsSampleTime, QWORD cnsSampleDuration, void *pvContext )
{
	HRESULT	hr;

	if( ppBuffer == NULL )
		return E_INVALIDARG;

	DWORD	flag = 0;
	if( dwFlags & WM_SFEX_NOTASYNCPOINT )	// �� Key frame
		flag |= AM_GBF_NOTASYNCPOINT;

	REFERENCE_TIME	StartTime = cnsSampleTime;
	REFERENCE_TIME	EndTime = cnsSampleTime + cnsSampleDuration;

	IMediaSample	*pSample = NULL;
	hr = Allocator()->GetBuffer( &pSample, &StartTime, &EndTime, flag );
	if( hr == S_OK )
	{
		*ppBuffer = new CWMBuffer(pSample);
		(*ppBuffer)->AddRef();
		(*ppBuffer)->SetLength(cbBuffer);
		pSample->Release();	// CWMBuffer�ضɤ����r��ǲ��ե�����Ȥ�������
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	�Хåե���_������ ( �����ȥץåȤ���Ҫ�󤵤줿���Ϥˤ��Υ᥽�åɤ����Ф�� )
//! @param		wStreamNum : ���ȥ�`�෬��
//! @param		cbBuffer : Ҫ�󤵤줿�Хåե�������
//! @param		ppBuffer : �Хåե��򷵤��ݥ��󥿤Υݥ���
//! @param		dwFlags : ��;�ե饰
//! @param		cnsSampleTime : ����ץ륿����
//! @param		cnsSampleDuration : ����ץ��ʾ�r�g
//! @param		pvContext : IWMReader::Start���`��r�˶ɤ��줿�ݥ���
//! @return		����`���`��
//! @note		�F�ڤ��Υ᥽�åɤ�ʹ�äƤ��ʤ������֤������˙C�ܤ��ʤ���
//----------------------------------------------------------------------------
STDMETHODIMP CWMAllocator::AllocateForOutputEx( DWORD dwOutputNum, DWORD cbBuffer, INSSBuffer **ppBuffer,
	DWORD dwFlags, QWORD cnsSampleTime, QWORD cnsSampleDuration, void *pvContext )
{
	HRESULT	hr;

	if( ppBuffer == NULL )
		return E_INVALIDARG;

	DWORD	flag = 0;
	if( dwFlags & WM_SFEX_NOTASYNCPOINT )	// �� Key frame
		flag |= AM_GBF_NOTASYNCPOINT;

	REFERENCE_TIME	StartTime = cnsSampleTime;
	REFERENCE_TIME	EndTime = cnsSampleTime + cnsSampleDuration;

	IMediaSample	*pSample;
	hr = Allocator()->GetBuffer( &pSample, &StartTime, &EndTime, flag );
	if( hr == S_OK )
	{
		*ppBuffer = new CWMBuffer(pSample);
		(*ppBuffer)->AddRef();
		pSample->Release();	// CWMBuffer�ضɤ����r��ǲ��ե�����Ȥ�������
	}
	return hr;
}

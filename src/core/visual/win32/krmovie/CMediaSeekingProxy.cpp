/****************************************************************************/
/*! @file
@brief IMediaSeeking Proxy

IMediaSeekingの呼び出しを代理する。
ピンがソースフィルタのIMediaSeekingを使用したい場合などに使う
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
//! @param		delegate : 処理を委任するためのクラス
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
//! @brief	  	処理を委任するシークインターフェイスを設定する
//! @param		seeker : 処理を委任するためのクラス
//----------------------------------------------------------------------------
void CMediaSeekingProxy::SetSeeker( IMediaSeeking *seeker )
{
//	if( m_Delegate.p != NULL )
//		Release();
	m_Delegate = seeker;
}
//----------------------------------------------------------------------------
//! @brief	  	要求されたインターフェイスを返す
//! @param		riid : インターフェイスのIID
//! @param		ppvObj : インターフェイスを返すポインターへのポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CMediaSeekingProxy::QueryInterface( REFIID riid, LPVOID *ppvObj )
{
	if( IID_IMediaSeeking == riid )
		return GetInterface( static_cast<IMediaSeeking*>(this), ppvObj );
	return E_NOINTERFACE;
}
//----------------------------------------------------------------------------
//! @brief	  	インターフェイスの参照カウントを 1 ずつ増やす
//! @return		新しい参照カウント値を返す
//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMediaSeekingProxy::AddRef()
{
	return MediaSeeking()->AddRef();
}
//----------------------------------------------------------------------------
//! @brief	  	インターフェイスの参照カウントを 1 ずつ減少させる
//! @return		新しい参照カウント値を返す
//----------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE CMediaSeekingProxy::Release()
{
	return MediaSeeking()->Release();
}
//----------------------------------------------------------------------------
//! @brief	  	シーク能力を取得する
//! @param		pCapabilities : シーク能力
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetCapabilities(DWORD *pCapabilities)
{
	return MediaSeeking()->GetCapabilities(pCapabilities);
}
//----------------------------------------------------------------------------
//! @brief	  	指定したシーク能力をストリームが持っているかどうかを問い合わせる
//! @param		pCapabilities : シーク能力
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::CheckCapabilities(DWORD *pCapabilities)
{
	return MediaSeeking()->CheckCapabilities(pCapabilities);
}
//----------------------------------------------------------------------------
//! @brief	  	タイムフォーマットがサポートされているかどうか確認する
//! @param		pFormat : タイムフォーマット
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::IsFormatSupported(const GUID *pFormat)
{
	return MediaSeeking()->IsFormatSupported(pFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	ストリームの優先タイム フォーマットを取得する
//! @param		pFormat : タイムフォーマット
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::QueryPreferredFormat(GUID *pFormat)
{
	return MediaSeeking()->QueryPreferredFormat(pFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	タイムフォーマットを設定する
//! @param		pFormat : タイムフォーマット
//! @return		エラーコード
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
//! @brief	  	現在のタイム フォーマットを取得する
//! @param		pFormat : タイムフォーマット
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetTimeFormat( GUID *pFormat)
{
	return MediaSeeking()->GetTimeFormat(pFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	ストリームの時間幅を取得する
//! @param		pDuration : 長さ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetDuration(LONGLONG *pDuration)
{
	return MediaSeeking()->GetDuration(pDuration);
}
//----------------------------------------------------------------------------
//! @brief	  	ストリームの停止時間を取得する
//! @param		pStop : 停止時間
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetStopPosition(LONGLONG *pStop)
{
	return MediaSeeking()->GetStopPosition(pStop);
}
//----------------------------------------------------------------------------
//! @brief	  	ストリームの現在時間を取得する
//! @param		pCurrent : 現在時間
//! @return		未サポート
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetCurrentPosition(LONGLONG *pCurrent)
{
	return MediaSeeking()->GetCurrentPosition(pCurrent);
}
//----------------------------------------------------------------------------
//! @brief	  	1 つのタイム フォーマットから別のタイム フォーマットに変換する
//! @param		pTarget : 変換されたタイムを受け取る変数へのポインタ
//! @param		pTargetFormat : ターゲット フォーマットのタイム フォーマット GUID へのポインタ。NULL の場合は、現在のフォーマットが使われる
//! @param		Source : 変換するタイム値
//! @param		pSourceFormat : 変換するフォーマットのタイム フォーマット GUID へのポインタ。NULL の場合は、現在のフォーマットが使われる
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::ConvertTimeFormat(LONGLONG *pTarget, const GUID *pTargetFormat, LONGLONG Source, const GUID *pSourceFormat)
{
	return MediaSeeking()->ConvertTimeFormat(pTarget,pTargetFormat,Source,pSourceFormat);
}
//----------------------------------------------------------------------------
//! @brief	  	現在位置と停止位置を設定する
//! @param		pCurrent : 現在位置を指定する変数へのポインタ、現在のタイム フォーマットの単位
//! @param		CurrentFlags : 位置を指定するためのフラグのビットごとの組み合わせ
//! @param		pStop : 終了タイムを指定する変数へのポインタ、現在のタイム フォーマットの単位
//! @param		StopFlags : 位置を指定するためのフラグのビットごとの組み合わせ
//! @return		エラーコード
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
//! @brief	  	現在の位置と停止位置を取得する
//! @param		pCurrent : 開始位置を受け取る変数へのポインタ
//! @param		pStop : 停止位置を受け取る変数へのポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetPositions(LONGLONG *pCurrent, LONGLONG *pStop)
{
	return MediaSeeking()->GetPositions(pCurrent,pStop);
}
//----------------------------------------------------------------------------
//! @brief	  	シークが有効なタイムの範囲を取得する
//! @param		pEarliest : シークが有効な最も早いタイムを受け取る変数へのポインタ
//! @param		pLatest : シークが有効な最も遅いタイムを受け取る変数へのポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetAvailable(LONGLONG *pEarliest, LONGLONG *pLatest)
{
	return MediaSeeking()->GetAvailable(pEarliest,pLatest);
}
//----------------------------------------------------------------------------
//! @brief	  	再生レートを設定する
//! @param		dRate : 再生レート
//! @return		エラーコード
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
//! @brief	  	再生レートを取得する
//! @param		dRate : 再生レート
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetRate(double *dRate)
{
	return MediaSeeking()->GetRate(dRate);
}
//----------------------------------------------------------------------------
//! @brief	  	開始位置の前にキューに入るデータの量を取得する
//! @param		pPreroll : プリロール タイムを受け取る変数へのポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::GetPreroll(LONGLONG *pllPreroll)
{
	return MediaSeeking()->GetPreroll(pllPreroll);
}
//----------------------------------------------------------------------------
//! @brief	  	指定したタイム フォーマットが現在使われているフォーマットかどうかを確認する
//! @param		pFormat : タイムフォーマット
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CMediaSeekingProxy::IsUsingTimeFormat(const GUID *pFormat)
{
	return MediaSeeking()->IsUsingTimeFormat(pFormat);
}


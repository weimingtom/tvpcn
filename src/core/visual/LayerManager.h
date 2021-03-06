//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Layer Management
//---------------------------------------------------------------------------
#ifndef LayerManagerH
#define LayerManagerH


#include "LayerIntf.h"
#include "drawable.h"

/*[*/
class tTJSNI_BaseLayer;
//---------------------------------------------------------------------------
// abstract class of Layer Manager 
//---------------------------------------------------------------------------
class iTVPLayerManager
{
public:
//-- object lifetime management
	//! @brief	歌孚カウンタをインクリメントする
	virtual void TJS_INTF_METHOD AddRef() = 0;

	//! @brief	歌孚カウンタをデクリメントする
	virtual void TJS_INTF_METHOD Release() = 0;

//-- draw device specific information
	//! @brief	宙鮫デバイス耕嗤の秤�鵑鰓O協する
	//! @param	data	宙鮫デバイス耕嗤の秤��
	//! @note	宙鮫デバイス耕嗤の秤�鵑鬟譽ぅ筌泪幽`ジャに�O協する。
	//!			レイヤマネ�`ジャではこの秤�鵑琳佗蹐砲弔い討��v岑しない。
	//!			宙鮫デバイス�箸把甚，吠垢辰燭蝓¬惷┐稜��鵑判Yびつけて砿尖する。
	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) = 0;

	//! @brief	宙鮫デバイス耕嗤の秤�鵑鯣ゝ辰垢�
	//! @return	宙鮫デバイス耕嗤の秤��
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const = 0;

//-- layer metrics
	//! @brief	プライマリレイヤのサイズを函誼する
	//! @param	w	レイヤの罪嫌(ピクセル�g了)
	//! @param	h	レイヤの�k嫌(ピクセル�g了)
	//! @return	函誼に撹孔すれば寔、払�，垢譴���
	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const = 0;

//-- layer structure information
	//! @brief	プライマリレイヤの函誼
	//! @return	プライマリレイヤ
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const = 0;

	//! @brief	フォ�`カスのあるレイヤの函誼
	//! @return	フォ�`カスのあるレイヤ
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const = 0;

	//! @brief	フォ�`カスのあるレイヤの�O協
	//! @param	layer	フォ�`カスのあるレイヤ
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;

//-- HID releted
	//! @brief		クリックされた
	//! @param		x		プライマリレイヤ恙�防呂砲�ける x 了崔
	//! @param		y		プライマリレイヤ恙�防呂砲�ける y 了崔
	virtual void TJS_INTF_METHOD NotifyClick(tjs_int x, tjs_int y) = 0;

	//! @brief		ダブルクリックされた
	//! @param		x		プライマリレイヤ恙�防呂砲�ける x 了崔
	//! @param		y		プライマリレイヤ恙�防呂砲�ける y 了崔
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y) = 0;

	//! @brief		マウスボタンが兀和された
	//! @param		x		プライマリレイヤ恙�防呂砲�ける x 了崔
	//! @param		y		プライマリレイヤ恙�防呂砲�ける y 了崔
	//! @param		mb		どのマウスボタンか
	//! @param		flags	フラグ(TVP_SS_*協方の�Mみ栽わせ)
	virtual void TJS_INTF_METHOD NotifyMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		マウスボタンが�xされた
	//! @param		x		プライマリレイヤ恙�防呂砲�ける x 了崔
	//! @param		y		プライマリレイヤ恙�防呂砲�ける y 了崔
	//! @param		mb		どのマウスボタンか
	//! @param		flags	フラグ(TVP_SS_*協方の�Mみ栽わせ)
	virtual void TJS_INTF_METHOD NotifyMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		マウスが卞�咾靴�
	//! @param		x		プライマリレイヤ恙�防呂砲�ける x 了崔
	//! @param		y		プライマリレイヤ恙�防呂砲�ける y 了崔
	//! @param		flags	フラグ(TVP_SS_*協方の�Mみ栽わせ)
	virtual void TJS_INTF_METHOD NotifyMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;

	//! @brief		マウスキャプチャを盾慧する
	//! @note		マウスキャプチャを盾慧すべき��栽にウィンドウから柵ばれる。
	virtual void TJS_INTF_METHOD ReleaseCapture() = 0;

	//! @brief		マウスがプライマリレイヤ翌に卞�咾靴�
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow() = 0;

	//! @brief		キ�`が兀された
	//! @param		key		�∀襯��`コ�`ド
	//! @param		shift	シフトキ�`の彜�B
	virtual void TJS_INTF_METHOD NotifyKeyDown(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		キ�`が�xされた
	//! @param		key		�∀襯��`コ�`ド
	//! @param		shift	シフトキ�`の彜�B
	virtual void TJS_INTF_METHOD NotifyKeyUp(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		キ�`による秘薦
	//! @param		key		猟忖コ�`ド
	virtual void TJS_INTF_METHOD NotifyKeyPress(tjs_char key) = 0;

	//! @brief		マウスホイ�`ルが指��した
	//! @param		shift	シフトキ�`の彜�B
	//! @param		delta	指��叔
	//! @param		x		プライマリレイヤ恙�防呂砲�ける x 了崔
	//! @param		y		プライマリレイヤ恙�防呂砲�ける y 了崔
	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;

	//! @brief		秘薦彜�Bのチェック
	//! @note		ウィンドウから�s1昼おきに、レイヤマネ�`ジャがユ�`ザからの秘薦の彜�Bを
	//!				壅チェックするために柵ばれる。レイヤ彜�Bの�篁�がユ�`ザの秘薦とは
	//!				掲揖豚に佩われた��栽、たとえばマウスカ�`ソルの和にレイヤが竃�Fした
	//!				のにもかかわらず、マウスカ�`ソルがそのレイヤの峺協する侘彜に�筝�されない
	//!				といった彜�rが�k伏しうる。このような彜�rに���Iするため、ウィンドウから
	//!				このメソッドが�s1昼おきに柵ばれる。
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;

//-- invalidation/update
	//! @brief		宙鮫デバイスが李むレイヤの竃薦侘塀を�O協する
	//! @param		type	レイヤ侘塀
	//! @note		デフォルトは ltOpaque 。宙鮫デバイスが麿の侘塀の鮫�颪魍�薦として
	//!				李むならばその侘塀を峺協する。ただし、プライマリレイヤの type
	//!				プロパティも揖��に�筝�すること。
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) = 0;

	//! @brief		蒙協の裳侘の壅宙鮫を勣箔する
	//! @param		r		プライマリレイヤ恙�防呂砲�ける裳侘
	//! @note		蒙協の裳侘の壅宙鮫をレイヤマネ�`ジャに��して勣箔する。
	//!				勣箔は���hされるだけでこのメソッドはすぐに��る。�g�Hにそれが
	//!				處麻されるのは UpdateToDrawDevice() を柵んだときである。
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r) = 0; // draw device -> layer

	//! @brief		坪否の壅宙鮫を佩う
	//! @note		坪否の壅宙鮫を佩う�Hに柵ぶ。このメソッド坪では、レイヤマネ�`ジャは
	//!				iTVPDrawDevice::StartBitmapCompletion()
	//!				iTVPDrawDevice::NotifyBitmapCompleted()
	//!				iTVPDrawDevice::EndBitmapCompletion() の光メソッドを喘い、
	//!				いままでに�筝�が佩われた�I囃などを��肝宙鮫デバイスに僕る。
	virtual void TJS_INTF_METHOD UpdateToDrawDevice() = 0;

//-- debug assist
	//! @brief		(Window->DrawDevice) レイヤ��夛をコンソ�`ルにダンプする
	virtual void TJS_INTF_METHOD DumpLayerStructure() = 0;
};
//---------------------------------------------------------------------------
/*]*/


//---------------------------------------------------------------------------
// tTVPLayerManager
//---------------------------------------------------------------------------
// layer mamager which is to be connected to draw device
//---------------------------------------------------------------------------
class tTVPLayerManager : public iTVPLayerManager, public tTVPDrawable
{
	tjs_int RefCount; //!< reference count
	tTJSNI_BaseWindow * Window;

	void * DrawDeviceData; //!< draw device specific information

	tTVPBaseBitmap * DrawBuffer;
	tTVPLayerType DesiredLayerType; //!< desired layer type by the draw device for this layer manager

	tTJSNI_BaseLayer * CaptureOwner;
	tTJSNI_BaseLayer * LastMouseMoveSent;

	std::vector<tTJSNI_BaseLayer *> ModalLayerVector;
		// pointer to modal layer vector
	tTJSNI_BaseLayer * FocusedLayer; // pointer to current focused layer
	tTJSNI_BaseLayer * Primary; // primary layer
	bool OverallOrderIndexValid;
	std::vector<tTJSNI_BaseLayer*> AllNodes;
		// hold overall nodes;
		// use GetAllNodes to retrieve the newest information of this
	tTVPComplexRect UpdateRegion; // window update region

	bool FocusChangeLock;

	bool VisualStateChanged;
		// flag for visual
		// state changing ( see tTJSNI_BaseLaye::NotifyChildrenVisualStateChanged)

	tjs_int LastMouseMoveX;
	tjs_int LastMouseMoveY;

	bool ReleaseCaptureCalled;

	bool InNotifyingHintOrCursorChange;

public:
	tTVPLayerManager(tTJSNI_BaseWindow *window);

private:
	virtual ~tTVPLayerManager();
public:
	virtual void TJS_INTF_METHOD AddRef();
	virtual void TJS_INTF_METHOD Release();

	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) { DrawDeviceData = data; }
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const { return DrawDeviceData; }

public:
	void RegisterSelfToWindow();
	void UnregisterSelfFromWindow();

public:
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) { DesiredLayerType = type; }

public: // methods from tTVPDrawable
	virtual tTVPBaseBitmap * GetDrawTargetBitmap(const tTVPRect &rect,
		tTVPRect &cliprect);

	virtual tTVPLayerType GetTargetLayerType();

	virtual void DrawCompleted(const tTVPRect &destrect,
		tTVPBaseBitmap *bmp, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity);

public:
	void AttachPrimary(tTJSNI_BaseLayer *pri); // attach primary layer to the manager
	void DetachPrimary(); // detach primary layer from the manager

	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const { return Primary; }
	bool IsPrimaryLayerAttached() const { return Primary != NULL; } 

	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const;

	void NotifyPart(tTJSNI_BaseLayer *lay); // notifies layer parting from its parent

	tTVPComplexRect & GetUpdateRegionForCompletion() { return UpdateRegion; }

private:

	void _RecreateOverallOrderIndex(tjs_uint& index,
		std::vector<tTJSNI_BaseLayer*>& nodes);

public:
	void InvalidateOverallIndex();
	void RecreateOverallOrderIndex();

	std::vector<tTJSNI_BaseLayer*> & GetAllNodes();

	void NotifyVisualStateChanged() { VisualStateChanged = true; }
	bool GetVisualStateChanged() { return VisualStateChanged; }
	void QueryUpdateExcludeRect();


public:

	void NotifyMouseCursorChange(tTJSNI_BaseLayer *layer, tjs_int cursor);
	void SetMouseCursor(tjs_int cursor);

	void GetCursorPos(tjs_int &x, tjs_int &y);
		// get cursor position in primary coordinates
	void SetCursorPos(tjs_int x, tjs_int y);
		// set cursor position in primary coordinates

	void NotifyHintChange(tTJSNI_BaseLayer *layer, const ttstr & hint);
	void SetHint(const ttstr &hint);
		// set layer hint to current window
	void NotifyLayerResize();  // layer -> window
	void NotifyWindowInvalidation(); // layer -> window

public:
	tTJSNI_BaseWindow * GetWindow() const { return Window; }
	void SetWindow(tTJSNI_BaseWindow *window);
	void NotifyResizeFromWindow(tjs_uint w, tjs_uint h); // draw device -> layer
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r); // draw device -> layer

	virtual void TJS_INTF_METHOD NotifyClick(tjs_int x, tjs_int y) { PrimaryClick(x, y); }
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y) { PrimaryDoubleClick(x, y); }
	virtual void TJS_INTF_METHOD NotifyMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
		{ PrimaryMouseDown(x, y, mb, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags)
		{ PrimaryMouseUp(x, y, mb, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags)
		{ PrimaryMouseMove(x, y, flags); }
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow()
		{ MouseOutOfWindow(); }
	virtual void TJS_INTF_METHOD NotifyKeyDown(tjs_uint key, tjs_uint32 shift)
		{ PrimaryKeyDown(key, shift); }
	virtual void TJS_INTF_METHOD NotifyKeyUp(tjs_uint key, tjs_uint32 shift)
		{ PrimaryKeyUp(key, shift); }
	virtual void TJS_INTF_METHOD NotifyKeyPress(tjs_char key)
		{ PrimaryKeyPress(key); }
	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y)
		{ PrimaryMouseWheel(shift, delta, x, y); }

	tTJSNI_BaseLayer * GetMostFrontChildAt(tjs_int x, tjs_int y,
		tTJSNI_BaseLayer *except = NULL, bool get_disabled = false);

	void PrimaryClick(tjs_int x, tjs_int y);
	void PrimaryDoubleClick(tjs_int x, tjs_int y);

	void PrimaryMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	void PrimaryMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);

	void PrimaryMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	void ForceMouseLeave();
	void ForceMouseRecheck();
	void MouseOutOfWindow();
	void LeaveMouseFromTree(tTJSNI_BaseLayer *root); // force to leave mouse

	virtual void TJS_INTF_METHOD ReleaseCapture();
	void ReleaseCaptureFromTree(tTJSNI_BaseLayer * layer);

	bool BlurTree(tTJSNI_BaseLayer *root); // remove focus from "root"
	tTJSNI_BaseLayer *SearchFirstFocusable(bool ignore_chain_focusable = true); // search first focusable layer


	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const { return FocusedLayer; }
	void CheckTreeFocusableState(tTJSNI_BaseLayer *root);
		// check newly added tree's focusable state
	bool SetFocusTo(tTJSNI_BaseLayer *layer, bool direction = true);
		// set focus to layer
	void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) { SetFocusTo(layer, false); }
	tTJSNI_BaseLayer *FocusPrev(); // focus to previous layer
	tTJSNI_BaseLayer *FocusNext(); // focus to next layer
	void ReleaseAllModalLayer(); // release all modal layer on invalidation
	void SetModeTo(tTJSNI_BaseLayer *layer); // set mode to layer
	void RemoveModeFrom(tTJSNI_BaseLayer *layer); // remove mode from layer
	void RemoveTreeModalState(tTJSNI_BaseLayer *root);
		// remove modal state from given tree
	tTJSNI_BaseLayer *GetCurrentModalLayer() const;

private:
	bool SearchAttentionPoint(tTJSNI_BaseLayer *target, tjs_int &x, tjs_int &y);
	void SetAttentionPointOf(tTJSNI_BaseLayer *layer);
	void DisableAttentionPoint();

public:
	void NotifyAttentionStateChanged(tTJSNI_BaseLayer *from);

private:
	void SetImeModeOf(tTJSNI_BaseLayer *layer);
	void ResetImeMode();

public:
	void NotifyImeModeChanged(tTJSNI_BaseLayer *from);

private:
	tjs_int EnabledWorkRefCount;
public:
	void SaveEnabledWork();
	void NotifyNodeEnabledState();
	void PrimaryKeyDown(tjs_uint key, tjs_uint32 shift);
	void PrimaryKeyUp(tjs_uint key, tjs_uint32 shift);
	void PrimaryKeyPress(tjs_char key);
	void PrimaryMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);

	void AddUpdateRegion(const tTVPComplexRect &rects);
	void AddUpdateRegion(const tTVPRect & rect);
	void PrimaryUpdateByWindow(const tTVPRect &rect);
	virtual void TJS_INTF_METHOD UpdateToDrawDevice();
	void NotifyUpdateRegionFixed();

public:
	void TJS_INTF_METHOD RecheckInputState();
		// To re-check current layer under current mouse position
		// and update hint, cursor type and process layer enter/leave.
		// This can be reasonably slow, about 1 sec interval.

public:
	void TJS_INTF_METHOD DumpLayerStructure();
};
//---------------------------------------------------------------------------



#endif

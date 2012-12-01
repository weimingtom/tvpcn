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
	//! @brief	���ե����󥿤򥤥󥯥���Ȥ���
	virtual void TJS_INTF_METHOD AddRef() = 0;

	//! @brief	���ե����󥿤�ǥ�����Ȥ���
	virtual void TJS_INTF_METHOD Release() = 0;

//-- draw device specific information
	//! @brief	�軭�ǥХ������Ф������O������
	//! @param	data	�軭�ǥХ������Ф����
	//! @note	�軭�ǥХ������Ф�����쥤��ޥͩ`������O�����롣
	//!			�쥤��ޥͩ`����ǤϤ�����������ˤĤ��Ƥ��v֪���ʤ���
	//!			�軭�ǥХ����Ȥ�Ŀӡ��ʹ�ä��ꡢ�ض������ȽY�ӤĤ��ƹ����롣
	virtual void TJS_INTF_METHOD SetDrawDeviceData(void * data) = 0;

	//! @brief	�軭�ǥХ������Ф�����ȡ�ä���
	//! @return	�軭�ǥХ������Ф����
	virtual void * TJS_INTF_METHOD GetDrawDeviceData() const = 0;

//-- layer metrics
	//! @brief	�ץ饤�ޥ�쥤��Υ�������ȡ�ä���
	//! @param	w	�쥤��κ��(�ԥ�����gλ)
	//! @param	h	�쥤��οk��(�ԥ�����gλ)
	//! @return	ȡ�ä˳ɹ�������桢ʧ������Ђ�
	virtual bool TJS_INTF_METHOD GetPrimaryLayerSize(tjs_int &w, tjs_int &h) const = 0;

//-- layer structure information
	//! @brief	�ץ饤�ޥ�쥤���ȡ��
	//! @return	�ץ饤�ޥ�쥤��
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() const = 0;

	//! @brief	�ե��`�����Τ���쥤���ȡ��
	//! @return	�ե��`�����Τ���쥤��
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() const = 0;

	//! @brief	�ե��`�����Τ���쥤����O��
	//! @param	layer	�ե��`�����Τ���쥤��
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;

//-- HID releted
	//! @brief		����å����줿
	//! @param		x		�ץ饤�ޥ�쥤�������Ϥˤ����� x λ��
	//! @param		y		�ץ饤�ޥ�쥤�������Ϥˤ����� y λ��
	virtual void TJS_INTF_METHOD NotifyClick(tjs_int x, tjs_int y) = 0;

	//! @brief		���֥륯��å����줿
	//! @param		x		�ץ饤�ޥ�쥤�������Ϥˤ����� x λ��
	//! @param		y		�ץ饤�ޥ�쥤�������Ϥˤ����� y λ��
	virtual void TJS_INTF_METHOD NotifyDoubleClick(tjs_int x, tjs_int y) = 0;

	//! @brief		�ޥ����ܥ���Ѻ�¤��줿
	//! @param		x		�ץ饤�ޥ�쥤�������Ϥˤ����� x λ��
	//! @param		y		�ץ饤�ޥ�쥤�������Ϥˤ����� y λ��
	//! @param		mb		�ɤΥޥ����ܥ���
	//! @param		flags	�ե饰(TVP_SS_*�����νM�ߺϤ碌)
	virtual void TJS_INTF_METHOD NotifyMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		�ޥ����ܥ����x���줿
	//! @param		x		�ץ饤�ޥ�쥤�������Ϥˤ����� x λ��
	//! @param		y		�ץ饤�ޥ�쥤�������Ϥˤ����� y λ��
	//! @param		mb		�ɤΥޥ����ܥ���
	//! @param		flags	�ե饰(TVP_SS_*�����νM�ߺϤ碌)
	virtual void TJS_INTF_METHOD NotifyMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		�ޥ������ƄӤ���
	//! @param		x		�ץ饤�ޥ�쥤�������Ϥˤ����� x λ��
	//! @param		y		�ץ饤�ޥ�쥤�������Ϥˤ����� y λ��
	//! @param		flags	�ե饰(TVP_SS_*�����νM�ߺϤ碌)
	virtual void TJS_INTF_METHOD NotifyMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;

	//! @brief		�ޥ�������ץ�����Ť���
	//! @note		�ޥ�������ץ�����Ť��٤����Ϥ˥�����ɥ�������Ф�롣
	virtual void TJS_INTF_METHOD ReleaseCapture() = 0;

	//! @brief		�ޥ������ץ饤�ޥ�쥤������ƄӤ���
	virtual void TJS_INTF_METHOD NotifyMouseOutOfWindow() = 0;

	//! @brief		���`��Ѻ���줿
	//! @param		key		���륭�`���`��
	//! @param		shift	���եȥ��`��״�B
	virtual void TJS_INTF_METHOD NotifyKeyDown(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		���`���x���줿
	//! @param		key		���륭�`���`��
	//! @param		shift	���եȥ��`��״�B
	virtual void TJS_INTF_METHOD NotifyKeyUp(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		���`�ˤ������
	//! @param		key		���֥��`��
	virtual void TJS_INTF_METHOD NotifyKeyPress(tjs_char key) = 0;

	//! @brief		�ޥ����ۥ��`�뤬��ܞ����
	//! @param		shift	���եȥ��`��״�B
	//! @param		delta	��ܞ��
	//! @param		x		�ץ饤�ޥ�쥤�������Ϥˤ����� x λ��
	//! @param		y		�ץ饤�ޥ�쥤�������Ϥˤ����� y λ��
	virtual void TJS_INTF_METHOD NotifyMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;

	//! @brief		����״�B�Υ����å�
	//! @note		������ɥ�����s1�뤪���ˡ��쥤��ޥͩ`���㤬��`�������������״�B��
	//!				�٥����å����뤿��˺��Ф�롣�쥤��״�B�Ή仯����`���������Ȥ�
	//!				��ͬ�ڤ��Ф�줿���ϡ����Ȥ��Хޥ������`������¤˥쥤�䤬���F����
	//!				�Τˤ⤫����餺���ޥ������`���뤬���Υ쥤���ָ��������״�ˉ������ʤ�
	//!				�Ȥ��ä�״�r���k�������롣���Τ褦��״�r�ˌ��I���뤿�ᡢ������ɥ�����
	//!				���Υ᥽�åɤ��s1�뤪���˺��Ф�롣
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;

//-- invalidation/update
	//! @brief		�軭�ǥХ���������쥤��γ�����ʽ���O������
	//! @param		type	�쥤����ʽ
	//! @note		�ǥե���Ȥ� ltOpaque ���軭�ǥХ�����������ʽ�λ��������Ȥ���
	//!				����ʤ�Ф�����ʽ��ָ�����롣���������ץ饤�ޥ�쥤��� type
	//!				�ץ�ѥƥ���ͬ���ˉ�����뤳�ȡ�
	virtual void TJS_INTF_METHOD SetDesiredLayerType(tTVPLayerType type) = 0;

	//! @brief		�ض��ξ��Τ����軭��Ҫ�󤹤�
	//! @param		r		�ץ饤�ޥ�쥤�������Ϥˤ��������
	//! @note		�ض��ξ��Τ����軭��쥤��ޥͩ`����ˌ�����Ҫ�󤹤롣
	//!				Ҫ���ӛ�h���������Ǥ��Υ᥽�åɤϤ����ˑ��롣�g�H�ˤ��줬
	//!				���㤵���Τ� UpdateToDrawDevice() �������Ȥ��Ǥ��롣
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect &r) = 0; // draw device -> layer

	//! @brief		���ݤ����軭���Ф�
	//! @note		���ݤ����軭���Ф��H�˺��֡����Υ᥽�å��ڤǤϡ��쥤��ޥͩ`�����
	//!				iTVPDrawDevice::StartBitmapCompletion()
	//!				iTVPDrawDevice::NotifyBitmapCompleted()
	//!				iTVPDrawDevice::EndBitmapCompletion() �θ��᥽�åɤ��ä���
	//!				���ޤޤǤˉ�����Ф�줿�I��ʤɤ�혴��軭�ǥХ������ͤ롣
	virtual void TJS_INTF_METHOD UpdateToDrawDevice() = 0;

//-- debug assist
	//! @brief		(Window->DrawDevice) �쥤�䘋��򥳥󥽩`��˥���פ���
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

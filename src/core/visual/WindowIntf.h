//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "Window" TJS Class implementation
//---------------------------------------------------------------------------
#ifndef WindowIntfH
#define WindowIntfH

#include "tjsNative.h"
#include "drawable.h"
#include "ComplexRect.h"
#include "tvpinputdefs.h"
#include "EventIntf.h"
#include "ObjectList.h"
#include "DrawDevice.h"




//---------------------------------------------------------------------------
// Window List Management
//---------------------------------------------------------------------------
extern void TVPClearAllWindowInputEvents();
extern void TVPDeliverDrawDeviceShow();
//---------------------------------------------------------------------------




/*[*/
//---------------------------------------------------------------------------
// Window related constants
//---------------------------------------------------------------------------
enum tTVPUpdateType
{
	utNormal, // only needed region
	utEntire // entire of window
};
//---------------------------------------------------------------------------
enum tTVPBorderStyle
{
	bsNone=0,  bsSingle=1,  bsSizeable=2,  bsDialog=3,  bsToolWindow=4,
	bsSizeToolWin =5
};
//---------------------------------------------------------------------------
enum tTVPMouseCursorState
{
	mcsVisible, // the mouse cursor is visible
	mcsTempHidden, // the mouse cursor is temporarily hidden
	mcsHidden // the mouse cursor is invisible
};
//---------------------------------------------------------------------------
/*]*/

/*[*/
//---------------------------------------------------------------------------
//! @brief Window basic interface
//---------------------------------------------------------------------------
class iTVPWindow
{
public:
	//! @brief	Ԫ����Υ�������������줿
	//! @note	�軭�ǥХ�������Ԫ����Υ�������������줿���Ȥ�֪ͨ���뤿��˺��֡�
	//!			������ɥ��� iTVPDrawDevice::GetSrcSize() ����ӳ�����Ԫ�����
	//!			��������ȡ�ä����ᡢ���`��ʤɤ�Ӌ����ФäƤ��� 
	//!			iTVPDrawDevice::SetTargetWindow() ����ӳ�����
	virtual void TJS_INTF_METHOD NotifySrcResize() = 0;

	//! @brief		�ޥ������`�������״��ǥե���Ȥˑ���
	//! @note		�ޥ������`�������״��ǥե���Ȥ���ˑ����������Ϥ˺���
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor() = 0; // set window mouse cursor to default

	//! @brief		�ޥ������`�������״���O������
	//! @param		cursor		�ޥ������`������״����
	virtual void TJS_INTF_METHOD SetMouseCursor(tjs_int cursor) = 0; // set window mouse cursor

	//! @brief		�ޥ������`�����λ�ä�ȡ�ä���
	//! @param		x			�軭�����ڤ����ˤˤ�����ޥ������`�����xλ��
	//! @param		y			�軭�����ڤ����ˤˤ�����ޥ������`�����yλ��
	virtual void TJS_INTF_METHOD GetCursorPos(tjs_int &x, tjs_int &y) = 0;
		// get mouse cursor position in primary layer's coordinates

	//! @brief		�ޥ������`�����λ�ä��O������
	//! @param		x			�軭�����ڤ����ˤˤ�����ޥ������`�����xλ��
	//! @param		y			�軭�����ڤ����ˤˤ�����ޥ������`�����yλ��
	virtual void TJS_INTF_METHOD SetCursorPos(tjs_int x, tjs_int y) = 0;

	//! @brief		������ɥ��Υޥ�������ץ�����Ť���
	//! @note		������ɥ��Υޥ�������ץ�����Ť��٤����Ϥ˺��֡�
	//! @note		���Υ᥽�åɤǤϻ����Ĥˤ� ::ReleaseCapture() �ʤɤ�
	//!				�ޥ����Υ���ץ�����_�Ť��뤳�ȡ�
	virtual void TJS_INTF_METHOD WindowReleaseCapture() = 0;

	//! @brief		�ĩ`����åץҥ�Ȥ��O������
	//! @param		text		�ҥ�ȥƥ�����(�������ФΈ��Ϥϥҥ�Ȥα�ʾ�򥭥�󥻥뤹��)
	virtual void TJS_INTF_METHOD SetHintText(const ttstr & text) = 0;

	//! @brief		עҕ�ݥ���Ȥ��O��
	//! @param		layer		�ե�������κ��ޤ��쥤��
	//! @param		x			�軭�����ڤ����ˤˤ�����עҕ�ݥ���Ȥ�xλ��
	//! @param		y			�軭�����ڤ����ˤˤ�����עҕ�ݥ���Ȥ�yλ��
	virtual void TJS_INTF_METHOD SetAttentionPoint(tTJSNI_BaseLayer *layer,
		tjs_int l, tjs_int t) = 0;

	//! @brief		עҕ�ݥ���Ȥν��
	virtual void TJS_INTF_METHOD DisableAttentionPoint() = 0;

	//! @brief		IME��`�ɤ��O��
	//! @param		mode		IME��`��
	virtual void TJS_INTF_METHOD SetImeMode(tTVPImeMode mode) = 0;

	//! @brief		IME��`�ɤΥꥻ�å�
	virtual void TJS_INTF_METHOD ResetImeMode() = 0;

	//! @brief		iTVPWindow::Update() �κ��ӳ�����Ҫ�󤹤�
	//! @note		������ɥ��ˌ����� iTVPWindow::Update() ��Τ��m���ʥ����ߥ󥰤�
	//!				���ӳ������Ȥ�Ҫ�󤹤롣
	//!				iTVPWindow::Update() �����ӳ������ޤǤϺλ� RequestUpdate() ��
	//!				����Ǥℿ����ͬ���Ǥ��롣�ޤ���һ�� iTVPWindow::Update() ��
	//!				���ӳ������ȡ��٤� RequestUpdate() ����Фʤ��ޤ��
	//!				iTVPWindow::Update() �Ϻ��Ф�ʤ���
	virtual void TJS_INTF_METHOD RequestUpdate() = 0;


	//! @brief		Window��iTJSDispatch2���󥿩`�ե��`����ȡ�ä���
	virtual iTJSDispatch2 * GetWindowDispatch() = 0;

};
//---------------------------------------------------------------------------
/*]*/


//---------------------------------------------------------------------------
// tTJSNI_BaseWindow
//---------------------------------------------------------------------------
class tTVPBaseBitmap;
class tTJSNI_BaseLayer;
class tTJSNI_BaseVideoOverlay;
class tTJSNI_BaseWindow : public tTJSNativeInstance, public iTVPWindow
{
	typedef tTJSNativeInstance inherited;

private:
	std::vector<tTJSVariantClosure> ObjectVector;
	bool ObjectVectorLocked;

protected:
	iTJSDispatch2 *Owner;
public:
	iTJSDispatch2 * GetOwnerNoAddRef() const { return Owner; }

public:
	tTJSNI_BaseWindow();
	~tTJSNI_BaseWindow();
	tjs_error TJS_INTF_METHOD
	Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

	bool IsMainWindow() const;
	virtual bool GetWindowActive() = 0;
	void FireOnActivate(bool activate_or_deactivate);


	//-- interface to draw device
public:
	tTJSVariant DrawDeviceObject; //!< Current Draw Device TJS2 Object
	iTVPDrawDevice * DrawDevice; //!< Current Draw Device
	void SetDrawDeviceObject(const tTJSVariant & val);
	const tTJSVariant & GetDrawDeviceObject() const { return DrawDeviceObject; }
	iTVPDrawDevice * GetDrawDevice() const { return DrawDevice ; }
	virtual void ResetDrawDevice() = 0;
	virtual iTJSDispatch2 * GetWindowDispatch() { if(Owner) Owner->AddRef(); return Owner; }


	//----- event dispatching
public:
	virtual bool CanDeliverEvents() const = 0; // implement this in each platform


public:

	void OnClose();
	void OnResize();
	void OnClick(tjs_int x, tjs_int y);
	void OnDoubleClick(tjs_int x, tjs_int y);
	void OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	void OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	void OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	void OnReleaseCapture();
	void OnMouseOutOfWindow();
	void OnMouseEnter();
	void OnMouseLeave();
	void OnKeyDown(tjs_uint key, tjs_uint32 shift);
	void OnKeyUp(tjs_uint key, tjs_uint32 shift);
	void OnKeyPress(tjs_char key);
	void OnFileDrop(const tTJSVariant &array);
	void OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);
	void OnPopupHide();
	void OnActivate(bool activate_or_deactivate);

	void ClearInputEvents();

	void PostReleaseCaptureEvent();

	//----- layer managermant
public:
	void RegisterLayerManager(iTVPLayerManager * manager);
	void UnregisterLayerManager(iTVPLayerManager * manager);


protected:
	tTVPRect WindowExposedRegion;
	tTVPBaseBitmap * DrawBuffer;

	bool WindowUpdating; // window is in updating

public:
	void NotifyWindowExposureToLayer(const tTVPRect &cliprect);

public:
	void NotifyUpdateRegionFixed(const tTVPComplexRect &updaterects); // is called by layer manager
	void UpdateContent(); // is called from event dispatcher
	void DeliverDrawDeviceShow();
	virtual void BeginUpdate(const tTVPComplexRect & rects);
	virtual void EndUpdate();
	virtual void TJS_INTF_METHOD RequestUpdate();
	virtual void TJS_INTF_METHOD NotifySrcResize(); // is called from primary layer
	virtual tTVPImeMode GetDefaultImeMode() const = 0;

	void DumpPrimaryLayerStructure();

	void RecheckInputState(); // slow timer tick (about 1 sec interval, inaccurate)

	void SetShowUpdateRect(bool b);


	//----- methods
	void Add(tTJSVariantClosure clo);
	void Remove(tTJSVariantClosure clo);

	//----- interface to menu object
private:
	iTJSDispatch2 * MenuItemObject;
public:
	iTJSDispatch2 * GetMenuItemObjectNoAddRef();

	//----- interface to video overlay object
protected:
	tObjectList<tTJSNI_BaseVideoOverlay> VideoOverlay;

public:
	void RegisterVideoOverlayObject(tTJSNI_BaseVideoOverlay *ovl);
	void UnregisterVideoOverlayObject(tTJSNI_BaseVideoOverlay *ovl);

};
//---------------------------------------------------------------------------

#include "WindowImpl.h" // must define tTJSNI_Window class

//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Window List
//---------------------------------------------------------------------------
class tTJSNI_Window;
extern tTJSNI_Window * TVPGetWindowListAt(tjs_int idx);
extern tjs_int TVPGetWindowCount();
extern tTJSNI_Window * TVPMainWindow; //  = NULL; // main window

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTJSNC_Window : TJS Window class
//---------------------------------------------------------------------------
class tTJSNC_Window : public tTJSNativeClass
{
public:
	tTJSNC_Window();
	static tjs_uint32 ClassID;

protected:
	tTJSNativeInstance *CreateNativeInstance();
	/*
		implement this in each platform.
		this must return a proper instance of tTJSNI_Window.
	*/
};
//---------------------------------------------------------------------------
extern tTJSNativeClass * TVPCreateNativeClass_Window();
	/*
		implement this in each platform.
		this must return a proper instance of tTJSNI_Window.
		usually simple returns: new tTJSNC_Window();
	*/
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Input Events
//---------------------------------------------------------------------------
class tTVPOnCloseInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnCloseInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnClose(); }
};
//---------------------------------------------------------------------------
class tTVPOnResizeInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnResizeInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnResize(); }
};
//---------------------------------------------------------------------------
class tTVPOnClickInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_int X;
	tjs_int Y;
public:
	tTVPOnClickInputEvent(tTJSNI_BaseWindow *win, tjs_int x, tjs_int y) :
		tTVPBaseInputEvent(win, Tag), X(x), Y(y) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnClick(X, Y); }
};
//---------------------------------------------------------------------------
class tTVPOnDoubleClickInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_int X;
	tjs_int Y;
public:
	tTVPOnDoubleClickInputEvent(tTJSNI_BaseWindow *win, tjs_int x, tjs_int y) :
		tTVPBaseInputEvent(win, Tag), X(x), Y(y) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnDoubleClick(X, Y); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseDownInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_int X;
	tjs_int Y;
	tTVPMouseButton Buttons;
	tjs_uint32 Flags;
public:
	tTVPOnMouseDownInputEvent(tTJSNI_BaseWindow *win, tjs_int x, tjs_int y,
		tTVPMouseButton buttons, tjs_uint32 flags) :
		tTVPBaseInputEvent(win, Tag), X(x), Y(y), Buttons(buttons), Flags(flags) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseDown(X, Y, Buttons, Flags); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseUpInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_int X;
	tjs_int Y;
	tTVPMouseButton Buttons;
	tjs_uint32 Flags;
public:
	tTVPOnMouseUpInputEvent(tTJSNI_BaseWindow *win, tjs_int x, tjs_int y,
		tTVPMouseButton buttons, tjs_uint32 flags) :
		tTVPBaseInputEvent(win, Tag), X(x), Y(y), Buttons(buttons), Flags(flags) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseUp(X, Y, Buttons, Flags); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseMoveInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_int X;
	tjs_int Y;
	tjs_uint32 Flags;
public:
	tTVPOnMouseMoveInputEvent(tTJSNI_BaseWindow *win, tjs_int x, tjs_int y,
		tjs_uint32 flags) :
		tTVPBaseInputEvent(win, Tag), X(x), Y(y), Flags(flags) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseMove(X, Y, Flags); }
};
//---------------------------------------------------------------------------
class tTVPOnReleaseCaptureInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnReleaseCaptureInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnReleaseCapture(); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseOutOfWindowInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnMouseOutOfWindowInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseOutOfWindow(); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseEnterInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnMouseEnterInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseEnter(); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseLeaveInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnMouseLeaveInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseLeave(); }
};
//---------------------------------------------------------------------------
class tTVPOnKeyDownInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_uint Key;
	tjs_uint32 Shift;
public:
	tTVPOnKeyDownInputEvent(tTJSNI_BaseWindow *win, tjs_uint key, tjs_uint32 shift) :
		tTVPBaseInputEvent(win, Tag), Key(key), Shift(shift) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnKeyDown(Key, Shift); }
};
//---------------------------------------------------------------------------
class tTVPOnKeyUpInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_uint Key;
	tjs_uint32 Shift;
public:
	tTVPOnKeyUpInputEvent(tTJSNI_BaseWindow *win, tjs_uint key, tjs_uint32 shift) :
		tTVPBaseInputEvent(win, Tag), Key(key), Shift(shift) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnKeyUp(Key, Shift); }
};
//---------------------------------------------------------------------------
class tTVPOnKeyPressInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_char Key;
public:
	tTVPOnKeyPressInputEvent(tTJSNI_BaseWindow *win, tjs_char key) :
		tTVPBaseInputEvent(win, Tag), Key(key) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnKeyPress(Key); }
};
//---------------------------------------------------------------------------
class tTVPOnFileDropInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tTJSVariant Array;
public:
	tTVPOnFileDropInputEvent(tTJSNI_BaseWindow *win, const tTJSVariant & val) :
		tTVPBaseInputEvent(win, Tag), Array(val) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnFileDrop(Array); }
};
//---------------------------------------------------------------------------
class tTVPOnMouseWheelInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	tjs_uint32 Shift;
	tjs_int WheelDelta;
	tjs_int X;
	tjs_int Y;
public:
	tTVPOnMouseWheelInputEvent(tTJSNI_BaseWindow *win, tjs_uint32 shift,
		tjs_int wheeldelta, tjs_int x, tjs_int y) :
		tTVPBaseInputEvent(win, Tag), Shift(shift), WheelDelta(wheeldelta),
		X(x), Y(y) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnMouseWheel(Shift, WheelDelta, X, Y); }
};
//---------------------------------------------------------------------------
class tTVPOnPopupHideInputEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
public:
	tTVPOnPopupHideInputEvent(tTJSNI_BaseWindow *win) :
		tTVPBaseInputEvent(win, Tag) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnPopupHide(); }
};
//---------------------------------------------------------------------------
class tTVPOnWindowActivateEvent : public tTVPBaseInputEvent
{
	static tTVPUniqueTagForInputEvent Tag;
	bool ActivateOrDeactivate;
public:
	tTVPOnWindowActivateEvent(tTJSNI_BaseWindow *win, bool activate_or_deactivate) :
		tTVPBaseInputEvent(win, Tag), ActivateOrDeactivate(activate_or_deactivate) {};
	void Deliver() const
	{ ((tTJSNI_BaseWindow*)GetSource())->OnActivate(ActivateOrDeactivate); }
};
//---------------------------------------------------------------------------



#endif

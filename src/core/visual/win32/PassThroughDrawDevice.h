//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" �軭�ǥХ�������
//---------------------------------------------------------------------------
#ifndef PASSTHROUGHDRAWDEVICE_H
#define PASSTHROUGHDRAWDEVICE_H

#include "DrawDevice.h"

class tTVPDrawer;
//---------------------------------------------------------------------------
//! @brief		��Pass Through���ǥХ���(��äȤ�����Ĥ��軭���Ф��ΤߤΥǥХ���)
//---------------------------------------------------------------------------
class tTVPPassThroughDrawDevice : public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;
	HWND TargetWindow;
	bool IsMainWindow;
	tTVPDrawer * Drawer; //!< �軭���Ф����

public:
	//! @brief	drawer�Υ�����
	enum tDrawerType
	{
		dtNone, //!< drawer �ʤ�
		dtDrawDib, //!< ��äȤ�g����drawer
		dtDBGDI, // GDI �ˤ����֥�Хåե���󥰤��Ф�drawer
		dtDBDD, // DirectDraw �ˤ����֥�Хåե���󥰤��Ф�drawer
		dtDBD3D // Direct3D �ˤ����֥�Хåե���󥰤��Ф�drawer
	};

private:
	tDrawerType DrawerType; //!< drawer �Υ�����
	tDrawerType PreferredDrawerType; //!< ʹ�ä������� drawer �Υ�����

	bool DestSizeChanged; //!< DestRect �Υ������ˉ�������ä���
	bool SrcSizeChanged; //!< SrcSize �ˉ�������ä���

public:
	tTVPPassThroughDrawDevice(); //!< ���󥹥ȥ饯��
private:
	~tTVPPassThroughDrawDevice(); //!< �ǥ��ȥ饯��

public:
	void SetToRecreateDrawer() { DestroyDrawer(); }
	void DestroyDrawer();
private:
	void CreateDrawer(tDrawerType type);
	void CreateDrawer(bool zoom_required, bool should_benchmark);

public:
	void EnsureDrawer();

	tDrawerType GetDrawerType() const { return DrawerType; }
	void SetPreferredDrawerType(tDrawerType type) { PreferredDrawerType = type; }
	tDrawerType GetPreferredDrawerType() const { return PreferredDrawerType; }

//---- LayerManager �ι����v�B
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);

//---- �軭λ�å������v�B
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);

//---- ���軭�v�B
	virtual void TJS_INTF_METHOD Show();

//---- LayerManager ����λ����ܤ��ɤ��v�B
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

//---- �ǥХå�֧Ԯ
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b);

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// tTJSNI_PassThroughDrawDevice
//---------------------------------------------------------------------------
class tTJSNI_PassThroughDrawDevice :
	public tTJSNativeInstance
{
	typedef tTJSNativeInstance inherited;

	tTVPPassThroughDrawDevice * Device;

public:
	tTJSNI_PassThroughDrawDevice();
	~tTJSNI_PassThroughDrawDevice();
	tjs_error TJS_INTF_METHOD
		Construct(tjs_int numparams, tTJSVariant **param,
			iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

public:
	tTVPPassThroughDrawDevice * GetDevice() const { return Device; }

};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_PassThroughDrawDevice
//---------------------------------------------------------------------------
class tTJSNC_PassThroughDrawDevice : public tTJSNativeClass
{
public:
	tTJSNC_PassThroughDrawDevice();

	static tjs_uint32 ClassID;

private:
	iTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------


#endif

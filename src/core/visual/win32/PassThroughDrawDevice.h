//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file "PassThrough" 宙鮫デバイス砿尖
//---------------------------------------------------------------------------
#ifndef PASSTHROUGHDRAWDEVICE_H
#define PASSTHROUGHDRAWDEVICE_H

#include "DrawDevice.h"

class tTVPDrawer;
//---------------------------------------------------------------------------
//! @brief		仝Pass Through々デバイス(もっとも児云議な宙鮫を佩うのみのデバイス)
//---------------------------------------------------------------------------
class tTVPPassThroughDrawDevice : public tTVPDrawDevice
{
	typedef tTVPDrawDevice inherited;
	HWND TargetWindow;
	bool IsMainWindow;
	tTVPDrawer * Drawer; //!< 宙鮫を佩うもの

public:
	//! @brief	drawerのタイプ
	enum tDrawerType
	{
		dtNone, //!< drawer なし
		dtDrawDib, //!< もっとも�g��なdrawer
		dtDBGDI, // GDI によるダブルバッファリングを佩うdrawer
		dtDBDD, // DirectDraw によるダブルバッファリングを佩うdrawer
		dtDBD3D // Direct3D によるダブルバッファリングを佩うdrawer
	};

private:
	tDrawerType DrawerType; //!< drawer のタイプ
	tDrawerType PreferredDrawerType; //!< 聞って圀しい drawer のタイプ

	bool DestSizeChanged; //!< DestRect のサイズに�筝�があったか
	bool SrcSizeChanged; //!< SrcSize に�筝�があったか

public:
	tTVPPassThroughDrawDevice(); //!< コンストラクタ
private:
	~tTVPPassThroughDrawDevice(); //!< デストラクタ

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

//---- LayerManager の砿尖�v�B
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);

//---- 宙鮫了崔サイズ�v�B
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main);
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);

//---- 壅宙鮫�v�B
	virtual void TJS_INTF_METHOD Show();

//---- LayerManager からの鮫�駟椶蔚匹潔v�B
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager);

//---- デバッグ屶址
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

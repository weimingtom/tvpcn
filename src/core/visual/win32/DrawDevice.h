//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//!@file �軭�ǥХ�������
//---------------------------------------------------------------------------
#ifndef DRAWDEVICE_H
#define DRAWDEVICE_H

#include "LayerIntf.h"
#include "LayerManager.h"
#include "ComplexRect.h"

class iTVPWindow;
class tTJSNI_BaseLayer;

/*[*/
//---------------------------------------------------------------------------
//! @brief		�軭�ǥХ������󥿩`�ե��`��
//---------------------------------------------------------------------------
class iTVPDrawDevice
{
public:
//---- ���֥��������������g����
	//! @brief		(Window��DrawDevice) �軭�ǥХ������Ɨ�����
	//! @note		������ɥ����Ɨ������Ȥ������뤤�Ϥۤ����軭�ǥХ�����
	//!				�O�����줿����ˤ����軭�ǥХ�������Ҫ�ʤ��ʤä��H�˺��Ф�롣
	//!				ͨ���������Ǥ� delete this ��g�Ф����軭�ǥХ������Ɨ����뤬������ǰ��
	//!				AddLayerManager() �Ǥ����軭�ǥХ����ι����¤���äƤ���
	//!				�쥤��ޥͩ`����򤹤٤� Release ���롣
	//!				�쥤��ޥͩ`����� Release �Ф� RemoveLayerManager() �����Ф��
	//!				�����Ԥ����뤳�Ȥ�ע�⤹�뤳�ȡ�
	virtual void TJS_INTF_METHOD Destruct() = 0;

//---- window interface �v�B
	//! @brief		(Window��DrawDevice) ������ɥ����󥿩`�ե��`�����O������
	//! @param		window		������ɥ����󥿩`�ե��`��
	//! @note		(TJS����) Window.drawDevice �ץ�ѥƥ����O������ֱ��˺��Ф�롣
	virtual void TJS_INTF_METHOD SetWindowInterface(iTVPWindow * window) = 0;

//---- LayerManager �ι����v�B
	//! @brief		(Window��DrawDevice) �쥤��ޥͩ`�����׷�Ӥ���
	//! @note		�ץ饤�ޥ�쥤�䤬������ɥ���׷�Ӥ����ȡ��ԄӵĤ˥쥤��ޥͩ`���㤬
	//!				���ɤ��졢���줬�軭�ǥХ����ˤ⤳�Υ᥽�åɤκ��ӳ����ˤ�֪ͨ����롣
	//!				�軭�ǥХ����Ǥ� iTVPLayerManager::AddRef() ����ӳ����ơ�׷�Ӥ��줿
	//!				�쥤��ޥͩ`������å����뤳�ȡ�
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager) = 0;

	//! @brief		(Window��DrawDevice) �쥤��ޥͩ`�������������
	//! @note		�ץ饤�ޥ�쥤�䤬 invalidate ������H�˺��ӳ�����롣
	//TODO: �ץ饤�ޥ�쥤��o���������뤤�ϥ�����ɥ��Ɨ��r�νK�˄I������������
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager) = 0;

//---- �軭λ�å������v�B
	//! @brief		(Window��DrawDevice) �軭�ȥ�����ɥ����O��
	//! @param		wnd		������ɥ��ϥ�ɥ�
	//! @param		is_main	�ᥤ�󥦥���ɥ��Έ��Ϥ���
	//! @note		������ɥ������軭�ȤȤʤ륦����ɥ��ϥ�ɥ��ָ�����뤿��˺��Ф�롣
	//!				���Ф��С�Window.borderStyle �ץ�ѥƥ���������줿�ꡢ�ե륹����`���
	//!				���Ф���Ȥ���ե륹����`�󤫤����r�ʤɡ�������ɥ��������ɤ����
	//!				���Ȥ����뤬�����Τ褦�ʈ��Ϥˤϡ�������ɥ������ä����Ɨ������ֱǰ��
	//!				wnd = NULL ��״�B�Ǥ��Υ᥽�åɤ����Ф�뤳�Ȥ�ע�⡣������ɥ�������
	//!				���줿���ȡ��٤��Є��ʥ�����ɥ��ϥ�ɥ���äƤ��Υ᥽�åɤ����Ф�롣
	//!				���Υ᥽�åɤϡ�������ɥ������ɤ��줿ֱ��˺��Ф�뱣�^�Ϥʤ���
	//!				�����Ƥ���һ������˥�����ɥ�����ʾ���줿ֱ��˺��Ф�롣
	virtual void TJS_INTF_METHOD SetTargetWindow(HWND wnd, bool is_main) = 0;

	//! @brief		(Window->DrawDevice) �軭���Τ��O��
	//! @note		������ɥ����顢�軭�ȤȤʤ���Τ��O�����뤿��˺��Ф�롣
	//!				�軭�ǥХ����ϡ�SetTargetWindow() ��ָ�����줿������ɥ��Υ��饤������I��Ρ�
	//!				���Υ᥽�åɤ�ָ�����줿���Τ˱�ʾ���Ф���Ҫ�����롣
	//!				���ξ��Τϡ�GetSrcSize �Ƿ��������ˌ�����Window.zoomNumer �� Window.zoomDenum
	//!				�ץ�ѥƥ��ˤ�뒈���ʤ䡢Window.layerLeft �� Window.layerTop ����ζ���줿
	//!				���ΤǤ��롣
	//!				���Υ᥽�åɤˤ�ä��軭���Τ����ä��Ȥ��Ƥ⡢���Υ����ߥ󥰤�
	//!				�軭�ǥХ����Ȥ����軭���Ф���Ҫ�Ϥʤ�(��Ҫ������Єe�᥽�åɤˤ��
	//!				���軭�α�Ҫ�Ԥ�֪ͨ����뤿��)��
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect) = 0;

	//! @brief		(Window->DrawDevice) Ԫ����Υ�������ä�
	//! @note		������ɥ����顢�軭���ΤΥ�������Q�����뤿���Ԫ����Υ�������
	//!				��Ҫ�ˤʤä��H�˺��Ф�롣������ɥ��Ϥ�����Ȥ� SetDestRectangle()
	//!				�᥽�åɤ��軭���Τ�֪ͨ���Ƥ�������ʤΤǡ�
	//!				�ʤ�餫����ζ�Τ��륵�����Ǥ����Ҫ�ϱؤ�����ʤ���
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h) = 0;

	//! @brief		(LayerManager��DrawDevice) �쥤�䥵���������֪ͨ
	//! @param		manager		�쥤��ޥͩ`����
	//! @note		�쥤��ޥͩ`����˥����å�����Ƥ���ץ饤�ޥ�쥤��Υ����������ä�
	//!				�H�˺��ӳ������
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) �쥤��λ���Ή����֪ͨ
	//! @param		manager		�쥤��ޥͩ`����
	//! @note		�쥤��λ���ˉ仯�����ä��H�˺��ӳ�����롣
	//!				����֪ͨ���ܤ�ȡ�ä���� iTVPLayerManager::UpdateToDrawDevice()
	//!				����ӳ����С�ԓ�����֤��軭�ǥХ����ˌ������軭�����뤳�Ȥ��Ǥ��롣
	//!				����֪ͨ���ܤ�ȡ�äƤ�oҕ���뤳�ȤϿ��ܡ����Έ��Ϥϡ�
	//!				�Τ� iTVPLayerManager::UpdateToDrawDevice() �������H�ˡ�
	//!				����ޤǤΉ���֤����٤��軭����롣
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager) = 0;

//---- ��`���`���󥿩`�ե��`���v�B
	//! @brief		(Window��DrawDevice) ����å����줿
	//! @param		x		�軭�����ڤˤ����� x λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		y		�軭�����ڤˤ����� y λ��(�軭���Τ����Ϥ�ԭ��)
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y) = 0;

	//! @brief		(Window��DrawDevice) ���֥륯��å����줿
	//! @param		x		�軭�����ڤˤ����� x λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		y		�軭�����ڤˤ����� y λ��(�軭���Τ����Ϥ�ԭ��)
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y) = 0;

	//! @brief		(Window��DrawDevice) �ޥ����ܥ���Ѻ�¤��줿
	//! @param		x		�軭�����ڤˤ����� x λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		y		�軭�����ڤˤ����� y λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		mb		�ɤΥޥ����ܥ���
	//! @param		flags	�ե饰(TVP_SS_*�����νM�ߺϤ碌)
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		(Window��DrawDevice) �ޥ����ܥ����x���줿
	//! @param		x		�軭�����ڤˤ����� x λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		y		�軭�����ڤˤ����� y λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		mb		�ɤΥޥ����ܥ���
	//! @param		flags	�ե饰(TVP_SS_*�����νM�ߺϤ碌)
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags) = 0;

	//! @brief		(Window��DrawDevice) �ޥ������ƄӤ���
	//! @param		x		�軭�����ڤˤ����� x λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		y		�軭�����ڤˤ����� y λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		flags	�ե饰(TVP_SS_*�����νM�ߺϤ碌)
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags) = 0;

	//! @brief		(Window��DrawDevice) �쥤��Υޥ�������ץ�����Ť���
	//! @note		�쥤��Υޥ�������ץ�����Ť��٤����Ϥ˥�����ɥ�������Ф�롣
	//! @note		WindowReleaseCapture() �Ȼ�ͬ���ʤ����ȡ�
	virtual void TJS_INTF_METHOD OnReleaseCapture() = 0;

	//! @brief		(Window��DrawDevice) �ޥ������軭��������ƄӤ���
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow() = 0;

	//! @brief		(Window��DrawDevice) ���`��Ѻ���줿
	//! @param		key		���륭�`���`��
	//! @param		shift	���եȥ��`��״�B
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		(Window��DrawDevice) ���`���x���줿
	//! @param		key		���륭�`���`��
	//! @param		shift	���եȥ��`��״�B
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift) = 0;

	//! @brief		(Window��DrawDevice) ���`�ˤ������
	//! @param		key		���֥��`��
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key) = 0;

	//! @brief		(Window��DrawDevice) �ޥ����ۥ��`�뤬��ܞ����
	//! @param		shift	���եȥ��`��״�B
	//! @param		delta	��ܞ��
	//! @param		x		�軭�����ڤˤ����� x λ��(�軭���Τ����Ϥ�ԭ��)
	//! @param		y		�軭�����ڤˤ����� y λ��(�軭���Τ����Ϥ�ԭ��)
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y) = 0;

	//! @brief		(Window->DrawDevice) ����״�B�Υ����å�
	//! @note		������ɥ�����s1�뤪���ˡ��쥤��ޥͩ`���㤬��`�������������״�B��
	//!				�٥����å����뤿��˺��Ф�롣�쥤��״�B�Ή仯����`���������Ȥ�
	//!				��ͬ�ڤ��Ф�줿���ϡ����Ȥ��Хޥ������`������¤˥쥤�䤬���F����
	//!				�Τˤ⤫����餺���ޥ������`���뤬���Υ쥤���ָ��������״�ˉ������ʤ�
	//!				�Ȥ��ä�״�r���k�������롣���Τ褦��״�r�ˌ��I���뤿�ᡢ������ɥ�����
	//!				���Υ᥽�åɤ��s1�뤪���˺��Ф�롣
	virtual void TJS_INTF_METHOD RecheckInputState() = 0;

	//! @brief		(LayerManager��DrawDevice) �ޥ������`�������״��ǥե���Ȥˑ���
	//! @param		manager		�쥤��ޥͩ`����
	//! @note		�ޥ������`�������״��ǥե���Ȥ���ˑ����������Ϥ˺��Ф��
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) �ޥ������`�������״���O������
	//! @param		manager		�쥤��ޥͩ`����
	//! @param		cursor		�ޥ������`������״����
	virtual void TJS_INTF_METHOD SetMouseCursor(iTVPLayerManager * manager, tjs_int cursor) = 0;

	//! @brief		(LayerManager��DrawDevice) �ޥ������`�����λ�ä�ȡ�ä���
	//! @param		manager		�쥤��ޥͩ`����
	//! @param		x			�ץ饤�ޥ�쥤���Ϥ����ˤˤ�����ޥ������`�����xλ��
	//! @param		y			�ץ饤�ޥ�쥤���Ϥ����ˤˤ�����ޥ������`�����yλ��
	//! @note		���ˤϥץ饤�ޥ�쥤���Ϥ����ˤʤΤǡ���Ҫ�ʤ�Љ�Q���Ф�
	virtual void TJS_INTF_METHOD GetCursorPos(iTVPLayerManager * manager, tjs_int &x, tjs_int &y) = 0;

	//! @brief		(LayerManager��DrawDevice) �ޥ������`�����λ�ä��O������
	//! @param		manager		�쥤��ޥͩ`����
	//! @param		x			�ץ饤�ޥ�쥤���Ϥ����ˤˤ�����ޥ������`�����xλ��
	//! @param		y			�ץ饤�ޥ�쥤���Ϥ����ˤˤ�����ޥ������`�����yλ��
	//! @note		���ˤϥץ饤�ޥ�쥤���Ϥ����ˤʤΤǡ���Ҫ�ʤ�Љ�Q���Ф�
	virtual void TJS_INTF_METHOD SetCursorPos(iTVPLayerManager * manager, tjs_int x, tjs_int y) = 0;

	//! @brief		(LayerManager��DrawDevice) ������ɥ��Υޥ�������ץ�����Ť���
	//! @param		manager		�쥤��ޥͩ`����
	//! @note		������ɥ��Υޥ�������ץ�����Ť��٤����Ϥ˥쥤��ޥͩ`���㤫����Ф�롣
	//! @note		������ɥ��Υޥ�������ץ���� OnReleaseCapture() ���_�ŤǤ���쥤��Υޥ�������ץ���
	//!				�Ȯ��ʤ뤳�Ȥ�ע�⡣������ɥ��Υޥ�������ץ��������OS�Υ�����ɥ������ƥ��
	//!				�C�ܤǤ��뤬���쥤��Υޥ�������ץ���ϼ��Ｊ�郎�쥤��ޥͩ`���㤴�Ȥ�
	//!				���Ԥ˹����Ƥ�����Ǥ��롣���Υ᥽�åɤǤϻ����Ĥˤ� ::ReleaseCapture() �ʤɤ�
	//!				�ޥ����Υ���ץ�����_�Ť��롣
	virtual void TJS_INTF_METHOD WindowReleaseCapture(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) �ĩ`����åץҥ�Ȥ��O������
	//! @param		manager		�쥤��ޥͩ`����
	//! @param		text		�ҥ�ȥƥ�����(�������ФΈ��Ϥϥҥ�Ȥα�ʾ�򥭥�󥻥뤹��)
	virtual void TJS_INTF_METHOD SetHintText(iTVPLayerManager * manager, const ttstr & text) = 0;

	//! @brief		(LayerManager��DrawDevice) עҕ�ݥ���Ȥ��O��
	//! @param		manager		�쥤��ޥͩ`����
	//! @param		layer		�ե�������κ��ޤ��쥤��
	//! @param		x			�ץ饤�ޥ�쥤���Ϥ����ˤˤ�����עҕ�ݥ���Ȥ�xλ��
	//! @param		y			�ץ饤�ޥ�쥤���Ϥ����ˤˤ�����עҕ�ݥ���Ȥ�yλ��
	//! @note		עҕ�ݥ���Ȥ�ͨ�������å�λ�äΤ��Ȥǡ�������IME�Υ���ݥ��åȥ�����ɥ���
	//!				��ʾ���줿�ꡢ��`���a���Β����R�������򒈴󤷤��ꤹ�롣IME������ݥ��åȥ�����ɥ���
	//!				��ʾ�����ꡢδ�_�������֤򤽤��˱�ʾ�����ꤹ���H�Υե���Ȥ� layer �ѥ��`��
	//!				��ʾ�����쥤�䤬�֤����ˤ�뤬���ץ饰���󤫤餽������ä����O��������
	//!				���륤�󥿩`�ե��`���Ͻ�ΤȤ���ʤ���
	//! @note		���ˤϥץ饤�ޥ�쥤���Ϥ����ˤʤΤǡ���Ҫ�ʤ�Љ�Q���Ф���
	virtual void TJS_INTF_METHOD SetAttentionPoint(iTVPLayerManager * manager, tTJSNI_BaseLayer *layer,
							tjs_int l, tjs_int t) = 0;

	//! @brief		(LayerManager��DrawDevice) עҕ�ݥ���Ȥν��
	//! @param		manager		�쥤��ޥͩ`����
	virtual void TJS_INTF_METHOD DisableAttentionPoint(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager��DrawDevice) IME��`�ɤ��O��
	//! @param		manager		�쥤��ޥͩ`����
	//! @param		mode		IME��`��
	virtual void TJS_INTF_METHOD SetImeMode(iTVPLayerManager * manager, tTVPImeMode mode) = 0;

	//! @brief		(LayerManager��DrawDevice) IME��`�ɤΥꥻ�å�
	//! @param		manager		�쥤��ޥͩ`����
	virtual void TJS_INTF_METHOD ResetImeMode(iTVPLayerManager * manager) = 0;

//---- �ץ饤�ޥ�쥤���v�B
	//! @brief		(Window��DrawDevice) �ץ饤�ޥ�쥤���ȡ��
	//! @return		�ץ饤�ޥ�쥤��
	//! @note		Window.primaryLayer ���i�߳����줿�H�ˤ��Υ᥽�åɤ����Ф�롣
	//!				��������˺��Ф�뤳�ȤϤʤ���
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer() = 0;

	//! @brief		(Window��DrawDevice) �ե��`�����Τ���쥤���ȡ��
	//! @return		�ե��`�����Τ���쥤��(NULL=�ե��`�����Τ���쥤�䤬�ʤ�����)
	//! @note		Window.focusedLayer ���i�߳����줿�H�ˤ��Υ᥽�åɤ����Ф�롣
	//!				��������˺��Ф�뤳�ȤϤʤ���
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer() = 0;

	//! @brief		(Window��DrawDevice) �ե��`�����Τ���쥤����O��
	//! @param		layer		�ե��`�����Τ���쥤��(NULL=�ե��`�����Τ���쥤�䤬�ʤ�״�B�ˤ���������)
	//! @note		Window.focusedLayer �������z�ޤ줿�H�ˤ��Υ᥽�åɤ����Ф�롣
	//!				��������˺��Ф�뤳�ȤϤʤ���
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer) = 0;


//---- ���軭�v�B
	//! @brief		(Window��DrawDevice) �軭���ΤΟo������֪ͨ
	//! @param		rect		�軭�����ڤ����ˤˤ����롢�o���ˤʤä��I��
	//!							(�軭���Τ����Ϥ�ԭ��)
	//! @note		�軭���Τ�һ�����뤤��ȫ�����o���ˤʤä��H�˥�����ɥ�����֪ͨ����롣
	//!				�軭�ǥХ����ϡ��ʤ�٤��礤�r�ڤ˟o���ˤʤä����֤����軭���٤��Ǥ��롣
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect) = 0;

	//! @brief		(Window��DrawDevice) ���¤�Ҫ��
	//! @note		�軭���Τ����ݤ����¤�״�B�˸��¤��٤������ߥ󥰤ǡ�������ɥ�������Ф�롣
	//!				iTVPWindow::RequestUpdate() �������ᡢ�����ƥब�軭�����ߥ󥰤���ä��H��
	//!				���Ф�롣ͨ�����軭�ǥХ����Ϥ��Υ����ߥ󥰤����ä��ƥ��ե�����`��
	//!				���`�ե��`���˻�����軭���롣
	virtual void TJS_INTF_METHOD Update() = 0;

	//! @brief		(Window->DrawDevice) ����α�ʾ
	//! @note		���ե�����`�󥵩`�ե��`�����軭���줿����򡢥��󥹥���`��˱�ʾ����
	//!				(���뤤�ϥե�åפ���) �����ߥ󥰤Ǻ��Ф�롣ͨ���� Update ��ֱ���
	//!				���Ф�뤬��VSync �������Є��ˤʤäƤ�����Ϥ� Update ֱ��ǤϤʤ���
	//!				VBlank �Ф˺��Ф������Ԥ����롣���ե�����`�󥵩`�ե��`����
	//!				ʹ��ʤ����Ϥϟoҕ���Ƥ��ޤ�ʤ���
	virtual void TJS_INTF_METHOD Show() = 0;

//---- LayerManager ����λ����ܤ��ɤ��v�B
	//! @brief		(LayerManager->DrawDevice) �ӥåȥޥåפ��軭���_ʼ����
	//! @param		manager		�軭���_ʼ����쥤��ޥͩ`����
	//! @note		�쥤��ޥͩ`���㤫���軭�ǥХ����ػ���ܞ�ͤ����ǰ�˺��Ф�롣
	//!				���Τ��ȡ�NotifyBitmapCompleted() ������λ������Ф졢�����
	//!				EndBitmapCompletion() �����Ф�롣
	//!				��Ҫ�ʤ�С����Υ����ߥ󥰤��軭�ǥХ����Ȥǥ��`�ե��`���Υ�å��ʤɤ�
	//!				�Ф����ȡ�
	virtual void TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager) = 0;

	//! @brief		(LayerManager->DrawDevice) �ӥåȥޥåפ��軭��֪ͨ����
	//! @param		manager		������ṩԪ�Υ쥤��ޥͩ`����
	//! @param		x			�ץ饤�ޥ�쥤���Ϥ����ˤˤ����뻭������λ��
	//! @param		y			�ץ饤�ޥ�쥤���Ϥ����ˤˤ����뻭����϶�λ��
	//! @param		bits		�ӥåȥޥåץǩ`��
	//! @param		bitmapinfo	�ӥåȥޥåפ���ʽ���
	//! @param		cliprect	bits �Τ������ɤβ��֤�ʹ�ä��������������
	//! @param		type		�ṩ����뻭���붨����ϳɥ�`��
	//! @param		opacity		�ṩ����뻭���붨���벻͸����(0255)
	//! @note		�쥤��ޥͩ`���㤬�ϳɤ����ˤ����Y�����軭�ǥХ������軭���Ƥ�餤�����H��
	//!				���Ф�롣һ�Ĥθ��¤��}���ξ��Τǘ��ɤ������Ϥ����뤿�ᡢ���Υ᥽�åɤ�
	//!				StartBitmapCompletion() �� EndBitmapCompletion() ���g���}���غ��Ф������Ԥ����롣
	//!				�����Ĥˤϡ�bits �� bitmapinfo �Ǳ����ӥåȥޥåפΤ�����cliprect ��
	//!				ʾ�������Τ� x, y λ�ä�ܞ�ͤ���Ф褤�����軭���Τδ󤭤��˺Ϥ碌��
	//!				�����sС�ʤɤ��軭�ǥХ����Ȥ��浹��Ҋ���Ҫ�����롣
	virtual void TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
		tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
		const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity) = 0;

	//! @brief		(LayerManager->DrawDevice) �ӥåȥޥåפ��軭��K�ˤ���
	//! @param		manager		�軭��K�ˤ���쥤��ޥͩ`����
	virtual void TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager) = 0;

//---- �ǥХå�֧Ԯ
	//! @brief		(Window->DrawDevice) �쥤�䘋��򥳥󥽩`��˥���פ���
	virtual void TJS_INTF_METHOD DumpLayerStructure() = 0;

	//! @brief		(Window->DrawDevice) ���¾��Τα�ʾ���Ф����ɤ������O������
	//! @param		b		��ʾ���Ф����ɤ���
	//! @note		�쥤���ʾ�C������ָ��¤��Ф��H�ξ��Τ��ʾ����
	//!				��ָ��¤����m���������Ƥ뤿���֧Ԯ�C�ܡ�
	//!				�gװ�����Ҫ�Ϥʤ������gװ���뤳�Ȥ����ޤ�����
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b) = 0;
};
//---------------------------------------------------------------------------
/*]*/

//---------------------------------------------------------------------------
//! @brief		�軭�ǥХ������󥿩`�ե��`���λ����Ĥʌgװ
//---------------------------------------------------------------------------
class tTVPDrawDevice : public iTVPDrawDevice
{
protected:
	iTVPWindow * Window;
	size_t PrimaryLayerManagerIndex; //!< �ץ饤�ޥ�쥤��ޥͩ`����
	std::vector<iTVPLayerManager *> Managers; //!< �쥤��ޥͩ`���������
	tTVPRect DestRect; //!< �軭��λ��

protected:
	tTVPDrawDevice(); //!< ���󥹥ȥ饯��
protected:
	virtual ~tTVPDrawDevice(); //!< �ǥ��ȥ饯��

public:
	//! @brief		ָ��λ�äˤ���쥤��ޥͩ`�����ä�
	//! @param		index		����ǥå���(0)
	//! @return		ָ��λ�äˤ���쥤��ޥͩ`����(AddRef����ʤ��Τ�ע��)��
	//!				ָ��λ�ä˥쥤��ޥͩ`���㤬�ʤ����NULL������
	iTVPLayerManager * GetLayerManagerAt(size_t index)
	{
		if(Managers.size() <= index) return NULL;
		return Managers[index];
	}

	//! @brief		Device��LayerManager��������ˤΉ�Q���Ф�
	//! @param		x		Xλ��
	//! @param		y		Yλ��
	//! @return		��Q�˳ɹ�������档����ʤ���ЂΡ�PrimaryLayerManagerIndex��ԓ������
	//!				�쥤��ޥͩ`���㤬�ʤ���ЂΤ�����
	//! @note		x, y �� DestRect�� (0,0) ��ԭ��Ȥ������ˤȤ��ƶɤ�����Ҋ�ʤ�
	bool TransformToPrimaryLayerManager(tjs_int &x, tjs_int &y);

	//! @brief		LayerManager��Device��������ˤΉ�Q���Ф�
	//! @param		x		Xλ��
	//! @param		y		Yλ��
	//! @return		��Q�˳ɹ�������档����ʤ���ЂΡ�PrimaryLayerManagerIndex��ԓ������
	//!				�쥤��ޥͩ`���㤬�ʤ���ЂΤ�����
	//! @note		x, y �� �쥤��� (0,0) ��ԭ��Ȥ������ˤȤ��ƶɤ�����Ҋ�ʤ�
	bool TransformFromPrimaryLayerManager(tjs_int &x, tjs_int &y);

//---- ���֥��������������g����
	virtual void TJS_INTF_METHOD Destruct();

//---- window interface �v�B
	virtual void TJS_INTF_METHOD SetWindowInterface(iTVPWindow * window);

//---- LayerManager �ι����v�B
	virtual void TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD RemoveLayerManager(iTVPLayerManager * manager);

//---- �軭λ�å������v�B
	virtual void TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD GetSrcSize(tjs_int &w, tjs_int &h);
	virtual void TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD NotifyLayerImageChange(iTVPLayerManager * manager);

//---- ��`���`���󥿩`�ե��`���v�B
	// window �� drawdevice
	virtual void TJS_INTF_METHOD OnClick(tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD OnDoubleClick(tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD OnMouseDown(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseUp(tjs_int x, tjs_int y, tTVPMouseButton mb, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnMouseMove(tjs_int x, tjs_int y, tjs_uint32 flags);
	virtual void TJS_INTF_METHOD OnReleaseCapture();
	virtual void TJS_INTF_METHOD OnMouseOutOfWindow();
	virtual void TJS_INTF_METHOD OnKeyDown(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyUp(tjs_uint key, tjs_uint32 shift);
	virtual void TJS_INTF_METHOD OnKeyPress(tjs_char key);
	virtual void TJS_INTF_METHOD OnMouseWheel(tjs_uint32 shift, tjs_int delta, tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD RecheckInputState();

	// layer manager �� drawdevice
	virtual void TJS_INTF_METHOD SetDefaultMouseCursor(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD SetMouseCursor(iTVPLayerManager * manager, tjs_int cursor);
	virtual void TJS_INTF_METHOD GetCursorPos(iTVPLayerManager * manager, tjs_int &x, tjs_int &y);
	virtual void TJS_INTF_METHOD SetCursorPos(iTVPLayerManager * manager, tjs_int x, tjs_int y);
	virtual void TJS_INTF_METHOD SetHintText(iTVPLayerManager * manager, const ttstr & text);
	virtual void TJS_INTF_METHOD WindowReleaseCapture(iTVPLayerManager * manager);

	virtual void TJS_INTF_METHOD SetAttentionPoint(iTVPLayerManager * manager, tTJSNI_BaseLayer *layer,
							tjs_int l, tjs_int t);
	virtual void TJS_INTF_METHOD DisableAttentionPoint(iTVPLayerManager * manager);
	virtual void TJS_INTF_METHOD SetImeMode(iTVPLayerManager * manager, tTVPImeMode mode);
	virtual void TJS_INTF_METHOD ResetImeMode(iTVPLayerManager * manager);

//---- �ץ饤�ޥ�쥤���v�B
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetPrimaryLayer();
	virtual tTJSNI_BaseLayer * TJS_INTF_METHOD GetFocusedLayer();
	virtual void TJS_INTF_METHOD SetFocusedLayer(tTJSNI_BaseLayer * layer);

//---- ���軭�v�B
	virtual void TJS_INTF_METHOD RequestInvalidation(const tTVPRect & rect);
	virtual void TJS_INTF_METHOD Update();
	virtual void TJS_INTF_METHOD Show() = 0;

//---- �ǥХå�֧Ԯ
	virtual void TJS_INTF_METHOD DumpLayerStructure();
	virtual void TJS_INTF_METHOD SetShowUpdateRect(bool b);

// �ۤ��Υ᥽�åɤˤĤ��Ƥόgװ���ʤ�
};
//---------------------------------------------------------------------------
#endif

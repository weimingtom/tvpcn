#ifndef __TJSINSTANCE_H__
#define __TJSINSTANCE_H__

#include "tjsbase.h"

/**
 * �g���g���C���X�^���X��ێ�����JavaScript�p�N���X���
 * �����o�����v���g�^�C�v�ɓo�^���܂�
 */
class TJSInstance : public TJSBase, iTJSNativeInstance {

public:
	// �������p
	static void init(Handle<ObjectTemplate> globalTemplate);

	/**
	 * �g���g���I�u�W�F�N�g�� Javascrip �I�u�W�F�N�g�ɕϊ�
	 * @param result �i�[��
	 * @param variant �ϊ���
	 * @return �ϊ����������� true
	 */
	static bool getJSObject(Local<Object> &result, const tTJSVariant &variant);

	// ���\�b�h�Ăяo���p
	static tjs_error getProp(Local<Object> obj, const tjs_char *membername, tTJSVariant *result);
	static tjs_error setProp(Local<Object> obj, const tjs_char *membername, const tTJSVariant *param);
	static tjs_error remove(Local<Object> obj, const tjs_char *membername);
	static tjs_error createMethod(Local<Object> obj, const tjs_char *membername, iTJSDispatch2 **result, tjs_int numparams, tTJSVariant **param);
	static tjs_error callMethod(Local<Object> obj, const tjs_char *membername, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
	
	// ---------------------------------------------------------------

	/**
	 * call �����p�̌�
	 * TJS�C���X�^���X����Javascript�C���X�^���X�̃��\�b�h�𒼐ڌĂяo��
	 */
	static tjs_error call(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
						  tTJSVariant *result,
						  tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
	
	/**
	 * missing �����p�̌�
	 * TJS�C���X�^���X�Ƀ����o�����݂��Ȃ������ꍇ�� Javascript�C���X�^���X���Q�Ƃ���
	 */
	static tjs_error missing(tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
							 tTJSVariant *result,
							 tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	// ---------------------------------------------------------------
	
	/**
	 * TJS�I�u�W�F�N�g�p�̃��\�b�h
	 * @param args ����
	 * @return ����
	 */
	static Handle<Value> tjsInvoker(const Arguments& args);

	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Q�b�^�[
	 */
	static Handle<Value> tjsGetter(Local<String> property, const AccessorInfo& info);
	
	/**
	 * TJS�I�u�W�F�N�g�p�̃v���p�e�B�Z�b�^�[
	 */
	static void tjsSetter(Local<String> property, Local<Value> value, const AccessorInfo& info);

	/**
	 * �R���X�g���N�^
	 * @param obj ���ȃI�u�W�F�N�g
	 * @param instance �o�C���h�Ώۂ�TJS�I�u�W�F�N�g
	 */
	TJSInstance(Handle<Object> obj, const tTJSVariant &instance);
	
private:

	/**
	 * �g���g���N���X���� Javascript �N���X�𐶐�
	 * @param args ����
	 * @return ����
	 */
	static Handle<Value> createTJSClass(const Arguments& args);

	/**
	 * �j������
	 */
	void invalidate();
	
	/**
	 * �I�u�W�F�N�g�̃f�B�X�|�[�U
	 */
	static void release(Persistent<Value> handle, void* parameter);

	/**
	 * TJS�I�u�W�F�N�g�̗L���m�F
	 * @param args ����
	 * @return ����
	 */
	static Handle<Value> tjsIsValid(const Arguments& args);

	/**
	 * TJS�I�u�W�F�N�g�ɑ΂���I�[�o���C�h����
	 * @param args ����
	 * @return ����
	 */
	static Handle<Value> tjsOverride(const Arguments& args);
	
	/**
	 * TJS�I�u�W�F�N�g�̃R���X�g���N�^
	 * @param args ����
	 * @return ����
	 */
	static Handle<Value> tjsConstructor(const Arguments& args);

	// NativeClass ID
	static int classId;
	
	// ���ȎQ�Ɨp
	Persistent<Object> self;

public:
	// NativeInstance �Ή��p�����o
	virtual tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	virtual void TJS_INTF_METHOD Invalidate();
	virtual void TJS_INTF_METHOD Destruct();
};

#endif

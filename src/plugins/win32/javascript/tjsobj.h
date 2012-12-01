#ifndef __TJSOBJ_H__
#define __TJSOBJ_H__

#include "tjsbase.h"

/**
 * �g���g���p�I�u�W�F�N�g��P���ێ�����JavaScript�p�N���X���
 */
class TJSObject : public TJSBase {

public:
	// �������p
	static void init();
	static void done();

	// �I�u�W�F�N�g����
	static Local<Object> toJSObject(const tTJSVariant &variant);

private:
	// �I�u�W�F�N�g��`
	static Persistent<ObjectTemplate> objectTemplate;

	// ����p
	static void release(Persistent<Value> object, void *parameter);

	// �A�N�Z�X�p���\�b�h
	static Handle<Value> getter(Local<String> property, const AccessorInfo& info);
	static Handle<Value> setter(Local<String> property, Local<Value> value, const AccessorInfo& info);
	static Handle<Value> caller(const Arguments& args);

	// �i�[���R���X�g���N�^
	TJSObject(Handle<Object> obj, const tTJSVariant &variant);
};

#endif
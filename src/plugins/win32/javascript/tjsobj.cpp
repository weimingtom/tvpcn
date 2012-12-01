#include "tjsobj.h"
extern Local<Value> toJSValue(const tTJSVariant &variant);
extern tTJSVariant toVariant(Handle<Value> value);

/**
 * Javascript�ɑ΂��ăG���[�ʒm
 */
Handle<Value>
ERROR_KRKR(tjs_error error)
{
	switch (error) {
	case TJS_E_MEMBERNOTFOUND:
		return ThrowException(String::New("member not found"));
	case TJS_E_NOTIMPL:
		return ThrowException(String::New("not implemented"));
	case TJS_E_INVALIDPARAM:
		return ThrowException(String::New("invalid param"));
	case TJS_E_BADPARAMCOUNT:
		return ThrowException(String::New("bad param count"));
	case TJS_E_INVALIDTYPE:
		return ThrowException(String::New("invalid type"));
	case TJS_E_INVALIDOBJECT:
		return ThrowException(String::New("invalid object"));
	case TJS_E_ACCESSDENYED:
		return ThrowException(String::New("access denyed"));
	case TJS_E_NATIVECLASSCRASH:
		return ThrowException(String::New("navive class crash"));
	default:
		return ThrowException(String::New("failed"));
	}
}

Handle<Value>
ERROR_BADINSTANCE()
{
	return ThrowException(String::New("bad instance"));
}

//----------------------------------------------------------------------------
// tTJSVariant���I�u�W�F�N�g�Ƃ��ĕێ����邽�߂̋@�\
//----------------------------------------------------------------------------

Persistent<ObjectTemplate> TJSObject::objectTemplate;

// �I�u�W�F�N�g��`������
void
TJSObject::init()
{
	objectTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
	objectTemplate->SetNamedPropertyHandler(getter, setter);
	objectTemplate->SetCallAsFunctionHandler(caller);
}

// �I�u�W�F�N�g��`���
void
TJSObject::done()
{
	objectTemplate.Dispose();
}

// �R���X�g���N�^
TJSObject::TJSObject(Handle<Object> obj, const tTJSVariant &variant) : TJSBase(variant)
{
	wrap(obj);
	Persistent<Object> ref = Persistent<Object>::New(obj);
	ref.MakeWeak(this, release);
}

// �p�����[�^���
void
TJSObject::release(Persistent<Value> object, void *parameter)
{
	TJSObject *self = (TJSObject*)parameter;
	if (self) {
		delete self;
	}
}

// �v���p�e�B�̎擾
Handle<Value>
TJSObject::getter(Local<String> property, const AccessorInfo& info)
{
	String::Value propName(property);
	if (wcscmp(*propName, TJSINSTANCENAME) == 0) {
		return Handle<Value>();
	}
	tTJSVariant self;
	if (getVariant(self, info.This())) {
		tjs_error error;
		tTJSVariant result;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropGet(0, *propName, NULL, &result, NULL))) {
			return toJSValue(result);
		} else {
			return ERROR_KRKR(error);
		}
	}
	return ERROR_BADINSTANCE();
}

// �v���p�e�B�̐ݒ�
Handle<Value>
TJSObject::setter(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
	tTJSVariant self;
	if (getVariant(self, info.This())) {
		String::Value propName(property);
		tTJSVariant param = toVariant(value);
		tjs_error error;
		if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().PropSet(TJS_MEMBERENSURE, *propName, NULL, &param, NULL))) {
			return Undefined();
		} else {
			return ERROR_KRKR(error);
		}
	}
	return ERROR_BADINSTANCE();
}

// ���\�b�h�̌Ăяo��
Handle<Value>
TJSObject::caller(const Arguments& args)
{
	tTJSVariant self;
	if (getVariant(self, args.This())) {
		Handle<Value> ret;
		
		// �����ϊ�
		tjs_int argc = args.Length();
		tTJSVariant **argv = new tTJSVariant*[argc];
		for (tjs_int i=0;i<argc;i++) {
			argv[i] = new tTJSVariant();
			*argv[i] = toVariant(args[i]);
		}

		if (self.AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Class", NULL) == TJS_S_TRUE) {
			// �N���X�I�u�W�F�N�g�Ȃ�R���X�g���N�^�Ăяo��
			iTJSDispatch2 *instance = NULL;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().CreateNew(0, NULL, NULL, &instance, argc, argv, NULL))) {
				ret = toJSValue(tTJSVariant(instance, instance));
				instance->Release();
			} else {
				ret = ERROR_KRKR(error);
			}
		} else {
			// ���\�b�h�Ăяo��
			tTJSVariant result;
			tjs_error error;
			if (TJS_SUCCEEDED(error = self.AsObjectClosureNoAddRef().FuncCall(0, NULL, NULL, &result, argc, argv, NULL))) {
				ret = toJSValue(result);
			} else {
				ret = ERROR_KRKR(error);
			}
		}

		// �������
		if (argv) {
			for (int i=0;i<argc;i++) {
				delete argv[i];
			}
			delete[] argv;
		}
		
		return ret;
	}
	return ERROR_BADINSTANCE();
}

// tTJSVariant ���I�u�W�F�N�g��
Local<Object>
TJSObject::toJSObject(const tTJSVariant &variant)
{
	Local<Object> obj = objectTemplate->NewInstance();
	new TJSObject(obj, variant);
	return obj;
}

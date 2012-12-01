/**
 * Javascript ���� �g���g���u���b�W����
 * �g���g���̃I�u�W�F�N�g�� XXXX �Ƃ��ĊǗ�����
 */

#include "tjsobj.h"
#include "tjsinstance.h"

// �l�̊i�[�E�擾�p
Local<Value> toJSValue(const tTJSVariant &variant);
tTJSVariant toVariant(Handle<Value> value);

#define JSOBJECTCLASS L"JavascriptObject"

/**
 * Javascript object �p iTJSDispatch2 ���b�p�[
 */
class iTJSDispatch2Wrapper : public tTJSDispatch
{
public:
	/**
	 * �R���X�g���N�^
	 * @param obj IDispatch
	 */
	iTJSDispatch2Wrapper(Handle<Object> obj) {
		this->obj = Persistent<Object>::New(obj);
	}
	
	/**
	 * �f�X�g���N�^
	 */
	~iTJSDispatch2Wrapper() {
		obj.Dispose();
	}

	/**
	 * �ێ����Ă�l��Ԃ�
	 */
	Local<Value> getValue() {
		return *obj;
	}

public:
	// �I�u�W�F�N�g����
	tjs_error TJS_INTF_METHOD CreateNew(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		iTJSDispatch2 **result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis) {
		return TJSInstance::createMethod(obj->ToObject(), membername, result, numparams, param);
	}

	// ���\�b�h�Ăяo��
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis
		) {
		return TJSInstance::callMethod(obj->ToObject(), membername, result, numparams, param, objthis);
	}

	// �v���p�e�B�擾
	tjs_error TJS_INTF_METHOD PropGet(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		tTJSVariant *result,
		iTJSDispatch2 *objthis) {
		if (!membername) {
			return TJS_E_NOTIMPL;
		}
		return TJSInstance::getProp(obj->ToObject(), membername, result);
	}

	// �v���p�e�B�ݒ�
	tjs_error TJS_INTF_METHOD PropSet(
		tjs_uint32 flag,
		const tjs_char *membername,
		tjs_uint32 *hint,
		const tTJSVariant *param,
		iTJSDispatch2 *objthis) {
		return TJSInstance::setProp(obj->ToObject(), membername, param);
	}

	// �����o�폜
	tjs_error TJS_INTF_METHOD DeleteMember(
		tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint,
		iTJSDispatch2 *objthis) {
		return TJSInstance::remove(obj->ToObject(), membername);
	}

	tjs_error TJS_INTF_METHOD IsInstanceOf(
		tjs_uint32 flag,
		const tjs_char * membername,
		tjs_uint32 *hint,
		const tjs_char * classname,
		iTJSDispatch2 *objthis
		) {
		if (membername == NULL && wcscmp(classname, JSOBJECTCLASS) == 0) {
			return TJS_S_TRUE;
		}
		return TJS_S_FALSE;
	}

protected:
	/// �����ێ��p
	Persistent<Object> obj;
};

//----------------------------------------------------------------------------
// �ϊ��p
//----------------------------------------------------------------------------

/**
 * tTJSVariant �� squirrel �̋�Ԃɓ�������
 * @param result javascrpt value
 * @param variant tTJSVariant
 */
Local<Value>
toJSValue(const tTJSVariant &variant)
{
	switch (variant.Type()) {
	case tvtVoid:
		return *Undefined();
	case tvtObject:
		{
			iTJSDispatch2 *obj = variant.AsObjectNoAddRef();
			if (obj == NULL) {
				// NULL�̏���
				return *Null();
			} else if (obj->IsInstanceOf(0, NULL, NULL, JSOBJECTCLASS, obj) == TJS_S_TRUE) {
				// Javascript ���b�s���O�I�u�W�F�N�g�̏ꍇ
				iTJSDispatch2Wrapper *wobj = (iTJSDispatch2Wrapper*)obj;
				return wobj->getValue();
			} else {
				Local<Object> result;
				if (TJSInstance::getJSObject(result, variant)) {
					// �o�^�ς݃C���X�^���X�̏ꍇ
					return result;
				}
				// �P�����b�s���O
				return TJSObject::toJSObject(variant);
			}
		}
		break;
	case tvtString:
		return String::New(variant.GetString(), -1);
	case tvtOctet:
		return *Null();
	case tvtInteger:
	case tvtReal:
		return Number::New((tTVReal)variant);
	}
	return *Undefined();
}

tTJSVariant
toVariant(Handle<Object> object, Handle<Object> context)
{
	tTJSVariant result;
	iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(object);
	iTJSDispatch2 *tjsctx = new iTJSDispatch2Wrapper(context);
	if (tjsobj && tjsctx) {
		result = tTJSVariant(tjsobj, tjsctx);
		tjsobj->Release();
		tjsctx->Release();
	} else {
		if (tjsobj) { tjsobj->Release(); };
		if (tjsctx) { tjsctx->Release(); };
	}
	return result;
}

tTJSVariant
toVariant(Handle<Object> object)
{
	tTJSVariant result;
	iTJSDispatch2 *tjsobj = new iTJSDispatch2Wrapper(object);
	if (tjsobj) {
		result = tTJSVariant(tjsobj, tjsobj);
		tjsobj->Release();
	}
	return result;
}

/**
 * javascript�l�� tTJSVariant �ɕϊ�����
 * @param value Javascript�l
 * @return tTJSVariant
 */
tTJSVariant
toVariant(Handle<Value> value)
{
	tTJSVariant result;
	if (value->IsNull()) {
		result = (iTJSDispatch2*)0;
	} else if (value->IsTrue()) {
		result = true;
	} else if (value->IsFalse()) {
		result = false;
	} else if (value->IsString()) {
		String::Value str(value);
		result = *str;
	} else if (value->IsFunction() || value->IsArray() || value->IsDate()) {
		// �P�����b�s���O
		result = toVariant(value->ToObject());
	} else if (value->IsObject()) {
		Local<Object> obj = value->ToObject();
		if (!TJSBase::getVariant(result, obj)) {
			// �P�����b�s���O
			result = toVariant(obj);
		}
	} else if (value->IsBoolean()) {
		result = value->BooleanValue();
	} else if (value->IsNumber()) {
		result = value->NumberValue();
	} else if (value->IsInt32()) {
		result = value->Int32Value();
	} else if (value->IsUint32()) {
		result = (tTVInteger)value->Uint32Value();
	}
	// value->IsUndefined()
	// value->IsExternal()
	return result;
}

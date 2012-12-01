#include <stdio.h>
#include "ncbind/ncbind.hpp"
#include <vector>
#include <v8.h>
#include <v8-debug.h>

using namespace v8;

// �g���g���I�u�W�F�N�g�N���X����
#include "tjsobj.h"
#include "tjsinstance.h"

// javascript ��ł� TJS2�̃O���[�o����Ԃ̎Q�Ɩ�
#define KIRIKIRI_GLOBAL L"krkr"
// TJS2 ��ł� javascript �̃O���[�o����Ԃ̎Q�Ɩ�
#define JAVASCRIPT_GLOBAL L"jsglobal"

// �l�̊i�[�E�擾�p
extern Local<Value> toJSValue(const tTJSVariant &variant);
extern tTJSVariant toVariant(Handle<Value> value);
extern void JSEXCEPTION(TryCatch *try_catch);

// �R�s�[���C�g�\�L
static const char *copyright =
#include "LICENSE.h"
;

// Javascript�O���[�o���R���e�L�X�g
Persistent<Context> mainContext;

//---------------------------------------------------------------------------

/**
 * Scripts �N���X�ւ� Javascript ���s���\�b�h�̒ǉ�
 */
class ScriptsJavascript {

public:
	ScriptsJavascript(){};

	/**
	 * javascript �X�N���v�g�̎��s
	 * @param script �X�N���v�g
	 * @param ... ����
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD _exec(const tjs_char *filename,
										   const tjs_char *scriptText,
										   tTJSVariant *result) {
		HandleScope handle_scope;
		Context::Scope context_scope(mainContext);
		TryCatch try_catch;

		Local<Script> script = Script::Compile(String::New(scriptText), filename ? String::New(filename) : Undefined());
		if (script.IsEmpty()) {
			// Print errors that happened during compilation.
			JSEXCEPTION(&try_catch);
		} else {
			Local<Value> ret = script->Run();
			if (ret.IsEmpty()) {
				JSEXCEPTION(&try_catch);
			} else {
				// ���ʂ��i�[
				if (result) {
					*result = toVariant(ret);
				}
			}
		}
		return TJS_S_OK;
	}

	
	/**
	 * javascript �X�N���v�g�̎��s
	 * @param script �X�N���v�g
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD exec(tTJSVariant *result,
										  tjs_int numparams,
										  tTJSVariant **param,
										  iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		return _exec(NULL, param[0]->GetString(), result);
	}

	/**
	 * javascript �X�N���v�g�̃t�@�C������̎��s
	 * @param filename �t�@�C����
	 * @param ... ����
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD execStorage(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		if (numparams <= 0) return TJS_E_BADPARAMCOUNT;
		const tjs_char *filename = param[0]->GetString();
		iTJSTextReadStream * stream = TVPCreateTextStreamForRead(filename, TJS_W(""));
		ttstr data;
		try {
			stream->Read(data, 0);
			stream->Destruct();
		}
		catch(...)
		{
			stream->Destruct();
			throw;
		}
		tjs_error ret = _exec(filename, data.c_str(), result);
		return ret;
	}

	/**
	 * �f�o�b�K�̗L����
	 * @param port �|�[�g�ԍ�(�f�t�H���g��5858)
	 * @return ���s����
	 */
	static tjs_error TJS_INTF_METHOD enableDebug(tTJSVariant *result,
												 tjs_int numparams,
												 tTJSVariant **param,
												 iTJSDispatch2 *objthis) {
		int  port  = numparams > 0 ? (int)*param[0] : 5858;
		bool wait  = numparams > 1 ? (int)*param[1] != 0 : false;
		Debug::EnableAgent("kirikiriV8", port, wait);
		return TJS_S_OK;
	}

	// �f�o�b�K�쓮�p
	static tjs_error TJS_INTF_METHOD processDebug(tTJSVariant *result,
												  tjs_int numparams,
												  tTJSVariant **param,
												  iTJSDispatch2 *objthis) {
		Context::Scope context_scope(mainContext);
		Debug::ProcessDebugMessages();
		return TJS_S_OK;
	}
};

NCB_ATTACH_CLASS(ScriptsJavascript, Scripts) {
	RawCallback("execJS",        &ScriptsJavascript::exec,          TJS_STATICMEMBER);
	RawCallback("execStorageJS", &ScriptsJavascript::execStorage,   TJS_STATICMEMBER);
	RawCallback("enableDebugJS", &ScriptsJavascript::enableDebug,   TJS_STATICMEMBER);
	RawCallback("processDebugJS", &ScriptsJavascript::processDebug, TJS_STATICMEMBER);
};

//---------------------------------------------------------------------------

/**
 * �o�^�����O
 */
static void PreRegistCallback()
{
	HandleScope handle_scope;

	// Copyright �\��
	TVPAddImportantLog(ttstr(copyright));

	// �O���[�o���e���v���[�g�̏���
	Local<ObjectTemplate> globalTemplate = ObjectTemplate::New();

	TJSInstance::init(globalTemplate);
	TJSObject::init();

	// �R���e�L�X�g����
	mainContext = Persistent<Context>::New(Context::New(NULL, globalTemplate));
	Context::Scope context_scope(mainContext);
	
	// �O���[�o���I�u�W�F�N�g�̏���
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		// �g���g���̃O���[�o���� Javascript �̃O���[�o����o�^����
		{
			tTJSVariant result = toVariant(mainContext->Global());
			global->PropSet(TJS_MEMBERENSURE, JAVASCRIPT_GLOBAL, NULL, &result, global);
		}
		// Javascript �� �O���[�o���ɋg���g���� �O���[�o����o�^����
		mainContext->Global()->Set(String::New(KIRIKIRI_GLOBAL), toJSValue(tTJSVariant(global, global)));
		global->Release();
	}
}

/**
 * �J��������
 */
static void PostUnregistCallback()
{
	mainContext.Dispose();
	TJSObject::done();
	V8::Dispose();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);

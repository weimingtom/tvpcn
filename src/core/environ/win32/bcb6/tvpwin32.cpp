//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TVP Win32 Project File
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "SysInitIntf.h"
#include "SysInitImpl.h"
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "ScriptMgnIntf.h"
#include "ScriptMgnImpl.h"
#include "tjsError.h"
#include "PluginImpl.h"
#include "SystemIntf.h"


//---------------------------------------------------------------------------
bool TVPCheckCmdDescription(void);
bool TVPCheckAbout(void);
void TVPOnError();
//---------------------------------------------------------------------------
USEFORM("..\HaltWarnFormUnit.cpp", TVPHaltWarnForm);
USEFORM("..\..\..\utils\win32\ConsoleFormUnit.cpp", TVPConsoleForm);
USEFORM("..\..\..\utils\win32\PadFormUnit.cpp", TVPPadForm);
USEFORM("..\MainFormUnit.cpp", TVPMainForm);
USEFORM("..\WatchFormUnit.cpp", TVPWatchForm);
USEFORM("..\..\..\visual\win32\WindowFormUnit.cpp", TVPWindowForm);
USEFORM("..\..\..\visual\win32\MenuContainerFormUnit.cpp", TVPMenuContainerForm);
USEFORM("..\..\..\visual\win32\FontSelectFormUnit.cpp", TVPFontSelectForm);
USEFORM("..\VersionFormUnit.cpp", TVPVersionForm);
USEFORM("..\..\..\..\tools\win32\krdevui\ConfMainFrameUnit.cpp", ConfMainFrame); /* TFrame: File Type */
//---------------------------------------------------------------------------
#ifdef TVP_SUPPORT_ERI
#	pragma link "../../../../Lib/liberina.lib"
#endif
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// try starting the program!
	bool engine_init = false;
	try
	{
		if(TVPCheckProcessLog()) return 0; // sub-process for processing object hash map log


		TVPInitScriptEngine();
		engine_init = true;

		// banner
		TVPAddImportantLog(TJS_W("Program started on ") + TVPGetOSName() +
			TJS_W(" (") + TVPGetPlatformName() + TJS_W(")"));

		// TVPInitializeBaseSystems
		TVPInitializeBaseSystems();

		Application->Initialize();

		if(TVPCheckPrintDataPath()) return 0;
		if(TVPCheckCmdDescription()) return 0;
		if(TVPExecuteUserConfig()) return 0; // userconf

		TVPSystemInit();

		if(TVPCheckAbout()) return 0; // version information dialog box;

		Application->Title = "���Ｊ��";
		Application->CreateForm(__classid(TTVPMainForm), &TVPMainForm);
		TVPLoadPluigins(); // load plugin module *.tpm
		if(TVPProjectDirSelected) TVPInitializeStartupScript();

		Application->Run();

		try
		{
			TVPSystemUninit();
		}
		catch(...)
		{
			// ignore errors
		}
	}
	catch (EAbort &e)
	{
		// nothing to do
	}
	catch (Exception &exception)
	{
		TVPOnError();
		if(!TVPSystemUninitCalled)
			Application->ShowException(&exception);
	}
	catch (eTJSScriptError &e)
	{
		TVPOnError();
		if(!TVPSystemUninitCalled)
			Application->ShowException(&Exception(e.GetMessage().AsAnsiString()));
	}
	catch (eTJS &e)
	{
		TVPOnError();
		if(!TVPSystemUninitCalled)
			Application->ShowException(&Exception(e.GetMessage().AsAnsiString()));
	}
	catch(...)
	{
		Application->ShowException(&Exception("Unknown error!"));
	}

	if(engine_init) TVPUninitScriptEngine();

#ifndef _DEBUG
	// delete application and exit forcely
	// this prevents ugly exception message on exit

	delete Application;
	ExitProcess(TVPTerminateCode);
#endif
	return TVPTerminateCode;
}
//---------------------------------------------------------------------------






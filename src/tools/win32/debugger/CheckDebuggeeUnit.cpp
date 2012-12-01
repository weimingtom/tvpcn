//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CheckDebuggeeUnit.h"
#include "MainUnit.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

// ע�⣺���ʤ륹��åɤ����Ф��� VCL �Υ᥽�å�/�v��/�ץ�ѥƥ���e��
// ��å����ФΥ��֥������Ȥˌ����Ƥ� Synchronize ��ʹ�äǤ��ޤ���
//
//      Synchronize(&UpdateCaption);
//
// ������ UpdateCaption �����¤Τ褦�˶��x��
//
//      void __fastcall DebuggeeCheckThread::UpdateCaption()
//      {
//        Form1->Caption = "����åɤ�������Q���ޤ���";
//      }
//---------------------------------------------------------------------------

__fastcall DebuggeeCheckThread::DebuggeeCheckThread(bool CreateSuspended)
	: TThread(CreateSuspended), is_first_break_(true), is_request_break_(false)
{
	OnTerminate = DebuggeeCheckThread::DebuggeeCheckThreadTerminate;
}
//---------------------------------------------------------------------------
void DebuggeeCheckThread::SetName()
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = "DebuggeeCheckThread";
	info.dwThreadID = -1;
	info.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD),(DWORD*)&info );
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::Execute()
{
	SetName();
	//---- Place thread code here ----

	command_.size_ = 0;
	command_.data_ = 0;
	debuggee_comm_area_addr_ = NULL;
	debuggee_comm_area_size_ = 0;
	debug_continue_status_ = DBG_CONTINUE;

	// ���ӌ���Υѥ��`�����餦
	Synchronize(&GetParameters);

	// �ǥХå�����	
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	BOOL result;
	if( work_folder_.Length() ) {
		result = ::CreateProcess( NULL,command_line_.c_str(),NULL,NULL,FALSE,
			NORMAL_PRIORITY_CLASS|DEBUG_ONLY_THIS_PROCESS,
			NULL,
			work_folder_.c_str(),
			&si,&proc_info_);
	} else {
		result = ::CreateProcess( NULL,command_line_.c_str(),NULL,NULL,FALSE,
			NORMAL_PRIORITY_CLASS|DEBUG_ONLY_THIS_PROCESS,
			NULL,NULL,&si,&proc_info_);
	}
	if( result == 0 ) {
		// error
		ShowLastError();
	} else {
		// �ץ����ǩ`�����O��
		Synchronize(&SetProcInfo);

#if 0
		// ���Ӵ���
		DWORD timeout = 50;	// 50ms�ϴ���
		while( Terminated == false ) {
			// wait for wakeup debuggee
			DWORD retwait = ::WaitForInputIdle( proc_info_.hProcess, timeout );
			if( retwait == -1 ) {
				result = 0;
				break;
			} else if( retwait == WAIT_TIMEOUT ) {
				// time out, retry
			} else {
				// ���Ӥ�֪ͨ
				Synchronize(&WakeupDebugee);
				break;
			}
		}
		if( result == 0 ) {
			ShowLastError();
			// �ץ������ƽK��
			::TerminateProcess(proc_info_.hProcess, 0);
		} else
#else
		Synchronize(&WakeupDebugee);
#endif
		{
			// �g����
			while( Terminated == false && result ) {
				DEBUG_EVENT deb_ev;
				DWORD timeout = 50;	// 50ms�ϴ���
				result = ::WaitForDebugEvent( &deb_ev, timeout );
				if( result ) {
					int breakev = HandleDebugEvent( deb_ev );
					if( breakev == 0 ) {
						break;
					} else if( breakev > 0 ) {
						// �ǥХå���A�Ф���
						::ContinueDebugEvent( proc_info_.dwProcessId, deb_ev.dwThreadId, debug_continue_status_ );
					} else if( breakev < 0 ) {
						// �֥쥤���k��
						bool is_break_called = false;
						while( Terminated == false ) {
							Synchronize(&GetCommand);
							if( command_.size_ > 0 && command_.data_ && debuggee_comm_area_addr_ && (int)command_.size_ <= debuggee_comm_area_size_ ) {
								DWORD dwWrite;
								BOOL retw = ::WriteProcessMemory( proc_info_.hProcess, debuggee_comm_area_addr_,
															command_.data_, command_.size_, &dwWrite );
								delete[] command_.data_;
								command_.data_ = NULL;
								if( retw == 0 || dwWrite != command_.size_ ) {
									// �����z��ʧ��
									ShowLastError();
									result = 0;
									command_.size_ = 0;
									break;
								}
								command_.size_ = 0;
								::ContinueDebugEvent( proc_info_.dwProcessId, deb_ev.dwThreadId, debug_continue_status_ );
								break;
							} else {
								if( is_break_called == false ) {
									Synchronize(&OnBreak);
									is_break_called = true;
								}
							}
							::Sleep(10);
						}
					}
				} else {
					// �����ॢ��������ǽK�ˤ������Ϥϡ����ƽK�ˤ��Ƥ��ޤ���
					DWORD lasterror = ::GetLastError();
					if( WAIT_TIMEOUT != lasterror && ERROR_SEM_TIMEOUT != lasterror ) {
						::TerminateProcess(proc_info_.hProcess, 0);
						result = 0;
						break;
					} else {
						result = 1;
					}
					// �֥�`��Ҫ�󤬤��뤫�ɤ��������å�
					Synchronize(&CheckBreakRequest);
					if( is_request_break_ ) {
						// �֥�`��Ҫ�󤬤���
						DWORD retsus = ::SuspendThread(proc_info_.hThread);
						if( retsus == (DWORD)-1 ) {
							// ����`�ǥ����ڥ�ɤǤ��ʤ�
							ShowLastError();
						} else {
							Synchronize(&GetCommand);
							if( command_.size_ > 0 && command_.data_ && debuggee_comm_area_addr_ && (int)command_.size_ <= debuggee_comm_area_size_ ) {
								DWORD dwWrite;
								BOOL retw = ::WriteProcessMemory( proc_info_.hProcess, debuggee_comm_area_addr_,
															command_.data_, command_.size_, &dwWrite );
								if( retw == 0 || dwWrite != command_.size_ ) {
									// �����z��ʧ��
									ShowLastError();
								}
								command_.size_ = 0;
							}
							if( command_.data_ ) delete[] command_.data_;
							command_.data_ = NULL;
							command_.size_ = 0;
						}
						if( retsus != (DWORD)-1 ) {
							// �ꥸ��`��
							retsus = ::ResumeThread(proc_info_.hThread);
							if( retsus == (DWORD)-1 ) {
								// �ꥸ��`��ʧ���������Ϥϡ����ƽK�ˤ��Ƥ��ޤ���
								::TerminateProcess(proc_info_.hProcess, 0);
								result = 0;
								break;
                            }
						}
					}
				}
			}
		}
	}
	if( result ) {
		::CloseHandle( proc_info_.hProcess );
		::CloseHandle( proc_info_.hThread );
    }
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::GetCommand()
{
	ScriptDebuggerForm->GetFirstCommand( command_ );
	debuggee_comm_area_addr_ = ScriptDebuggerForm->GetDebugeeAreaAddr();
	debuggee_comm_area_size_ = ScriptDebuggerForm->GetDebugeeAreaSize();
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::CheckBreakRequest()
{
	is_request_break_ = ScriptDebuggerForm->IsRequestBreak;
	if( is_request_break_ ) ScriptDebuggerForm->SetBreakCommand();
}
//---------------------------------------------------------------------------
//! @return : �I���@�A���뤫�ɤ���
//! @retval 0 : �K��
//! @retval > 0 : �@�A
//! @retval < 0 : �֥�`��
int __fastcall DebuggeeCheckThread::HandleDebugEvent( DEBUG_EVENT& debug )
{
	debug_continue_status_ = DBG_CONTINUE;
	switch(debug.dwDebugEventCode){
		case OUTPUT_DEBUG_STRING_EVENT:	// �ǥХå������Ф����Ť���
			return HandleDebugString( debug );
		case CREATE_PROCESS_DEBUG_EVENT:// �ץ��������ɤ���
//			debug.u.CreateProcessInfo Ԕ����δ����
			debug_string_ = AnsiString("�ץ��������ɤ���ޤ�����");
			Synchronize(&SetDebugString);
			break;
		case CREATE_THREAD_DEBUG_EVENT:	// ����åɤ����ɤ���
			debug_string_ = AnsiString("����å� ( 0x")
				+ AnsiString::IntToHex( debug.dwThreadId, 8 )
				+ AnsiString(" ) �����ɥ쥹 ")
				+ AnsiString::IntToHex( (int)debug.u.CreateThread.lpStartAddress, 8 )
				+ AnsiString(" ���_ʼ����ޤ�����");
			Synchronize(&SetDebugString);
			break;
		case EXIT_THREAD_DEBUG_EVENT:	// ����åɤ��K�ˤ���
			debug_string_ = AnsiString("����å� ( 0x")
				+ AnsiString::IntToHex( debug.dwThreadId, 8 )
				+ AnsiString(") �ϥ��`�� ")
				+ AnsiString::IntToHex( debug.u.ExitThread.dwExitCode, 8 )
				+ AnsiString(" �ǽK�ˤ��ޤ�����");
			Synchronize(&SetDebugString);
			break;
		case LOAD_DLL_DEBUG_EVENT:		// DLL���`�ɤ���
			return HandleDllLoad( debug );
		case UNLOAD_DLL_DEBUG_EVENT:	// DLL�򥢥��`�ɤ���
			return HandleDllUnload( debug );
		case EXCEPTION_DEBUG_EVENT:		// ���⤬�k������
			return HandleDebugException( debug );
		case RIP_EVENT:					// RIP���٥��
			break;
		case EXIT_PROCESS_DEBUG_EVENT:	// �ץ������K�ˤ���
			debug_string_ = AnsiString("�ץ����ϥ��`�� 0x")
				+ AnsiString::IntToHex( debug.u.ExitProcess.dwExitCode, 8 )
				+ AnsiString(" �ǽK�ˤ��ޤ�����");
			Synchronize(&SetDebugString);
			return 0;
	}
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall DebuggeeCheckThread::HandleDebugException( DEBUG_EVENT& debug )
{
	debug_continue_status_ = DBG_EXCEPTION_NOT_HANDLED;

	AnsiString theadStr( AnsiString( "����å� ( " ) + AnsiString::IntToHex( debug.dwThreadId, 8 ) + AnsiString(" ) ") );
	AnsiString epiStr( AnsiString( " ( EPI = 0x" ) + AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionAddress, 8 ) + AnsiString(" ) ") );

	switch( debug.u.Exception.ExceptionRecord.ExceptionCode ) {
		case EXCEPTION_ACCESS_VIOLATION:
			if( debug.u.Exception.ExceptionRecord.NumberParameters >= 2 ) {
				debug_string_ = theadStr + AnsiString("�ǡ�0x");
				debug_string_ += AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionInformation[1], 8 );
				debug_string_ += AnsiString("��");
				if( debug.u.Exception.ExceptionRecord.ExceptionInformation[0] ) {
					debug_string_ += AnsiString("�����z���Ф˥��������`��������ޤ���") + epiStr;
				} else {
					debug_string_ += AnsiString("�i���z���Ф˥��������`��������ޤ���") + epiStr;
				}
			} else {
				debug_string_ = theadStr + AnsiString("�ǡ����������`��������ޤ���") + epiStr;
			}
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			debug_string_ = theadStr + AnsiString("�ǡ����Фι�����˥�������������ޤ���") + epiStr;
			break;
		case EXCEPTION_BREAKPOINT:
			debug_continue_status_ = DBG_CONTINUE;
			if( is_first_break_ ) {
				is_first_break_ = false;
			} else {
				// 1��Ŀ�Υ֥�`���ݥ���Ȥϥ���ȥ�`�ݥ���Ȥǰk������ģ��
//				debug_string_ = theadStr + AnsiString("�����֥�`���ݥ���Ȥ�ֹͣ���ޤ���") + epiStr;
//				Synchronize(&SetDebugString);
				return ( (debug.u.Exception.ExceptionRecord.ExceptionFlags & EXCEPTION_NONCONTINUABLE) ? 0 : -1 );
			}
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			debug_string_ = theadStr + AnsiString("�ǡ����饤���ȥ������ץ���󤬰k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			debug_string_ = theadStr + AnsiString("�ǡ�����С�������η���Ҏ�������㤬�k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			debug_string_ = theadStr + AnsiString("�ǡ�����С��������0���㤬�k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			debug_string_ = theadStr + AnsiString("�ǡ�����С������������Y����10�MС�������_�˱�F���뤳�Ȥγ����ʤ����㤬�k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			debug_string_ = theadStr + AnsiString("�ǡ�����С�������������⤬�k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_OVERFLOW:
			debug_string_ = theadStr + AnsiString("�ǡ�����С���������`�Щ`�ե�`���k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			debug_string_ = theadStr + AnsiString("�ǡ�����С�����������å����`�Щ`�ե�`���k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			debug_string_ = theadStr + AnsiString("�ǡ�����С������������`�ե�`���k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			debug_string_ = theadStr + AnsiString("�ǡ�����������(invalid instruction)�Όg�Ф��Ф��ޤ���") + epiStr;
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			debug_string_ = theadStr + AnsiString("�ϡ����ڤ��Ƥ��ʤ��ک`���˥����������褦�Ȥ��������ƥ�ϥک`�����`�ɤǤ��ޤ���Ǥ���") + epiStr;
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			debug_string_ = theadStr + AnsiString("�ǡ�0�ˤ����㤬�Ф��ޤ���") + epiStr;
			break;
		case EXCEPTION_INT_OVERFLOW:
			debug_string_ = theadStr + AnsiString("�ǡ������Υ��`�Щ`�ե�`���k�����ޤ���") + epiStr;
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			debug_string_ = theadStr + AnsiString("�ǡ�����ϥ�ɥ餬���������ä�����ǥ����ѥå���˷����ޤ�������ˮ�����Z��ʹ�ä���ץ���ޤϤ�������˛Q�����������ޤ���") + epiStr;
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			debug_string_ = theadStr + AnsiString("�ǡ��A�в����ܤ�������ᡢ����ˌg�Ф���ޤ���") + epiStr;
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			debug_string_ = theadStr + AnsiString("�ϡ��������F�ڤΥޥ����`�ɤ��S����Ƥ��ʤ�����(instruction)��g�Ф��褦�Ȥ��ޤ���") + epiStr;
			break;
		case EXCEPTION_SINGLE_STEP:
			debug_continue_status_ = DBG_CONTINUE;
			debug_string_ = theadStr + AnsiString("�ǡ����ƥå׌g�Ф��Ф��ޤ���") + epiStr;
			break;
		case EXCEPTION_STACK_OVERFLOW:
			debug_string_ = theadStr + AnsiString("�ǡ������å����`�Щ`�ե�`���k�����ޤ���") + epiStr;
			break;
		default:
			debug_continue_status_ = DBG_CONTINUE;
			debug_string_ = theadStr + AnsiString("�ǡ����������� ( ���`�� : 0x")
				+ AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionCode, 8 )
				+ (" ) ���k�����ޤ���") + epiStr;

			if( debug.u.Exception.ExceptionRecord.NumberParameters ) {
				debug_string_ += AnsiString(" ׷����� :");
				for( int i = 0; i < debug.u.Exception.ExceptionRecord.NumberParameters; i++ ) {
					debug_string_ += AnsiString(" 0x") + AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionInformation[i], 8 );
				}
			}
//			CONTEXT threadContext;
//			memset( &threadContext, 0, sizeof(threadContext) );
//			threadContext.ContextFlags = CONTEXT_ALL;
//			if( ::GetThreadContext( proc_info_.hThread, (LPCONTEXT)&threadContext ) ) {
//				
//			}
			Synchronize(&SetDebugString);
			return 1;
	}
	Synchronize(&SetDebugString);

	return ( (debug.u.Exception.ExceptionRecord.ExceptionFlags & EXCEPTION_NONCONTINUABLE) ? 0 : 1 );
}
//---------------------------------------------------------------------------
int __fastcall DebuggeeCheckThread::HandleDebugString( DEBUG_EVENT& debug )
{
	void* buffer;
	size_t len = debug.u.DebugString.nDebugStringLength;
	if( len == 0 ) return 1;
	bool isunicode = debug.u.DebugString.fUnicode ? true : false;

	if( isunicode ) {
		buffer = (void*)new wchar_t[len];
		len = len * sizeof(wchar_t);
	} else {
		buffer = (void*)new char[len];
		len = len * sizeof(char);
	}

	// �ǥХå������Ф��i�߳���
	DWORD dwRead;
	BOOL result = ::ReadProcessMemory( proc_info_.hProcess, debug.u.DebugString.lpDebugStringData,
						buffer, len, &dwRead );
	if( result == 0 ) {
        ShowLastError();
	} else {
		if( isunicode ) {
			debug_string_ = AnsiString( (wchar_t*)buffer );
		} else {
			debug_string_ = AnsiString( (char*)buffer );
		}
		Synchronize(&SetDebugString);
	}
	delete[] buffer;
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall DebuggeeCheckThread::HandleDllLoad( DEBUG_EVENT& debug )
{
	if( debug.u.LoadDll.lpImageName ) {
		// �ե�����������äƤ���r
		std::string dllname;
		wchar_t wcBuf[MAX_PATH];
		LONG_PTR lpData;
		DWORD dwAccessByte;
		::ReadProcessMemory( proc_info_.hProcess, debug.u.LoadDll.lpImageName, &lpData, sizeof(LONG_PTR), &dwAccessByte );
		::ReadProcessMemory( proc_info_.hProcess, (void *)lpData, wcBuf, sizeof(WCHAR)*MAX_PATH, &dwAccessByte );

		if( debug.u.LoadDll.fUnicode ) {
			wchar_t* name = (wchar_t*)wcBuf;
			dllname = std::string( AnsiString( name ).c_str() );
			PushDllInfo( debug.u.LoadDll.lpBaseOfDll, dllname );
		} else {
			char* name = (char*)wcBuf;
			dllname = std::string( AnsiString( name ).c_str() );
			PushDllInfo( debug.u.LoadDll.lpBaseOfDll, dllname );
		}


		debug_string_ = AnsiString( dllname.c_str() );
		debug_string_ += AnsiString( "����`�ɤ���ޤ���" );
		Synchronize(&SetDebugString);
	} else {
		// ��äƤ��ʤ��Ȥ�

	}
#if 0
	typedef struct _LOAD_DLL_DEBUG_INFO {
		HANDLE  hFile;                   /* DLL�Υե�����ϥ�ɥ� */
		LPVOID  lpBaseOfDll;             /* DLL�Υ٩`�����ɥ쥹 */
		DWORD   dwDebugInfoFileOffset;   /* �ǥХå����ޤǤΥ��ե��å� */
		DWORD   nDebugInfoSize;          /* �ǥХå����Υ����� */
		LPVOID  lpImageName;             /* DLL�Υե������� */
		WORD    fUnicode;                /* DLL�Υե������������֥��`�ɥե饰 */
	} LOAD_DLL_DEBUG_INFO;
#endif
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall DebuggeeCheckThread::HandleDllUnload( DEBUG_EVENT& debug )
{
	std::string dllname;
	if( GetDllInfo( debug.u.UnloadDll.lpBaseOfDll, dllname ) ) {
		debug_string_ = AnsiString( dllname.c_str() );
		debug_string_ += AnsiString( "�������`�ɤ���ޤ���" );
		Synchronize(&SetDebugString);
	}
	return 1;
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::PushDllInfo( LPVOID baseaddr, const std::string& filename )
{
	dll_info_.insert( std::map<LPVOID, std::string>::value_type( baseaddr, filename ) );
}
//---------------------------------------------------------------------------
bool __fastcall DebuggeeCheckThread::GetDllInfo( LPVOID baseaddr, std::string& filename )
{
	std::map<LPVOID, std::string>::const_iterator i = dll_info_.find( baseaddr );
	if( i != dll_info_.end() ) {
		filename = i->second;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::OnBreak()
{
	ScriptDebuggerForm->OnBreak();
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::SetDebugString()
{
	ScriptDebuggerForm->AppendDebugString( debug_string_ );
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::DebuggeeCheckThreadTerminate(TObject *Sender)
{
	ScriptDebuggerForm->TarminateDebugeeCheckThread();
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::GetParameters()
{
	command_line_ = ScriptDebuggerForm->DebuggeeCommandLine;
	work_folder_ = ScriptDebuggerForm->DebuggeeWorkingFolder;
}
//---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::WakeupDebugee()
{
	ScriptDebuggerForm->WakeupDebugee();
}
 //---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::SetProcInfo()
{
	ScriptDebuggerForm->SetProcInfo( proc_info_ );
}
 //---------------------------------------------------------------------------
void __fastcall DebuggeeCheckThread::ShowLastError()
{
	LPVOID lpMsgBuf;
	::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPTSTR)&lpMsgBuf, 0, NULL );
	::MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	LocalFree(lpMsgBuf);
}
//---------------------------------------------------------------------------

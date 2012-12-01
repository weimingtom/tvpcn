//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "CheckDebuggeeUnit.h"
#include "MainUnit.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

// 注意：異なるスレッドが所有する VCL のメソッド/関数/プロパティを別の
// レッド所有のオブジェクトに対しては Synchronize を使用できます。
//
//      Synchronize(&UpdateCaption);
//
// 例えば UpdateCaption を以下のように定義し
//
//      void __fastcall DebuggeeCheckThread::UpdateCaption()
//      {
//        Form1->Caption = "スレッドから書き換えました";
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

	// 起動対象のパラメータをもらう
	Synchronize(&GetParameters);

	// デバッギ起動	
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
		// プロセスデータを設定
		Synchronize(&SetProcInfo);

#if 0
		// 起動待ち
		DWORD timeout = 50;	// 50msは待つ
		while( Terminated == false ) {
			// wait for wakeup debuggee
			DWORD retwait = ::WaitForInputIdle( proc_info_.hProcess, timeout );
			if( retwait == -1 ) {
				result = 0;
				break;
			} else if( retwait == WAIT_TIMEOUT ) {
				// time out, retry
			} else {
				// 起動を通知
				Synchronize(&WakeupDebugee);
				break;
			}
		}
		if( result == 0 ) {
			ShowLastError();
			// プロセス強制終了
			::TerminateProcess(proc_info_.hProcess, 0);
		} else
#else
		Synchronize(&WakeupDebugee);
#endif
		{
			// 実行中
			while( Terminated == false && result ) {
				DEBUG_EVENT deb_ev;
				DWORD timeout = 50;	// 50msは待つ
				result = ::WaitForDebugEvent( &deb_ev, timeout );
				if( result ) {
					int breakev = HandleDebugEvent( deb_ev );
					if( breakev == 0 ) {
						break;
					} else if( breakev > 0 ) {
						// デバッグを続行する
						::ContinueDebugEvent( proc_info_.dwProcessId, deb_ev.dwThreadId, debug_continue_status_ );
					} else if( breakev < 0 ) {
						// ブレイク発生
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
									// 書き込み失敗
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
					// タイムアウト以外で終了した場合は、強制終了してしまう。
					DWORD lasterror = ::GetLastError();
					if( WAIT_TIMEOUT != lasterror && ERROR_SEM_TIMEOUT != lasterror ) {
						::TerminateProcess(proc_info_.hProcess, 0);
						result = 0;
						break;
					} else {
						result = 1;
					}
					// ブレーク要求があるかどうかチェック
					Synchronize(&CheckBreakRequest);
					if( is_request_break_ ) {
						// ブレーク要求がある
						DWORD retsus = ::SuspendThread(proc_info_.hThread);
						if( retsus == (DWORD)-1 ) {
							// エラーでサスペンドできない
							ShowLastError();
						} else {
							Synchronize(&GetCommand);
							if( command_.size_ > 0 && command_.data_ && debuggee_comm_area_addr_ && (int)command_.size_ <= debuggee_comm_area_size_ ) {
								DWORD dwWrite;
								BOOL retw = ::WriteProcessMemory( proc_info_.hProcess, debuggee_comm_area_addr_,
															command_.data_, command_.size_, &dwWrite );
								if( retw == 0 || dwWrite != command_.size_ ) {
									// 書き込み失敗
									ShowLastError();
								}
								command_.size_ = 0;
							}
							if( command_.data_ ) delete[] command_.data_;
							command_.data_ = NULL;
							command_.size_ = 0;
						}
						if( retsus != (DWORD)-1 ) {
							// リジューム
							retsus = ::ResumeThread(proc_info_.hThread);
							if( retsus == (DWORD)-1 ) {
								// リジューム失敗した場合は、強制終了してしまう。
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
//! @return : 処理を継続するかどうか
//! @retval 0 : 終了
//! @retval > 0 : 継続
//! @retval < 0 : ブレーク
int __fastcall DebuggeeCheckThread::HandleDebugEvent( DEBUG_EVENT& debug )
{
	debug_continue_status_ = DBG_CONTINUE;
	switch(debug.dwDebugEventCode){
		case OUTPUT_DEBUG_STRING_EVENT:	// デバッグ文字列を受信した
			return HandleDebugString( debug );
		case CREATE_PROCESS_DEBUG_EVENT:// プロセスを生成した
//			debug.u.CreateProcessInfo 詳細は未対応
			debug_string_ = AnsiString("プロセスが生成されました。");
			Synchronize(&SetDebugString);
			break;
		case CREATE_THREAD_DEBUG_EVENT:	// スレッドを生成した
			debug_string_ = AnsiString("スレッド ( 0x")
				+ AnsiString::IntToHex( debug.dwThreadId, 8 )
				+ AnsiString(" ) がアドレス ")
				+ AnsiString::IntToHex( (int)debug.u.CreateThread.lpStartAddress, 8 )
				+ AnsiString(" で開始されました。");
			Synchronize(&SetDebugString);
			break;
		case EXIT_THREAD_DEBUG_EVENT:	// スレッドが終了した
			debug_string_ = AnsiString("スレッド ( 0x")
				+ AnsiString::IntToHex( debug.dwThreadId, 8 )
				+ AnsiString(") はコード ")
				+ AnsiString::IntToHex( debug.u.ExitThread.dwExitCode, 8 )
				+ AnsiString(" で終了しました。");
			Synchronize(&SetDebugString);
			break;
		case LOAD_DLL_DEBUG_EVENT:		// DLLをロードした
			return HandleDllLoad( debug );
		case UNLOAD_DLL_DEBUG_EVENT:	// DLLをアンロードした
			return HandleDllUnload( debug );
		case EXCEPTION_DEBUG_EVENT:		// 例外が発生した
			return HandleDebugException( debug );
		case RIP_EVENT:					// RIPイベント
			break;
		case EXIT_PROCESS_DEBUG_EVENT:	// プロセスが終了した
			debug_string_ = AnsiString("プログラムはコード 0x")
				+ AnsiString::IntToHex( debug.u.ExitProcess.dwExitCode, 8 )
				+ AnsiString(" で終了しました。");
			Synchronize(&SetDebugString);
			return 0;
	}
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall DebuggeeCheckThread::HandleDebugException( DEBUG_EVENT& debug )
{
	debug_continue_status_ = DBG_EXCEPTION_NOT_HANDLED;

	AnsiString theadStr( AnsiString( "スレッド ( " ) + AnsiString::IntToHex( debug.dwThreadId, 8 ) + AnsiString(" ) ") );
	AnsiString epiStr( AnsiString( " ( EPI = 0x" ) + AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionAddress, 8 ) + AnsiString(" ) ") );

	switch( debug.u.Exception.ExceptionRecord.ExceptionCode ) {
		case EXCEPTION_ACCESS_VIOLATION:
			if( debug.u.Exception.ExceptionRecord.NumberParameters >= 2 ) {
				debug_string_ = theadStr + AnsiString("で、0x");
				debug_string_ += AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionInformation[1], 8 );
				debug_string_ += AnsiString("を");
				if( debug.u.Exception.ExceptionRecord.ExceptionInformation[0] ) {
					debug_string_ += AnsiString("書き込み中にアクセス違反がありました") + epiStr;
				} else {
					debug_string_ += AnsiString("読み込み中にアクセス違反がありました") + epiStr;
				}
			} else {
				debug_string_ = theadStr + AnsiString("で、アクセス違反がありました") + epiStr;
			}
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			debug_string_ = theadStr + AnsiString("で、配列の範囲外にアクセスがありました") + epiStr;
			break;
		case EXCEPTION_BREAKPOINT:
			debug_continue_status_ = DBG_CONTINUE;
			if( is_first_break_ ) {
				is_first_break_ = false;
			} else {
				// 1回目のブレークポイントはエントリーポイントで発生する模様
//				debug_string_ = theadStr + AnsiString("が、ブレークポイントで停止しました") + epiStr;
//				Synchronize(&SetDebugString);
				return ( (debug.u.Exception.ExceptionRecord.ExceptionFlags & EXCEPTION_NONCONTINUABLE) ? 0 : -1 );
			}
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			debug_string_ = theadStr + AnsiString("で、アライメントエクセプションが発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数の非正規化数演算が発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数の0除算が発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数の演算結果を10進小数で正確に表現することの出来ない演算が発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数演算例外が発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_OVERFLOW:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数オーバーフローが発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数スタックオーバーフローが発生しました") + epiStr;
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			debug_string_ = theadStr + AnsiString("で、浮動小数点数アンダーフローが発生しました") + epiStr;
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			debug_string_ = theadStr + AnsiString("で、不正な命令(invalid instruction)の実行が行われました") + epiStr;
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			debug_string_ = theadStr + AnsiString("は、存在していないページにアクセスしようとし、システムはページをロードできませんでした") + epiStr;
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			debug_string_ = theadStr + AnsiString("で、0による除算が行われました") + epiStr;
			break;
		case EXCEPTION_INT_OVERFLOW:
			debug_string_ = theadStr + AnsiString("で、整数のオーバーフローが発生しました") + epiStr;
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			debug_string_ = theadStr + AnsiString("で、例外ハンドラが不正な配置を例外ディスパッチャに返しました。高水準言語を使用するプログラマはこの例外に決して遭遇しません") + epiStr;
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			debug_string_ = theadStr + AnsiString("で、続行不可能な例外の後、さらに実行されました") + epiStr;
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			debug_string_ = theadStr + AnsiString("は、操作が現在のマシンモードで許されていない命令(instruction)を実行しようとしました") + epiStr;
			break;
		case EXCEPTION_SINGLE_STEP:
			debug_continue_status_ = DBG_CONTINUE;
			debug_string_ = theadStr + AnsiString("で、ステップ実行が行われました") + epiStr;
			break;
		case EXCEPTION_STACK_OVERFLOW:
			debug_string_ = theadStr + AnsiString("で、スタックオーバーフローが発生しました") + epiStr;
			break;
		default:
			debug_continue_status_ = DBG_CONTINUE;
			debug_string_ = theadStr + AnsiString("で、不明な例外 ( コード : 0x")
				+ AnsiString::IntToHex( (int)debug.u.Exception.ExceptionRecord.ExceptionCode, 8 )
				+ (" ) が発生しました") + epiStr;

			if( debug.u.Exception.ExceptionRecord.NumberParameters ) {
				debug_string_ += AnsiString(" 追加情報 :");
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

	// デバッグ文字列を読み出す
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
		// ファイル名が入っている時
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
		debug_string_ += AnsiString( "がロードされました" );
		Synchronize(&SetDebugString);
	} else {
		// 入っていないとき

	}
#if 0
	typedef struct _LOAD_DLL_DEBUG_INFO {
		HANDLE  hFile;                   /* DLLのファイルハンドル */
		LPVOID  lpBaseOfDll;             /* DLLのベースアドレス */
		DWORD   dwDebugInfoFileOffset;   /* デバッグ情報までのオフセット */
		DWORD   nDebugInfoSize;          /* デバッグ情報のサイズ */
		LPVOID  lpImageName;             /* DLLのファイル名 */
		WORD    fUnicode;                /* DLLのファイル名の文字コードフラグ */
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
		debug_string_ += AnsiString( "がアンロードされました" );
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

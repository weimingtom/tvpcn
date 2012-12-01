/****************************************************************************/
/*! @file
@brief ブレークポイントのあるファイルと行番号を保持する

-----------------------------------------------------------------------------
	Copyright (C) T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2010/04/18
@note
*****************************************************************************/

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#include <map>
#include <string>

enum tTJSDBGHOOKType {
	DBGHOOK_PREV_EXE_LINE,	//!< ライン実行時
	DBGHOOK_PREV_CALL,		//!< 関数コール
	DBGHOOK_PREV_RETURN,	//!< リターン時
	DBGHOOK_PREV_EXCEPT,	//!< 例外射出時
	DBGHOOK_PREV_BREAK,		//!< スクリプト中のブレーク
};
// gee = debuggee
// ger = debugger
enum tTJSDBGEvent {
	DBGEV_GEE_LOG = 0x8000,		//!< gee -> ger ログを出力 (数値に特に意味はない)
	DBGEV_GEE_BREAK,			//!< gee -> ger 停止通知
	DBGEV_GEE_STACK_TRACE,		//!< gee -> ger スタックトレース情報通知
	DBGEV_GEE_LOCAL_VALUE,		//!< gee -> ger ローカル変数情報
	DBGEV_GEE_REQUEST_SETTINGS,	//!< gee -> ger 例外通知有無、ブレークポイント情報等を要求
	DBGEV_GEE_CLASS_VALUE,		//!< gee -> ger クラス変数情報

	DBGEV_GER_EXEC = 0x9000,	//!< ger -> gee 実行
	DBGEV_GER_BREAK,			//!< ger -> gee 一時停止
	DBGEV_GER_STEP,				//!< ger -> gee ステップ
	DBGEV_GER_TRACE,			//!< ger -> gee トレース
	DBGEV_GER_RETURN,			//!< ger -> gee リターン
	DBGEV_GER_BREAKPOINT_START,	//!< ger -> gee ブレークポイント情報送信開始
	DBGEV_GER_BREAKPOINT,		//!< ger -> gee ブレークポイント情報
	DBGEV_GER_BREAKPOINT_END,	//!< ger -> gee ブレークポイント情報送信終了
	DBGEV_GER_EXCEPTION_FLG,	//!< ger -> gee 例外発生時に停止するかどうか
};

struct BreakpointLine {
	typedef std::map<int,int>		lines;
	typedef lines::iterator			iterator;
	typedef lines::const_iterator	const_iterator;

	lines	Lines;

	bool IsBreakPoint( int lineno ) const {
		const_iterator j = Lines.find( lineno );
		if( j != Lines.end() ) {
			return true;
		}
		return false;
	}
	void SetBreakPoint( int lineno ) {
		iterator i = Lines.find( lineno );
		if( i == Lines.end() ) {
			std::pair<iterator, bool> ret = Lines.insert( BreakpointLine::lines::value_type( lineno, lineno ) );
			assert( ret.second );
		}
	}
	void ClearBreakPoint( int lineno ) {
		Lines.erase( lineno );
	}
};

struct Breakpoints {
	typedef std::map<std::wstring,BreakpointLine>	breakpoints;
	typedef breakpoints::iterator					iterator;
	typedef breakpoints::const_iterator				const_iterator;

	breakpoints BreakPoint;

	void SetBreakPoint( const std::wstring& filename, int lineno ) {
		iterator i = BreakPoint.find( filename );
		if( i == BreakPoint.end() ) {
			std::pair<iterator, bool> ret = BreakPoint.insert( breakpoints::value_type( filename, BreakpointLine() ) );
			assert( ret.second );
			i = ret.first;
		}
		i->second.SetBreakPoint(lineno);
	}
	void ClearBreakPoint( const std::wstring& filename, int lineno ) {
		iterator i = BreakPoint.find( filename );
		if( i == BreakPoint.end() ) {
			return;
		}
		i->second.ClearBreakPoint( lineno );
	}
	bool IsBreakPoint( const std::wstring& filename, int lineno ) const {
		const_iterator i = BreakPoint.find( filename );
		if( i != BreakPoint.end() ) {
			return i->second.IsBreakPoint(lineno);
		}
		return false;
	}
	const BreakpointLine* GetBreakPointLines( const std::wstring& filename ) const {
		const_iterator i = BreakPoint.find( filename );
		if( i != BreakPoint.end() ) {
			return &(i->second);
		}
		return NULL;
	}
	BreakpointLine* GetBreakPointLines( const std::wstring& filename ) {
		iterator i = BreakPoint.find( filename );
		if( i != BreakPoint.end() ) {
			return &(i->second);
		}
		return NULL;
	}
	bool HasBreakPoint( const std::wstring& filename ) {
		iterator i = BreakPoint.find( filename );
		if( i != BreakPoint.end() ) {
			return( i->second.Lines.size() > 0 );
		}
		return false;
	}
	void ClearAll() {
		BreakPoint.clear();
	}
};



#endif // __DEBUGGER_H__

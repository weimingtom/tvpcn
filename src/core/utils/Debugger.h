/****************************************************************************/
/*! @file
@brief �֥�`���ݥ���ȤΤ���ե�������з��Ť򱣳֤���

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
	DBGHOOK_PREV_EXE_LINE,	//!< �饤��g�Еr
	DBGHOOK_PREV_CALL,		//!< �v�����`��
	DBGHOOK_PREV_RETURN,	//!< �꥿�`��r
	DBGHOOK_PREV_EXCEPT,	//!< ��������r
	DBGHOOK_PREV_BREAK,		//!< ������ץ��ФΥ֥�`��
};
// gee = debuggee
// ger = debugger
enum tTJSDBGEvent {
	DBGEV_GEE_LOG = 0x8000,		//!< gee -> ger ������� (�������ؤ���ζ�Ϥʤ�)
	DBGEV_GEE_BREAK,			//!< gee -> ger ֹ֪ͣͨ
	DBGEV_GEE_STACK_TRACE,		//!< gee -> ger �����å��ȥ�`�����֪ͨ
	DBGEV_GEE_LOCAL_VALUE,		//!< gee -> ger ��`����������
	DBGEV_GEE_REQUEST_SETTINGS,	//!< gee -> ger ����֪ͨ�Пo���֥�`���ݥ�������Ȥ�Ҫ��
	DBGEV_GEE_CLASS_VALUE,		//!< gee -> ger ���饹�������

	DBGEV_GER_EXEC = 0x9000,	//!< ger -> gee �g��
	DBGEV_GER_BREAK,			//!< ger -> gee һ�rֹͣ
	DBGEV_GER_STEP,				//!< ger -> gee ���ƥå�
	DBGEV_GER_TRACE,			//!< ger -> gee �ȥ�`��
	DBGEV_GER_RETURN,			//!< ger -> gee �꥿�`��
	DBGEV_GER_BREAKPOINT_START,	//!< ger -> gee �֥�`���ݥ������������_ʼ
	DBGEV_GER_BREAKPOINT,		//!< ger -> gee �֥�`���ݥ�������
	DBGEV_GER_BREAKPOINT_END,	//!< ger -> gee �֥�`���ݥ����������ŽK��
	DBGEV_GER_EXCEPTION_FLG,	//!< ger -> gee ����k���r��ֹͣ���뤫�ɤ���
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

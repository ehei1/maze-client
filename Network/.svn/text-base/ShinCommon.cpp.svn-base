#include "stdafx.h"
#include "ShinCommon.h"
#include <stdio.h>

#ifdef _UNICODE
	LPCTSTR MultiToWide( LPCSTR txt )
	{
		static wchar_t msg[MAX_PATH];
		ZeroMemory( msg, _countof(msg) );

		MultiByteToWideChar( CP_ACP, 0, txt, strlen(txt), msg, _countof(msg) );
		return msg;
	}

	LPCSTR WideToMulti( LPCTSTR txt )
	{
		static char msg[MAX_PATH];
		ZeroMemory( msg, _countof(msg) );

		WideCharToMultiByte( CP_ACP, 0, txt, wcslen(txt), msg, _countof(msg), 0, 0 );
		return msg;
	}
#endif

// 090624, OutputDebug Modify
void OutputDebug( LPCTSTR pMsg, ... )
{
	TCHAR msg[512] = {0,};
	
	if( pMsg )
	{
		va_list vList;
		va_start( vList, pMsg );
		_vstprintf_s( msg, sizeof(msg)/sizeof(*msg) - 1, pMsg, vList );
		va_end( vList );
	}

	_tcscat_s( msg, _countof(msg), _T("\n") );
	OutputDebugString( msg );
}

// 090624, File Log
void OutputFile( LPCTSTR szFileName, LPCTSTR pMsg, ... )
{
	TCHAR msg[512] = {0,};
	
	if( pMsg )
	{
		va_list vList;
		va_start( vList, pMsg );
		_vstprintf_s( msg, sizeof(msg)/sizeof(*msg) - 1, pMsg, vList );
		va_end( vList );
	}

	// File Log
	FILE* fp = NULL;

	fp = _tfsopen( szFileName, _T("a+"), _SH_DENYRW );
	if( fp )
	{
		SYSTEMTIME sysTime;
		GetLocalTime( &sysTime );

		_ftprintf( fp, _T("[%04d-%02d-%02d %02d:%02d:%02d] %s\n"), sysTime.wYear, 
															sysTime.wMonth, 
															sysTime.wDay, 
															sysTime.wHour, 
															sysTime.wMinute, 
															sysTime.wSecond, 
															msg );

		fclose( fp );
	}
}

// √‚√≥: http://www.gpgstudy.com/forum/viewtopic.php?t=795&highlight=%C7%D8%BD%AC+%B9%AE%C0%DA%BF%AD
DWORD GetHashCodeFromTxt( LPCTSTR txt )
{
	const DWORD dwLength = (DWORD)_tcslen( txt );
	DWORD dwHashCode = 5381;

	for(DWORD i = 0 ; i < dwLength ; ++i )
	{
		DWORD ch = DWORD( txt[ i ] );
		dwHashCode = ((dwHashCode << 5) + dwHashCode) + ch;
	}

	return dwHashCode;
}

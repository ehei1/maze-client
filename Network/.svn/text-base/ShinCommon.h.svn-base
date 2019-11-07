#pragma once

#include <assert.h>
#include "ShinIOCP.h"
#include "PacketDataOperator.h"


#ifndef ASSERT
	#define ASSERT(x)				assert(x)
#endif

#define LOG_FILE_NAME			_T("NetworkLog.txt")

#ifdef _UNICODE
	LPCTSTR MultiToWide( LPCSTR txt );
	LPCSTR WideToMulti( LPCTSTR txt );


	#ifdef _DEBUG
		#define ___FUNCTION___	MultiToWide(__FUNCTION__)
	#else
		#define ___FUNCTION___	_T("")
	#endif
#else
	#define		___FUNCTION___	__FUNCTION__
	#define		MultiToWide( txt )	txt
	#define		WideToMulti( txt )	txt
#endif

void OutputDebug( LPCTSTR pMsg, ... );
void OutputFile( LPCTSTR szFileName, LPCTSTR pMsg, ... );

DWORD GetHashCodeFromTxt( LPCTSTR txt );
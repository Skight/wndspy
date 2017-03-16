#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_WS_SPYDLL_OPT_H
#define HS_WS_SPYDLL_OPT_H

///////////////////////////////////////////////////////////////////
#define WS_LIB_DLLENTRYPOINT

#define HS_NO_GLOBALVAR_HERE
#include "..\WndSpy.h"

///////////////////////////////////////////////////////////////////

#define _NO_MSVCRT

#if _MSC_VER <=1200
#pragma comment(linker, "/OPT:NOWIN98")
#endif

#ifndef _DEBUG
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#if defined(_NO_MSVCRT)
#pragma comment (linker, "/nodefaultlib:msvcrt.lib")
#endif
#pragma comment (linker, "/nodefaultlib:libc.lib")
#pragma comment (linker, "/nodefaultlib:libcmt.lib")
#pragma comment (linker, "/nodefaultlib:libcd.lib")
#pragma comment (linker, "/nodefaultlib:libcmtd.lib")
#pragma comment (linker, "/nodefaultlib:msvcrtd.lib")
#else
#pragma comment (linker, "/nodefaultlib:libc.lib")
#pragma comment (linker, "/nodefaultlib:libcd.lib")
#pragma comment (linker, "/nodefaultlib:msvcrt.lib")
#endif //_DEBUG

#pragma comment(lib, "msvcrt.lib")
#ifdef WS_LIB_DLLENTRYPOINT
#if defined(_NO_MSVCRT)
#pragma comment(linker, "/entry:DllMain")
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
#else
#pragma comment(lib, "msvcrt.lib")
#endif //_NO_MSVCRT
#else
#pragma comment (linker, "/noentry")
#endif //WS_LIB_DLLENTRYPOINT


//////////////////////////////////////////////////////////////////////////
/* 
	#define WS_LIB_MAP_MSG_PARAM
*/
#ifdef WS_LIB_MAP_MSG_PARAM

BOOL wsMapDataToTarget(HWND hwnd, BOOL IsHex, INT iAlign, LPTSTR szWParam, LPTSTR szLParam);
LRESULT CALLBACK HookProc_MapData(int nCode,
								WPARAM wParam,
								LPARAM lParam);

void LibMapDataString(void);
void DoMapDataString(LPTSTR szString);

INT Lib_MapStringToAdr(LPTSTR szString, LPTSTR szGlobalBuf, INT iAdrPos, INT iMaxGloBufLen);
INT Lib_MapStructToAdr(LPTSTR szString, TCHAR chStart, TCHAR chEnd, BOOL isHex, INT iAlign, 
					 BYTE* pGlobalDataBuf, INT iAdrPos, INT iMaxGloBufSize);


#ifdef _HS_COMMON_FUN_STR2INT
INT Lib_HexStrToInt(LPTSTR szHex);
INT Lib_DecStrToInt(LPTSTR szDecNum);
#endif //_HS_COMMON_FUN

INT Lib_NumStrToStruct(LPTSTR szDataStr, PVOID lpData, INT iAlign, BOOL IsHex, INT iMaxByte);

#endif //WS_LIB_MAP_MSG_PARAM


///////////////////////////////////////////////////////////////////



#endif //HS_WS_SPYDLL_OPT_H
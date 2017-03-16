#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_WNDSPYDLL_TYPE_H
#define HS_HEADER_WNDSPYDLL_TYPE_H

#include <Windows.h>

//////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
#define SPYDLL_LARGE_STR_LEN	260
#define SPYDLL_CMDLINE_STR_LEN	256

#else
#define SPYDLL_LARGE_STR_LEN	324
#define SPYDLL_CMDLINE_STR_LEN	320

#endif //UNICODE

#if defined(UNICODE)
#define SPYDLL_SMALL_STR_LEN	64
#else
#define SPYDLL_SMALL_STR_LEN	128
#endif

#define WS_LIB_DATA_DLLNAME

typedef struct tag_SpyAppData {

#ifndef WS_LIB_DATA_DLLNAME
	HINSTANCE g_hInstWndSpyDll;
#endif //!WS_LIB_DATA_DLLNAME

	HWND	hwndspyMsg;
	short	siWinVer;

#ifdef WS_LIB_DATA_DLLNAME
	TCHAR	szDllName[MAX_PATH];
#endif //WS_LIB_DATA_DLLNAME

}SPYAPPDATA;



//////////////////////////////////////////////////////////////////////////

#ifdef UNICODE

#define CHECK_WNDSPY_DLL_SIZE	10752

#else

#define CHECK_WNDSPY_DLL_SIZE	9216

#endif //UNICODE


//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_WNDSPYDLL_TYPE_H
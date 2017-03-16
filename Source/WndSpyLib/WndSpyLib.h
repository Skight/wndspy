#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SPYDLL_H
#define HS_HEADER_DEF_SPYDLL_H

//////////////////////////////////////////////////////////////////////////

#include "LibTypeDef.h"
#include "WndSpylibOpt.h"

//////////////////////////////////////////////////////////////////////////
//DEFINE FOR LOCAL USE ONLY...

#define DoEnsureUnHook(_phHook) wsUnSetWndHook(_phHook)
#define UnSetHook(_phHook) wsUnSetWndHook(_phHook)

#define SetHookStub(_hwnd, _idThread, _idHook, _hookProc, _phook) \
wsSetWndHook(_hwnd, _idThread, _idHook, _hookProc, _phook)


//////////////////////////////////////////////////////////////////////////

VOID wsInitData(SPYAPPDATA*);
VOID wsUnSetWndHook(HHOOK* phHookVar);
HHOOK wsSetWndHook(HWND hwnd, DWORD idThread, int idHook, HOOKPROC lpfn,  HHOOK* phHookVar);

LRESULT CALLBACK HookProc_GetWndInfo(int, WPARAM, LPARAM);
BOOL wsHookGetWndInfo(HWND hwnd, DWORD idThread,  HWND hwndMsgWnd);

VOID wsGetWndInfo(HWND hwnd, SPY_WININFOEX* pSWIex, HWND hwndMsgWnd);

VOID LibGetWindowInfo(VOID);
VOID LibGetModuleFileName(HINSTANCE hInst, LPTSTR szFileNameBuf);

INT wsReadWndInfo(SPY_PROCESSINFO* pSPI, SPY_WININFOEX* pSWIex);

LRESULT CALLBACK HookProc_GetWndText(int, WPARAM, LPARAM);
BOOL wsHookGetWndText(HWND hwnd, DWORD idThread, LPTSTR szPasswordBuf);

//////////////////////////////////////////////////////////////////////////

typedef struct tag_SpecSysModule
{
	HINSTANCE winsrv_dll;
	HINSTANCE win32k_sys;

}HINT_KSMOD;//Instance of known system modules


BOOL LibGiveKnownFileName(HWND hwndTag, HINSTANCE hInst, LPTSTR szFileNameBuf);

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_SPYDLL_H
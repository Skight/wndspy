#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SPYFUNC_H
#define HS_HEADER_DEF_SPYFUNC_H
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GuessSpecProcessPath_OutProc(LPTSTR szSpecPath, LPARAM lParam);
BOOL GetExeFullNameForPE32Ex(PROCESSENTRY32* pPe32);
DWORD GetProcessInfoFromWindow(HWND, SPY_PROCESSINFO*);
VOID GetPSWText(HWND hwndPSW, LPTSTR szPSWbuf);
BOOL SpyGetWindowInfo(HWND, SPY_WININFO*);
VOID SpyTryGetWndInfoEx(HWND hwnd, SPY_PROCESSINFO* pSPI, SPY_WININFOEX* pSWIex);
VOID SpyGetWindowInfoEx(HWND hwnd, SPY_PROCESSINFO* pSPI, SPY_WININFOEX* pSWIex);
BOOL GetProcessFileNameFromWindow(HWND hwnd, LPTSTR szFileName, UINT iMaxLen);
//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_SPYFUNC_H


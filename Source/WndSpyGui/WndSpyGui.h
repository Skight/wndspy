#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_WNDSPYGUI_H
#define HS_HEADER_DEF_WNDSPYGUI_H

//////////////////////////////////////////////////////////////////////////

#include "..\WndSpy.h"

//////////////////////////////////////////////////////////////////////////

#include "SpyMainDlg.h"
#include "SpyMsgWnd.h"
#include "SpyMainDlgTabs.h"
#include "SpyMainToolTips.h"
#include "SpyToolContainer.h"
#include "DlgAbout.h"
#include "DlgAppSettings.h"
#include "DlgEffectOption.h"
#include "DlgSndMsg.h"
#include "DlgIconExtractor.h"
#include "DlgSaveIconOption.h"
#include "DlgColorSpy.h"
#include "DlgPswSniffer.h"
#include "DlgWndEnumer.h"
#include "DlgWndModifier.h"
//////////////////////////////////////////////////////////////////////////

#define NO_TRANS	255
VOID DoSetWindowTrans(HWND hwndTarget, BYTE byAlpha);


extern BOOL g_bIsNcBtnDownDispatched;

VOID CALLBACK TimerProc_Effect_NCButtonDownDispatch(HWND hwnd, 
													UINT uMsg, 
													UINT_PTR idEvent, 
													DWORD dwTime);

//////////////////////////////////////////////////////////////////////////
#define TIMERPROC_NCBUTTONDOWN_DISPATCH	TimerProc_Effect_NCButtonDownDispatch
//////////////////////////////////////////////////////////////////////////

VOID KillTimer_NcBtnDispatch(HWND hwnd);

BOOL CallSpecWndProcRoutine(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


//////////////////////////////////////////////////////////////////////////
COLORREF GetTinctureColor(VOID);

#define WM_MY_SETPANCURSOR  (WM_USER_FIRST_APPCTRL+1)

#define PAN_RECT_CX		31
#define PAN_RECT_CY		28

//SubClassParamWndProc : SCP_Param_Static_PickCursorPan
LRESULT APIENTRY SCP_Param_Static_PickCursorPan(HWND hwnd, 
												UINT message, 
												WPARAM wParam, 
												LPARAM lParam);


VOID InitPickPanCtrl(HWND hwndPickPan, WND_SUBCLASS_DATA* pWSCD, HCURSOR hCurInPan);

#define PickPan_SetCursor(_hwndPickPan, _hCursorInPan) \
SendMessage(_hwndPickPan, WM_MY_SETPANCURSOR, (WPARAM)NULL, (LPARAM)_hCursorInPan)

//////////////////////////////////////////////////////////////////////////

VOID Workaround_BugTrans(HWND hwnd, DWORD dwFlagBit);

#define OnWorkaround_BugTrans(_hwnd, _WndBitFlag) \
if(!IS_FLAG_SETTED(g_dwBitFlag, _WndBitFlag)) Workaround_BugTrans(_hwnd, _WndBitFlag)

#define NoTrace_BugTrans(_WndBitFlag)	\
if(!IS_FLAG_SETTED(g_dwBitFlag, _WndBitFlag)) g_dwBitFlag|=_WndBitFlag

VOID DoSetMainIconAndCenterWnd(HWND hwnd);

//////////////////////////////////////////////////////////////////////////

//#define MAINDLG_TIRM_MEM

#if defined(MAINDLG_TIRM_MEM) && defined(_TIRM_MEM)

#define CASE_OPTION_WM_ACTIVATE() \
case WM_ACTIVATE: \
{if(LOWORD(wParam)==WA_INACTIVE && \
	HIWORD(wParam)==0 && g_hwnd_TaskModal==NULL) \
	{ MACRO_TIRM_MEMORY( (g_siWinVer >= WINVER_WIN2K) ) /*MessageBeep(0);*/} \
break;} //MACRO_CASE_OPTION_WM_ACTIVATE(0)

#else 

#define CASE_OPTION_WM_ACTIVATE()

#endif //defined(MAINDLG_TIRM_MEM) && defined(_TIRM_MEM)

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_WNDSPYGUI_H
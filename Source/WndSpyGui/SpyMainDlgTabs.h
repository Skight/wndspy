#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SPYMAINDLGTABS_H
#define HS_HEADER_DEF_SPYMAINDLGTABS_H
//////////////////////////////////////////////////////////////////////////

#include "..\WndSpy.h"

#include "SpyMainDlgTabs\DlgTabFinder.h"
#include "SpyMainDlgTabs\DlgTabGeneral.h"
#include "SpyMainDlgTabs\DlgTabStyles.h"
#include "SpyMainDlgTabs\DlgTabClass.h"
#include "SpyMainDlgTabs\DlgTabProcess.h"
#include "SpyMainDlgTabs\DlgTabMani.h"

//////////////////////////////////////////////////////////////////////////

BOOL SetTabChildDlgInfo(VOID);
VOID WINAPI OnTabbedDialogInit(VOID);
VOID WINAPI OnTabSelChanged(VOID);
VOID WINAPI OnChildDialogInit(HWND hwnd);

#ifdef TAB_DYNAMIC_CREATE
VOID ReBuildTabCtrl(VOID);
#endif

//////////////////////////////////////////////////////////////////////////

VOID DoClearWndSpyText(BOOL bIsAll);
VOID OnRefreshFinderDlgEdits(HWND hwnd);

//////////////////////////////////////////////////////////////////////////
#define ID_CMD_ADDHWND		(MAKEWPARAM(IDM_SPYTARGET,0xABCD))

LRESULT APIENTRY SCP_HwndFileEdit(HWND hwnd, 
								  UINT message, 
								  WPARAM wParam, 
								  LPARAM lParam);


#define DoHighlightWindow(_hwnd, _iBlinkTimes) \
SendMessage(GetDlgItem(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, IDC_EDIT_HWND), \
WM_MY_HIGHLIGHT, (WPARAM)_hwnd, (LPARAM)_iBlinkTimes)

//////////////////////////////////////////////////////////////////////////

#endif // HS_HEADER_DEF_SPYMAINDLGTABS_H

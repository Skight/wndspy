#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SPYMAINDLG_H
#define HS_HEADER_DEF_SPYMAINDLG_H
//////////////////////////////////////////////////////////////////////////

#define TIMER_EFFACT_DELAY	0xEEEE
#define TIMER_NCLBTN_DELAY	(TIMER_EFFACT_DELAY+WM_NCLBUTTONDOWN)
#define TIMER_NCRBTN_DELAY	(TIMER_EFFACT_DELAY+WM_NCRBUTTONDOWN)

#define TIMER_REFRESH_DELAY	2
#define TIMER_SIZE_DELAY	3
#define TIMER_SC_DELAY		4

//////////////////////////////////////////////////////////////////////////
//MACRO:SpyMain_Command_PreRoutine(3)
#define SpyMain_Command_PreRoutine(_hwnd, _wParam, _lParam) \
KillTimer(_hwnd, TIMER_NCLBTN_DELAY); \
KillTimer(_hwnd, TIMER_NCRBTN_DELAY); \
if( LOWORD(_wParam)!=ID_CMD_EXIT ) OnWorkaround_BugTrans(_hwnd, BOOLEAN_BIT_ONCELOCK)
//MACRO_END:SpyMain_Command_PreRoutine(3)

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_SpyMain(HWND hwnd, 
							  UINT message, 
							  WPARAM wParam, 
							  LPARAM lParam);


VOID OnOpenHelpTopics(VOID);
VOID OnSetMainDlgMenu(HWND hwnd);
VOID OnSetMenuState(HMENU hMenu);
VOID OnShowTrayIcon(VOID);
VOID SaveWindowStringsThreadProc(PVOID);
VOID ExportImportProfileThreadProc(PVOID IdCmd);
//////////////////////////////////////////////////////////////////////////

#endif // HS_HEADER_DEF_SPYMAINDLG_H


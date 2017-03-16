#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_MANTAB_0_H
#define HS_HEADER_DEF_MANTAB_0_H
//////////////////////////////////////////////////////////////////////////

#define WM_MY_MOUSEMOVE		(WM_USER_FIRST_TAB_FINDER + 1)
#define	WM_MY_GETHWND		(WM_USER_FIRST_TAB_FINDER + 6)

//////////////////////////////////////////////////////////////////////////

#define FINDER_TIMER_DELAY_STARTFINDER		1
#define FINDER_TIMER_DELAY_TEXTUPDATE		2
#define FINDER_TIMER_GETSPYINFOEX			3
#define FINDER_TIMER_MOUSEMOVE_AFTER_TABWND	4
#define FINDER_TIMER_DELAY_LOCKUPDATE		5
#define FINDER_TIMER_HOTKEY_SPYIT			6
#define FINDER_TIMER_START_DRAG				7

//MACRO DoPlayEventSound(1)
#define DoPlayEventSound(_IsPlay)	\
if(_IsPlay) PlaySound(MAKEINTRESOURCE(IDR_WAVE_WIN2WIN), \
g_hInstance,SND_ASYNC|SND_RESOURCE|SND_NOWAIT)
//MACRO End DoPlayEventSound(1)
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK TabDlgProc_Finder(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);


#define DSFBS_NORMAL 0
#define DSFBS_DISABLE_ALL 1
#define DSFBS_GREY_ALL 2
/***************************
iFlag can be the following predefined values:
#define DSFBS_NORMAL		0
#define DSFBS_DISABLE_ALL	1
#define DSFBS_GREY_ALL		2
****************************/
VOID DoSetFinderBtnState(INT iFlag);

VOID DoSetFinderInfoText(HWND hwndFind);

BOOL CALLBACK DlgProc_SpyTarget(HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_MANTAB_0_H


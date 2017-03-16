#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_WNDSPY_H
#define HS_HEADER_DEF_WNDSPY_H

//////////////////////////////////////////////////////////////////////////

#ifndef UNICODE
#define VER_CHARSET TEXT("ANSI")
#else
#define VER_CHARSET TEXT("Unicode")
#endif //!UNICODE

#define VER_NUM_MAJOR	1
#define VER_NUM_MINOR	6
#define VER_BUILD	TEXT(__DATE__)

//////////////////////////////////////////////////////////////////////////

#ifndef WINVER
#define WINVER 0x0501
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

/////////////////////////////////////////////////////////////////////////
#pragma comment (lib, "comctl32.lib" )
#pragma comment (lib, "Winmm.lib" )
#pragma comment (lib, "msvcrt.lib" )

//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <stdio.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////
#include <uxtheme.h>
#include <Richedit.h>
//////////////////////////////////////////////////////////////////////////
#include "AppOpt.h"
#include "resource.h"
#include "contrib.h"
#include "hsCommon.h"
//////////////////////////////////////////////////////////////////////////
#define STR_CHANGELINE				TEXT("\r\n")
#define STR_INI_SEC_SETTING			TEXT("Settings")
#define STR_INI_SEC_RECENT			TEXT("Recent")
#define STR_INI_KEY_BACKUP			TEXT("Backup")
#define STR_INI_KEY_SAVETEXT		TEXT("SaveText")
#define STR_INI_KEY_SAVEICON		TEXT("SaveIcon")
#define STR_INI_SEC_MSGOPT			TEXT("Message Options")
#define STR_INI_SEC_MSGMEMO			TEXT("Message Memos")

#define STR_APP_TEMPDIR				TEXT("WSTemp")
#define STR_APP_DLLFILE				TEXT("WndSpy.dll")

#define STR_NONE					TEXT("(None)")

#define STR_SPYMSGWND_CLASS			TEXT("SpyMsgWnd")
#define STR_SPYMAINDLG_CLASS		TEXT("SpyMainDlg")
#define STR_SPYTOOLCONTAINNER_CLASS	TEXT("SpyToolContainer")

//////////////////////////////////////////////////////////////////////////

#define ID_SUBMENU_EDIT_HWNDFILE		0
#define ID_SUBMENU_SBTN_ICONEXT			1
#define ID_SUBMENU_BTN_SNDMSG			2
#define ID_SUBMENU_LV_PSW				3
#define ID_SUBMENU_TV_WTREE				4
//////////////////////////////////////////////////////////////////////////

#define TRANS_ALPHA_DEFAULT 64
#define TRANS_ALPHA_MIN		32
#define TRANS_ALPHA_MAX		192


#define DEF_SPYHOTKEY	MAKEWORD((WORD)'Q', HOTKEYF_CONTROL)

#define BASE_COLOR_R	0 //0xFF
#define BASE_COLOR_G	0
#define BASE_COLOR_B	0
#define BASE_COLOR_RGB	RGB(BASE_COLOR_R,BASE_COLOR_G,BASE_COLOR_B)
//////////////////////////////////////////////////////////////////////////

#define WM_USER_FIRST_WND_BKMSGWND		(WM_USER + 100)
#define WM_USER_FIRST_WND_SPYMAIN		(WM_USER + 200)
#define WM_USER_FIRST_TAB_FINDER		(WM_USER + 300)
#define WM_USER_FIRST_DLG_ICONEXT		(WM_USER + 600)
#define WM_USER_FIRST_DLG_COLORSPY		(WM_USER + 700)
#define WM_USER_FIRST_DLG_WNDMOD		(WM_USER + 800)
#define WM_USER_FIRST_APPCTRL			(WM_USER + 1000)
#define WM_USER_FIRST_TOOLCONTAINER		(WM_USER + 2000)
#define WM_USER_FIRST_SENDMSG			(WM_USER + 3000)
#define WM_USER_FIRST_COMMON			(WM_USER + 5000)
//////////////////////////////////////////////////////////////////////////
#define WM_MY_GETHWNDTARGET			(WM_USER_FIRST_COMMON + 1)
#define WM_MY_HIGHLIGHT				(WM_USER_FIRST_COMMON + 2)
//////////////////////////////////////////////////////////////////////////

#define BOOLEAN_BIT_DLG_SENDMSG		0x00000001
#define BOOLEAN_BIT_DLG_SAVEICON	0x00000002
#define BOOLEAN_BIT_ICONEXT			0x00000004
#define BOOLEAN_BIT_COLORCATCHER	0x00000008

#define BOOLEAN_BIT_ONCELOCK		0x10000000
#define BOOLEAN_BIT_BKWND			0x20000000
#define BOOLEAN_BIT_DLG_TOPCHK		0x40000000
#define BOOLEAN_BIT_DLG_SPYMAIN		0x80000000
#define BOOLEAN_BIT_TAB_FINDER		0x00010000
#define BOOLEAN_BIT_TAB_GENERAL		0x00020000
#define BOOLEAN_BIT_TAB_STYLE		0x00040000
#define BOOLEAN_BIT_TAB_CLASS		0x00080000
#define BOOLEAN_BIT_TAB_PROCESS		0x00100000
#define BOOLEAN_BIT_TAB_MANI		0x00200000
#define BOOLEAN_BIT_TAB_MANI_MSGCB	0x00400000

#define BOOLEAN_BIT_OPTIONUPDATE	BOOLEAN_BIT_ONCELOCK
#define BOOLEAN_BIT_ONCEUPDATE		BOOLEAN_BIT_ONCELOCK

//Start define BOOLEAN_BIT_TABS...
#define BOOLEAN_BIT_TABS	\
	(BOOLEAN_BIT_TAB_FINDER|\
	BOOLEAN_BIT_TAB_GENERAL|\
	BOOLEAN_BIT_TAB_STYLE|	\
	BOOLEAN_BIT_TAB_CLASS|	\
	BOOLEAN_BIT_TAB_PROCESS|\
BOOLEAN_BIT_TAB_MANI)
//End define BOOLEAN_BIT_TABS...

#define TIMER_FORWARD_HOOK_SPYWNDINFO			10
#define TIMER_FORWARD_HOOK_SPYWNDINFO_TIMEOUT	11
#define TIMER_FORWARD_SHOWLONGCMDLINE			12

#define IDM_MINIMIZE_TO_TRAY		41000
/////////////////////////////////////////////////////////////////////////////
typedef struct tag_WS_Instances
{
	HINSTANCE wndspy_dll;
	HINSTANCE psapi_dll;
	HINSTANCE user32_dll;
	HINSTANCE uxtheme_dll;
	
}HINSTANCE_LIBRARYS;

typedef struct tag_SpyHotKeyInfo
{
	WORD wEnableFlag;
	WORD Hotkey;
	WORD wDelay;
	POINTS ptsOffset;
	
}SPY_HOTKEY_INFO;

typedef struct tag_WS_Options
{
	BOOL IsShareDll;
	BOOL IsPrefix;
	BOOL IsAutoHide;
	BOOL IsShowTrayIcon;
	BOOL IsMinToTray;
	BOOL IsLockUpdate;
	BOOL IsTopMost;
	BOOL IsNoInvisible;
	BOOL IsDigFind;
	BOOL IsSelfFind;
	BOOL IsTrans;
	BOOL IsShowPosCo;
	BOOL IsPlaySound;
	BOOL IsMultiInst;

	COLORREF color;
	// wFlag can be one of the following Vaules:
	// DWR_FLAG_FOCUS_RECT, DWR_FLAG_CUSTOM_PEN, DWR_FLAG_FOCUS_BLOCK
	// and DWR_FLAG_LOCKUPATE , DWR_FLAG_LOCKUPATE can also be combined with.
	WORD wFlag;
	WORD siAlpha;
	SPY_HOTKEY_INFO SpyHotkey;
	
}WS_APP_OPTIONS;

#define FINDWINDOW_OPTION ((DWORD)MAKELONG(g_option.IsDigFind, g_option.IsNoInvisible))

//////////////////////////////////////////////////////////////////////////

#define TAB_FINDER			0
#define TAB_GENERAL			1
#define	TAB_STYLES			2
#define TAB_CLASS			3
#define TAB_PROCESS			4
#define TAB_MANI			5
#define C_MAIN_TAB_PAGES	6

typedef struct tag_ChildDlgInfo
{
	HWND hwnd;
	DLGTEMPLATE* apRes;
	DLGPROC dlgproc;
	BOOL bIsResLoaded;

} CHILDDLGINFO;

typedef struct tag_DlgHeader 
{
	/*
	* 	CDI[0] >> Finder
	* 	CDI[1] >> General
	* 	CDI[2] >> Styles
	* 	CDI[3] >> Class
	* 	CDI[4] >> Process
	*	CDI[5] >> Mani
	*/
	CHILDDLGINFO CDI[C_MAIN_TAB_PAGES];
	BOOL bIsCDILoaded;
	INT iCurrent;
	HWND hwndCurrent;
	HWND hwndTab;
	RECT rcDisplay;

} DLGHDR;

//////////////////////////////////////////////////////////////////////////
//1>> dwProcessID and pe32.th32ProcessID 
//2>> ProcStrs.szszExePath and pe32.szExeFile
//and...
//some member-vars in inner-struct pe32 are multiple usages,
//for example: pe32.dwFlags is used as a process' priority DWORD.
typedef struct tag_Spy_ProcessInfo
{
	DWORD dwProcessID; 
	DWORD dwThreadID;
	//pe32.th32DefaultHeapID for SepcPathNum.
	PROCESSENTRY32 pe32; 
	//See hsCommon\ProcessFunc.h
	PROCESS_STRINGS ProcStrs; 
	
#ifdef _HS_ENABLE_ITHREADPRIORITY
	INT iThreadPriority; //GetThreadPriority()
#endif
	
} SPY_PROCESSINFO;


typedef struct tag_Spy_WindowInfo
{
	HWND hwnd;
#define C_SPYINFO_HWND	5
	/*
	* 	hwndArray[0] >> hwnd Owner
	* 	hwndArray[1] >> hwnd Parent
	* 	hwndArray[2] >> hwnd FirstChild
	* 	hwndArray[3] >> hwnd Next
	* 	hwndArray[4] >> hwnd Previous
	*/
	HWND hwndArray[C_SPYINFO_HWND];
	
#define MAX_CAPTION_STRBUF_LEN	320
	BOOL isLargeText;
	TCHAR szCaption[MAX_CAPTION_STRBUF_LEN];
	TCHAR szClassName[MAX_CLASS_NAME]; //GetClassName()
	
}SPY_WININFO;

typedef struct tag_Spy_WindowInfoEx
{
	BOOL IsSelf;
	BOOL IsWndVisable;
	BOOL IsWndEnable;
	HINSTANCE hWndInstance;
	WNDPROC wndproc;
	WINDOWINFO wi;
	WNDCLASSEX wcex;
	SPY_WININFO swi;
	TCHAR szWndModuleName[MAX_PATH];
	TCHAR szClassModuleName[MAX_PATH];
#define MAX_MENU_NAME_LEN	128
	TCHAR szClassMenuName[MAX_MENU_NAME_LEN];
	
}SPY_WININFOEX;

//////////////////////////////////////////////////////////////////////////

#define ST_END_NODE							0xFFFF
#define ST_END								0xFFFFFFFF
#define ST_FLAG_ISEX						0x0010
#define ST_FLAG_CTRLEX						0x0020
#define ST_FLAG_RESETSTATE					0x0001

#define LINK_KEEPSTATE						0xFFFF0000
#define LINK_RESETSTATE						0xFFFF0001
#define LINK_KEEPSTATE_ISEX					0xFFFF0010
#define LINK_RESETSTATE_ISEX				0xFFFF0011
#define LINK_CTRLEX_ISEX					0xFFFF0030

#define Make_SymbolTableItem(_x)			{(LONG)_x, TEXT(#_x)TEXT("\0")}
#define Link_SymbolTable(_wFlag, _x)		{_wFlag, (LPTSTR)_x}
#define End_SymbolTable()					{ST_END, NULL}

typedef struct tab_SymbolTabeItem
{
	LONG lValue;
	LPTSTR szSymbol;

}SYMBOL_TABLE_ITEM;


typedef struct TagSymbolFilterParam
{
	HWND hwndlist;
	SYMBOL_TABLE_ITEM* STitems;
	INT iPos;
	DWORD dwVal;

}SYMBOL_FILTER_PARAM;

//////////////////////////////////////////////////////////////////////////

#include "WndSpyLib\LibTypeDef.h"

//////////////////////////////////////////////////////////////////////////

typedef VOID (*LPFSPYLIB_INITDATA)(SPYAPPDATA*);
typedef VOID (*LPFSPYLIB_BASEGETWNDINFO)(HWND, SPY_WININFOEX*, HWND);
typedef BOOL (*LPFSPYLIB_HOOKGETWNDINFO)(HWND, DWORD, HWND);
typedef BOOL (*LPFSPYLIB_HOOKGETPSW)(HWND, DWORD, LPTSTR);
typedef INT (*LPFSPYLIB_READWNDINFO)(SPY_PROCESSINFO*, SPY_WININFOEX*);

#define SpyLib_InitData(_SpyAppData) \
lpfSpyLibInitHookData(_SpyAppData)
#define SpyLib_BaseGetWndInfoEx(_hwndTag, _pSWIex, _hwndBK) \
lpfSpyLibBaseGetWndInfoEx(_hwndTag, _pSWIex, _hwndBK)
#define SpyLib_HookGetWndInfoEx(_hwndTag, _idThread, _hwndBK) \
lpfSpyLibHookGetWndInfoEx(_hwndTag, _idThread, _hwndBK)
#define SpyLib_ReadWndInfoEx(_pSPI, _pSWIex) \
lpfSpyLibReadWndInfoEx(_pSPI, _pSWIex)
#define SpyLib_HookGetPSW(_hwndTag, _idThread, _szPasswordBuf) \
lpfSpyLibHookGetPSW(_hwndTag, _idThread, _szPasswordBuf)

//////////////////////////////////////////////////////////////////////////
#ifdef WS_LIB_MAP_MSG_PARAM

typedef VOID (*LPFSPYLIB_CLEARHOOK)(HHOOK);
#define SpyClearHook(_hHook) if(lpfSpyLibClearHook) lpfSpyLibClearHook(_hHook)

typedef INT (*LPFSPYLIB_GETDATAFROMSTR)(LPTSTR, PVOID , INT, BOOL, INT);
typedef BOOL (*LPFSPYLIB_MAPDATATOTARGET)(HWND, BOOL, INT, LPTSTR, LPTSTR);

#define GetDataFromNumStr(p1,p2,p3,p4,p5) lpfGetDataFromStr(p1,p2,p3,p4,p5)
#define DoMapDataToTarget(p1,p2,p3,p4,p5) \
if(lpfSpyMapDataToTarget) lpfSpyMapDataToTarget(p1,p2,p3,p4,p5)

#else ////////////////////////////////////////////////////////////////////

#define GetDataFromNumStr(p1,p2,p3,p4,p5) NumStrToStruct(p1,p2,p3,p4,p5)

#endif //WS_LIB_MAP_MSG_PARAM
//////////////////////////////////////////////////////////////////////////

typedef DWORD (WINAPI *LPFGETMODULEFILENAMEEX)(HANDLE, HMODULE, LPTSTR, DWORD);
typedef BOOL (WINAPI *LPFSETLAYEREDWINDOWATTRIBUTES)(HWND, COLORREF, BYTE, DWORD);

#ifdef GetModuleFileNameEx
#undef GetModuleFileNameEx
#endif
#define GetModuleFileNameEx(p1,p2,p3,p4) \
	if(lpfGetModuleFileNameEx) \
lpfGetModuleFileNameEx(p1,p2,p3,p4) 


#ifdef SetLayeredWindowAttributes
#undef SetLayeredWindowAttributes
#endif
#define SetLayeredWindowAttributes(p1,p2,p3,p4) \
	if(lpfSetLayeredWindowAttributes) lpfSetLayeredWindowAttributes(p1,p2,p3,p4) 
//SetLayeredWindowAttributes(p1,p2,p3,p4)

//////////////////////////////////////////////////////////////////////////

#ifndef HS_NO_GLOBALVAR_HERE 
#include "GlobalVar.h"
#endif

#include "SpyFunc.h"
#include "WndSpyGui\WndSpyGui.h"

//////////////////////////////////////////////////////////////////////////

ATOM RegisterClassSpyBackMsgWnd(HINSTANCE hInstance);
ATOM RegisterClassSpyDlgClass(HINSTANCE hInstance);
BOOL CreateSpyLibFile(LPTSTR szDllName);
BOOL LoadSpyLibFuncs(LPTSTR szDllName);

LPTSTR GetInstanceDir(LPTSTR lpStrDirBuf);
//LPTSTR GetIniFileName(LPTSTR szIniFileName);
LPTSTR GetAppProfileName(LPTSTR szProfileName, LPTSTR szFileExt);
#define GetCfgFileName(_szCfgFileName) GetAppProfileName(_szCfgFileName, TEXT(".cfg"))
#define GetIniFileName(_szIniFileName) GetAppProfileName(_szIniFileName, TEXT(".ini"))


VOID WriteAppOption(BOOL isAll);
VOID LoadAppOption(void);

BOOL IsAutoOpenSavedFile(void);
BOOL WriteUserPathToINI(LPTSTR szFilePath, BOOL IsDir, LPTSTR szIniKey);
LPTSTR ReadUserPathFromINI_EX(LPTSTR szDirPathBuf, LPTSTR szIniKey, BOOL isDefInit);

#define ReadUserPathFromINI(_szDirPathBuf, _szIniKey) \
ReadUserPathFromINI_EX(_szDirPathBuf, _szIniKey, TRUE)

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_WNDSPY_H

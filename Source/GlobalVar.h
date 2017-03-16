#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef INC_GLOBALVAR_H
#define INC_GLOBALVAR_H

//////////////////////////////////////////////////////////////////////////
//#include "WndSpy.h"
///////////////////////////////////////////////////////////////////////////
#ifdef INC_GLOBALVAR_CPP
#define WM_QUERYAFXWNDPROC  0x0360
#define WM_SIZEPARENT       0x0361
#define WM_SETMESSAGESTRING 0x0362
#define WM_IDLEUPDATECMDUI  0x0363
#define WM_INITIALUPDATE    0x0364
#define WM_COMMANDHELP      0x0365
#define WM_HELPHITTEST      0x0366
#define WM_EXITHELPMODE     0x0367
#define WM_RECALCPARENT     0x0368
#define WM_SIZECHILD        0x0369
#define WM_KICKIDLE         0x036A
#define WM_QUERYCENTERWND   0x036B
#define WM_DISABLEMODAL     0x036C
#define WM_FLOATSTATUS      0x036D
#define WM_QUERY3DCONTROLS  0x036F
#define WM_SOCKET_NOTIFY    0x0373
#define WM_SOCKET_DEAD      0x0374
#define WM_POPMESSAGESTRING 0x0375
#define WM_HELPPROMPTADDR   0x0376
#define WM_QUEUE_SENTINEL   0x0379
#define WM_ACTIVATETOPLEVEL 0x036E
#define WM_CTLCOLOR         0x0019
#define WM_SYSTEMERROR		0x0017
#define LVM_SETWORKAREA		LVM_FIRST + 65
#endif // INC_GLOBALVAR_CPP
//////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;
extern HINSTANCE_LIBRARYS g_hInst;
extern HACCEL g_hAccel;

extern HWND g_hwnd, g_hwndBK, g_hwndTC;

#ifdef WNDSPY_MAINDLGTOOLTIP
extern HWND g_hwndTTMain;
#endif

extern HWND g_hwndTTFinder, g_hwndTTCalc, g_hwndTTGetIcon;
extern HICON g_hiconFinder, g_hiconCalc, g_hiconGetIcon;
extern HWND g_hwndTT[C_MAIN_TAB_PAGES];

extern HWND g_hwnd_TaskModal;
extern HWND g_hwnd_IconExtr;
extern HWND g_hwnd_AboutWS;
extern HWND g_hwnd_AppSetting;
extern HWND g_hwnd_SpyTarget;
extern HWND g_hwnd_EffectOption;

//////////////////////////////////////////////////////////////////////////
#define C_TOOLS	 4

extern HWND g_hwndTool[C_TOOLS];

#define g_hwnd_WndModEr		g_hwndTool[0]
#define g_hwnd_ColorSpy		g_hwndTool[1]
#define g_hwnd_PSWsniffer	g_hwndTool[2]
#define g_hwnd_WndEnumEr	g_hwndTool[3]

//////////////////////////////////////////////////////////////////////////

extern DLGHDR g_TabDlgHdr;

extern WS_APP_OPTIONS g_option;

extern SHORT g_siWinVer;
extern BOOL g_isHookSignal;
extern BOOL g_IsFinding;
extern BOOL g_IsIdle;

extern DWORD g_dwBitFlag;
//extern DWORD g_dwBuf;
extern HANDLE g_hTarget;
//extern HMENU g_hPopMenu;

extern SPY_WININFOEX g_spyinfo_SWIex;
extern SPY_PROCESSINFO g_spyinfo_SPI;

#define LABEL_WINID_LEN	16
extern TCHAR g_szLabelIdCtrl[LABEL_WINID_LEN];
extern TCHAR g_szLabelIdMenu[LABEL_WINID_LEN];
extern LPCTSTR g_szFormat;
extern const TCHAR *g_szVarFormat[];

#define HEX_FMT_NOPREFIX	g_szVarFormat[0]
#define HEX_FMT_PREFIX		g_szVarFormat[1]

extern TCHAR g_szSpyDllName[MAX_PATH];
extern TCHAR g_strBuf[MAX_STRBUF_LEN];

#ifdef _DEBUG
	extern TCHAR g_debugStr[1024*10];
#endif

//////////////////////////////////////////////////////////////////////////
extern LPFGETMODULEFILENAMEEX lpfGetModuleFileNameEx;
extern LPFSETLAYEREDWINDOWATTRIBUTES lpfSetLayeredWindowAttributes;

extern LPFSPYLIB_INITDATA lpfSpyLibInitHookData;
extern LPFSPYLIB_HOOKGETWNDINFO lpfSpyLibHookGetWndInfoEx;
extern LPFSPYLIB_BASEGETWNDINFO lpfSpyLibBaseGetWndInfoEx;
extern LPFSPYLIB_READWNDINFO lpfSpyLibReadWndInfoEx;
extern LPFSPYLIB_HOOKGETPSW lpfSpyLibHookGetPSW;

#ifdef WS_LIB_MAP_MSG_PARAM
extern LPFSPYLIB_CLEARHOOK lpfSpyLibClearHook;
extern LPFSPYLIB_GETDATAFROMSTR lpfGetDataFromStr;
extern LPFSPYLIB_MAPDATATOTARGET lpfSpyMapDataToTarget;
#endif
//////////////////////////////////////////////////////////////////////////

extern const SYMBOL_TABLE_ITEM ST_WndMsg[];
extern const SYMBOL_TABLE_ITEM ST_SysCursors[];
extern const SYMBOL_TABLE_ITEM ST_BrushColors[];

//////////////////////////////////////////////////////////////////////////

/*
 * Class styles
 */

extern const SYMBOL_TABLE_ITEM ST_ClassStyle[];

/*
 * Window Styles
 */
extern const SYMBOL_TABLE_ITEM ST_WindowStyle[];

extern const SYMBOL_TABLE_ITEM ST_TopWnd[];

extern const SYMBOL_TABLE_ITEM ST_ChildWnd[];

/*
*DialogStyles
*/

extern const SYMBOL_TABLE_ITEM ST_Dialog[];
/*
 * Edit Control Styles
 */
extern const SYMBOL_TABLE_ITEM ST_Edit[];

/*
 * RichEdit Control Styles
 */
extern const SYMBOL_TABLE_ITEM ST_RichEdit[];

/*
*Button Control Styles
*/
extern const SYMBOL_TABLE_ITEM ST_Button[];


/*
*Static Control Styles
*/
extern const SYMBOL_TABLE_ITEM ST_Static[];


/*
*Listbox Styles
*/
extern const SYMBOL_TABLE_ITEM ST_Listbox[];
/*
*ComboBox (and Ex) styles 
*/

extern const SYMBOL_TABLE_ITEM ST_ComboBox[];

/*
*ScrollBar Styles
*/

extern const SYMBOL_TABLE_ITEM ST_ScrollBar[];

/*
*SysPager Styles
*/
extern const SYMBOL_TABLE_ITEM ST_Pager[];

/*
* SysListView32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_ListView[];

/*
* SysTreeView32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_TreeView[];

/*
* SysTabControl32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_TabControl[];

/*
* common control styles 
*/
extern const SYMBOL_TABLE_ITEM ST_HeaderStatusToolbar[];

/*
* SysHeader32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_Header[];

/*
*msctls_statusbar32  styles
*/
extern const SYMBOL_TABLE_ITEM ST_Statusbar[];

/*
* ToolbarWindow32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_Toolbar[];
/*
* ReBarWindow32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_ReBar[];

/*
* tooltips_class32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_Tooltip[];

/*
* msctls_trackbar32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_Trackbar[];

/*
* msctls_updown32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_Updown[];

/*
*msctls_progress32  styles
*/
extern const SYMBOL_TABLE_ITEM ST_Progress[];

/*
* SysAnimate32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_Animate[];

/*
* SysMonthCal32 styles
*/
extern const SYMBOL_TABLE_ITEM ST_MonthCal[];

/*
*SysDateTimePick32  styles
*/
extern const SYMBOL_TABLE_ITEM ST_DateTime[];

/*
*MDIClient  styles
*/
extern const SYMBOL_TABLE_ITEM ST_MDIClient[];

//////////////////////////////////////////////////////////////////////////
#define KWC_WND			0
#define KWC_DLG			1
#define KWC_BTN			2
#define KWC_EDIT		3
#define KWC_REDIT		4
#define KWC_COMBO		5
#define KWC_COMBOEX		6
#define KWC_HEADER		7
#define KWC_HOTKEY		8
#define KWC_LISTBOX		9
#define KWC_LISTVIEW	10
#define KWC_PAGER		11
#define KWC_PROGRESS	12
#define KWC_TREEVIEW	13
#define KWC_TAB			14
#define KWC_TOOLBAR		15
#define KWC_REBAR		16
#define KWC_SCROLL		17
#define KWC_STATUS		18
#define KWC_STATIC		19
#define KWC_TOOLTIP		20
#define KWC_TRACKBAR	21
#define KWC_UPDOWN		22
#define KWC_IPADDRESS	23
#define KWC_ANIMATE		24
#define KWC_DATETIME	25
#define KWC_MONTHCAL	26
#define KWC_MENU		27
#define KWC_MDI			28

extern const SYMBOL_TABLE_ITEM ST_KnownClassList[];

extern const SYMBOL_TABLE_ITEM ST_ClassStyleList[];

//////////////////////////////////////////////////////////////////////////
//#endif //INC_GLOBALVAR_CPP
#endif //INC_GLOBALVAR_H ...END...

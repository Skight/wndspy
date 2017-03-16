#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_WNDDLGCTRL
#define HS_INC_HEADER_WNDDLGCTRL
//////////////////////////////////////////////////////////////////////////

HWND MousePointedDlgItemEx(HWND hwnd, UINT uFlag);

#define MousePointedDlgItem(_hwnd) MousePointedDlgItemEx(_hwnd, CWP_ALL)

#define MousePointedDlgCtrlID(_hDlg) GetDlgCtrlID(MousePointedDlgItem(_hDlg))

void SetWindowVisible(HWND hwnd, UINT idCtrl, BOOL bIshow);
void SetWindowEnable(HWND hwnd, UINT idCtrl, BOOL bIsEnable);

#define EnableDlgItem(_hDlg, _idCtrl, _bIsEnabe)	\
EnableWindow(GetDlgItem(_hDlg, _idCtrl), _bIsEnabe)

#define SetDlgItemFocus(_hDlg, _idCtrl)	\
SendMessage(_hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(_hDlg, _idCtrl), TRUE)
//SetFocus(GetDlgItem(_hDlg, _idCtrl))

#define _HS_SETWINDOWTOPMOST_FUNC
#ifdef _HS_SETWINDOWTOPMOST_FUNC
BOOL SetWindowTopmost(HWND hwndTarget, BOOL bIsTop);
#else
#define SetWindowTopmost(_hwndTarget, _bIsTop)	\
SetWindowPos(_hwndTarget, _bIsTop? HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0, SWP_NOMOVE|SWP_NOSIZE)
#endif

VOID CopyIconToStaticCtrl(HWND hwnd, UINT uIdCtrl, HICON hIcon, BOOL fIcon, INT IconSize);

#define DoSetCtrlIcon(_hwnd, _IdCtrl, _hIcon, _IconSize) \
CopyIconToStaticCtrl(_hwnd, _IdCtrl, _hIcon, TRUE, _IconSize)
#define DoSetCtrlCursor(_hwnd, _IdCtrl, _hCursor, _CursorSize) \
CopyIconToStaticCtrl(_hwnd, _IdCtrl, (HICON)_hCursor, FALSE, _CursorSize)

#define DoUnSetCtrlIcon(_hwnd, _IdCtrl) \
DestroyIcon((HICON)SendDlgItemMessage(_hwnd, _IdCtrl, STM_GETIMAGE, IMAGE_ICON, 0))
#define DoUnSetCtrlCursor(_hwnd, _IdCtrl) \
DestroyCursor((HCURSOR)SendDlgItemMessage(_hwnd, _IdCtrl, STM_GETIMAGE, IMAGE_CURSOR, 0))


void SetButtonDefState(HWND hwnd, UINT idBtn, BOOL IsRemove);
#define AddDefButtonState(_hwnd, _idBtn) \
SetButtonDefState(_hwnd, _idBtn, TRUE);
#define RemoveDefButtonState(_hwnd, _idBtn) \
SetButtonDefState(_hwnd, _idBtn, FALSE);


void BringWindowToForeground(HWND hwnd);

BOOL IsMainWndHidden(HWND hwnd);

void DoEnableDlgItems(HWND hwnd, 
					  INT iIDstart, 
					  INT iIDend, 
					  INT iDdelta, 
					  BOOL bEnable);

BOOL NotifyIconMessage(HWND hwnd, 
					   UINT uTrayIconID, 
					   DWORD dwMessage, 
					   UINT uCallbackMsg, 
					   HICON hIcon, 
					   PCTSTR pszTip, 
					   PCTSTR pszInfoTitle, 
					   PCTSTR pszInfo);

//////////////////////////////////////////////////////////////////////////
HWND Isolation_CreateMyTooltip (HINSTANCE hInstance, 
								HWND hwnd, 
								DWORD dwStyle);

#define CreateMyTooltip(_hwnd)  \
Isolation_CreateMyTooltip(g_hInstance, _hwnd, TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX)

#define CreateMyTooltipEx(_hwnd, _dwStyle)  \
Isolation_CreateMyTooltip(g_hInstance, _hwnd, _dwStyle)

//////////////////////////////////////////////////////////////////////////
void Isolation_AddControlTip(HINSTANCE hInstance, 
							 HWND hwndTT, 
							 HWND hwndDlg, 
							 UINT uControlId, 
							 UINT uFlag, 
							 LPTSTR szTipText);

#define AddControlTipEx(_hwndTT, _hwndDlg, _IdCtrl, _tipText, _uFlag)   \
Isolation_AddControlTip(g_hInstance, _hwndTT, _hwndDlg, _IdCtrl, _uFlag, _tipText)

#define AddControlTip(_hwndTT, _hwndDlg, _IdCtrl, _tipText)  \
Isolation_AddControlTip(g_hInstance, _hwndTT, _hwndDlg, _IdCtrl, \
TTF_SUBCLASS|TTF_IDISHWND, _tipText)

//////////////////////////////////////////////////////////////////////////

int CDECL MessageBoxPrintf (HWND hwndOwner, UINT uType, LPCTSTR szCaption, LPCTSTR szFormat, ...);
void EditPrintf (HWND hwndEdit, LPTSTR szFormat, ...);
void DlgItemPrintf(HWND hwndDlg, int iItemID, LPCTSTR szFormat, ...);
INT MsgBoxPrintf(LPCTSTR szFormat, ...);
//////////////////////////////////////////////////////////////////////////
BOOL IsEditTextExceedDispRect(HWND hwndEdit);
//////////////////////////////////////////////////////////////////////////
BOOL OnToolTipNeedTextNotify(LPTSTR szStrBuf, INT iMaxStrLen, WPARAM wParam, LPARAM lParam);
//////////////////////////////////////////////////////////////////////////
void DoSetEditItemsLayout(HWND hwndParent, INT iMargin, INT iIdStart, INT idEnd , INT iDelta);
//////////////////////////////////////////////////////////////////////////
LRESULT DoSelComboBoxString(HWND hwndCB);

//////////////////////////////////////////////////////////////////////////

BOOL DoRegisterHotKey(HWND hwnd, UINT uidHotkey , WORD wSpyHotkey, BOOL isRegister);

BOOL OnInitComCtrl(DWORD dwICC);

//////////////////////////////////////////////////////////////////////////
extern HHOOK hslib_hHookCenterDlg;
#ifdef _HS_HCD_SETBOSSHWND
extern HWND hslib_hwndBoss;
#endif
LRESULT CALLBACK HookProc_CenterDlg(int nCode, WPARAM wParam, LPARAM lParam);
INT Hs_MessageBox(HWND hwndOwner, LPCTSTR szMsgText, LPCTSTR szCaption, UINT uType, HOOKPROC hookProcCallback);


#ifdef _HS_HCD_SETBOSSHWND
//#Macro SetWndhookCenterDlg(1) with _HS_HCD_SETBOSSHWND set...
#define SetWndhookCenterDlg(_hwnd)	\
hslib_hwndBoss=_hwnd, \
hslib_hHookCenterDlg=SetWindowsHookEx(WH_CALLWNDPROC, HookProc_CenterDlg, NULL, GetCurrentThreadId())
//Macro End SetWndhookCenterDlg(1)
//#Macro UnhookCenterDlg(void)
#define UnSetWndhookCenterDlg()	\
if(UnhookWindowsHookEx(hslib_hHookCenterDlg)) hslib_hwndBoss=NULL,hslib_hHookCenterDlg=NULL
//Macro End UnhookCenterDlg(void)
#else
//#Macro SetWndhookCenterDlg(0)
#define SetWndhookCenterDlg()	\
if(hslib_hHookCenterDlg){UnhookWindowsHookEx(hslib_hHookCenterDlg);} \
hslib_hHookCenterDlg=SetWindowsHookEx(WH_CALLWNDPROC, HookProc_CenterDlg, NULL, GetCurrentThreadId())
//Macro End SetWndhookCenterDlg(0)

//#Macro UnSetWndhookCenterDlg(0)
#define UnSetWndhookCenterDlg()	\
if(UnhookWindowsHookEx(hslib_hHookCenterDlg)) hslib_hHookCenterDlg=NULL
//Macro End UnhookCenterDlg(void)
#endif //end _HS_HCD_SETBOSSHWND

//////////////////////////////////////////////////////////////////////////
#define HS_APPDLG_MOVEEX

#ifdef HS_APPDLG_MOVEEX
extern HHOOK hslib_hHookMoveWndEx;
LRESULT CALLBACK HookProc_WndMoveEx(int nCode,
									WPARAM wParam,
									LPARAM lParam);
//#Macro SetWndhookWndMoveEx(0)
#define SetWndhookWndMoveEx()	\
if(hslib_hHookMoveWndEx){UnhookWindowsHookEx(hslib_hHookMoveWndEx);} \
hslib_hHookMoveWndEx=SetWindowsHookEx(WH_MOUSE, HookProc_WndMoveEx, NULL, GetCurrentThreadId())
//Macro End SetWndhookWndMoveEx(0)

//Macro UnSetWndhookWndMoveEx(0)
#define UnSetWndhookWndMoveEx()	\
if(UnhookWindowsHookEx(hslib_hHookMoveWndEx)) hslib_hHookMoveWndEx=NULL
//Macro End UnSetWndhookWndMoveEx(0)

#endif //HS_APPDLG_MOVEEX
//////////////////////////////////////////////////////////////////////////

//Macro ShowMessageBox(4)
#define ShowMessageBox(_hwnd, _szText, _szCaption, _uType)	\
Hs_MessageBox(_hwnd, _szText, _szCaption, _uType, NULL)
//Macro End ShowMessageBox(4)


//Macro PopMessageBox(1)
#define PopMessageBox(_szText)	\
MessageBox(NULL, _szText, TEXT(""), MB_TOPMOST|MB_TASKMODAL);
//Macro End PopMessageBox(1)


INT Isolation_MessageBoxFromResStrEx(HINSTANCE hInstance, HWND hwnd, 
						 LPTSTR StrText, UINT idStrText, LPTSTR StrCaption, 
						 UINT idStrCaption, UINT uType, LPTSTR lpszUserIcon);

#define MessageBoxFromResStrEx(_hInstance, _hwndOwner, _idStrText, _idStrCaption, _uType, _szUserIcon) \
Isolation_MessageBoxFromResStrEx(_hInstance, _hwndOwner, NULL, \
_idStrText, NULL, _idStrCaption, _uType, _szUserIcon)

#define MessageBoxFromResStr(_hInstance, _hwndOwner, _idStrText, _idStrCaption, _uType) \
MessageBoxFromResStrEx(_hInstance, _hwndOwner, _idStrText, _idStrCaption, _uType&~MB_USERICON, NULL)

#define MessageBox_MsgFromResStr(_hInstance, _hwndOwner, _idStrText, _StrCaption, _uType) \
Isolation_MessageBoxFromResStrEx(_hInstance, _hwndOwner, NULL, \
_idStrText, _StrCaption, NULL, _uType&~MB_USERICON, NULL)

//////////////////////////////////////////////////////////////////////////

VOID CALLBACK TimerProc_Effect_Redraw(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#define SetTimerRedraw(_hwnd, _idTimer, _timedelay) \
SetTimer(_hwnd, _idTimer, _timedelay, TimerProc_Effect_Redraw)

//////////////////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_WNDDLGCTRL
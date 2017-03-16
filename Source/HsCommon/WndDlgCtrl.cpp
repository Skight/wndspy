#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "FileOpEx.h"
#include "WndDlgCtrl.h"

HWND MousePointedDlgItemEx(HWND hwnd, UINT uFlag)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	return ChildWindowFromPointEx(hwnd, pt, uFlag);
}

void DoEnableDlgItems(HWND hwnd, INT iIDstart, INT iIDend, INT iDdelta, BOOL bEnable)
{		
	INT i;
	for(i=iIDstart; i<=iIDend; i+=iDdelta)
	{
		EnableWindow(GetDlgItem(hwnd,i),bEnable);
	}
}


void SetWindowVisible(HWND hwnd, UINT idCtrl, BOOL bIshow)
{
	if(idCtrl)
	{
		hwnd=GetDlgItem(hwnd,idCtrl);
	}
	
	ShowWindow(hwnd, bIshow? SW_SHOW: SW_HIDE);

}
//////////////////////////////////////////////////////////////////////////

void SetWindowEnable(HWND hwnd, UINT idCtrl, BOOL bIsEnable)
{
	if(idCtrl)
	{
		hwnd=GetDlgItem(hwnd,idCtrl);
	}
	if( (bIsEnable?TRUE:FALSE) != 
		(IsWindowEnabled(hwnd)?TRUE:FALSE) )
 	{
		EnableWindow(hwnd, bIsEnable);
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL OnInitComCtrl(DWORD dwICC)
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = dwICC;
	return InitCommonControlsEx(&icex);
}

//////////////////////////////////////////////////////////////////////////

VOID CopyIconToStaticCtrl(HWND hwnd, UINT uIdCtrl, HICON hIcon, BOOL fIcon, INT IconSize)
{

	if(uIdCtrl)
	{
		hwnd=GetDlgItem(hwnd, uIdCtrl);
	}

	if(fIcon)
	{
		DestroyIcon((HICON)
			SendMessage(hwnd, STM_SETIMAGE, IMAGE_ICON, 
			hIcon? (LPARAM)CopyImage(hIcon, IMAGE_ICON, IconSize, IconSize, NULL):NULL));
	}
	else
	{

		DestroyCursor((HCURSOR)
			SendMessage(hwnd, STM_SETIMAGE, IMAGE_CURSOR, 
			hIcon? (LPARAM)CopyImage(hIcon, IMAGE_CURSOR, IconSize, IconSize, NULL):NULL));
	}

	ShowWindow(hwnd, hIcon? SW_SHOW:SW_HIDE);

}

//////////////////////////////////////////////////////////////////////////

void SetButtonDefState(HWND hwnd, UINT idBtn, BOOL IsAdd)
{
	if(idBtn)
	{
		hwnd=GetDlgItem(hwnd,idBtn);
	}
	SetWindowLong(hwnd, GWL_STYLE, IsAdd?
		GetWindowLong(hwnd, GWL_STYLE)|BS_DEFPUSHBUTTON :
		GetWindowLong(hwnd, GWL_STYLE)&~BS_DEFPUSHBUTTON );

}

//////////////////////////////////////////////////////////////////////////

#ifdef _HS_SETWINDOWTOPMOST_FUNC

BOOL SetWindowTopmost(HWND hwndTarget, BOOL bIsTop)
{

	if(bIsTop)
	{
		if(GetWindowLong(hwndTarget,GWL_EXSTYLE)&WS_EX_TOPMOST)
		{
			return TRUE;
		}
		SetWindowPos(hwndTarget,HWND_NOTOPMOST,0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
		Sleep(10);
	}

	return SetWindowPos(hwndTarget, 
		bIsTop ? HWND_TOPMOST: HWND_NOTOPMOST, 
		0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
}

#endif

//////////////////////////////////////////////////////////////////////////

void BringWindowToForeground(HWND hwnd)
{
	SetForegroundWindow(hwnd);
	ShowWindow(hwnd, SW_RESTORE);
}


BOOL IsMainWndHidden(HWND hwnd)
{
	return (IsIconic(hwnd) || 
			!IsWindowVisible(hwnd));

} //IsMainWndVisible();

//////////////////////////////////////////////////////////////////////////
BOOL NotifyIconMessage(HWND hwnd, 
					   UINT uTrayIconID, 
					   DWORD dwMessage, 
					   UINT uCallbackMsg, 
					   HICON hIcon, 
					   PCTSTR pszTip, 
					   PCTSTR pszInfoTitle, 
					   PCTSTR pszInfo)
{
#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif	
	
	NOTIFYICONDATA nid;
	
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = uTrayIconID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage = uCallbackMsg;
	nid.hIcon = hIcon;

	if (lstrlen(pszTip))
	{
		lstrcpyn(nid.szTip, pszTip, sizeof(nid.szTip));
		nid.uFlags |= NIF_TIP;
	}
	if (lstrlen(pszInfoTitle) && lstrlen(pszInfo))
	{
		lstrcpyn(nid.szInfo, pszInfo, sizeof(nid.szInfo));
		lstrcpyn(nid.szInfoTitle, pszInfoTitle, sizeof(nid.szInfoTitle));
		nid.uFlags |= NIF_INFO;
		nid.uTimeout = 15000; //10s||30s
		nid.dwInfoFlags = NIIF_INFO;
	}


	return Shell_NotifyIcon(dwMessage, &nid);
	
} //NotifyIconMessage()


//////////////////////////////////////////////////////////////////////////

HWND Isolation_CreateMyTooltip (HINSTANCE hInstance, HWND hwnd, DWORD dwStyle)
{
	
	TOOLINFO ti;
	HWND hwndTT;
	
	hwndTT=CreateWindowEx(WS_EX_TOPMOST,
        TOOLTIPS_CLASS,
        NULL,
        WS_POPUP|dwStyle,		
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hwnd,
        NULL,
        hInstance,
        NULL
        );
	
    SetWindowPos(hwndTT,
        HWND_TOPMOST,
        0,0,0,0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); //ÖÃ¶¥...
	
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.hinst = hInstance;
	ti.uId = 0;
	ti.lpszText = NULL;
	ti.rect.left = 0;    
	ti.rect.top = 0;
	ti.rect.right = 0;
	ti.rect.bottom = 0;
	
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000);
 	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 500);
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_RESHOW, 10000);

	return hwndTT;
	
}
//////////////////////////////////////////////////////////////////////////

void Isolation_AddControlTip(HINSTANCE hInstance, HWND hwndTT, HWND hwndDlg, 
							 UINT uControlId, UINT uFlag, LPTSTR szTipText) 
{
	
	HWND hwndCtrl=GetDlgItem(hwndDlg,uControlId);
	TOOLINFO ti={0};
	RECT rect;
	
	if(!hwndCtrl)
	{
		return;
	}
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = uFlag;
	ti.hinst = hInstance;
	ti.lpszText = szTipText;

	if(uFlag&TTF_IDISHWND)
	{
		ti.hwnd = hwndDlg;
		ti.uId = (UINT_PTR)hwndCtrl;
	}
	else
	{
		ti.hwnd = hwndCtrl;
		ti.uId = uControlId;
		GetClientRect (hwndCtrl, &rect);
		ti.rect.left = rect.left;    
		ti.rect.top = rect.top;
		ti.rect.right = rect.right;
		ti.rect.bottom = rect.bottom;
	}

	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

}

//////////////////////////////////////////////////////////////////////////

HHOOK hslib_hHookCenterDlg=NULL;

#ifdef _HS_HCD_SETBOSSHWND
HWND hslib_hwndBoss=NULL;
#endif

LRESULT CALLBACK HookProc_CenterDlg(int nCode,
    WPARAM wParam,
    LPARAM lParam)
{
	LRESULT lRet=0;
	if(nCode<0) 
	{
		lRet=CallNextHookEx(hslib_hHookCenterDlg,nCode,wParam,lParam);
	}

	if( ((CWPSTRUCT*)lParam)->message==WM_SHOWWINDOW && 
		((CWPSTRUCT*)lParam)->wParam==TRUE )
	{
		if( !IS_FLAG_SETTED(GetWindowLong(((CWPSTRUCT*)lParam)->hwnd, GWL_STYLE), WS_CHILD) 
			&& CheckWindowClass(((CWPSTRUCT*)lParam)->hwnd, WNDCLASS_DLG) )
		{

#ifdef _HS_HCD_SETBOSSHWND
				MoveWndToWndCenter( ((CWPSTRUCT*)lParam)->hwnd, 
					hslib_hwndBoss==NULL ? 
					GetWindow(((CWPSTRUCT*)lParam)->hwnd, GW_OWNER): hslib_hwndBoss);
#else
				MoveWndToWndCenter( ((CWPSTRUCT*)lParam)->hwnd, 
					GetWindow(((CWPSTRUCT*)lParam)->hwnd, GW_OWNER));
#endif
		}
	}

	return 0;

}

#ifdef HS_APPDLG_MOVEEX

#define TIMER_DELAY_CHECKDISPATCHNCBTNDOWN		0xEEEF

VOID CALLBACK TimerProc_Delay_CheckDispatchNCBtnDown(HWND hwnd, 
													UINT uMsg, 
													UINT_PTR idEvent, 
													DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	if(GetForegroundWindow()==hwnd && GetAsyncKeyState(VK_LBUTTON))
	{
		PostMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
	}
}

HHOOK hslib_hHookMoveWndEx=NULL;

LRESULT CALLBACK HookProc_WndMoveEx(int nCode,
									WPARAM wParam,
									LPARAM lParam)
{

	LRESULT lRet=0;

	if(nCode<0) 
	{
		lRet=CallNextHookEx(hslib_hHookCenterDlg,nCode,wParam,lParam);
	}
	
	if( wParam==WM_LBUTTONDOWN )
	{
		if( !IS_FLAG_SETTED(GetWindowLong(((MOUSEHOOKSTRUCT*)lParam)->hwnd, GWL_STYLE), WS_CHILD) )
		{
			if( IS_FLAG_SETTED
				(GetWindowLong(((MOUSEHOOKSTRUCT*)lParam)->hwnd, GWL_EXSTYLE), 
				WS_EX_CONTEXTHELP) )
			{
				HWND hwndMouseOn;
				hwndMouseOn=MousePointedDlgItemEx(
					((MOUSEHOOKSTRUCT*)lParam)->hwnd, 
					CWP_SKIPINVISIBLE|CWP_SKIPDISABLED|CWP_SKIPTRANSPARENT);

				if( hwndMouseOn!=((MOUSEHOOKSTRUCT*)lParam)->hwnd &&  
					GetWindowLong(hwndMouseOn, GWL_STYLE)&(SS_NOTIFY|SS_SUNKEN|WS_TABSTOP) )
				{
					return lRet;
				}
			}
			SetTimer(((MOUSEHOOKSTRUCT*)lParam)->hwnd, 
						TIMER_DELAY_CHECKDISPATCHNCBTNDOWN, 
						USER_TIMER_MINIMUM, TimerProc_Delay_CheckDispatchNCBtnDown);
		}
	}
	return lRet;
} //HookProc_WndMoveEx()

#endif //HS_APPDLG_MOVEEX

INT Hs_MessageBox(HWND hwndOwner, LPCTSTR szMsgText, LPCTSTR szCaption, UINT uType, HOOKPROC hookProcCallback) 
{
	INT iRet=MB_OK;
	if(hookProcCallback==NULL)
	{
		hookProcCallback=HookProc_CenterDlg;
	}
	if(hslib_hHookCenterDlg){UnhookWindowsHookEx(hslib_hHookCenterDlg);}
	hslib_hHookCenterDlg=SetWindowsHookEx(WH_CALLWNDPROC, hookProcCallback, NULL, GetCurrentThreadId());
#ifdef HS_APPDLG_MOVEEX
	if(hslib_hHookMoveWndEx){UnhookWindowsHookEx(hslib_hHookMoveWndEx);}
	hslib_hHookMoveWndEx=SetWindowsHookEx(WH_MOUSE, HookProc_WndMoveEx, NULL, GetCurrentThreadId());
#endif

	iRet=MessageBox(hwndOwner,  szMsgText, szCaption, uType);

#ifdef HS_APPDLG_MOVEEX
	UnhookWindowsHookEx(hslib_hHookMoveWndEx);
	hslib_hHookMoveWndEx=NULL;
#endif
	UnhookWindowsHookEx(hslib_hHookCenterDlg);
	hslib_hHookCenterDlg=NULL;
	return iRet;
}

//////////////////////////////////////////////////////////////////////////

INT Isolation_MessageBoxFromResStrEx(HINSTANCE hInstance, HWND hwnd, 
						 LPTSTR StrText, UINT idStrText, LPTSTR StrCaption, 
						 UINT idStrCaption, UINT uType, LPTSTR lpszUserIcon)
{
	INT iRet;
	TCHAR szMsgBuf[MAX_STRBUF_LEN];
	TCHAR szCaptionBuf[MAX_PATH];
	LPTSTR lpszMsg=szMsgBuf;
	LPTSTR lpszCaption=szCaptionBuf;

	if(idStrText)
	{
		LoadString(hInstance, idStrText, lpszMsg, MAX_STRBUF_LEN);
	}
	else
	{
		lpszMsg=StrText;
	}
	
	if(idStrCaption)
	{
		LoadString(hInstance, idStrCaption, lpszCaption, MAX_PATH);
	}
	else
	{
		lpszCaption=StrCaption;
	}

	if( IS_FLAG_SETTED(uType, MB_USERICON) )
	{
		MSGBOXPARAMS mbp={0};
		mbp.cbSize=sizeof(MSGBOXPARAMS);
		mbp.hwndOwner=hwnd;
		mbp.hInstance=hInstance;
		mbp.lpszText=lpszMsg;
		mbp.lpszCaption=lpszCaption;
		mbp.dwStyle=uType;
		mbp.lpszIcon=lpszUserIcon;
		//mbp.dwContextHelpId=0;
		//mbp.lpfnMsgBoxCallback=NULL;
		mbp.dwLanguageId=LANG_NEUTRAL;

		SetWndhookCenterDlg();
#ifdef HS_APPDLG_MOVEEX
		SetWndhookWndMoveEx();
#endif	
		MessageBeep(MB_ICONASTERISK);
		iRet=MessageBoxIndirect(&mbp);

#ifdef HS_APPDLG_MOVEEX
		UnSetWndhookWndMoveEx();
#endif
		UnSetWndhookCenterDlg();

	}
	else
	{
		iRet=ShowMessageBox(hwnd, lpszMsg, lpszCaption, uType);
	}

	return iRet;

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//The following two functions are copied from Programming Windows thw5th edtion...
int CDECL MessageBoxPrintf (HWND hwndOwner, UINT uType, LPCTSTR szCaption, LPCTSTR szFormat, ...)
{
     TCHAR   szBuffer [1024] ;
     va_list pArgList ;

          // The va_start macro (defined in STDARG.H) is usually equivalent to:
          // pArgList = (char *) &szFormat + sizeof (szFormat) ;

     va_start (pArgList, szFormat) ;

          // The last argument to wvsprintf points to the arguments

     wvsprintf (szBuffer, szFormat, pArgList) ;

          // The va_end macro just zeroes out pArgList for no good reason

     va_end (pArgList) ;

     return ShowMessageBox(hwndOwner, szBuffer, szCaption, uType) ;
}
////////////////////////////////////////////////////////////

void EditPrintf (HWND hwndEdit, LPTSTR szFormat, ...)

{
	TCHAR  szBuffer [1024];
	va_list	pArgList;
	
	va_start (pArgList, szFormat);
	
	wvsprintf (szBuffer, szFormat, pArgList);
	
	va_end (pArgList);

	lstrcat(szBuffer,TEXT("\r\n"));
	
	SendMessage (hwndEdit, EM_SETSEL,GetWindowTextLength(hwndEdit), -1);
	
	SendMessage (hwndEdit, EM_REPLACESEL, FALSE, (LPARAM) szBuffer);
	
	SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0);
	
}
////////////////////////////////////////////////////////////////////////


INT MsgBoxPrintf(LPCTSTR szFormat, ...)
{
     TCHAR szBuffer[1024];
     va_list pArgList;

     va_start (pArgList, szFormat);
     wvsprintf (szBuffer, szFormat, pArgList);
     va_end (pArgList) ;

     return ShowMessageBox(NULL, szBuffer, TEXT(""), MB_TOPMOST);
}

void DlgItemPrintf(HWND hwndDlg, int iItemID, LPCTSTR szFormat, ...)
{
	
	TCHAR  szBuffer [1024]; //max to 1024
	
	va_list	pArgList;
	
	va_start (pArgList, szFormat);
	
	wvsprintf (szBuffer, szFormat, pArgList);
	
	va_end (pArgList);

// 	if(iItemID)
// 	{
		SendDlgItemMessage(hwndDlg, iItemID, WM_SETTEXT, 0, (LPARAM)szBuffer);
// 	}
// 	else
// 	{
//		SendMessage(hwndDlg, WM_SETTEXT, 0, (LPARAM)szBuffer);
//	}
}

////////////////////////////////////////////////////////////////////////
BOOL IsEditTextExceedDispRect(HWND hwndEdit)
{
	int iLen;
	TCHAR szEditBuf[MAX_CLASS_NAME];
	HDC hdc;
	HGDIOBJ hfontPre;
	SIZE size;
	RECT rect;

	if( iLen=GetWindowText(hwndEdit, szEditBuf,MAX_CLASS_NAME) )
	{
		hdc=GetDC(hwndEdit);
		hfontPre=SelectObject(hdc,(HGDIOBJ)SendMessage(hwndEdit,WM_GETFONT,0,0));
		GetTextExtentPoint32(hdc,szEditBuf,iLen,&size);
		SelectObject(hdc,hfontPre);
		ReleaseDC(hwndEdit,hdc);
		SendMessage(hwndEdit,EM_GETRECT,0,(LPARAM)&rect);

		return ( (size.cx) > (rect.right-rect.left) ||
			size.cy * SendMessage(hwndEdit,EM_GETLINECOUNT,0,0) > (rect.bottom-rect.top) );
	}
	else
	{
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////

BOOL OnToolTipNeedTextNotify(LPTSTR szStrBuf, INT iMaxStrLen, WPARAM wParam, LPARAM lParam)
{
	INT iTextLen;
	
	if(	((LPNMTTDISPINFO)lParam)->hdr.code==TTN_NEEDTEXT )
	{
		((LPNMTTDISPINFO)lParam)->lpszText=NULL;
		if(IsEditTextExceedDispRect((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom))
		{
			iTextLen=GetWindowTextLength((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom);
			if( iTextLen > 0 )
			{
				if(iTextLen > iMaxStrLen-1)
				{
					GetWindowText((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom, szStrBuf, iMaxStrLen-4);
					lstrcpyn(szStrBuf + min(lstrlen(szStrBuf), iMaxStrLen-4), TEXT("..."), 4);
					//szStrBuf is a global buffer, lstrcat() might cause sync-problem sometimes...
				}
				else
				{
					GetWindowText((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom, szStrBuf, iMaxStrLen);
				}
				((LPNMTTDISPINFO)lParam)->lpszText=szStrBuf;
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////
void DoSetEditItemsLayout(HWND hwndParent, INT iMargin, INT iIdStart, INT idEnd , INT iDelta)
{
	RECT rc;
	INT i;

	for(i=iIdStart; i<idEnd; i+=iDelta)
	{
		if(!GetWindowRect(GetDlgItem(hwndParent,i), &rc) )
		{
			return;
		}

		ScreenToWindowRect(hwndParent,&rc);
		SetWindowPos(GetDlgItem(hwndParent, i+iDelta),NULL, 
			rc.left, rc.bottom + iMargin, NULL, NULL, 
			SWP_NOSIZE|SWP_NOZORDER);
	}
}


//////////////////////////////////////////////////////////////////////////

LRESULT DoSelComboBoxString(HWND hwndCB)
{
	LONG lRet;
	INT iBuf;
	TCHAR szBuf[1024];

	iBuf=SendMessage(hwndCB, WM_GETTEXT, 1024, (LPARAM)szBuf);
	lRet=SendMessage(hwndCB, CB_FINDSTRING, 0, (LPARAM)szBuf);

	if(lRet!=CB_ERR)
	{

		if( iBuf==SendMessage(hwndCB, CB_GETLBTEXTLEN, lRet, 0) )
		{
			iBuf=SendMessage(hwndCB, CB_GETEDITSEL, 0, 0);
			lRet=SendMessage(hwndCB, CB_SETCURSEL, lRet, 0);
			SendMessage(hwndCB, CB_SETEDITSEL, 0, iBuf);
		}
		else
		{
			lRet=CB_ERR;
		}
	}

	return lRet;
}
/////////////////////////////////////////////////////////////////////////
BOOL DoRegisterHotKey(HWND hwnd, UINT uidHotkey , WORD wSpyHotkey, BOOL isRegister)
{
	UINT uModKey=0;
	UINT uVKey=0;
	
	UnregisterHotKey(hwnd, uidHotkey);
	
	if(!isRegister)
	{
		return TRUE;
	}

	if (!wSpyHotkey)
	{
		return FALSE;
	}

	uVKey=LOBYTE(wSpyHotkey);

	if ( HIBYTE(wSpyHotkey) & HOTKEYF_ALT )
	{
		uModKey |= MOD_ALT;
	}
	if ( HIBYTE(wSpyHotkey) & HOTKEYF_SHIFT )
	{
		uModKey |= MOD_SHIFT;
		
	}
	if ( HIBYTE(wSpyHotkey) & HOTKEYF_CONTROL )
	{
		uModKey |= MOD_CONTROL;
	}
	
	return RegisterHotKey(hwnd, uidHotkey, uModKey, uVKey);
}
//////////////////////////////////////////////////////////////////////////


VOID CALLBACK TimerProc_Effect_Redraw(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	InvalidateRect(hwnd, NULL, TRUE);
	SendMessage(hwnd, WM_NCPAINT, 1, 0);
}

//////////////////////////////////////////////////////////////////////////
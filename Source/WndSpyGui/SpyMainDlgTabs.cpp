#include "WndSpyGui.h"
#include "SpyMainDlgTabs.h"
#include "SndMsgFunc.h"

//////////////////////////////////////////////////////////////////////////
//Using global variables: g_TabDlgHdr, g_hInstance, g_hwnd, g_strBuf
//////////////////////////////////////////////////////////////////////////

BOOL SetTabChildDlgInfo(VOID)
{
	if( !g_TabDlgHdr.bIsCDILoaded )
	{
		g_TabDlgHdr.CDI[0].dlgproc = TabDlgProc_Finder;
		g_TabDlgHdr.CDI[1].dlgproc = TabDlgProc_General;
		g_TabDlgHdr.CDI[2].dlgproc = TabDlgProc_Styles;
		g_TabDlgHdr.CDI[3].dlgproc = TabDlgProc_Class;
		g_TabDlgHdr.CDI[4].dlgproc = TabDlgProc_Process;
		g_TabDlgHdr.CDI[5].dlgproc = TabDlgProc_Mani;
		
		g_TabDlgHdr.CDI[0].apRes = LockDlgResource(MAKEINTRESOURCE(IDD_TAB_FINDER)); 
		g_TabDlgHdr.CDI[1].apRes = LockDlgResource(MAKEINTRESOURCE(IDD_TAB_GENERAL)); 
		g_TabDlgHdr.CDI[2].apRes = LockDlgResource(MAKEINTRESOURCE(IDD_TAB_STYLES)); 
		g_TabDlgHdr.CDI[3].apRes = LockDlgResource(MAKEINTRESOURCE(IDD_TAB_CLASS)); 
		g_TabDlgHdr.CDI[4].apRes = LockDlgResource(MAKEINTRESOURCE(IDD_TAB_PROCESS)); 
		g_TabDlgHdr.CDI[5].apRes = LockDlgResource(MAKEINTRESOURCE(IDD_TAB_MANI)); 
		
		g_TabDlgHdr.iCurrent=0;
	}
	
	for(int i=0; i<C_MAIN_TAB_PAGES; i++)
	{
		if(g_TabDlgHdr.CDI[i].apRes==NULL)
		{
			g_TabDlgHdr.bIsCDILoaded=FALSE;
			DEBUG_ERROR_MSG(TEXT("LockDlgResource(g_TabDlgHdr.apRes[i]) error!")) 
				//debug purpose only..
				MessageBeep(MB_ICONEXCLAMATION);
			return FALSE;
		}

#ifdef _FORCE_SET_STYLE
		g_TabDlgHdr.CDI[i].apRes->dwExtendedStyle = WS_EX_CONTROLPARENT;		
		g_TabDlgHdr.CDI[i].apRes->style = WS_CHILD|WS_TABSTOP|DS_3DLOOK|DS_CONTROL|DS_SETFONT;	
#endif
		g_TabDlgHdr.CDI[i].hwnd = CreateDialogIndirect(g_hInstance, 
			g_TabDlgHdr.CDI[i].apRes, g_hwnd, g_TabDlgHdr.CDI[i].dlgproc);
	}

	g_TabDlgHdr.bIsCDILoaded=TRUE;

	return TRUE;
	
} //SetTabChildDlgInfo();

//////////////////////////////////////////////////////////////////////////

VOID WINAPI OnTabSelChanged(VOID) 
{
    int iSel = TabCtrl_GetCurSel(g_TabDlgHdr.hwndTab);
	
	if( iSel < C_MAIN_TAB_PAGES )
	{
		ShowWindow(g_TabDlgHdr.CDI[g_TabDlgHdr.iCurrent].hwnd, SW_HIDE);
		ShowWindow(g_TabDlgHdr.CDI[iSel].hwnd, SW_SHOW);
		g_TabDlgHdr.iCurrent=iSel;
		g_TabDlgHdr.hwndCurrent=g_TabDlgHdr.CDI[iSel].hwnd;
	}
	
	if( !(GetKeyState(VK_RIGHT)&0x8000) && 
		!(GetKeyState(VK_LEFT)&0x8000) )
	{
		SetDlgItemFocus(g_hwnd, IDC_TAB_RECT);
	}
	
} //OnTabSelChanged()

//////////////////////////////////////////////////////////////////////////

VOID WINAPI OnTabbedDialogInit(VOID)
{
	g_TabDlgHdr.hwndTab=GetDlgItem(g_hwnd,IDC_TAB);
	
	//adjust Tab-display area according to IDC_TAB_RECT...
	GetWindowRect(GetDlgItem(g_hwnd,IDC_TAB_RECT), &g_TabDlgHdr.rcDisplay);
	ScreenToClient(g_hwnd, (PPOINT)&g_TabDlgHdr.rcDisplay);
	ScreenToClient(g_hwnd, (PPOINT)&g_TabDlgHdr.rcDisplay+1);
	
	// Load child dialog resource and 
	// initialize other members in struct DLGHDR(Tab-Dialogs-Header)...
	SetTabChildDlgInfo();
	
	TCITEM tie;
	tie.mask=TCIF_TEXT/*|TCIF_IMAGE*/; 
    tie.iImage=-1;
	tie.pszText=g_strBuf;
	
	for(int i=0; i<C_MAIN_TAB_PAGES; i++)
	{
		LoadString(g_hInstance, IDS_TAB_0+i, g_strBuf, 32);
		TabCtrl_InsertItem(g_TabDlgHdr.hwndTab, i, &tie);
	}

	OnTabSelChanged();
	
} //OnTabbedDialogInit() 

//////////////////////////////////////////////////////////////////////////

VOID WINAPI OnChildDialogInit(HWND hwnd) 
{
	SetWindowPos(hwnd, HWND_TOP, 
		g_TabDlgHdr.rcDisplay.left, g_TabDlgHdr.rcDisplay.top, 
		g_TabDlgHdr.rcDisplay.right-g_TabDlgHdr.rcDisplay.left,
		g_TabDlgHdr.rcDisplay.bottom-g_TabDlgHdr.rcDisplay.top, NULL);

	EnableThemeDialogTexture(hwnd, ETDT_ENABLETAB);

	ShowWindow(hwnd, SW_HIDE);
	
} //OnChildDialogInit();

//////////////////////////////////////////////////////////////////////////

#ifdef TAB_DYNAMIC_CREATE

void ReBuildTabCtrl(VOID)
{
	RECT rect;
	
	GetWindowRect(g_TabDlgHdr.hwndTab, &rect);
	ScreenToClient(g_hwnd, (POINT*)&rect);
	ScreenToClient(g_hwnd, ((POINT*)&rect+1));
	
	DestroyWindow(g_TabDlgHdr.hwndTab);
	
	g_TabDlgHdr.hwndTab=CreateWindow(/*WS_EX_NOPARENTNOTIFY,*/
		WC_TABCONTROL, NULL, 
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_TABSTOP|TCS_FOCUSNEVER,
		rect.left, rect.top, rect.right-rect.left,rect.bottom-rect.top,
		g_hwnd, (HMENU)IDC_TAB, g_hInstance, NULL);
	
	SendMessage(g_TabDlgHdr.hwndTab,WM_SETFONT,
		(WPARAM)SendMessage(g_hwnd,WM_GETFONT,0,0),TRUE);
	
	OnTabbedDialogInit();
	
} //ReBuildTabCtrl()

#endif //TAB_DYNAMIC_CREATE

//////////////////////////////////////////////////////////////////////////

VOID OnRefreshFinderDlgEdits(HWND hwnd)
{
	for(int i=IDC_EDIT_HWND; i<=IDC_EDIT_PRE; i++)
	{
		/*InvalidateRect(GetDlgItem(hwnd,i),NULL,1);*/
		SendDlgItemMessage(hwnd, i, WM_NCPAINT, TRUE, 0);
	}
	
} //OnRefreshFinderDlgEdits()

//////////////////////////////////////////////////////////////////////////

VOID DoClearWndSpyText(BOOL bIsAll)
{
	INT i;
	for(i=IDC_EDIT_HWND; i<= IDC_EDIT_PRE; i++)
	{
		SetDlgItemText(g_TabDlgHdr.CDI[0].hwnd, i, TEXT(""));
	}
	if(!bIsAll)
	{ 
		return; 
	}
	for(i=IDC_EDIT_HWND; i<= IDC_EDIT_CLASS; i++)
	{
		SetDlgItemText(g_TabDlgHdr.CDI[1].hwnd, i, TEXT(""));
	}
	for(i=IDC_EDIT_WNDRECT; i<= IDC_EDIT_HINSTANCE; i++)
	{
		SetDlgItemText(g_TabDlgHdr.CDI[1].hwnd, i, TEXT(""));
	}
	for(i=IDC_ICONBTN_BIG; i<=IDC_ICONBTN_MORE; i++)
	{
		ShowWindow(GetDlgItem(g_TabDlgHdr.CDI[1].hwnd, i), SW_HIDE);
	}
	
	SetDlgItemText(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WS, TEXT(""));
	SetDlgItemText(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WSEX, TEXT(""));
	SetWindowVisible(g_TabDlgHdr.CDI[2].hwnd, IDC_STATIC_CTRLWSEX, FALSE);
	SetWindowVisible(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WSEX2, FALSE);
	
	SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_CLASS, TEXT(""));
	for(i=IDC_EDIT_CLSATOM; i<=IDC_EDIT_MODULEFILE; i++)
	{
		SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, i, TEXT(""));
	}
	
	for(i=IDC_SICON_BIG; i<=IDC_SICON_CURSOR; i++)
	{
		ShowWindow(GetDlgItem(g_TabDlgHdr.CDI[3].hwnd, i), SW_HIDE);
	}
	SetWindowVisible(g_TabDlgHdr.CDI[3].hwnd, IDC_BTN_LOCATE, FALSE);
	
	for(i=IDC_EDIT_PID; i<=IDC_EDIT_WNDMODULE; i++)
	{
		SetDlgItemText(g_TabDlgHdr.CDI[4].hwnd, i, TEXT(""));
	}
	SetWindowVisible(g_TabDlgHdr.CDI[4].hwnd, IDC_BTN_LOCATE1, FALSE);
	SetWindowVisible(g_TabDlgHdr.CDI[4].hwnd, IDC_BTN_LOCATE2, FALSE);
	
	SetDlgItemText(g_TabDlgHdr.CDI[5].hwnd, IDC_CBDL_HWND, TEXT(""));
	
	g_dwBitFlag|=BOOLEAN_BIT_TABS;
	if(g_TabDlgHdr.iCurrent) //Refresh if...
	{
		SendMessage(g_TabDlgHdr.CDI[g_TabDlgHdr.iCurrent].hwnd, WM_SHOWWINDOW, TRUE, 0);
	}
				
} //DoClearWndSpyText()

//////////////////////////////////////////////////////////////////////////

VOID CALLBACK  TimerProc_OnViewDepends(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#define OnViewFileDependsFromWndText(_hwndText) \
		SetTimer(_hwndText, (UINT)_hwndText, USER_TIMER_MINIMUM, TimerProc_OnViewDepends)

//////////////////////////////////////////////////////////////////////////

LRESULT APIENTRY SCP_HwndFileEdit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	static HMENU hMenu=NULL;
	static int iTick=-1;
	static bool isRedraw=FALSE;
	static HWND hwndToFlash=NULL;
	
	switch (message)
	{
		
	case WM_TIMER:
		{
			if(iTick-- > 0)
			{
				if(IsWndOverWnd((HWND)wParam, g_hwnd))
				{
					isRedraw=TRUE;
				}
				DrawWindowRect((HWND)wParam, DWR_FLAG_FOCUS_BLOCK|DWR_FLAG_TOPMOST, RGB(0,0,0));
			}
			else //now iTick==-1;
			{
				KillTimer(hwnd, wParam);
				
#ifdef SCP_HWNDFILEEDIT_LOCKUPDATE
				LockWindowUpdate(NULL);
#endif
				if(!IsWindowEnabled(g_hwnd))
				{
					EnableWindow(g_hwnd, TRUE);
				}
				else if(isRedraw)
				{
					isRedraw=FALSE;
					InvalidateRect(g_hwnd, NULL,TRUE);
					SendMessage(g_hwnd, WM_NCPAINT, 1, 0);
					InvalidateRect((HWND)wParam, NULL, TRUE);
					PostMessage((HWND)wParam, WM_NCPAINT, 1, 0);
				}
				
				DoRefreshIfWndIsTaskBar((HWND)wParam);
				
				if( IsMenu(hMenu) )
				{
					EnableMenuItem(GetSubMenu(hMenu,0), IDM_EDIT_HIGHLIGHT, MF_BYCOMMAND|MF_ENABLED);
				}
				
				hwndToFlash=NULL;
			}
			
			break;
		} //WM_TIMER
		
	case WM_MY_HIGHLIGHT:
		{
			if( hwndToFlash==NULL && IsWindow((HWND)wParam) )
			{
				hwndToFlash=(HWND)wParam;

				if( IsWndOverWnd(hwndToFlash, g_hwnd) )
				{
					EnableWindow(g_hwnd, FALSE);
				}
#ifdef SCP_HWNDFILEEDIT_LOCKUPDATE
				LockWindowUpdate(GetDesktopWindow());
#endif

				iTick=(INT)(lParam*2); //lParam*2 //blink (lParam) times
				SetTimer(hwnd, (UINT_PTR)hwndToFlash, 100, NULL);
			}
			return 0;
		}

	case WM_CONTEXTMENU:
		{
			RECT rect;
			POINT point;
			HWND hwndBuf;
			HMENU hSubMenu;
			INT iMenuID;
			DWORD dwSelStart, dwSelEnd;
			
			GetWindowText(hwnd, g_strBuf, MAX_PATH);
			
			if(IsFileExists(g_strBuf))
			{
				hwndBuf=NULL;
			}
			else
			{
				if( GetDlgCtrlID(hwnd)==IDC_EDIT_HWND )
				{
					hwndBuf=GetParent(hwnd);
					if( GetParent(hwndBuf)==g_hwnd )
					{
						hwndBuf=g_spyinfo_SWIex.swi.hwnd;
					}
					else
					{
						SendMessage(hwndBuf, WM_MY_GETHWNDTARGET, 0, (LPARAM)&hwndBuf);
					}
				}
				else if( g_TabDlgHdr.iCurrent==TAB_MANI && 
					IsChild(g_TabDlgHdr.CDI[TAB_MANI].hwnd, hwnd) )
				{
					hwndBuf=g_smHdr.hwndTarget;
				}
				else
				{
					hwndBuf=(HWND)GetWindowHex(hwnd);
				}
				
				if( !IsWindow(hwndBuf) )
				{
					break;
				}
			}
			
			hMenu=LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_EDIT));
			hSubMenu=GetSubMenu(hMenu, 0);
			
			if(hwndBuf==NULL) //It's not HWND edit but a file path edit, see above...
			{
				if( !SearchPath(NULL, TEXT("DEPENDS.EXE"), NULL, NULL, NULL, NULL) && 
					!IsRegKeyExits(HKEY_CLASSES_ROOT, TEXT("exefile\\shell\\View Dependencies"))
					)
				{
					DeleteMenu(hSubMenu, IDM_DEPEND, MF_BYCOMMAND);
				}
			}
			else
			{
				//8 is the Seperator index defined in Menu Res.
				DeleteMenu(hSubMenu, 8, MF_BYPOSITION); 
				DeleteMenu(hSubMenu, IDM_LOCATEFILE, MF_BYCOMMAND);
				DeleteMenu(hSubMenu, IDM_DEPEND, MF_BYCOMMAND);
			}
			SendMessage(hwnd, EM_GETSEL, (WPARAM)&dwSelStart, (LPARAM)&dwSelEnd);
			if( dwSelStart==dwSelEnd )
			{
				EnableMenuItem(hSubMenu, IDM_EDIT_COPY, MF_BYCOMMAND|MF_GRAYED);
			}
			else if( dwSelStart==0 && dwSelEnd==(DWORD)GetWindowTextLength(hwnd) )
			{
				EnableMenuItem(hSubMenu, IDM_EDIT_SELALL, MF_BYCOMMAND|MF_GRAYED);
			}
			
			if(GetWindowLong(hwnd,GWL_STYLE)&ES_READONLY)
			{
				DeleteMenu(hSubMenu, 4, MF_BYPOSITION);
				DeleteMenu(hSubMenu, IDM_EDIT_CUT, MF_BYCOMMAND);
				DeleteMenu(hSubMenu, IDM_EDIT_PASTE, MF_BYCOMMAND);
				DeleteMenu(hSubMenu, IDM_EDIT_DEL, MF_BYCOMMAND);
			}
			else
			{
				EnableMenuItem(hSubMenu, IDM_EDIT_CUT, 
					dwSelStart!=dwSelEnd ? MF_ENABLED : MF_GRAYED);
				EnableMenuItem(hSubMenu, IDM_EDIT_DEL, 
					dwSelStart!=dwSelEnd ? MF_ENABLED : MF_GRAYED);
				EnableMenuItem(hSubMenu, IDM_EDIT_PASTE, 
					IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
			}
			
			if ( IsWndRectEmpty(hwndBuf) ||
				IsIconic(GetAncestor(hwndBuf, GA_ROOT)) )
			{
				DeleteMenu(hSubMenu, (GetWindowLong(hwnd,GWL_STYLE)&ES_READONLY) ? 2:6, MF_BYPOSITION);
				DeleteMenu(hSubMenu, IDM_EDIT_HIGHLIGHT, MF_BYCOMMAND);
			}
			else if( IsWindow(hwndToFlash) )
			{	 
				EnableMenuItem(hSubMenu, IDM_EDIT_HIGHLIGHT, MF_BYCOMMAND|MF_GRAYED);
			}
			
			GetCursorPos(&point);
			GetWindowRect(hwnd, &rect);
			if(!PtInRect(&rect, point))
			{
				point.x=rect.left + ((rect.right-rect.left)>>1);
				point.y=rect.top + ((rect.bottom-rect.top)>>1);
			}
			
			iMenuID = TrackPopupMenu(hSubMenu, 
				TPM_RETURNCMD|TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
			
			DestroyMenu(hMenu);
			hMenu=NULL;
			
			switch( LOWORD(iMenuID) )
			{
				
			case IDM_EDIT_COPY:
				{
					SendMessage(hwnd, WM_COPY, 0, 0);
					break;
				}
			case IDM_EDIT_SELALL:
				{
					SendMessage(hwnd, EM_SETSEL, 0, -1);
					break;
				}
			case IDM_EDIT_PASTE:
				{
					SendMessage(hwnd, WM_PASTE, 0, 0);
					break;
				}
			case IDM_EDIT_CUT:
				{
					SendMessage(hwnd, WM_CUT, 0, 0);
					break;
				}
			case IDM_EDIT_DEL:
				{
					SendMessage(hwnd, WM_CLEAR, 0, 0);	
					break;
				}
			case IDM_EDIT_HIGHLIGHT:
				{
					SendMessage(hwnd, WM_MY_HIGHLIGHT, (WPARAM)hwndBuf, 3);
					break;
				}
			case IDM_LOCATEFILE:
				{
					OnLocateFileFromWndText(hwnd);
					break;
				}
			case IDM_DEPEND:
				{
					OnViewFileDependsFromWndText(hwnd);
					break;
				}
				
			} //switch(iMenuID)

			return 0;
		} //WM_CONTEXTMENU
		
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}//SCP_HwndFileEdit()


VOID CALLBACK TimerProc_OnViewDepends(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	TCHAR szModuleFile[MAX_PATH];
	TCHAR szCmdLine[DEF_CMDLINE_STR_LEN];
	HWND hwndRoot;
	KillTimer(hwnd, idEvent);

	hwndRoot=GetAncestor(hwnd, GA_ROOT);
	SetWindowEnable(hwndRoot, NULL, FALSE);
	SendMessage(hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)szModuleFile);
	if( IsFileExists(szModuleFile) )
	{
		if( 32 >= (INT)ShellExecute(NULL, TEXT("View Dependencies"), szModuleFile, NULL, NULL, SW_SHOWNORMAL) )
		{
			lstrcpyn(szCmdLine, TEXT("DEPENDS.EXE"), DEF_CMDLINE_STR_LEN);
			lstrcat(szCmdLine, TEXT(" \"") );
			lstrcat(szCmdLine, szModuleFile);
			lstrcat(szCmdLine, TEXT("\"") );

#ifdef UNICODE
			WinExecEx(szCmdLine, SW_SHOWNORMAL);
#else
			WinExec(szCmdLine,SW_SHOWNORMAL);
#endif
		}
	}
	SetWindowEnable(hwndRoot, NULL, TRUE);
	//MACRO_TRIM_MEMORY(g_siWinVer>WINVER_WIN9X)
}

//////////////////////////////////////////////////////////////////////////

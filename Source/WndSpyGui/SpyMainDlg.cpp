#include "WndSpyGui.h"
#include "SpyMainDlg.h"
#include "SndMsgFunc.h"
//////////////////////////////////////////////////////////////////////////

VOID OnSetMainDlgMenu(HWND hwnd)
{
	TCHAR szBuf[TINY_STRBUF_LEN]; 
	HMENU hMenu;
	
	hMenu=LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));

#ifndef UNICODE
	if( g_siWinVer<WINVER_WIN2K )
	{
		EnableMenuItem(GetSubMenu(hMenu, 0), 0, MF_GRAYED|MF_BYPOSITION);
		EnableMenuItem(hMenu, IDM_SAVE_MSGSET, MF_GRAYED|MF_BYCOMMAND);
		EnableMenuItem(hMenu, IDM_LOAD_MSGSET, MF_GRAYED|MF_BYCOMMAND);
		EnableMenuItem(hMenu, IDM_SAVE_WNDSTRS, MF_GRAYED|MF_BYCOMMAND);
	}
#endif //!UNICODE

	if( !IsFileExists(GetAppProfileName(g_strBuf, TEXT(".chm"))) )
	{
		DeleteMenu(hMenu, IDM_HELP_CHM, MF_BYCOMMAND);
		//SubMenu "help" index: 3 , the left separate index 0.
		DeleteMenu(GetSubMenu(hMenu, 3), 0, MF_BYPOSITION);
	}

	SetMenu(hwnd,hMenu);
	GetMenuString(hMenu, ID_CMD_TOPMOST, szBuf, TINY_STRBUF_LEN, MF_BYCOMMAND);
	OnSetMenuState(hMenu);

	hMenu = GetSystemMenu(hwnd, FALSE);
	DeleteMenu(hMenu,SC_MAXIMIZE,MF_BYCOMMAND);
	DeleteMenu(hMenu,SC_SIZE,MF_BYCOMMAND);
	InsertMenu(hMenu, GetMenuItemCount(hMenu)-2, 
		MF_STRING|MF_BYPOSITION, ID_CMD_TOPMOST, szBuf);
	
} //OnSetMainDlgMenu()

//////////////////////////////////////////////////////////////////////////

VOID OnSetMenuState(HMENU hMenu)
{
	DWORD dwBuf;
	
	CheckMenuItem(hMenu, IDM_SHOWTRAYICON, 
		MF_BYCOMMAND|(g_option.IsShowTrayIcon?MF_CHECKED:MF_UNCHECKED));
	
	CheckMenuItem(hMenu, ID_CMD_TOPMOST, 
		MF_BYCOMMAND|(g_option.IsTopMost?MF_CHECKED:MF_UNCHECKED));
	
	CheckMenuItem(hMenu, IDM_HEXPREFIX, 
		MF_BYCOMMAND|(g_option.IsPrefix?MF_CHECKED:MF_UNCHECKED));
	
	CheckMenuItem(hMenu, ID_CMD_AUTOHIDE, 
		MF_BYCOMMAND|(g_option.IsAutoHide?MF_CHECKED:MF_UNCHECKED));
	

	CheckMenuItem(hMenu, IDM_TRANS, 
		MF_BYCOMMAND|(g_option.IsTrans?MF_CHECKED:MF_UNCHECKED));
	if(!lpfSetLayeredWindowAttributes)
	{
		EnableMenuItem(hMenu, IDM_TRANS, MF_BYCOMMAND|MF_GRAYED);
	}
	
	CheckMenuItem(hMenu, IDM_NOINVISIBLE, 
		MF_BYCOMMAND|(g_option.IsNoInvisible?MF_CHECKED:MF_UNCHECKED));

	CheckMenuItem(hMenu, IDM_SHOWPOSCO, 
		MF_BYCOMMAND|(g_option.IsShowPosCo?MF_CHECKED:MF_UNCHECKED));
	
	CheckMenuItem(hMenu, IDM_DRAWTOPMOST, 
		MF_BYCOMMAND|((g_option.wFlag&DWR_FLAG_TOPMOST) || 
		(g_option.IsAutoHide && g_option.IsLockUpdate)? MF_CHECKED:MF_UNCHECKED));
	EnableMenuItem(hMenu, IDM_DRAWTOPMOST, 
		MF_BYCOMMAND|
		((g_option.IsAutoHide && g_option.IsLockUpdate)? MF_GRAYED:MF_ENABLED));
	
	if(LOBYTE(g_option.wFlag)==DWR_FLAG_FOCUS_BLOCK)
	{
		dwBuf=IDM_DRAWBLOCK;
	}
	else if(LOBYTE(g_option.wFlag)==DWR_FLAG_CUSTOM_PEN)
	{
		dwBuf=IDM_DRAWLINE;
	}
	else if(LOBYTE(g_option.wFlag)==DWR_FLAG_VITREOUS_PEN)
	{
		dwBuf=IDM_DRAWVITREOUS;
	}
	else
	{
		dwBuf=IDM_DRAWRECT;
	}
	CheckMenuRadioItem(hMenu, IDM_DRAWRECT, IDM_DRAWBLOCK, dwBuf ,MF_BYCOMMAND);

} //OnSetMenuState()

//////////////////////////////////////////////////////////////////////////

VOID OnMainDlgTopMost(BOOL isFromChkBtn)
{
	
	DWORD dwState;
	
	if( isFromChkBtn )
	{
		g_option.IsTopMost=(BOOL)IsDlgButtonChecked(g_hwnd, ID_CMD_TOPMOST);
	}
	else
	{
		g_option.IsTopMost=!g_option.IsTopMost;
		CheckDlgButton(g_hwnd, ID_CMD_TOPMOST, g_option.IsTopMost ? BST_CHECKED : BST_UNCHECKED);
	}

	SetWindowPos(g_hwnd, 
		g_option.IsTopMost? HWND_TOPMOST:HWND_NOTOPMOST, 
		NULL, NULL, NULL, NULL, SWP_NOMOVE|SWP_NOSIZE);
	
	if(g_hwndTC)
	{
		SetWindowPos(g_hwndTC, 
			g_option.IsTopMost? HWND_TOPMOST:HWND_NOTOPMOST, 
			NULL, NULL, NULL, NULL, SWP_NOMOVE|SWP_NOSIZE);
		//SetActiveWindow(g_hwnd);
	}

	dwState=g_option.IsTopMost? MF_CHECKED: MF_UNCHECKED;
	CheckMenuItem(GetMenu(g_hwnd), ID_CMD_TOPMOST, MF_BYCOMMAND|dwState);	
	CheckMenuItem(GetSystemMenu(g_hwnd,FALSE), ID_CMD_TOPMOST, MF_BYCOMMAND|dwState);
	
} //OnMainDlgTopMost()

//////////////////////////////////////////////////////////////////////////

VOID OnShowTrayIcon(VOID)
{
	HICON hIcon;
	TCHAR szBuf[MINI_STRBUF_LEN];
	HMENU hSysMenu=GetSystemMenu(g_hwnd, FALSE);

	if(!g_option.IsShowTrayIcon)
	{
		NotifyIconMessage(g_hwndBK, 
			ID_TRAYICON_MAIN, NIM_DELETE, 
			WM_MY_NOTIFYICON, NULL, NULL, NULL, NULL);

		DeleteMenu(hSysMenu, IDM_MINIMIZE_TO_TRAY, MF_BYCOMMAND);
		DeleteMenu(hSysMenu, GetMenuItemCount(hSysMenu)-1, MF_BYPOSITION);

		if(	IsMainWndHidden(g_hwnd) )
		{
			ShowWindow(g_hwnd, SW_RESTORE);
		}
	}
	else
	{
		GetWindowText(g_hwnd, szBuf, MINI_STRBUF_LEN);

		hIcon=(HICON)LoadImage(g_hInstance, 
			MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON), 
			GetSystemMetrics(SM_CYSMICON), NULL);
			//here, do not use shared icon...
		
		NotifyIconMessage(g_hwndBK, 
			ID_TRAYICON_MAIN, NIM_ADD,
			WM_MY_NOTIFYICON, hIcon, szBuf, NULL, NULL);
		DestroyIcon(hIcon);
		
		LoadString(g_hInstance, IDS_MINI_TO_TRAY, szBuf, MINI_STRBUF_LEN);
		InsertMenu(hSysMenu, GetMenuItemCount(hSysMenu),
			MF_STRING|MF_BYPOSITION, IDM_MINIMIZE_TO_TRAY, szBuf);
		InsertMenu(hSysMenu, GetMenuItemCount(hSysMenu)-1,
			MF_SEPARATOR|MF_BYPOSITION, NULL, NULL);
	}

} //OnShowTrayIcon()

//////////////////////////////////////////////////////////////////////////

//#define MAINDLG_TIMER_SIZE_DELAY

BOOL CALLBACK DlgProc_SpyMain(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	static HWND hwndFG=hwnd;
	INT iBuf;
	
	switch (message)
	{

	case WM_INITDIALOG:
		{
			//g_hwnd should be initialized here, 
			//before calling global-var-referenced functions as OnTabbedDialogInit(), etc...

			g_hwnd=hwnd;
			OnTabbedDialogInit();
			OnSetMainDlgMenu(hwnd);
			DoCreateIconTooltip();

			if( g_option.IsTopMost )
			{
				CheckDlgButton(hwnd, ID_CMD_TOPMOST, BST_CHECKED);
				OnMainDlgTopMost(1);
			}
			g_dwBitFlag|=BOOLEAN_BIT_DLG_TOPCHK;
			
			if(g_option.IsAutoHide)
			{
				CheckDlgButton(hwnd, ID_CMD_AUTOHIDE, BST_CHECKED);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_CMD_AUTOHIDE, 0), 1);
			}
			if(g_option.IsShowTrayIcon)
			{
				g_option.IsShowTrayIcon=FALSE; //reversal it first
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SHOWTRAYICON, 0), 0);
			}
			
			return FALSE;
		}

	case WM_NOTIFY:
		{
			if( ((LPNMHDR)lParam)->code==TCN_SELCHANGE )
			{
				OnTabSelChanged();
			}
			break;
		}

	case WM_WINDOWPOSCHANGED:
		{
			if( IS_FLAG_SETTED(g_dwBitFlag, BOOLEAN_BIT_DLG_TOPCHK) )
			{			
				if( g_option.IsTopMost != (BOOL)
					((GetWindowLong(hwnd,GWL_EXSTYLE)&WS_EX_TOPMOST)==WS_EX_TOPMOST) )
				{
					OnMainDlgTopMost(0);
				}
			}
			break;
		}

	case WM_SYSCOLORCHANGE:
		{
			SendMessage(g_TabDlgHdr.hwndTab, WM_SYSCOLORCHANGE, wParam, lParam);
			break;
		}

	case WM_SYNCPAINT:
		{
			NoTrace_BugTrans(BOOLEAN_BIT_ONCEUPDATE);
			return FALSE;
		}

	case WM_NCPAINT:
		{
			if( wParam==1 && hwnd!=GetForegroundWindow() )
			{
				SetTimer(hwnd, TIMER_REFRESH_DELAY, 50, NULL);
			}
			break;
		}

	case WM_TIMER:
		{
			KillTimer(hwnd, wParam);
			if( wParam==TIMER_REFRESH_DELAY )
			{
				if( (GetWindowThreadProcessId(
					GetForegroundWindow(), NULL)
					!= GetCurrentThreadId())
					&&  IsWindow(g_TabDlgHdr.hwndCurrent) )
				{
					if( hwndFG!=GetForegroundWindow() )
					{
						hwndFG=GetForegroundWindow();
					}
					else //hwndFG==GetForegroundWindow();
					{
						//no repetition...
						break;
					}
					if( !IsIconic(hwnd) && hwndFG!=NULL )
					{
						//TEST: MessageBeep(0);
						RedrawWindow(g_TabDlgHdr.hwndCurrent, 
							NULL, NULL, RDW_ERASE|RDW_NOINTERNALPAINT|RDW_FRAME|
							RDW_INVALIDATE|RDW_ALLCHILDREN);
					}
				}
				return TRUE;
			}
			else if( wParam==TIMER_SC_DELAY )
			{
				ShowWindow(hwnd, SW_MINIMIZE);
				ShowWindow(hwnd, SW_HIDE);
				return TRUE;
			}
#ifdef MAINDLG_TIMER_SIZE_DELAY		//{{
			else if(wParam==TIMER_SIZE_DELAY)
			{
				PostMessage(hwnd, WM_SIZE, 0, 0);
				return TRUE;
			}
#endif //MAINDLG_TIMER_SIZE_DELAY	//}}

			break;
		} //WM_TIMER...

#ifdef MAINDLG_TIMER_SIZE_DELAY		//{{
	case WM_THEMECHANGED:
		{
			SetTimer(hwnd, TIMER_SIZE_DELAY, 15000, NULL);
			break;
		}
#endif //MAINDLG_TIMER_SIZE_DELAY	//}}

	case WM_NCACTIVATE:
		{
			if( wParam==FALSE )
			{
				break;
			}
			else
			{
				iBuf=MousePointedDlgCtrlID(hwnd);
				if( GetAsyncKeyState(VK_LBUTTON) && 
					iBuf>=ID_CMD_TOPMOST && iBuf<=ID_CMD_EXIT )
				{
					break;
				}
			}
		}
		//fall through...
	case WM_INITMENU:
		{
			PostMessage(hwnd, WM_KILLFOCUS, NULL, IDC_TAB_RECT);
			break;
		}

	case WM_KILLFOCUS:
		{
			if( lParam==IDC_TAB_RECT )
			{
				iBuf=GetDlgCtrlID(GetFocus());
				if( iBuf>=ID_CMD_TOPMOST &&	iBuf<=ID_CMD_EXIT )
				{
					SetDlgItemFocus(hwnd, IDC_TAB_RECT);
					if(iBuf==ID_CMD_EXIT)
					{
						RemoveDefButtonState(hwnd, iBuf);
					}
				}
				return TRUE;
			}
			break;
		}

	case WM_SIZE:
		{
			if( SIZE_MINIMIZED == wParam )
			{
				CloseWindow(hwnd);
			}
			else if( SIZE_MAXIMIZED == wParam )
			{
				ShowWindow(hwnd, SW_RESTORE);
			}
			else
			{
				RECT rect, rc;
				GetWindowRect(hwnd, &rect);
				GetWindowRect(GetDlgItem(hwnd, ID_CMD_EXIT), &rc);

				// Calculate prefered size (put results in rc.left and rc.top)...

				rc.left = rc.right + 
					GetSystemMetrics(SM_CXDLGFRAME) + 12 - rect.left;
				rc.top = rc.bottom + GetSystemMetrics(SM_CYDLGFRAME) + 
					(GetSystemMetrics(SM_CYCAPTION) > 20 ? 9:8) - rect.top;

				// if actual size is not, resize it to prefered size...

				if( rect.right-rect.left != rc.left || 
					rect.bottom-rect.top != rc.top )
				{
					SetWindowPos(hwnd, NULL, 0, 0, 
					rc.left, rc.top, SWP_NOMOVE|SWP_NOZORDER);
				}
			}
			break;
		}

	case WM_NCLBUTTONDOWN:
		{
			if( wParam==HTCAPTION && g_bIsNcBtnDownDispatched==FALSE )
			{
				PostMessage(hwnd, WM_KILLFOCUS, NULL, IDC_TAB_RECT);
			}
		}//fall through...
	case WM_GETMINMAXINFO:
	case WM_STYLECHANGING:
	case WM_MENUSELECT:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDOWN:
		{
			return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
		}

	case WM_COMMAND:
		{
			SpyMain_Command_PreRoutine(hwnd, wParam, lParam);

			switch ( LOWORD(wParam) )
			{

			case IDM_SHOWTRAYICON:
				{
					g_option.IsShowTrayIcon = !g_option.IsShowTrayIcon;
					OnShowTrayIcon();
					break;
				}
			case IDM_REFRESH:
				{
					if(IsWindow(g_spyinfo_SWIex.swi.hwnd))
					{
						SetTimer(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
							FINDER_TIMER_GETSPYINFOEX, 150, NULL);
					}
					else if(lParam!=IDM_HEXPREFIX)
					{
						MessageBeep(0);
					}
					return TRUE;
				}
			case IDM_HEXPREFIX:
				{
					g_option.IsPrefix = !g_option.IsPrefix;
					g_szFormat = g_option.IsPrefix? g_szVarFormat[1]:g_szVarFormat[0];
					PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDM_REFRESH,0), IDM_HEXPREFIX);
					break;
				}
			case IDM_NOINVISIBLE:
				{
					g_option.IsNoInvisible=!g_option.IsNoInvisible;
					break;
				}
			case IDM_SHOWPOSCO:
				{
					g_option.IsShowPosCo=!g_option.IsShowPosCo;
					break;
				}
			case IDM_TRANS:
				{
					g_option.IsTrans=!g_option.IsTrans;
					break;
				}

			case IDM_DRAWRECT:
				{
					g_option.wFlag=MAKEWORD(DWR_FLAG_FOCUS_RECT,HIBYTE(g_option.wFlag));
					break;
				}
			case IDM_DRAWVITREOUS:
				{
					g_option.wFlag=MAKEWORD(DWR_FLAG_VITREOUS_PEN,HIBYTE(g_option.wFlag));
					break;
				}
			case IDM_DRAWLINE:
				{
					g_option.wFlag=MAKEWORD(DWR_FLAG_CUSTOM_PEN,HIBYTE(g_option.wFlag));
					break;
				}
			case IDM_DRAWBLOCK:
				{
					g_option.wFlag=MAKEWORD(DWR_FLAG_FOCUS_BLOCK,HIBYTE(g_option.wFlag));
					break;
				}
			case IDM_DRAWTOPMOST:
				{
					if(g_option.wFlag&DWR_FLAG_TOPMOST)
					{
						g_option.wFlag&=~DWR_FLAG_TOPMOST;
					}
					else
					{
						g_option.wFlag|=DWR_FLAG_TOPMOST;
					}
					break;
				}
			case IDM_EFFECT_DETAIL:
				{
					if(IsWindow(g_hwnd_EffectOption))
					{
						BringWindowToForeground(g_hwnd_EffectOption);
					}
					else
					{
						DialogBox(g_hInstance, 
							MAKEINTRESOURCE(IDD_EFFECT_OPTION), hwnd, DlgProc_EffectOption);
					}
					return TRUE;
				}

			case IDM_APP_SETTING:
				{
					if(IsWindow(g_hwnd_AppSetting))
					{
						BringWindowToForeground(g_hwnd_AppSetting);
					}
					else
					{
						DialogBox(g_hInstance, 
							MAKEINTRESOURCE(IDD_APP_SETTING), hwnd, DlgProc_APPSetting);
					}
					return TRUE;
				}
			
			case IDM_FINDNEXT:
			case IDM_SPYTARGET:
				{
					if( IsWindow(g_hwnd_SpyTarget) )
					{
						BringWindowToForeground(g_hwnd_SpyTarget);
					}
					else
					{
						DialogBox(g_hInstance, 
							MAKEINTRESOURCE(IDD_TOOL_SPYTARGET), hwnd, DlgProc_SpyTarget);
					}
					return TRUE;
				}
			case IDM_RUN_COLORSPY:
			case IDM_RUN_WNDMOD:
			case IDM_RUN_PSWSNIFFER:
			case IDM_RUN_WNDENUMER:
				{
					PostMessage(GetToolContainerWindow(), WM_COMMAND, wParam, 0);
					return TRUE;
				}
			case IDM_LOAD_MSGSET:
			case IDM_SAVE_MSGSET:
				{
					_beginthread(ExportImportProfileThreadProc, 0, (PVOID)LOWORD(wParam));
					SetDlgItemFocus(hwnd, IDC_TAB_RECT);
					return TRUE;
				}
			case IDM_SAVE_WNDSTRS:
				{
					_beginthread(SaveWindowStringsThreadProc, 0, NULL);
					SetDlgItemFocus(hwnd, IDC_TAB_RECT);
					return TRUE;
				}

			case IDM_HELP_CHM:
				{
					OnOpenHelpTopics();
					return TRUE;
				}

			case IDM_ABOUT:
				{
					if(IsWindow(g_hwnd_AboutWS))
					{
						BringWindowToForeground(g_hwnd_AboutWS);
					}
					else
					{
						DialogBox(g_hInstance, 
							MAKEINTRESOURCE(IDD_ABOUT), hwnd, DlgProc_AboutWS);
					}
					return TRUE;
				}

			case ID_CMD_TOPMOST:
				{
					OnMainDlgTopMost((BOOL)lParam);
					return TRUE;
				}

			case ID_CMD_AUTOHIDE:
				{
					if( lParam==NULL )//from menu...
					{
						g_option.IsAutoHide=!g_option.IsAutoHide;
							CheckDlgButton(hwnd, ID_CMD_AUTOHIDE, 
							g_option.IsAutoHide? BST_CHECKED:BST_UNCHECKED);
					}
					else
					{
						g_option.IsAutoHide=(BOOL)IsDlgButtonChecked(hwnd, ID_CMD_AUTOHIDE);
					}
					break;
				}

			case ID_CMD_EXIT:
				{
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					return TRUE;
				}

			case IDCANCEL:
				{
					CloseWindow(hwnd);
					return TRUE;
				}
			case SC_RESTORE:
				{
					ShowWindow(hwnd, SW_RESTORE);
					return TRUE;
				}

			default:
				if(lParam)
				{
					//Forward to child dlg
					PostMessage(g_TabDlgHdr.hwndCurrent, WM_COMMAND, wParam, lParam);
					return TRUE;
				}
				return FALSE;

			}//switch(LOWORD(wParam))
					
			//Menu Commands should break to here to update state...
			OnSetMenuState(GetMenu(hwnd));
			return FALSE;

		} //WM_COMMAND

	case WM_SYSCOMMAND:
		{
			CallSpecWndProcRoutine(hwnd, message, wParam, lParam);

			switch( LOWORD(wParam) )
			{
			case ID_CMD_TOPMOST:
				{
					OnMainDlgTopMost(0);
					break;
				}
			case IDM_MINIMIZE_TO_TRAY:
				{
					SetTimer(hwnd, TIMER_SC_DELAY, USER_TIMER_MINIMUM, NULL);
					break;
				}
			}

			return FALSE;
		} //WM_SYSCOMMAND

	case WM_CLOSE:
		{
			if( IsWindow(g_hwndTC) )
			{
				if( IsToolWindowExists() )
				{
					if( IDYES!=MessageBoxFromResStrEx(g_hInstance, 
						hwnd, IDS_ALLEXIT_CONFIRM, IDS_APPNAME, 
						MB_YESNO|MB_USERICON, MAKEINTRESOURCE(IDI_MAIN)) )
					{
						SetForegroundWindow(g_hwndTC);
						return TRUE;
					}
				}
			}

			ShowWindow(hwnd, SW_HIDE);
			DestroyWindow(hwnd);
			
			return FALSE;
		}
	
	case WM_DESTROY:
		{
			SendMessage(g_hwndBK, WM_CLOSE, 0, 0);
						
			DoDestroyIconTooltip();
			
			for(iBuf=0; iBuf<C_MAIN_TAB_PAGES; iBuf++)
			{
				DestroyWindow(g_TabDlgHdr.CDI[iBuf].hwnd);
			}
			
			if( IsWindow(g_hwndTC) )
			{
				SendMessage(g_hwndTC, WM_CLOSE, 0, 0);
			}
			
			PostQuitMessage(0);
			break;
		}

	} //switch(message)
	
	return FALSE;

} //DlgProc_SpyMain();

//////////////////////////////////////////////////////////////////////////

VOID OnOpenHelpTopics(VOID)
{
	TCHAR szHelpFile[MAX_PATH];
	TCHAR szCmdLine[MAX_PATH];

	if( IsFileExists(GetAppProfileName(szHelpFile, TEXT(".chm"))) )
	{
		wsprintf(szCmdLine, TEXT("%s \"%s\"\0"), TEXT("hh.exe"), szHelpFile);

#ifdef UNICODE
		WinExecEx(szCmdLine, SW_MAXIMIZE);
#else
		WinExec(szCmdLine, SW_MAXIMIZE);
#endif
	}
}

//////////////////////////////////////////////////////////////////////////

VOID SaveWindowStringsThreadProc(PVOID pFoo)
{
#define _LARGE_SIZE_WNDTEXT				5120000
#define	_SHOWLOADING_MIN_SIZE_WNDTEXT	2048000

#define KWC_IEWND	631770

	DWORD dwBuf=NULL;
	HWND hwndPB=NULL;
	BOOL IsGrouping;
	BOOL IsAppend;
	INT iSupportedType=KWC_WND;
	INT iWndTextBufLen=0;
	LPTSTR lpszWndTextBuf=NULL;
	TCHAR szBuf[MAX_PATH];
	TCHAR szFileName[MAX_PATH];

	if( !IsWindow(g_spyinfo_SWIex.swi.hwnd) )
	{
		MessageBeep(0);
		_endthread();
	}

	//set app global task-modal-window, 
	//remember always set it NULL when finishing task-modal process...
	g_hwnd_TaskModal=g_hwnd; 

	/**	iSupportedType = (INT)GetKnownClassID(g_spyinfo_SWIex.swi.szClassName);
	dwBuf = GetWindowLong(g_spyinfo_SWIex.swi.hwnd, GWL_STYLE); **/


	if( !IsIEWindow(g_spyinfo_SWIex.swi.hwnd) )
	{
		if(g_dwBitFlag&BOOLEAN_BIT_TAB_STYLE)
		{
			SendMessage(g_TabDlgHdr.CDI[TAB_STYLES].hwnd, WM_SHOWWINDOW, 0, 0);
		}
		iSupportedType = (INT)
			SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_STYLES].hwnd, IDC_CBDL_KNOWN, CB_GETCURSEL, 0, 0);
		dwBuf = g_spyinfo_SWIex.wi.dwStyle;
		
		switch (iSupportedType)
		{
			
		case KWC_COMBO:
			{
				if( (dwBuf&CBS_HASSTRINGS) || 
					!(dwBuf&(CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE)) )
				{
					iWndTextBufLen = GetAllTextsFromListWnd(NULL, 
						NULL, g_spyinfo_SWIex.swi.hwnd, CB_GETCOUNT, NULL, CB_GETLBTEXTLEN);
				}
				break;
			}
		case KWC_LISTBOX:
			{
				if( (dwBuf&LBS_HASSTRINGS) || 
					!(dwBuf&(LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE)) )
				{
					iWndTextBufLen = GetAllTextsFromListWnd(NULL, 
						NULL, g_spyinfo_SWIex.swi.hwnd, LB_GETCOUNT, NULL, LB_GETTEXTLEN);
				}
				break;
			}
		case KWC_LISTVIEW:
			{
				iWndTextBufLen = 
					GetAllTextsFromListViewCtrl(g_spyinfo_SWIex.swi.hwnd, NULL, g_hwnd, NULL, FALSE);
				break;
			}
		case KWC_HEADER:
			{
				iWndTextBufLen = 
					GetAllTextsFromHeaderCtrl(NULL, g_spyinfo_SWIex.swi.hwnd, NULL, NULL, FALSE);
				break;
			}
		case KWC_TAB:
			{
				iWndTextBufLen = 
					GetAllTextsFromTabCtrl(g_spyinfo_SWIex.swi.hwnd, NULL);
				break;
			}
		case KWC_TREEVIEW:
			{
				iWndTextBufLen =
					GetAllTextsFromTreeView(g_spyinfo_SWIex.swi.hwnd, NULL, NULL);
				break;
			}
		case KWC_STATUS:
			{
				iWndTextBufLen = 
					GetAllTextsFromStatusBar(g_spyinfo_SWIex.swi.hwnd, NULL);
				break;
			}
		default:
			{
				iSupportedType=KWC_WND;
				SendMessageTimeout(g_spyinfo_SWIex.swi.hwnd, 
					WM_GETTEXTLENGTH, 0, 0, 
					SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&iWndTextBufLen);
				break;
			}
		}//switch() end...
		
	}
	else //IE window
	{
		iSupportedType=KWC_IEWND;
		iWndTextBufLen=_SHOWLOADING_MIN_SIZE_WNDTEXT;
	}

	dwBuf=0;
	if( iWndTextBufLen <= 0 )
	{
		//if no text or not supported, or other exception..
		dwBuf=IDS_NO_TEXT_TO_SAVE;
	}
	else if( iSupportedType==KWC_WND &&
		iWndTextBufLen > _LARGE_SIZE_WNDTEXT ) 
	{
		//if text of one item is more than 5M chars, prompt don't be foolish.
		dwBuf=IDS_TEXT_TOO_LONG;
	}
	if(dwBuf!=0)
	{
		MessageBoxFromResStr(g_hInstance, g_hwnd, 
			(UINT)dwBuf, IDS_NOTE, MB_TOPMOST|MB_ICONINFORMATION);
		g_hwnd_TaskModal=NULL;
		_endthread();
	}
	
	if( iSupportedType==KWC_LISTVIEW )
	{
		IsGrouping=(BOOL)(IDYES==MessageBoxFromResStr(g_hInstance, 
			g_hwnd, IDS_SAVE_GROUPING, IDS_SAVE_OPT, MB_TOPMOST|MB_YESNO|MB_ICONQUESTION));
	}
	
	ReadUserPathFromINI(szBuf, STR_INI_KEY_SAVETEXT);
	MakeFileTypeFromStrRes(g_hInstance, IDS_FILETYPE_TXT, 
		g_strBuf, NULL, TEXT("*.txt"));

	if(	GetSaveFileNameDialog(NULL, g_hwnd, szFileName, 
		NULL, szBuf, g_strBuf, TEXT ("txt"), 
			OFN_HIDEREADONLY|OFN_EXPLORER|OFN_PATHMUSTEXIST|
			OFN_NONETWORKBUTTON, OFN_EX_NOPLACESBAR, g_siWinVer>WINVER_WIN9X)
		)
	{

		IsAppend=(BOOL)DoGetFileSize(szFileName);
		if( IsAppend )
		{
			LoadString(g_hInstance, IDS_MSG_ISOW, szBuf, MAX_PATH);
			wsprintf(g_strBuf, szBuf, szFileName);
			LoadString(g_hInstance, IDS_MSG_ISOW_TITLE, szBuf, MAX_PATH);
			MessageBeep(MB_ICONASTERISK);
			dwBuf=ShowMessageBox(g_hwnd, g_strBuf, szBuf, MB_TOPMOST|MB_ICONQUESTION|MB_YESNOCANCEL);
			if(dwBuf==IDCANCEL)
			{
				g_hwnd_TaskModal=NULL;
				_endthread();
			}
			else if(dwBuf==IDYES) //OverWrite it.
			{
				IsAppend=FALSE;
			}
		}

		iWndTextBufLen+=MAX_PATH; //more space for ending-suffix

		if( iWndTextBufLen > _SHOWLOADING_MIN_SIZE_WNDTEXT )
		{
			LoadingBar_Open();

			if( iSupportedType==KWC_LISTVIEW || 
				iSupportedType==KWC_TREEVIEW )
			{
				hwndPB=LoadingBar_GetProgressBar();
			}
			else
			{
				hwndPB=(HWND)TRUE;
				LoadingBar_AutoStep();
			}
		}
		
		if(IsAppend)
		{
			lstrcpyn(szBuf, TEXT("\r\n\r\n"), MAX_PATH);
			lstrcatchar(szBuf, '=', 40);
			lstrcat(szBuf, TEXT("\r\n\r\n"));
			WriteStrToFile(szFileName, szBuf, IsAppend);
		}

		if( iSupportedType==KWC_IEWND )
		{
			CoInitialize(NULL);
			if( !GetHtmlSource(GetIEDocInterface(g_spyinfo_SWIex.swi.hwnd), NULL, NULL, szFileName, IsAppend) )
			{
				FormatMessage(
					FORMAT_MESSAGE_FROM_SYSTEM|
					FORMAT_MESSAGE_IGNORE_INSERTS| 
					FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, 
					ERROR_CAN_NOT_COMPLETE, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
					(LPTSTR)&lpszWndTextBuf, 0, NULL);
				WriteStrToFile(szFileName, lpszWndTextBuf, IsAppend);
				LocalFree((HLOCAL)lpszWndTextBuf);
			}
			CoUninitialize();
		}
		else
		{

			lpszWndTextBuf=(LPTSTR)
				VirtualAlloc(NULL, sizeof(TCHAR)*(iWndTextBufLen), MEM_COMMIT, PAGE_READWRITE);

			if(lpszWndTextBuf)
			{
				if( iSupportedType==KWC_COMBO )
				{
					GetAllTextsFromListWnd(lpszWndTextBuf, 
						iWndTextBufLen, g_spyinfo_SWIex.swi.hwnd, CB_GETCOUNT, CB_GETLBTEXT, CB_GETLBTEXTLEN);
				}
				else if( iSupportedType==KWC_LISTBOX )
				{
					GetAllTextsFromListWnd(lpszWndTextBuf, 
						iWndTextBufLen, 
						g_spyinfo_SWIex.swi.hwnd, LB_GETCOUNT, LB_GETTEXT, LB_GETTEXTLEN);
				}
				else if( iSupportedType==KWC_LISTVIEW )
				{
					GetAllTextsFromListViewCtrl(g_spyinfo_SWIex.swi.hwnd, lpszWndTextBuf, g_hwnd, hwndPB, IsGrouping);
				}
				else if( iSupportedType==KWC_HEADER )
				{
					GetAllTextsFromHeaderCtrl(NULL, g_spyinfo_SWIex.swi.hwnd, NULL, lpszWndTextBuf, TRUE);
				}
				else if( iSupportedType==KWC_STATUS )
				{
					GetAllTextsFromStatusBar(g_spyinfo_SWIex.swi.hwnd, lpszWndTextBuf);
				}
				else if( iSupportedType==KWC_TAB )
				{
					
					GetAllTextsFromTabCtrl(g_spyinfo_SWIex.swi.hwnd, lpszWndTextBuf);
				}
				else if( iSupportedType== KWC_TREEVIEW )
				{
					GetAllTextsFromTreeView(g_spyinfo_SWIex.swi.hwnd, lpszWndTextBuf, hwndPB);
				}
				else
				{
					SendMessageTimeout(g_spyinfo_SWIex.swi.hwnd, WM_GETTEXT, 
						iWndTextBufLen, (LPARAM)lpszWndTextBuf, 
						SMTO_NOTIMEOUTIFNOTHUNG, 1000, NULL);
				}
				
				WriteStrToFile(szFileName, lpszWndTextBuf, IsAppend);

				VirtualFree((PVOID)lpszWndTextBuf, 0, MEM_RELEASE);
			}
			else
			{
				LastErrorMessageBox(g_hwnd, g_strBuf, MAX_STRBUF_LEN);
				WriteStrToFile(szFileName, g_strBuf, IsAppend);
			}
		}

		if(hwndPB)
		{
			LoadingBar_Close();
		}

		WriteUserPathToINI(szFileName, FALSE, STR_INI_KEY_SAVETEXT);
		
		if( IsAutoOpenSavedFile() )
		{
			if( IsAppend > _LARGE_SIZE_WNDTEXT )
			{
				DoLocateFile(szFileName, FALSE);
			}
			else
			{
				lstrcpyn(g_strBuf, szFileName, MAX_PATH);
				OnOpenTextFileAsync(GetDlgItem(g_hwnd, IDC_TAB_RECT), g_strBuf);
			}
		}
	}
	
	MACRO_TRIM_MEMORY(g_siWinVer>WINVER_WIN9X)
		g_hwnd_TaskModal=NULL;
	_endthread();

}//SaveWindowStringsThreadProc()

//////////////////////////////////////////////////////////////////////////

VOID ExportImportProfileThreadProc(PVOID IdCmd)
{

	BOOL IsOkay=TRUE;
	BOOL isSave;
	TCHAR szTitle[TINY_STRBUF_LEN];
	TCHAR szBuf[MAX_PATH];
	TCHAR szFileName[MAX_PATH];

	g_hwnd_TaskModal=g_hwnd; 
	isSave=(BOOL)((UINT)IdCmd==IDM_SAVE_MSGSET);

	if( isSave && 
		!GetPrivateProfileSection(STR_INI_SEC_MSGOPT, 
			szBuf, MAX_PATH, GetCfgFileName(g_strBuf)) )
	{
		MessageBoxFromResStr(g_hInstance, g_hwnd, 
			IDS_NO_MSGCFG, IDS_NOTE, MB_ICONINFORMATION);
		IsOkay=FALSE;
	}

	if( IsOkay )
	{
		LoadString(g_hInstance, 
			isSave?IDS_CFG_EXPORT:IDS_CFG_IMPORT, szTitle, TINY_STRBUF_LEN);
		
		MakeFileTypeFromStrRes(g_hInstance, IDS_FILETYPE_TXT, 
			g_strBuf, NULL, TEXT("*.txt"));
		
		if(	CommonDialog_File(g_siWinVer>WINVER_WIN9X, isSave, 
			NULL, g_hwnd, szFileName, 
			szTitle, ReadUserPathFromINI(szBuf, STR_INI_KEY_BACKUP), 
			g_strBuf, TEXT ("txt"), NULL, OFN_EX_NOPLACESBAR) )
		{
			if(isSave)
			{
				ExportSndMsgProfile(szFileName);
			}
			else
			{
				ImportSndMsgProfile(szFileName);
			}
			
			WriteUserPathToINI(szFileName, FALSE, STR_INI_KEY_BACKUP);
		}
	}
	g_hwnd_TaskModal=NULL;
	_endthread();

}//ExportImportProfileThreadProc()

//////////////////////////////////////////////////////////////////////////

#include "WndSpyGui.h"
#include "SndMsgFunc.h"
#include "SpyMsgWnd.h"

//////////////////////////////////////////////////////////////////////////

LRESULT OnMainTrayNotify(HWND hwnd, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc_MsgBackWnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

{
	static HICON hIconGreyGoBtn = (HICON)LoadImage(g_hInstance, 
		MAKEINTRESOURCE(IDI_ICON_GOTO_GREY), IMAGE_ICON, 16, 16, LR_SHARED);
	INT iBuf;
	TCHAR strBuf[MAX_PATH];
	
	switch (message)
	{
		
	case WM_CREATE:
		{
			//Set g_hwndBK firstly...
			g_hwndBK = hwnd;
			
			if(g_option.SpyHotkey.wEnableFlag)
			{
				g_option.SpyHotkey.wEnableFlag=(WORD)
					DoRegisterHotKey(hwnd, ID_HOTKEY_SPYIT, g_option.SpyHotkey.Hotkey, TRUE);
			}
			return 0;
		}
		
	case WM_HOTKEY:
		{
			if( wParam==ID_HOTKEY_SPYIT )
			{
				SetTimer(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
					FINDER_TIMER_HOTKEY_SPYIT, g_option.SpyHotkey.wDelay, NULL);
			}
			break;
		}
		
	case WM_SETTEXT:
		{
			if( lstrcmpi((LPTSTR)lParam,SPY_HOOK_SIGNAL)==0 )
			{
				g_isHookSignal=TRUE;
				
				SetTimer(hwnd, TIMER_FORWARD_HOOK_SPYWNDINFO, USER_TIMER_MINIMUM, NULL);
			}
			else if( lstrcmpi((LPTSTR)lParam, SPY_BASE_SIGNAL)==0 )
			{
				g_isHookSignal=FALSE;
				
				GetModuleFileName(NULL, strBuf, MAX_PATH);
				if( lstrcmpi(g_spyinfo_SWIex.szWndModuleName, strBuf)==0 )
				{
					lstrcpyn(g_spyinfo_SWIex.szWndModuleName, 
						g_spyinfo_SPI.pe32.szExeFile, MAX_PATH);
				}
				if( lstrcmpi(g_spyinfo_SWIex.szClassModuleName, strBuf)==0 )
				{
					lstrcpyn(g_spyinfo_SWIex.szClassModuleName, 
						g_spyinfo_SPI.pe32.szExeFile, MAX_PATH);
				}
				
				SendMessage(hwnd, WM_MY_PRINT_SPYWNDINFO, 0, 0);
				
				if( g_siWinVer>WINVER_WIN9X && 
					lstrlen(g_spyinfo_SPI.ProcStrs.szCommandLine) > DEF_CMDLINE_STR_LEN )
				{
					SetTimer(hwnd, TIMER_FORWARD_SHOWLONGCMDLINE, USER_TIMER_MINIMUM, NULL);
				}
			}
			break;
		}
		
	case WM_MY_PRINT_SPYWNDINFO:
		{
			if(lstrcmpi(g_spyinfo_SWIex.swi.szClassName, WNDCLASS_DESKTOP)==0)
			{
				wsprintf(strBuf, g_szFormat, g_spyinfo_SWIex.swi.hwnd);
				lstrcat(strBuf, TEXT(" (Desktop)"));
				SetDlgItemText(g_TabDlgHdr.CDI[0].hwnd, IDC_EDIT_HWND, strBuf);
				SetDlgItemText(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_HWND, strBuf);
			}
			else
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_HWND,
					g_szFormat, g_spyinfo_SWIex.swi.hwnd);
			}

			if( g_spyinfo_SWIex.swi.isLargeText || 
				lstrcmpi(g_spyinfo_SWIex.swi.szClassName, TEXT("MS_WINNOTE"))==0 )
			{
				CopyWndTextToWnd(g_spyinfo_SWIex.swi.hwnd, 
					GetDlgItem(g_TabDlgHdr.CDI[0].hwnd, IDC_EDIT_CAPTION), 
					MAX_CAPTION_STRBUF_LEN, NULL, g_hInstance, IDS_TIP_OMISSION);
				CopyWndTextToWnd(g_spyinfo_SWIex.swi.hwnd, 
					GetDlgItem(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_CAPTION), 
					MAX_STRBUF_LEN, NULL, NULL, NULL);
			}
			else
			{
				SetDlgItemText(g_TabDlgHdr.CDI[1].hwnd, 
					IDC_EDIT_CAPTION, g_spyinfo_SWIex.swi.szCaption);
			}

			SetDlgItemText(g_TabDlgHdr.CDI[1].hwnd, 
				IDC_EDIT_CLASS, g_spyinfo_SWIex.swi.szClassName);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDRECT,
				TEXT("(%d,%d)-(%d,%d) %dx%d"),
				g_spyinfo_SWIex.wi.rcWindow.left,
				g_spyinfo_SWIex.wi.rcWindow.top,
				g_spyinfo_SWIex.wi.rcWindow.right,
				g_spyinfo_SWIex.wi.rcWindow.bottom, 
				g_spyinfo_SWIex.wi.rcWindow.right - g_spyinfo_SWIex.wi.rcWindow.left,
				g_spyinfo_SWIex.wi.rcWindow.bottom - g_spyinfo_SWIex.wi.rcWindow.top);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_CLIENTRECT,
				TEXT("(%d,%d)-(%d,%d) %dx%d"),
				g_spyinfo_SWIex.wi.rcClient.left,
				g_spyinfo_SWIex.wi.rcClient.top,
				g_spyinfo_SWIex.wi.rcClient.right,
				g_spyinfo_SWIex.wi.rcClient.bottom, 
				g_spyinfo_SWIex.wi.rcClient.right - g_spyinfo_SWIex.wi.rcClient.left,
				g_spyinfo_SWIex.wi.rcClient.bottom - g_spyinfo_SWIex.wi.rcClient.top);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_HINSTANCE, 
				g_szFormat, g_spyinfo_SWIex.hWndInstance);
			
			if(g_spyinfo_SWIex.wndproc)
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDPROC,
					g_option.IsPrefix? TEXT("0x%08X%s"):TEXT("%08X%s"), 
					g_spyinfo_SWIex.wndproc,
					IsWindowUnicode(g_spyinfo_SWIex.swi.hwnd)? TEXT(" (Unicode)"):TEXT("") );
			}
			else
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDPROC,
					TEXT("(n/a)%s"),
					IsWindowUnicode(g_spyinfo_SWIex.swi.hwnd)? TEXT(" (Unicode)"):TEXT("") );
			}
			

			// GetMenu() can do more than GetDlgCtrlID() now, 
			// but according to MSDN, we can't assert what GetMenu will do in the futher.
			// so...

			if(g_spyinfo_SWIex.wi.dwStyle&WS_CHILD)
			{
				iBuf=GetDlgCtrlID(g_spyinfo_SWIex.swi.hwnd);
				DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDID, 
					g_option.IsPrefix? TEXT("0x%08X (%d)"):TEXT("%08X (%d)"), 
					iBuf, iBuf);
			}
			else
			{
				iBuf=(INT)GetMenu(g_spyinfo_SWIex.swi.hwnd);
				if(iBuf)
				{
					DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDID, 
						g_szFormat, iBuf);
				}
				else
				{
					SetDlgItemText(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDID, STR_NONE);
				}
			}
			
			HICON hIconBig, hIconSmall;

			if( lstrcmpi(g_spyinfo_SWIex.swi.szClassName, TEXT("Static"))==0 && 
				IS_FLAGS_MARKED(g_spyinfo_SWIex.wi.dwStyle, SS_ICON|WS_CHILD) )
			{
				SendMessageTimeout(g_spyinfo_SWIex.swi.hwnd, 
					STM_GETIMAGE, IMAGE_ICON, 0, 
					SMTO_NOTIMEOUTIFNOTHUNG, 100, (PDWORD_PTR)&hIconBig);
				hIconSmall=hIconBig;
			}
			else
			{
				SendMessageTimeout(g_spyinfo_SWIex.swi.hwnd, 
					WM_GETICON, ICON_BIG, 0, 
					SMTO_NOTIMEOUTIFNOTHUNG, 100, (PDWORD_PTR)&hIconBig);
				
				SendMessageTimeout(g_spyinfo_SWIex.swi.hwnd, 
					WM_GETICON, ICON_SMALL, 0, 
					SMTO_NOTIMEOUTIFNOTHUNG, 100, (PDWORD_PTR)&hIconSmall);
			}

			DoSetCtrlIcon(g_TabDlgHdr.CDI[1].hwnd, IDC_ICONBTN_BIG, hIconBig, 32);
			DoSetCtrlIcon(g_TabDlgHdr.CDI[1].hwnd, IDC_ICONBTN_SMALL, hIconSmall, 16);

			DlgItemPrintf(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WS,
				g_szFormat, g_spyinfo_SWIex.wi.dwStyle);
			DlgItemPrintf(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WSEX,
				g_szFormat, g_spyinfo_SWIex.wi.dwExStyle);
			
			SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_CLASS,
				g_spyinfo_SWIex.swi.szClassName);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_CLSATOM,
				g_option.IsPrefix? TEXT("0x%04X"):TEXT("%04X"), 
				LOWORD(g_spyinfo_SWIex.wcex.lpszClassName));
			
			DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_CS,
				g_szFormat, g_spyinfo_SWIex.wcex.style);
			
			DoSetCtrlIcon(g_TabDlgHdr.CDI[3].hwnd, IDC_SICON_BIG, g_spyinfo_SWIex.wcex.hIcon, 32);
			DoSetCtrlIcon(g_TabDlgHdr.CDI[3].hwnd, IDC_SICON_SMALL, g_spyinfo_SWIex.wcex.hIconSm, 16);
			DoSetCtrlCursor(g_TabDlgHdr.CDI[3].hwnd, IDC_SICON_CURSOR, g_spyinfo_SWIex.wcex.hCursor, 32);
			
			if( !g_spyinfo_SWIex.wcex.hIcon && !g_spyinfo_SWIex.wcex.hIconSm )
			{
				SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HICON, STR_NONE);
			}
			else if( g_spyinfo_SWIex.wcex.hIcon && g_spyinfo_SWIex.wcex.hIconSm )
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HICON, 
					g_option.IsPrefix? TEXT("0x%08X%s 0x%08X%s"):TEXT("%08X%s\t%08X%s"), 
					g_spyinfo_SWIex.wcex.hIcon, TEXT("(Big)"), 
					g_spyinfo_SWIex.wcex.hIconSm, TEXT("(Small)"));
			}
			else
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HICON,
					g_option.IsPrefix? TEXT("0x%08X%s"):TEXT("%08X%s"), 
					g_spyinfo_SWIex.wcex.hIcon? g_spyinfo_SWIex.wcex.hIcon:g_spyinfo_SWIex.wcex.hIconSm,
					g_spyinfo_SWIex.wcex.hIcon? TEXT("(Big)"):TEXT("(Small)"));
			}
			if(g_spyinfo_SWIex.wcex.hCursor)
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HCURSOR,
					g_szFormat, g_spyinfo_SWIex.wcex.hCursor);
			}
			else
			{
				SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HCURSOR,
					STR_NONE);
			}
			
			if(g_spyinfo_SWIex.wcex.hbrBackground)
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HBKBRUSH,
					g_szFormat, 
					g_spyinfo_SWIex.wcex.hbrBackground);
			}
			else
			{
				SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HBKBRUSH, STR_NONE);
			}
			
			DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_CLSWNDPROC,
				g_szFormat, g_spyinfo_SWIex.wcex.lpfnWndProc);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HCLSMUDLE,
				g_szFormat, g_spyinfo_SWIex.wcex.hInstance);
			
			SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_MENUNAME,
				g_spyinfo_SWIex.szClassMenuName);
			
			SetDlgItemText(g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_MODULEFILE,
				g_spyinfo_SWIex.szClassModuleName);
			
			SetWindowVisible(g_TabDlgHdr.CDI[3].hwnd,IDC_BTN_LOCATE, 
				IsFileExists(g_spyinfo_SWIex.szClassModuleName));
			
			SetWindowVisible(g_TabDlgHdr.CDI[4].hwnd,IDC_BTN_LOCATE1, 
				IsFileExists(g_spyinfo_SPI.pe32.szExeFile));
			SetWindowVisible(g_TabDlgHdr.CDI[4].hwnd,IDC_BTN_LOCATE2, 
				IsFileExists(g_spyinfo_SWIex.szWndModuleName));	
			
			DlgItemPrintf(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_PID,
				g_szFormat, 
				g_spyinfo_SPI.pe32.th32ProcessID);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_TID,
				g_szFormat, 
				g_spyinfo_SPI.dwThreadID);
			
			DlgItemPrintf(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_PRIORITY,
				g_option.IsPrefix? TEXT("(%d)-(0x%08X)"):TEXT("(%d)-(%08X)"), 
				g_spyinfo_SPI.pe32.pcPriClassBase, 
				g_spyinfo_SPI.pe32.dwFlags);
			
			if(g_spyinfo_SPI.pe32.cntThreads)
			{
				DlgItemPrintf(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_THREADNUM,
					TEXT("%d"), g_spyinfo_SPI.pe32.cntThreads);
			}
			else
			{
				SetDlgItemText(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_THREADNUM,
					TEXT("(n/a)"));
			}
			
			SetDlgItemText(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_CURDIR,
				g_spyinfo_SPI.ProcStrs.szCurrentDirectory);
			SetDlgItemText(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_CMDLINE,
				g_spyinfo_SPI.ProcStrs.szCommandLine);
			SetDlgItemText(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_IMAGEFILE,
				g_spyinfo_SPI.pe32.szExeFile);
			SetDlgItemText(g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_WNDMODULE,
				g_spyinfo_SWIex.szWndModuleName);

			DlgItemPrintf(g_TabDlgHdr.CDI[5].hwnd, IDC_CBDL_HWND, 
				SndMsgOpt_IsDec(g_smHdr.SMO.dwFlagSMH)? TEXT("%u"):g_szFormat, 
				g_spyinfo_SWIex.swi.hwnd);
			
			g_dwBitFlag|=BOOLEAN_BIT_TABS;
			//if not Finder Tabpage, print all spyinfo at once...
			if(g_TabDlgHdr.iCurrent)
			{
				SendMessage(g_TabDlgHdr.CDI[g_TabDlgHdr.iCurrent].hwnd, WM_SHOWWINDOW, TRUE, 0);
			}
			
			//////////////////////////////////////////////////////////////////////////
#ifdef _WNDSPY_ALWAYSSHOWMAINDLG
			
			if(	IsMainWndHidden(g_hwnd) )
			{
				ShowWindow(g_hwnd, SW_RESTORE);
			}
#endif
			//////////////////////////////////////////////////////////////////////////
			return 0;
			
		} //WM_MY_PRINT_SPYWNDINFO
		
	case WM_TIMER:
		{
			switch (wParam)
			{
			case TIMER_FORWARD_HOOK_SPYWNDINFO:
				{
					KillTimer(hwnd, wParam);

					iBuf=SpyLib_ReadWndInfoEx(&g_spyinfo_SPI, &g_spyinfo_SWIex);
					
					SendMessage(hwnd, WM_MY_PRINT_SPYWNDINFO, (WPARAM)1, (LPARAM)0);
					
					if( iBuf==0 || iBuf>SPYDLL_CMDLINE_STR_LEN )
					{
						if( g_siWinVer>WINVER_WIN9X )
						{
							SetTimer(hwnd, TIMER_FORWARD_SHOWLONGCMDLINE, USER_TIMER_MINIMUM, NULL);
						}
					}
					return 0;
				}
			case TIMER_FORWARD_HOOK_SPYWNDINFO_TIMEOUT:
				{
					KillTimer(hwnd,wParam);
					if(g_isHookSignal==FALSE)
					{
						SpyTryGetWndInfoEx(g_spyinfo_SWIex.swi.hwnd, &g_spyinfo_SPI, &g_spyinfo_SWIex);
					}
					return 0;
				}
			case TIMER_FORWARD_SHOWLONGCMDLINE:
				{
					KillTimer(hwnd,wParam);
					GetProcessInfoDirStrs(g_spyinfo_SPI.pe32.th32ProcessID, 
						&g_spyinfo_SPI.ProcStrs, GetDlgItem(g_TabDlgHdr.CDI[TAB_PROCESS].hwnd, IDC_EDIT_CMDLINE));
					return 0;
				}
			case IDC_EDIT_HWND:
				{
					if(g_spyinfo_SWIex.swi.hwnd && !IsWindow(g_spyinfo_SWIex.swi.hwnd) )
					{
						KillTimer(hwnd,wParam);
						DlgItemPrintf(g_TabDlgHdr.CDI[0].hwnd, IDC_EDIT_HWND,
							g_option.IsPrefix? TEXT("0x%08X %s"):TEXT("%08X %s"), 
							g_spyinfo_SWIex.swi.hwnd,TEXT("(Destroyed)"));
						DlgItemPrintf(g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_HWND,
							g_option.IsPrefix? TEXT("0x%08X %s"):TEXT("%08X %s"), 
							g_spyinfo_SWIex.swi.hwnd,TEXT("(Destroyed)"));
						g_spyinfo_SWIex.IsSelf=0;
						g_dwBitFlag|=BOOLEAN_BIT_TAB_MANI;
						if(g_TabDlgHdr.iCurrent==5)
						{
							SendMessage(g_TabDlgHdr.CDI[5].hwnd,WM_SHOWWINDOW, TRUE, 0);
						}
						
					}
					return 0;
				}
			case IDC_EDIT_OWNER:
			case IDC_EDIT_PARENT:
			case IDC_EDIT_CHILD:
			case IDC_EDIT_NEXT:
			case IDC_EDIT_PRE:
				{
					if( g_spyinfo_SWIex.swi.hwndArray[wParam-IDC_EDIT_OWNER] && 
						!IsWindow(g_spyinfo_SWIex.swi.hwndArray[wParam-IDC_EDIT_OWNER]) )
					{
						KillTimer(hwnd,wParam);
						DlgItemPrintf(g_TabDlgHdr.CDI[0].hwnd, wParam, 
							g_option.IsPrefix? TEXT("0x%08X %s"):TEXT("%08X %s"), 
							g_spyinfo_SWIex.swi.hwndArray[wParam-IDC_EDIT_OWNER], 
							TEXT("(Destroyed)"));
						SendDlgItemMessage(g_TabDlgHdr.CDI[0].hwnd, wParam + IDC_BTN_OWNER-IDC_EDIT_OWNER, 
							STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIconGreyGoBtn);
						EnableWindow(GetDlgItem(g_TabDlgHdr.CDI[0].hwnd, 
							wParam + IDC_BTN_OWNER-IDC_EDIT_OWNER), FALSE);
					}
					return 0;
				}
			case TIMER_SHOW_LOADING:
				{
					KillTimer(hwnd,wParam);
					DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_LOADING), g_hwnd, DlgProc_Loading);
					return 0;
				}	
			}// end switch(wParam) @ WM_TIMER ... 
			return 0;
		} //WM_TIMER end...
		
	case WM_MY_NOTIFYICON:
		{
			return OnMainTrayNotify(hwnd, lParam);
		}

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case ID_CMD_SPYTARGET:
				{
					if( IsWindow((HWND)lParam) )
					{
						g_spyinfo_SWIex.swi.hwnd=(HWND)lParam;
						SetTimer(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
							FINDER_TIMER_GETSPYINFOEX, USER_TIMER_MINIMUM, NULL);
						DoPlayEventSound(g_option.IsPlaySound);
						BringWindowToForeground(g_hwnd);
					}
					else
					{
						MessageBeep(0);
					}
					break;
				}
			}
			return 0;
		}
		
	case WM_GETMINMAXINFO:
		{
			((LPMINMAXINFO)lParam)->ptMaxPosition.x = -1;
			((LPMINMAXINFO)lParam)->ptMaxPosition.y = -1;
			((LPMINMAXINFO)lParam)->ptMaxSize.x = 0;
			((LPMINMAXINFO)lParam)->ptMaxSize.y = 0;
			return 0;
		}

 	//never let this window show...
	case WM_SETFOCUS:
		{
			ShowWindow(hwnd, SW_HIDE);
			break;
		}

#ifdef APP_FUNC_ONSETTOOLTIPCOLOR	
	case WM_SYSCOLORCHANGE:
		{
			PostMessage(hwnd,WM_MY_COLORSET,0,0);
			break;
		}
	case WM_MY_COLORSET:
		{
			OnSetTooltipColor();
			return 0;
		}
#endif //APP_FUNC_ONSETTOOLTIPCOLOR

	case WM_DESTROY:
		{
			NotifyIconMessage(hwnd, ID_TRAYICON_MAIN, NIM_DELETE, 
				WM_MY_NOTIFYICON, NULL, NULL, NULL, NULL);
			break;
		}
		
	} //switch(message) end...
	
	return DefWindowProc(hwnd, message, wParam, lParam);
	
} //WndProc_MsgBackWnd() end...

//////////////////////////////////////////////////////////////////////////

LRESULT OnMainTrayNotify(HWND hwnd, LPARAM lParam)
{
	TCHAR strBuf[MAX_PATH];

	if( IsWindow(g_hwnd_TaskModal) && 
		(lParam == WM_RBUTTONUP || 
		lParam == WM_LBUTTONDBLCLK)  )
	{
		MessageBeep(0);
		BringWindowToForeground(g_hwnd_TaskModal);
		return 1;
	}
	else if (lParam == WM_RBUTTONUP)
	{
		HMENU hMenu;
		hMenu=CreatePopupMenu();
		
		if( IsMainWndHidden(g_hwnd) )
		{
			if(!IsWindowVisible(g_hwnd))
			{
				LoadString(g_hInstance, IDS_SHOW_MAINDLG, strBuf, MAX_PATH);
			}
			else
			{
				GetMenuString(GetSystemMenu(g_hwnd,FALSE), SC_RESTORE, 
					strBuf, MAX_PATH, MF_BYCOMMAND);
			}
			AppendMenu(hMenu, MF_STRING, SC_RESTORE, strBuf);	
			AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			SetMenuDefaultItem(hMenu, SC_RESTORE, FALSE);
		}
		
		HMENU hSubMenu;
		hSubMenu=LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));
		GetMenuString(hSubMenu, 1, strBuf, MAX_PATH, MF_BYPOSITION);
		AppendMenu(hMenu, MF_POPUP, (UINT)GetSubMenu(hSubMenu,1), strBuf);
		GetMenuString(hSubMenu, 2, strBuf, MAX_PATH, MF_BYPOSITION);
		AppendMenu(hMenu, MF_POPUP, (UINT)GetSubMenu(hSubMenu,2), strBuf);
		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL) ;
		GetMenuString(hSubMenu, ID_CMD_EXIT, strBuf, MAX_PATH, MF_BYCOMMAND);
		AppendMenu(hMenu, MF_STRING, ID_CMD_EXIT, strBuf);
		OnSetMenuState(hMenu);
		
		POINT point;
		GetCursorPos(&point);
		SetForegroundWindow(hwnd);
		lParam=TrackPopupMenu(hMenu, TPM_RETURNCMD|TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
		PostMessage(hwnd, WM_NULL, 0, 0);
		DestroyMenu(hSubMenu);
		DestroyMenu(hMenu);
		if(lParam)
		{
			PostMessage(g_hwnd, WM_COMMAND, MAKELPARAM(lParam,0), 0);
		}
	}
	else if (lParam == WM_LBUTTONDBLCLK)
	{
		// TODO: bring tool-windows to top...

		BringWindowToForeground(g_hwnd);	
		
		if( IsWindow(g_hwndTC) )
		{
			SetForegroundWindow(g_hwndTC);
		}

	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DlgProc_Loading(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
#define PROGRESS_DEF_RANGE	   1000	
	
	switch (message)
	{
	case  WM_INITDIALOG:
		{
			g_hwnd_TaskModal=hwnd;
			g_hTarget=(HANDLE)GetDlgItem(hwnd, IDC_PROGRESS);
			MoveWndToWndCenter(hwnd, g_hwnd);
			return TRUE;
		}
		
	case WM_MY_LOADING_STATE:
		{
			if(lParam==TRUE)
			{
				SendMessage((HWND)g_hTarget, PBM_SETRANGE32, 0, PROGRESS_DEF_RANGE);
				SendMessage((HWND)g_hTarget, PBM_SETPOS, 0, 0);
				SetTimer(hwnd, 
					TIMER_PROGRESS_EFFECT, USER_TIMER_MINIMUM, NULL);
			}
			else
			{
				SendMessage((HWND)g_hTarget, PBM_SETPOS, 
					(WPARAM)SendMessage((HWND)g_hTarget, PBM_GETRANGE, FALSE, NULL), 0);
				EnableWindow(hwnd, FALSE);
				Sleep(100);
				EndDialog(hwnd, 0);
			}
			return TRUE;
		}
		
	case WM_TIMER:
		{
			// terrible bogus progress bar effect codes
			// but still...
			// just put them here...
			INT iPos;
			iPos = SendMessage((HWND)g_hTarget, PBM_GETPOS, 0, 0);
			
			if( iPos >= PROGRESS_DEF_RANGE-1 )
			{
				KillTimer(hwnd, TIMER_PROGRESS_EFFECT);
			}
			else if( iPos >= PROGRESS_DEF_RANGE-400 && (iPos%50==0) )
			{
				SetTimer(hwnd, TIMER_PROGRESS_EFFECT, iPos/5, NULL);
			}
			else if( iPos > PROGRESS_DEF_RANGE-768 && 
				iPos < PROGRESS_DEF_RANGE-384 )
			{
				if(	0 == (iPos%50) )
				{
					SetTimer(hwnd, TIMER_PROGRESS_EFFECT, 
						max(USER_TIMER_MINIMUM, iPos%32), NULL);
				}
			}
			
			SendMessage((HWND)g_hTarget, PBM_SETPOS, min(PROGRESS_DEF_RANGE, iPos+1), 0);	
			return TRUE;
		}
		
	case WM_LBUTTONDOWN:
		{
			PostMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}	
		
	case WM_DESTROY:
		{
			g_hTarget=NULL;
			g_hwnd_TaskModal=NULL;
			break;
		}
		
	}//switch message...
	
	return FALSE;

} //DlgProc_Loading()

//////////////////////////////////////////////////////////////////////////
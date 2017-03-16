#include "..\SpyMainDlgTabs.h"

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK TabDlgProc_Finder(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static WND_SUBCLASS_DATA wsc_DragFinder;
	static HCURSOR hCurFinder;
	static BOOL isPreviousTabWnd=FALSE;
	static HWND hwndDragPickerPan;
	static HWND hwndFocus=NULL;
	HWND hwndFind;
	POINT pt;

	switch (message)
	{

	case WM_INITDIALOG:
		{
			OnChildDialogInit(hwnd);
		
			// arrange Edits layout.
			// then arrange Buttons layout according to their Edit buddy.

			DoSetEditItemsLayout(hwnd, 1, IDC_EDIT_HWND, IDC_EDIT_PRE, 1);
			RECT rc;
			for(int i=IDC_EDIT_OWNER; i<=IDC_EDIT_PRE; i++)
			{
				GetWindowRect(GetDlgItem(hwnd,i), &rc);
				ScreenToWindowRect(hwnd, &rc);
				SetWindowPos(GetDlgItem(hwnd, i+IDC_BTN_OWNER-IDC_EDIT_OWNER), 
					NULL, rc.right+2, rc.top+2, NULL, NULL, SWP_NOSIZE|SWP_NOZORDER);
			}

#ifdef _RESOURCE_INDEPENDENCY
			g_TabDlgHdr.CDI[TAB_FINDER].hwnd=hwnd;
			DoSetFinderBtnState(DSFBS_DISABLE_ALL);
#endif
			hCurFinder=LoadCursor(g_hInstance, MAKEINTRESOURCE(IDI_CUR_DRAGFIND));
			hwndDragPickerPan=GetDlgItem(hwnd, IDC_ICON_DRAG);
			SetWindowPos(hwndDragPickerPan, NULL, 
				NULL, NULL, PAN_RECT_CX, PAN_RECT_CY, SWP_NOMOVE|SWP_NOZORDER);
			wsc_DragFinder.WndProcBuf=SCP_Param_Static_PickCursorPan;
			SetWndSubClassParam(hwndDragPickerPan, &wsc_DragFinder);
			PickPan_SetCursor(hwndDragPickerPan, hCurFinder);

			SetDlgItemSubClass(hwnd, IDC_BTN_OWNER, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_BTN_PARENT, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_BTN_CHILD, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_BTN_NEXT, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_BTN_PRE, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_EDIT_HWND, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_EDIT_OWNER, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_EDIT_PARENT, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_EDIT_CHILD, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_EDIT_NEXT, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_EDIT_PRE, SCP_HwndFileEdit);

			return TRUE;

		}//WM_INITDIALOG
		
	case WM_LBUTTONDOWN:
		{
			PostMessage(g_hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}

	case WM_MY_MOUSEMOVE:
		{
			GetCursorPos(&pt);
			ScreenToClient(hwnd,&pt);
			SendMessage(hwnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(pt.x,pt.y));
			return TRUE;
		}

	case WM_MOUSEMOVE:
		{
			if(g_IsFinding && g_IsIdle)
			{
				g_IsIdle=FALSE; //Set Busy state(not idle)...

				pt.x=(int)(short)LOWORD(lParam); 
				pt.y=(int)(short)HIWORD(lParam);
				ClientToScreen(hwnd, &pt);
				
				if( g_option.IsShowPosCo && !g_option.IsAutoHide )
				{	
					//Show Cursor's position...

					wsprintf(g_strBuf, TEXT("%d-%d"), pt.x, pt.y);
					SendDlgItemMessage(hwnd, IDC_STATIC_POS, WM_SETTEXT, 0, (LPARAM)g_strBuf);
				}
				
				//Get the handle to the window which being under mouse cursor...

				hwndFind = FindRealWindowFromPointEx(
					g_option.IsTrans ? g_hwnd : NULL, pt, FINDWINDOW_OPTION);
				
				if(hwndFind!=hwndFocus)
				{
					if( isPreviousTabWnd )
					{
						SetTimer(hwnd, FINDER_TIMER_MOUSEMOVE_AFTER_TABWND, 60, NULL);
						break;
					}
					g_spyinfo_SWIex.IsSelf = 
						( GetWindowThreadProcessId(hwndFind,NULL)==GetCurrentThreadId() );
					if(	g_spyinfo_SWIex.IsSelf && !g_option.IsSelfFind )
					{
						//if the current window belongs to Wndspy itself.
						//Clear states and break out... 

						RemoveWindowFocus(hwndFocus, g_option.wFlag, g_option.color);
						DoSetFinderInfoText(NULL);
						g_spyinfo_SWIex.swi.hwnd=NULL;
						hwndFocus=NULL;
						g_IsIdle=TRUE;
						break;
					}
					if(!g_option.IsAutoHide) 
					{
						//Get and display simple Window Info 
						//(ie. g_spyinfo_SWIex.swi) for finder tabpage
						//See DoSetFinderInfoText() and SPY_WININFOEX

						DoSetFinderInfoText(hwndFind);
						
						//If it is System TabSwitch Window,	
						//get all window info at once. 

						if( lstrcmpi(g_spyinfo_SWIex.swi.szClassName, WNDCLASS_TABSWITCH)==0 )
						{
							isPreviousTabWnd=TRUE;
							SetTimer(hwnd, FINDER_TIMER_GETSPYINFOEX, USER_TIMER_MINIMUM, NULL);
						}
					}

					//Remove the previous(existing) hwndFocus foucs (XOR draw)...

					if(hwndFocus)
					{
						RemoveWindowFocus(hwndFocus, g_option.wFlag, g_option.color);
					}
					hwndFocus=hwndFind;	//Update hwndFocus
					
					//then, Draw foucs on current finding window... 

					//in certain case, we should delay drawing foucs, 
					//bc. the focus might be brushed away by previous edit-window-async-update...

					if( !g_option.IsAutoHide &&
						(g_option.wFlag&DWR_FLAG_TOPMOST ||
						g_option.wFlag&DWR_FLAG_FOCUS_BLOCK) )
					{
						//check timer-delay-draw is needed...

						if( IsWndOverWnd(hwndFocus, hwnd) ) 
						{
							SetTimer(hwnd, FINDER_TIMER_DELAY_TEXTUPDATE, USER_TIMER_MINIMUM, NULL);
							break;
						}
					}

					//no need to delay, do normal drawing focus...
					DrawWindowFocus(hwndFocus, g_option.wFlag, g_option.color);
					
				} //if(hwndFind!=hwndFocus) end...
				
				g_IsIdle=TRUE; //Set Idle State 
				//(to delay-drawing break, the state will be setted in WM_TIMRER)...
				
			}//if(IsFinding && IsIdle) 
			
			break;

		} //WM_MOUSEMOVE end...
	
	//case WM_CANCELMODE:		
	case WM_CAPTURECHANGED:
		{
			if(g_IsFinding)
			{
				SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
				InvalidateRect(NULL,NULL,TRUE);
				KillTimer(hwnd, FINDER_TIMER_DELAY_TEXTUPDATE);
			}
			break;
		}
	case WM_LBUTTONUP:
		{
			if(g_IsFinding)
			{
				g_IsFinding=FALSE;
				
				if(g_IsIdle==FALSE)
				{
					KillTimer(hwnd, FINDER_TIMER_DELAY_STARTFINDER);
				}
				if(hwndFocus)
				{
					RemoveWindowFocus(hwndFocus, g_option.wFlag, g_option.color);
				}

				SetDlgItemText(hwnd, IDC_STATIC_POS, TEXT(""));
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				PickPan_SetCursor(hwndDragPickerPan, hCurFinder);

				if(g_option.IsPlaySound)
				{
					PlaySound(MAKEINTRESOURCE(IDR_WAVE_WIN2WIN), g_hInstance, 
						(g_option.IsAutoHide?SND_SYNC:(SND_ASYNC|SND_NOWAIT))|SND_NODEFAULT|SND_RESOURCE);
				}
				if(g_option.IsAutoHide)
				{
					g_spyinfo_SWIex.swi.hwnd=hwndFocus;
					if(g_option.IsLockUpdate && g_option.IsAutoHide)
					{
						KillTimer(hwnd, FINDER_TIMER_DELAY_LOCKUPDATE);
						LockWindowUpdate(NULL);
					}
					ShowWindow(g_hwnd,SW_SHOW);
				}
				else
				{
					if(g_option.IsTrans)
					{
						DoSetWindowTrans(g_hwnd, NO_TRANS);

						if(g_siWinVer < WINVER_WINXPNEWER)
						{
							InvalidateRect(g_hwnd,NULL,TRUE);
						}
					}
					
				}

				//Then, release mouse capture and get all spy info...
				
				ReleaseCapture();
				
				if(g_spyinfo_SWIex.swi.hwnd)
				{
					if( g_option.IsAutoHide || 
						!SendDlgItemMessage(hwnd, IDC_EDIT_OWNER, WM_GETTEXTLENGTH, 0, 0) )
					{
						DoSetFinderInfoText(g_spyinfo_SWIex.swi.hwnd);
					}
					SpyGetWindowInfoEx(g_spyinfo_SWIex.swi.hwnd, &g_spyinfo_SPI, &g_spyinfo_SWIex);
					if(g_spyinfo_SWIex.IsSelf && g_option.IsSelfFind)
					{
						InvalidateRect(g_spyinfo_SWIex.swi.hwnd, NULL, TRUE);
					}
				}
				else
				{
					ZeroMemory(&g_spyinfo_SWIex, sizeof(SPY_WININFOEX));
					ZeroMemory(&g_spyinfo_SPI, sizeof(SPY_PROCESSINFO));
					DoClearWndSpyText(TRUE);
				}
				
				//lastly, update hwndarrays button state
				DoSetFinderBtnState(DSFBS_NORMAL);
				
				/*[not necessary]*/
				//DoRefreshIfWndIsTaskBar(NULL); 
				
			}//if(isFinding)
			break;

		}//WM_LBUTTONUP end...

		
	case WM_TIMER:
		{
			KillTimer(hwnd,wParam);
			switch(wParam)
			{
			case FINDER_TIMER_GETSPYINFOEX:
				{
					DoSetFinderInfoText(g_spyinfo_SWIex.swi.hwnd);
					SpyGetWindowInfoEx(g_spyinfo_SWIex.swi.hwnd, &g_spyinfo_SPI, &g_spyinfo_SWIex);
					if(g_IsFinding==FALSE)
					{
						DoSetFinderBtnState(DSFBS_NORMAL);
					}
					return TRUE;
				}
			case FINDER_TIMER_MOUSEMOVE_AFTER_TABWND:
				{
					isPreviousTabWnd=FALSE;
					g_IsIdle=TRUE;
					SendMessage(hwnd, WM_MY_MOUSEMOVE, 0, 0);
					return TRUE;
				}
			case FINDER_TIMER_DELAY_TEXTUPDATE:
				{
					DrawWindowFocus(hwndFocus, g_option.wFlag, g_option.color);
					g_IsIdle=TRUE; //Set Idle state...
					return TRUE;
				}
			case FINDER_TIMER_DELAY_STARTFINDER:
				{
					if(g_option.IsAutoHide && g_option.IsLockUpdate)
					{
						SetTimer(hwnd, FINDER_TIMER_DELAY_LOCKUPDATE, 500, NULL);
					}
					g_IsIdle=TRUE; //Set Idle state...
					SendMessage(hwnd, WM_MY_MOUSEMOVE, 0, 0);
					return TRUE;
				}
			case FINDER_TIMER_DELAY_LOCKUPDATE:
				{
					LockWindowUpdate(GetDesktopWindow());
					return TRUE;
				}
			case FINDER_TIMER_HOTKEY_SPYIT:
				{
					GetCursorPos(&pt);
					pt.x+=g_option.SpyHotkey.ptsOffset.x;
					pt.y+=g_option.SpyHotkey.ptsOffset.y;
					
					hwndFind=FindRealWindowFromPointEx(NULL, pt, FINDWINDOW_OPTION);
					if( !IsWindow(hwndFind) )
					{
						MessageBeep(0);
						return TRUE;
					}
					g_spyinfo_SWIex.swi.hwnd=hwndFind;
					SetTimer(hwnd, FINDER_TIMER_GETSPYINFOEX, USER_TIMER_MINIMUM, NULL);
					DoPlayEventSound(g_option.IsPlaySound);
					return TRUE;
				}
			case FINDER_TIMER_START_DRAG:
				{
					DoPlayEventSound(g_option.IsPlaySound);
					DoSetFinderBtnState(DSFBS_GREY_ALL);
					PickPan_SetCursor(hwndDragPickerPan, NULL);
					
					if(g_option.IsAutoHide)
					{
						ShowWindow(g_hwnd, SW_HIDE);
					}
					else //not auto-hide main window...
					{
						SetWindowVisible(hwnd, IDC_STATIC_POS, g_option.IsShowPosCo);
						
						if(lpfSetLayeredWindowAttributes==NULL)
						{
							g_option.IsTrans=FALSE;
						}
						if(g_option.IsTrans)
						{
							DoSetWindowTrans(g_hwnd, (BYTE)g_option.siAlpha);
						}
					}
					//then...
					SetCursor(hCurFinder);
					SetCapture(hwnd);
					
					if(g_option.IsLockUpdate && g_option.IsAutoHide)
					{
						g_option.wFlag|=DWR_FLAG_LOCKUPATE;
					}
					else
					{
						g_option.wFlag&=~DWR_FLAG_LOCKUPATE;
					}
					
					if( g_option.IsLockUpdate && g_option.IsAutoHide 
						|| g_option.IsTrans )
					{
						g_IsIdle=FALSE;
						SetTimer(hwnd, FINDER_TIMER_DELAY_STARTFINDER, 160, NULL);
					}
					else 
					{
						g_IsIdle=TRUE;
					}
					g_IsFinding=TRUE;
					isPreviousTabWnd=FALSE;
					hwndFocus=NULL;
					return TRUE;
				}
			}// end switch @ WM_TIMER...
			
			return TRUE;

		} //WM_TIMER end..


	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_ICON_DRAG:
				{
					if( STN_CLICKED!=HIWORD(wParam) )
					{
						break;
					}
					SetFocus(hwndDragPickerPan);
					SetTimer(hwnd, FINDER_TIMER_START_DRAG, USER_TIMER_MINIMUM, NULL);
					return TRUE;
				}//case IDC_ICON_DRAG
				
			case IDC_BTN_OWNER:
			case IDC_BTN_PARENT:
			case IDC_BTN_CHILD:
			case IDC_BTN_NEXT:
			case IDC_BTN_PRE:
				{
					if( STN_CLICKED!=HIWORD(wParam) &&
						STN_DBLCLK!=HIWORD(wParam) )
					{
						break;
					}

					hwndFind=(HWND)
						GetDlgItemHex(hwnd, LOWORD(wParam)-(IDC_BTN_OWNER-IDC_EDIT_OWNER));
					
					if( !IsWindow(hwndFind) )
					{
						hwndFind=g_spyinfo_SWIex.swi.hwndArray[LOWORD(wParam)-(IDC_BTN_OWNER)];
						if( !IsWindow(hwndFind) )
						{
							DoSetFinderBtnState(DSFBS_NORMAL);
							break;
						}
					}
					else if(hwndFind == g_spyinfo_SWIex.swi.hwnd)
					{
						//ignore redundancy...
						break;
					}

					//play event sound, and 
					//update g_spyinfo_SWIex.swi.hwnd...
					//then, SetTimer to do the real thing...

					DoPlayEventSound(g_option.IsPlaySound);
					g_spyinfo_SWIex.swi.hwnd=hwndFind;
					SetTimer(hwnd, FINDER_TIMER_GETSPYINFOEX, USER_TIMER_MINIMUM, NULL);
					return TRUE;
				} //case IDC_BTN_OWNER ~ IDC_BTN_PRE:
				
			}//switch @ WM_COMMAND
			
			break;

		} //WM_COMMAND end...

		
	case WM_NOTIFY:
		{
			return OnToolTipNotify(wParam, lParam);
		}
		
	}//switch message...
	
	return FALSE;
	
} //TabDlgProc_Finder();

//////////////////////////////////////////////////////////////////////////

VOID DoSetFinderBtnState(INT iFlag)
{
	int i;
	HICON hicon[2];
	
	hicon[0]=(HICON)LoadImage(g_hInstance, 
		MAKEINTRESOURCE(IDI_ICON_GOTO_GREY), IMAGE_ICON, 16, 16, LR_SHARED);
	hicon[1]=(HICON)LoadImage(g_hInstance, 
		MAKEINTRESOURCE(IDI_ICON_GOTO), IMAGE_ICON, 16, 16, LR_SHARED);
	
	if( iFlag != DSFBS_NORMAL )
	{
		for(i=IDC_BTN_OWNER; i<=IDC_BTN_PRE; i++)
		{
			SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, 
				STM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon[0]);
			KillTimer(g_hwndBK, i-(IDC_BTN_OWNER-IDC_EDIT_OWNER));
		}
		KillTimer(g_hwndBK, IDC_EDIT_HWND);
		
		if(iFlag==DSFBS_DISABLE_ALL)
		{
			DoEnableDlgItems(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, IDC_BTN_OWNER, IDC_BTN_PRE, 1, FALSE);
		}
	}
	else //iFlag == DSFBS_NORMAL
	{
		SetTimer(g_hwndBK, IDC_EDIT_HWND, 
			SPY_CHECK_WND_TIMER_INTERVAL, NULL);
		
		for(i=IDC_BTN_OWNER; i<=IDC_BTN_PRE; i++)
		{
			if( g_spyinfo_SWIex.swi.hwndArray[i-IDC_BTN_OWNER] )
			{
				SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, 
					STM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon[1]);
				EnableDlgItem(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, TRUE);
				
				SetTimer(g_hwndBK, i-(IDC_BTN_OWNER-IDC_EDIT_OWNER), 
					SPY_CHECK_WND_TIMER_INTERVAL, NULL);
			}
			else
			{
				SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, 
					STM_SETIMAGE, IMAGE_ICON, (LPARAM)hicon[TAB_FINDER]);
				EnableDlgItem(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, FALSE);
			}
		}
		
	}
	
} //DoSetFinderBtnState()

//////////////////////////////////////////////////////////////////////////

VOID DoSetFinderInfoText(HWND hwndFind)
{
	TCHAR szEditBuf[MAX_NUMSTR_LEN];
	
	if(hwndFind==NULL)
	{
		DoClearWndSpyText(FALSE);
		return;
	}
	
	SpyGetWindowInfo(hwndFind, &g_spyinfo_SWIex.swi);
	
	wsprintf(szEditBuf, g_szFormat, g_spyinfo_SWIex.swi.hwnd);
	
	SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
		IDC_EDIT_HWND, WM_SETTEXT, 0, (LPARAM)szEditBuf);
	
	for(int i=IDC_EDIT_OWNER; i<=IDC_EDIT_PRE; i++)
	{
		if(g_spyinfo_SWIex.swi.hwndArray[i-IDC_EDIT_OWNER])
		{
			wsprintf(szEditBuf, g_szFormat, g_spyinfo_SWIex.swi.hwndArray[i-IDC_EDIT_OWNER]);
			SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, WM_SETTEXT, 0, (LPARAM)szEditBuf);
		}
		else
		{
			SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, i, WM_SETTEXT, 0, (LPARAM)STR_NONE);
		}
	}

	SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
		IDC_EDIT_CLASS, WM_SETTEXT, 0, (LPARAM)g_spyinfo_SWIex.swi.szClassName);
	
	if(g_option.IsSelfFind && g_spyinfo_SWIex.IsSelf)
	{
		SendMessage(hwndFind, WM_GETTEXT, MAX_PATH, (LPARAM)g_spyinfo_SWIex.swi.szCaption);
	}
	
	SendDlgItemMessage(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
		IDC_EDIT_CAPTION, WM_SETTEXT, 0, (LPARAM)g_spyinfo_SWIex.swi.szCaption);
	
} //DoSetFinderInfoText()

//////////////////////////////////////////////////////////////////////////


BOOL CALLBACK DlgProc_SpyTarget(HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam)
{
	HWND hwndBuf;
	HWND hwndFind;
	BOOL isHex;
	
	switch (message)
	{

	case WM_INITDIALOG:
		{
			SetWindowTextFromStrRes(hwnd, IDC_SBTN_HEXDEC, g_hInstance,IDS_HEX);
			
			hwndBuf=CreateMyTooltip(hwnd);
			AddControlTip(hwndBuf, hwnd, 
				IDC_SBTN_HEXDEC, MAKEINTRESOURCE(IDS_TIP_HEXDEC));

			SetDlgItemSubClass(hwnd, IDC_EDIT_HWND, SCP_Edit_SelAll);
			SetDlgItemSubClass(hwnd, IDC_SBTN_HEXDEC, SCP_Static_HandCusror);

			if( GetWindow(hwnd, GW_OWNER)==g_hwnd )
			{
				g_hwnd_SpyTarget=hwnd;
				DoSetMainIconAndCenterWnd(hwnd);
			}
			else if(lParam!=NULL)
			{
				SetWindowText(hwnd, (LPTSTR)lParam);
			}

			return FALSE;
		}

	case WM_MY_GETHWND:
		{
			*(HWND*)lParam=GetWindowLong(hwnd,GWL_USERDATA)?
				(HWND)GetWindowInt(GetDlgItem(hwnd,IDC_EDIT_HWND)):
			(HWND)GetWindowHex(GetDlgItem(hwnd,IDC_EDIT_HWND));
			
			return *(INT*)lParam;
		}

	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{

			case IDC_SBTN_HEXDEC:
				{
					if( STN_CLICKED != HIWORD(wParam) &&
						STN_DBLCLK !=HIWORD(wParam) )
					{
						break;
					}
					SendMessage(hwnd, WM_MY_GETHWND, 0, (LPARAM)&hwndFind);
					isHex=GetWindowLong(hwnd,GWL_USERDATA)?TRUE:FALSE; //·´×ª
					SetWindowLong(hwnd,GWL_USERDATA, isHex?FALSE:TRUE);
					SetWindowTextFromStrRes(hwnd, IDC_SBTN_HEXDEC, 
						g_hInstance, isHex?IDS_HEX:IDS_DEC);
					if(hwndFind)
					{
						wsprintf(g_strBuf, isHex?g_szFormat:TEXT("%u"),hwndFind);
						SetDlgItemText(hwnd, IDC_EDIT_HWND, g_strBuf);
					}
					break;
				}

			case IDC_EDIT_HWND:
				{
					if(HIWORD(wParam) ==EN_CHANGE)
					{
						SendMessage(hwnd, WM_MY_GETHWND, 0, (LPARAM)&hwndFind);
						SetWindowEnable(hwnd, IDOK, IsWindow(hwndFind));
					}
					break;
				}
				
			case IDOK:
				{
					SendMessage(hwnd, WM_MY_GETHWND, 0, (LPARAM)&hwndFind);
					if(IsWindow(hwndFind))
					{
						hwndBuf=GetWindow(hwnd, GW_OWNER);
						if( hwndBuf==g_hwnd )
						{
							g_spyinfo_SWIex.swi.hwnd=hwndFind;
							SetTimer(g_TabDlgHdr.CDI[TAB_FINDER].hwnd, 
								FINDER_TIMER_GETSPYINFOEX, USER_TIMER_MINIMUM, NULL);
							DoPlayEventSound(g_option.IsPlaySound);
							
							if(IsIconic(g_hwnd))
							{
								ShowWindow(hwnd, SW_HIDE);
								ShowWindowAsync(g_hwnd, SW_RESTORE);
							}
						}
						else
						{
							PostMessage(hwndBuf, WM_COMMAND, ID_CMD_ADDHWND, (LPARAM)hwndFind);
						}
					}
					else
					{
						MessageBeep(0);
						SetWindowEnable(hwnd, IDOK, FALSE);
						break;
					}
					
				} //fall through..
			case IDCANCEL:
				{
					if( GetWindow(hwnd, GW_OWNER)==g_hwnd )
					{
						g_hwnd_SpyTarget=NULL;
					}
					EndDialog(hwnd, FALSE);
					break;
				}
			}//switch @ wm_command
			
			return FALSE;
		}//WM_COMMAND

	}//switch message
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);

} //DlgProc_SpyTarget()


//////////////////////////////////////////////////////////////////////////

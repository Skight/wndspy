#include "..\SpyMainDlgTabs.h"
#include "..\SndMsgFunc.h"

//////////////////////////////////////////////////////////////////////////

BOOL OnMsgOptTipNotify(LPARAM lParam);

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK TabDlgProc_Mani(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
		
	case WM_INITDIALOG:
		{
			OnChildDialogInit(hwnd);
			
			OnInitMsgList(GetDlgItem(hwnd,IDC_COMBO_MSG));
			
			HICON hicon;
			
			if( g_siWinVer > WINVER_WIN2K && 
				g_siWinVer < WINVER_VISTA)
			{
				ExtractIconEx(TEXT("SHELL32.DLL"), 24, NULL, &hicon, 1);
			}
			else
			{
				ExtractIconEx(TEXT("calc.exe"), 0, NULL, &hicon, 1);
			}
			
			SendDlgItemMessage(hwnd, IDC_ICONBTN_CALC, 
				STM_SETIMAGE, IMAGE_ICON, (LPARAM)CopyImage(hicon, IMAGE_ICON, 16, 16, NULL));
			DestroyIcon(hicon);
			
			SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_ADDSTRING, 0, (LPARAM)TEXT("Normal"));
			SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETITEMDATA, 0, (LPARAM)SW_SHOWNORMAL);
			SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_ADDSTRING, 0, (LPARAM)TEXT("Minimized"));
			SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETITEMDATA, 1, (LPARAM)SW_MINIMIZE);
			SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_ADDSTRING, 0, (LPARAM)TEXT("Maximized"));
			SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETITEMDATA, 2, (LPARAM)SW_MAXIMIZE);
			
			SendDlgItemMessage(hwnd, IDC_CBDL_HWND, CB_ADDSTRING, 0, (LPARAM)TEXT("HWND_BROADCAST"));
			SendDlgItemMessage(hwnd, IDC_CBDL_HWND, CB_SETITEMDATA, 0, (LPARAM)HWND_BROADCAST);
			SendDlgItemMessage(hwnd, IDC_CBDL_HWND, CB_LIMITTEXT, MAX_HWNDTAG_STR_LEN, 0);
			SendDlgItemMessage(hwnd, IDC_COMBO_MSG, CB_LIMITTEXT, MAX_MSG_STR_LEN, 0);
			SetDlgItemText(hwnd,IDC_EDIT_WPARAM, TEXT("0"));
			SetDlgItemText(hwnd,IDC_EDIT_LPARAM, TEXT("0"));
			SendDlgItemMessage(hwnd, IDC_EDIT_WPARAM, EM_SETLIMITTEXT, MAX_SNDMSG_STR_LEN-16, 0);
			SendDlgItemMessage(hwnd, IDC_EDIT_LPARAM, EM_SETLIMITTEXT, MAX_SNDMSG_STR_LEN-16, 0);

			SetDlgItemSubClass(hwnd, IDC_EDIT_WPARAM,SCP_Edit_SelAll);
			SetDlgItemSubClass(hwnd, IDC_EDIT_LPARAM,SCP_Edit_SelAll);
			SetDlgItemSubClass(GetWindow(GetDlgItem(hwnd,IDC_CBDL_HWND),GW_CHILD), NULL, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_SBTN_HEXDEC, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_ICONBTN_CALC, SCP_Static_HandCusror);			
			SetDlgItemSubClass(hwnd, IDC_BTN_SENDMESSAGE, SubClassProcSndMsgBtn);
			
			return TRUE;
		}
		
		
	case WM_LBUTTONDOWN:
		{
			PostMessage(g_hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}
		
	case WM_SHOWWINDOW:
		{
			if( g_dwBitFlag&BOOLEAN_BIT_TAB_MANI )
			{
				g_dwBitFlag&=~BOOLEAN_BIT_TAB_MANI;
				
				SetWindowEnable(hwnd,IDC_BTN_APPLY,FALSE);
				DoEnableDlgItems(hwnd, IDC_CHK_VISIBLE, IDC_CBDL_SIZESTATE, 1, !g_spyinfo_SWIex.IsSelf);
				
				if( !IsWindow(g_spyinfo_SWIex.swi.hwnd) )
				{
					CheckDlgButton(hwnd,IDC_CHK_VISIBLE, BST_UNCHECKED);
					CheckDlgButton(hwnd,IDC_CHK_ENABLED, BST_UNCHECKED);
					CheckDlgButton(hwnd,IDC_CHK_ONTOP, BST_UNCHECKED);
					SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETCURSEL, -1, 0);
					if(g_spyinfo_SWIex.swi.hwnd == (HWND)
						GetWindowInteger(GetDlgItem(hwnd, IDC_CBDL_HWND), 
						SndMsgOpt_IsHex(g_smHdr.SMO.dwFlagSMH)) )
					{
						SetDlgItemText(hwnd, IDC_CBDL_HWND, TEXT(""));
					}
				}
				else
				{	
					g_spyinfo_SWIex.IsWndEnable=IsWindowEnabled(g_spyinfo_SWIex.swi.hwnd);
					g_spyinfo_SWIex.IsWndVisable=IsWindowVisible(g_spyinfo_SWIex.swi.hwnd);

					CheckDlgButton(hwnd,IDC_CHK_ENABLED, 
						g_spyinfo_SWIex.IsWndEnable?BST_CHECKED:BST_UNCHECKED);

					CheckDlgButton(hwnd,IDC_CHK_VISIBLE,
						g_spyinfo_SWIex.IsWndVisable?BST_CHECKED:BST_UNCHECKED);

					CheckDlgButton(hwnd,IDC_CHK_ONTOP,
						(GetWindowLong(g_spyinfo_SWIex.swi.hwnd, 
						GWL_EXSTYLE)&WS_EX_TOPMOST) ? BST_CHECKED:BST_UNCHECKED);

					SetWindowEnable(hwnd,IDC_CHK_ONTOP, 
						((GetWindowLong(g_spyinfo_SWIex.swi.hwnd,GWL_STYLE)&WS_CHILD) || 
						g_spyinfo_SWIex.IsSelf) ? FALSE:TRUE);
					
					if( IsZoomed(g_spyinfo_SWIex.swi.hwnd) )
					{
						SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETCURSEL, 2, 0);
					}
					else if( IsIconic(g_spyinfo_SWIex.swi.hwnd) )
					{
						SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETCURSEL, 1, 0);
					}
					else
					{
						SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETCURSEL, 0, 0);
					}
				}
				
				PostMessage(hwnd, WM_COMMAND, 
					MAKEWPARAM(IDC_CBDL_HWND, CBN_EDITUPDATE), 
					(LPARAM)GetDlgItem(hwnd, IDC_CBDL_HWND));
				
			}//if..else...
			
			break;
		}

	case WM_NOTIFY:
		{
			if( OnMsgOptTipNotify(lParam) )
			{
				break;
			}
			return OnToolTipNotify(wParam, lParam);
		}

	case WM_COMMAND:
		{
			INT iState;
			switch ( LOWORD(wParam) )
			{

			case IDC_BTN_APPLY:
				{
					if( !IsWindow( g_spyinfo_SWIex.swi.hwnd) || 
						g_spyinfo_SWIex.IsSelf )
					{
						MessageBeep(0);
					}
					else
					{
						
						if( BST_UNCHECKED==IsDlgButtonChecked(hwnd, IDC_CHK_VISIBLE) )
						{
							iState=SW_HIDE;
						}
						else
						{
							iState=SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, 
								CB_GETITEMDATA, 
								(WPARAM)SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, 
								CB_GETCURSEL, 0, 0),
								(LPARAM)0);
						}
						
						if(iState==SW_MINIMIZE)
						{
							CloseWindow(g_spyinfo_SWIex.swi.hwnd);
						}
						else
						{
							ShowWindowAsync(g_spyinfo_SWIex.swi.hwnd, iState); 
						}
						
						SetWindowEnable(g_spyinfo_SWIex.swi.hwnd, NULL, 
							(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_ENABLED)==BST_CHECKED);
						
						//g_dwBitFlag&=~BOOLEAN_BIT_DLG_TOPCHK;
						SetWindowTopmost(g_spyinfo_SWIex.swi.hwnd, 
							(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_ONTOP)==BST_CHECKED);
					}
					
					SetWindowEnable(hwnd, IDC_BTN_APPLY, FALSE);
					SetFocus(GetDlgItem(hwnd,IDC_SBTN_HEXDEC));//effect purpose only...
					
					break;
				}

			case IDC_CBDL_SIZESTATE:
				if(CBN_SELCHANGE != HIWORD(wParam) )
				{
					break;
				}//fall through...
			case IDC_CHK_ONTOP:
			case IDC_CHK_ENABLED:
			case IDC_CHK_VISIBLE:
				{
					if(IsWindow( g_spyinfo_SWIex.swi.hwnd) )
					{
						SetWindowEnable(hwnd, IDC_BTN_APPLY, TRUE);
					}
					else
					{
						if(LOWORD(wParam)!=IDC_CBDL_SIZESTATE)
						{
							CheckDlgButton(hwnd,LOWORD(wParam), BST_UNCHECKED);
						}
						else
						{
							SendDlgItemMessage(hwnd, IDC_CBDL_SIZESTATE, CB_SETCURSEL, -1, 0);
						}
					}
					break;
				}
				
			case IDC_ICONBTN_CALC:
				{
					if( STN_CLICKED==HIWORD(wParam))
					{
						OnRunCalc((HWND)lParam);
					}
					break;
				}
				
			case IDM_POSTMSG:
			case IDM_SNDMSG:
			case IDC_BTN_SENDMESSAGE:
				{
					OnSendMessageToTarget(hwnd, &g_smHdr, LOWORD(wParam)==IDM_POSTMSG);
					return TRUE;
				}
				
			case IDC_BTN_MSGSETTING:
				{
					if( INIT_ERROR ==
						DialogBox(g_hInstance, 
						MAKEINTRESOURCE(IDD_MSG_SETTING), g_hwnd, DlgProc_SndMsgSetting) )
					{
						SetWindowEnable(hwnd, IDC_BTN_MSGSETTING, FALSE);
					}
					return TRUE;
				}
				
			case IDC_EDIT_WPARAM:
			case IDC_EDIT_LPARAM:
				{
					if(	HIWORD(wParam) == EN_KILLFOCUS)
					{
						if( 0==GetWindowTextLength((HWND)lParam) )
						{
							SetWindowText((HWND)lParam, TEXT("0"));
						}
					}
					break;
				}
				
			case IDC_CBDL_HWND:
				{
					if(	HIWORD(wParam) == CBN_SELCHANGE)
					{
						PostMessage(hwnd, WM_COMMAND, 
							MAKEWPARAM(IDC_CBDL_HWND,CBN_EDITUPDATE), 
							(LPARAM)GetDlgItem(hwnd,IDC_CBDL_HWND));
					}
					else if(HIWORD(wParam) ==CBN_EDITUPDATE)
					{
						SetWindowEnable(hwnd, IDC_BTN_SENDMESSAGE,
							GetTargetHwndForSMH(hwnd, &g_smHdr) && 
							IsWindowEnabled(GetDlgItem(hwnd,IDC_BTN_MSGSETTING)));
					}
					break;
				}
				
			case IDC_COMBO_MSG:
				{
					if(	HIWORD(wParam) == CBN_SELCHANGE )
					{
						g_smHdr.iMsgIndex=SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
						SetWindowEnable(hwnd, IDC_BTN_MSGSETTING, g_smHdr.iMsgIndex!=CB_ERR);
					}
					else if(HIWORD(wParam) ==CBN_EDITUPDATE)
					{
						g_smHdr.iMsgIndex=GetMessageForSMH(hwnd, &g_smHdr);
						SetWindowEnable(hwnd, IDC_BTN_MSGSETTING, g_smHdr.iMsgIndex!=CB_ERR );
					}
					else
					{
						break;
					}

					PostMessage(hwnd, WM_COMMAND, 
						MAKEWPARAM(IDC_CBDL_HWND,CBN_EDITUPDATE), 
						(LPARAM)GetDlgItem(hwnd,IDC_CBDL_HWND));
					SndMsgOpt_GoToLoadProfile(hwnd, 100);
					
					break;
				}
				
			case IDC_SBTN_HEXDEC:
				{
					if( STN_CLICKED == HIWORD(wParam) ||
						STN_DBLCLK ==HIWORD(wParam) )
					{
						if( GetSndMsgHeader(hwnd, &g_smHdr, FALSE) )
						{
							//DoSwitchHexDec contains ReleaseSndMsgHeader() call...
							DoSwitchHexDec(hwnd, SndMsgOpt_IsDec(g_smHdr.SMO.dwFlagSMH), &g_smHdr, TRUE);
						}
					}
					break;
				}

			}//switch LOWORD(wParam)
			
			return FALSE;
		}//WM_COMMAND
				
	case WM_DESTROY:
		{
			DoUnSetCtrlIcon(hwnd, IDC_ICONBTN_CALC);
			break;
		}
		
	}//switch message...
	
	return FALSE;
			 
}

//////////////////////////////////////////////////////////////////////////

VOID CALLBACK TimerProc_SetCalcWndPos(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	HWND hwndCalc=FindWindow(TEXT("SciCalc"), NULL);
	
	if( IsWindow(hwndCalc) )
	{
		KillTimer(hwnd, idEvent);
		SetWindowPos(hwndCalc, 
			g_option.IsTopMost?HWND_TOPMOST:HWND_NOTOPMOST, 
			NULL, NULL, NULL, NULL, SWP_NOMOVE|SWP_NOSIZE);
	}
	
} //TimerProc_SetCalcWndPos();

void OnRunCalc(HWND hwndTimerOwner)
{
	HKEY hKey=NULL;
	HWND hwndCalc=NULL;
	DWORD dwVar=0;
	
	hwndCalc=FindWindow(TEXT("SciCalc"), NULL);
				
	if(hwndCalc)
	{
		SendMessage(hwndCalc, WM_COMMAND, 
			g_siWinVer<=WINVER_WIN9X? MAKEWPARAM(303,0):MAKEWPARAM(304,0), 0);
		
		SetForegroundWindow(hwndCalc);
		ShowWindow(hwndCalc, SW_RESTORE);
	}
	else
	{
		if(g_siWinVer>=WINVER_VISTA)
		{		
			if( ERROR_SUCCESS == 
				RegOpenKeyEx(HKEY_CURRENT_USER,
				TEXT("Software\\Microsoft\\Calc"),
				0, KEY_SET_VALUE, &hKey) )
			{
				RegSetValueEx(hKey, TEXT("layout"), NULL, REG_DWORD, (LPBYTE)&dwVar, sizeof(DWORD));
			}
			
			if(hKey) RegCloseKey(hKey);
		}
		else
		{
			WriteProfileString(TEXT("SciCalc"), TEXT("layout"), TEXT("0"));
		}
		
		WinExec("calc.exe", SW_RESTORE);
	}
	
	SetTimer(hwndTimerOwner, 1000, USER_TIMER_MINIMUM, TimerProc_SetCalcWndPos);
	
} //OnRunCalc();

//////////////////////////////////////////////////////////////////////////

VOID OnInitMsgList(HWND hwndList)
{
	INT i=0;
	INT iBuf;
	
	ComboBox_SetMinVisible(hwndList, 16);
	SendMessage(hwndList, CB_SETDROPPEDWIDTH, 199, 0);

	while( ST_WndMsg[i].lValue != ST_END )
	{
		iBuf=SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)ST_WndMsg[i].szSymbol);
		SendMessage(hwndList, CB_SETITEMDATA, iBuf, (LPARAM)ST_WndMsg[i].lValue);
		i++;
	}

} //OnInitMsgList();

//////////////////////////////////////////////////////////////////////////

BOOL OnMsgOptTipNotify(LPARAM lParam)
{
	TCHAR szBuf[MAX_PATH];
	INT iParamIndex;
	INT iCtlId;
	iCtlId=GetDlgCtrlID((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom);
	if(	((LPNMTTDISPINFO)lParam)->hdr.code==TTN_NEEDTEXT &&
		( iCtlId==IDC_STATIC_WPARAM || iCtlId==IDC_STATIC_LPARAM ) )
	{
		GetWindowText((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom, g_strBuf, MAX_PATH);
		iParamIndex= ( iCtlId==IDC_STATIC_WPARAM ? 0:1 );
		GetParamTypeString(szBuf, 
			SndMsgParamOpt_GetDataFor(g_smHdr.SMO.wFlagParam[iParamIndex]), 
			SndMsgParamOpt_GetDataType(g_smHdr.SMO.wFlagParam[iParamIndex]));
		lstrcat(g_strBuf, TEXT(":"));
		lstrcat(g_strBuf, szBuf);
		((LPNMTTDISPINFO)lParam)->lpszText=g_strBuf;
		return TRUE;
	}
	return FALSE;

} //OnWndRectTipNotify()

//////////////////////////////////////////////////////////////////////////

LRESULT APIENTRY SubClassProcSndMsgBtn(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	
	if( message==WM_RBUTTONUP )
	{
		POINT pt;
		pt.x=(int)(short)LOWORD(lParam); 
		pt.y=(int)(short)HIWORD(lParam);
		ClientToScreen(hwnd, &pt);
		
		HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_EDIT));

		TrackPopupMenu(GetSubMenu(hMenu, ID_SUBMENU_BTN_SNDMSG), 
			TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_TabDlgHdr.CDI[TAB_MANI].hwnd, NULL) ;
		
		DestroyMenu(hMenu);
	}
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
	
} //SubClassProcSndMsgBtn();

//////////////////////////////////////////////////////////////////////////

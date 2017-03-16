#include "..\SpyMainDlgTabs.h"

BOOL CALLBACK TabDlgProc_Class(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

	case WM_INITDIALOG :
		{
			OnChildDialogInit(hwnd);

			SetDlgItemSubClass(hwnd,IDC_SICON_BIG,SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd,IDC_SICON_SMALL,SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd,IDC_BTN_LOCATE,SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd,IDC_EDIT_MODULEFILE,SCP_HwndFileEdit);

			return TRUE;
		}

	case WM_LBUTTONDOWN:
		{
			PostMessage(g_hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}

	case WM_SHOWWINDOW:
		{
			if(g_dwBitFlag&BOOLEAN_BIT_TAB_CLASS)
			{
				g_dwBitFlag&=~BOOLEAN_BIT_TAB_CLASS;

				OnFilterClassStyle(GetDlgItem(hwnd,IDC_CBDL_CS), 
					g_spyinfo_SWIex.wcex.style);
			}

			break;
		}

	case WM_NOTIFY:
		{
			INT iVar;
			iVar=GetDlgCtrlID((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom);
			if(	((LPNMTTDISPINFO)lParam)->hdr.code==TTN_NEEDTEXT )
			{
				if( iVar==IDC_SICON_CURSOR || iVar==IDC_EDIT_HCURSOR)
				{
					if(g_spyinfo_SWIex.wcex.hCursor)
					{
						((LPNMTTDISPINFO)lParam)->lpszText=OnGetClassHCursorSymbol(NULL);
					}
					break;
				}
				else if(iVar==IDC_EDIT_HBKBRUSH && 
					g_spyinfo_SWIex.wcex.hbrBackground )
				{
					((LPNMTTDISPINFO)lParam)->lpszText=OnGetClassHBrushSymbol(g_strBuf);
					break;
				}
#ifdef WS_TIP_CLASSSTYLE
				else if(iVar==IDC_CBDL_CS)
				{
					g_strBuf[0]=0;
					GetAllTextsFromListWnd(g_strBuf, MAX_STRBUF_LEN, 
						(HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom, 
						CB_GETCOUNT, CB_GETLBTEXT, CB_GETLBTEXTLEN);
					((LPNMTTDISPINFO)lParam)->lpszText=g_strBuf;
					break;
				}
#endif
				
			}//TTN_NEEDTEXT...

			return	OnToolTipNotify(wParam, lParam);

		}

	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_SICON_BIG:
			case IDC_SICON_SMALL:
				{
					if( STN_CLICKED==HIWORD(wParam))
					{
						g_hTarget=(HANDLE)
							SendDlgItemMessage(hwnd, LOWORD(wParam), STM_GETIMAGE, IMAGE_ICON, 0);
						DialogBox(g_hInstance, 
							MAKEINTRESOURCE(IDD_SAVEICON), g_hwnd, DlgProc_IconColorDepthsSel);
					}
					break;
				}
				
			case IDC_BTN_LOCATE:
				{
					if( STN_CLICKED==HIWORD(wParam))
					{
						OnLocateFileFromCtrlAsync(hwnd, IDC_EDIT_MODULEFILE);
						DoDisableCtrlForAMoment(hwnd, LOWORD(wParam), 500);
					}
					break;
				}
			}
			break;
		}//WM_COMMAND

	case WM_DESTROY:
		{
			DoUnSetCtrlIcon(hwnd, IDC_SICON_BIG);
			DoUnSetCtrlIcon(hwnd, IDC_SICON_SMALL);
			DoUnSetCtrlCursor(hwnd, IDC_SICON_CURSOR);
			break;
		}
		
	}//switch message...
	
	return FALSE;
			 
} //TabDlgProc_Class()

//////////////////////////////////////////////////////////////////////////

LPTSTR OnGetClassHCursorSymbol(LPTSTR szTextBuf)
{
	INT i;
	if(g_spyinfo_SWIex.wcex.hCursor)
	{
		i=0;
		while(ST_SysCursors[i].lValue!=ST_END)
		{
			if( g_spyinfo_SWIex.wcex.hCursor ==
				LoadCursor(NULL, (LPTSTR)ST_SysCursors[i].lValue) )
			{
				if(szTextBuf)
				{
				 wsprintf(szTextBuf, TEXT("LoadCursor(0, %s)"), ST_SysCursors[i].szSymbol);
				}
				return ST_SysCursors[i].szSymbol;
			}
			i++;
		}
	}
	return NULL;

} //OnGetClassHCursorSymbol()

//////////////////////////////////////////////////////////////////////////

LPTSTR OnGetClassHBrushSymbol(LPTSTR szTextBuf)
{
	INT i;

	if(g_spyinfo_SWIex.wcex.hbrBackground)
	{
		i=0;
		while(ST_BrushColors[i].lValue!=ST_END)
		{
			if( ST_BrushColors[i].lValue+1 ==
				(LONG)g_spyinfo_SWIex.wcex.hbrBackground)
			{
				wsprintf(szTextBuf, TEXT("%s + 1"), ST_BrushColors[i].szSymbol);
				return szTextBuf;
			}
			i++;
		}
	}
	return NULL;

} //OnGetClassHBrushSymbol()

//////////////////////////////////////////////////////////////////////////

void OnFilterClassStyle(HWND hwndComBox, DWORD dwCS)
{
	int i=0;
	DWORD dwCSBuf=0;
	TCHAR szCS[64];

	SendMessage(hwndComBox, CB_RESETCONTENT, 0, 0);
	if(dwCS==NULL)
	{
		SetWindowEnable(hwndComBox,NULL,FALSE);
		return;
	}

	SetWindowEnable(hwndComBox,NULL,TRUE);

	while( ST_ClassStyle[i].lValue!=ST_END )
	{
		if( ST_ClassStyle[i].lValue == (LONG)
			(ST_ClassStyle[i].lValue&dwCS)
			)//if...
		{
			SendMessage(hwndComBox, 
				CB_ADDSTRING, 0, (LPARAM)ST_ClassStyle[i].szSymbol);
			dwCSBuf |= ST_ClassStyle[i].lValue;
		}
		i++;

	}//while...

	dwCSBuf=dwCS&~dwCSBuf;

	if( dwCSBuf )
	{
		wsprintf(szCS, g_szFormat, dwCSBuf);
		SendMessage(hwndComBox, CB_ADDSTRING, 0, (LPARAM)szCS);
	}

	SendMessage(hwndComBox, CB_SETCURSEL, 0, 0);

} //OnFilterClassStyle()

//////////////////////////////////////////////////////////////////////////
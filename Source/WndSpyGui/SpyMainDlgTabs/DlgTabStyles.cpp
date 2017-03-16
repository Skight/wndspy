#include "..\SpyMainDlgTabs.h"

BOOL CALLBACK TabDlgProc_Styles(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
		
	{
	case WM_INITDIALOG:
		{
			OnChildDialogInit(hwnd);
			
			OnInitAliasList(GetDlgItem(hwnd, IDC_CBDL_KNOWN));

			return TRUE;
		}
		
	case WM_LBUTTONDOWN:
		{
			PostMessage(g_hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}

#ifdef HS_COLOR_LISTBOX	
	case   WM_CTLCOLORLISTBOX: 
		{

			if(GetDlgCtrlID((HWND)lParam)!=IDC_LIST_WS &&
				GetDlgCtrlID((HWND)lParam)!=IDC_LIST_WSEX //|| 
				/*!IsWindowEnabled((HWND)lParam)*/)
			{
				break;
			}
			
			HBRUSH hBrush;
			hBrush = GetSysColorBrush(COLOR_MENU);
			SetBkColor((HDC)wParam, GetSysColor(COLOR_MENU));
			SetTextColor((HDC)wParam, (COLORREF)GetSysColor(COLOR_MENUTEXT));
			
			return (INT)hBrush;
		}
#endif// HS_COLOR_LISTBOX
		
	case WM_SHOWWINDOW:
		{
			if(g_dwBitFlag&BOOLEAN_BIT_TAB_STYLE)
			{
				g_dwBitFlag&=~BOOLEAN_BIT_TAB_STYLE;

				SetSelKnownClassIndex(hwnd, IDC_CBDL_KNOWN, g_spyinfo_SWIex.swi.szClassName);
			}

			break;
		}


	case WM_COMMAND:
		{	
			switch ( LOWORD(wParam) )
			{
			case IDC_LIST_WS:
			case IDC_LIST_WSEX:
				{
					if(LBN_SELCHANGE == HIWORD(wParam))
					{
						lstrcpyn(g_strBuf,TEXT("\0"),2);

						SendMessage((HWND)lParam, LB_GETTEXT, 
							(WPARAM)SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0), 
							(LPARAM)g_strBuf);
						SetDlgItemText(hwnd,IDC_EDIT_SYMBOL, g_strBuf);
					}
					break;
				}

			case IDC_CBDL_KNOWN:
				{
					if(LBN_SELCHANGE == HIWORD(wParam))
					{
					//	MessageBeep(0);
					OnFilterWndStyle(GetDlgItem(hwnd, IDC_CBDL_KNOWN), 
							GetDlgItem(hwnd,IDC_LIST_WS), 
							GetDlgItem(hwnd,IDC_LIST_WSEX),
							g_spyinfo_SWIex.wi.dwStyle,
							g_spyinfo_SWIex.wi.dwExStyle);
					SetDlgItemText(hwnd,IDC_EDIT_SYMBOL, TEXT(""));
					}
					break;

				}
				break;
			}//switch @ wm_command
			
			break;
		}//WM_COMMAND
		
		
	}//switch message...
	
	return FALSE;

} //TabDlgProc_Styles()


//////////////////////////////////////////////////////////////////////////
void OnInitAliasList(HWND hwndComBox)
{
	INT i=0;
	INT iIndex;
	ComboBox_SetMinVisible(hwndComBox, 15);
	//SendMessage(hwndComBox, CB_SETDROPPEDWIDTH, 125, 0);
	while(ST_ClassStyleList[i].szSymbol)
	{
		iIndex=SendMessage(hwndComBox, CB_ADDSTRING, 0, (LPARAM)ST_ClassStyleList[i].szSymbol);
		SendMessage(hwndComBox, CB_SETITEMDATA, iIndex, (LPARAM)ST_ClassStyleList[i].lValue);
		i++;
	}

}//OnInitAliasList()

//////////////////////////////////////////////////////////////////////////
LONG GetKnownClassID(LPTSTR szClassName)
{
	LONG lResult=0;
	INT i=0;

	while(ST_KnownClassList[i].szSymbol)
	{
		if ( 0 == lstrcmpi(szClassName, ST_KnownClassList[i].szSymbol) )
		{
			lResult=ST_KnownClassList[i].lValue;
			break;
		}
		i++;
	}

	return lResult;

}//GetKnownClassID()

//////////////////////////////////////////////////////////////////////////

LONG SetSelKnownClassIndex(HWND hwndDlg, INT iComboBoxId, LPTSTR szClassName)
{

	LONG lResult=GetKnownClassID(szClassName);

	SetWindowEnable(hwndDlg, iComboBoxId, !lResult);
	SendDlgItemMessage(hwndDlg, iComboBoxId, CB_SETCURSEL,
		(WPARAM)lResult, 0);
	SendMessage(hwndDlg, WM_COMMAND, 
					MAKEWPARAM(iComboBoxId,LBN_SELCHANGE), 0);
	return lResult;

}//SetSelKnownClassIndex()

//////////////////////////////////////////////////////////////////////////

LRESULT DoFilterSymbol(SYMBOL_FILTER_PARAM* sfParam)
{
	TCHAR szOutput[128];
	TCHAR szFormat[64];

	if(sfParam->STitems)
	{
		while( HIWORD(sfParam->STitems[sfParam->iPos].lValue)!=0xFFFF )
		{
			if( sfParam->STitems[sfParam->iPos].lValue == (LONG)
				(sfParam->STitems[sfParam->iPos].lValue&sfParam->dwVal)
				)//if...
			{
				wsprintf(szFormat, TEXT("(%s) %%s\0"), g_szFormat);
				wsprintf(szOutput, szFormat, 
					sfParam->STitems[sfParam->iPos].lValue,
					sfParam->STitems[sfParam->iPos].szSymbol
					);
				//DEBUG_ERROR_MSG(szOutput)
				SendMessage(sfParam->hwndlist, LB_ADDSTRING, 0, (LPARAM)szOutput);
				sfParam->dwVal &= ~sfParam->STitems[sfParam->iPos].lValue;
			}
			sfParam->iPos++;

		}//while...

		return (LONG)LOWORD(sfParam->STitems[sfParam->iPos].lValue);
	}
	else
	{
		wsprintf(szFormat, TEXT("(%s) (Unknown)\0"), g_szFormat);
		wsprintf(szOutput, szFormat, sfParam->dwVal);
		SendMessage(sfParam->hwndlist, LB_ADDSTRING, 0, (LPARAM)szOutput);
		return 0x0000FFFF;
	}

} //DoFilterSymbol()

//////////////////////////////////////////////////////////////////////////
LRESULT DoFilterSpecCtrlExStyle(SYMBOL_FILTER_PARAM* pSFP)
{
	SYMBOL_TABLE_ITEM* STitemsBuf;
	LONG lResult=0x00000000;
	pSFP->dwVal=0;
	pSFP->dwVal=SendMessage(g_spyinfo_SWIex.swi.hwnd, 
			(UINT)pSFP->STitems[pSFP->iPos].szSymbol, 0,0);
	
	if(pSFP->dwVal)
	{
		lResult=0xEEEE0000;
		DlgItemPrintf(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WSEX2, g_szFormat, pSFP->dwVal);
		SetWindowEnable(pSFP->hwndlist,NULL,TRUE);
	}

	pSFP->iPos+=1;
	lResult|=DoFilterSymbol(pSFP);

	if(pSFP->dwVal)
		{
			STitemsBuf=pSFP->STitems;
			pSFP->STitems=NULL;
			DoFilterSymbol(pSFP);
			pSFP->STitems=STitemsBuf;
		}

	return lResult;

} //DoFilterSpecCtrlExStyle()

//////////////////////////////////////////////////////////////////////////
void OnFilterWndStyle(HWND hwndCombo, 
					  HWND hwndlist, HWND hwndlistEx,
					  DWORD dwVal, DWORD dwValEx)
{

	SYMBOL_FILTER_PARAM sfp;
	SYMBOL_TABLE_ITEM* STitems;
	DWORD dwWS=dwVal;
	DWORD dwWSex=dwValEx;
	DWORD dwWSBuf=0;
	DWORD dwWSexBuf=0;
	BOOL IsEx2=FALSE;
	BOOL isEx=FALSE;
	LONG lResult;

	SendMessage(hwndlist, LB_RESETCONTENT, 0, 0);
	SendMessage(hwndlistEx, LB_RESETCONTENT, 0, 0);
	SetWindowEnable(hwndlist,NULL, dwVal? TRUE:FALSE);
	SetWindowEnable(hwndlistEx,NULL, dwValEx? TRUE:FALSE);

	if(dwVal==0 && dwValEx==0)
	{
		SendMessage(hwndCombo, CB_SETCURSEL, -1, 0);
		return;
	}

	STitems=(SYMBOL_TABLE_ITEM*)SendMessage(hwndCombo, CB_GETITEMDATA, 
		(WPARAM)SendMessage(hwndCombo, CB_GETCURSEL, 0, 0), 0);

	if(!STitems)
	{
		STitems=(SYMBOL_TABLE_ITEM*)ST_WindowStyle;
	}

	sfp.STitems=STitems;
	sfp.hwndlist=hwndlist;
	sfp.dwVal=dwVal;
	sfp.iPos=0;
	do
	{
		lResult=DoFilterSymbol(&sfp);
		if(isEx)
		{
			dwWSexBuf |= dwValEx & ~sfp.dwVal;
			dwValEx=sfp.dwVal;
		} 
		else 
		{
			dwWSBuf |= dwVal & ~sfp.dwVal;
			dwVal=sfp.dwVal;
		}
		
		if( lResult != 0xFFFF )
		{
			if(sfp.STitems[sfp.iPos].szSymbol) //link to another SymbolTable?
			{
				if(lResult&ST_FLAG_CTRLEX)
				{
					sfp.hwndlist=hwndlistEx;
					lResult=DoFilterSpecCtrlExStyle(&sfp);
					if(HIWORD(lResult))
					{
						IsEx2=TRUE;
					}
					lResult=LOWORD(lResult);
				}
				else
				{
					sfp.STitems=(SYMBOL_TABLE_ITEM *)
						sfp.STitems[sfp.iPos].szSymbol;
					sfp.iPos=0;
				}
			}
			else
			{
				sfp.iPos++;
			}
			
			isEx=lResult&ST_FLAG_ISEX;
			if (isEx)
			{
				sfp.hwndlist=hwndlistEx;
				sfp.dwVal=dwValEx;
			}
			else
			{
				sfp.hwndlist=hwndlist;
				sfp.dwVal=dwVal;
			}

			if(lResult&ST_FLAG_RESETSTATE)
			{
				sfp.dwVal=isEx ? dwWSex : dwWS; 
			}

		}

	}
	while(lResult != 0xFFFF);

	sfp.hwndlist=hwndlist;
	sfp.iPos=0;
	sfp.dwVal=dwWS&~dwWSBuf;
	if(dwWS&WS_SYSMENU)
	{
		sfp.STitems=(SYMBOL_TABLE_ITEM*)ST_TopWnd;
	}
	else
	{
		sfp.STitems=(SYMBOL_TABLE_ITEM*)ST_ChildWnd;
	}
	DoFilterSymbol(&sfp);

	sfp.STitems=NULL;
	if( sfp.dwVal )
	{
		sfp.hwndlist=hwndlist;
		DoFilterSymbol(&sfp);
	}	

	sfp.dwVal=dwWSex&~dwWSexBuf;
	if( sfp.dwVal )
	{
		sfp.hwndlist=hwndlistEx;
		DoFilterSymbol(&sfp);
	}

	SetWindowVisible(g_TabDlgHdr.CDI[2].hwnd, IDC_EDIT_WSEX2, IsEx2);
	SetWindowVisible(g_TabDlgHdr.CDI[2].hwnd, IDC_STATIC_CTRLWSEX, IsEx2);

} //OnFilterWndStyle()

//////////////////////////////////////////////////////////////////////////
#include "WndSpyGui.h"
#include "DlgPswSniffer.h"


//////////////////////////////////////////////////////////////////////////

#define WND_MIN_X	420
#define WND_MIN_Y	220

#define LVPSW_COL_HWND	0
#define LVPSW_COL_TEXT	1
#define LVPSW_COL_LEN	2
#define LVPSW_WIDTH_HWND	105
#define LVPSW_WIDTH_TEXT	180
#define LVPSW_WIDTH_LEN		50

#define REFRESH_INTERVAL	300

//////////////////////////////////////////////////////////////////////////


DWORD_PTR ListView_FN_GetItemData(HWND hwndLV, INT iIndex)
{
	LVITEM lvi;
	lvi.lParam=0;
	lvi.mask=LVIF_PARAM;
	lvi.iItem=iIndex;
	ListView_GetItem(hwndLV, &lvi);
	return lvi.lParam;
}

INT ListView_FN_GetItemText(HWND hwndLV, INT iItem, INT iSubItem, LPTSTR szTextBuf, INT iMaxBufLen)
{
	LVITEM lvi;
	lvi.mask=LVIF_TEXT;
	lvi.iSubItem=iSubItem;
	lvi.cchTextMax=iMaxBufLen;
	lvi.pszText=szTextBuf;
	return SendMessage(hwndLV, LVM_GETITEMTEXT, (WPARAM)iItem, (LPARAM)&lvi);
}

//////////////////////////////////////////////////////////////////////////

typedef struct tag_ParamsWrapper_PswLister
{
	HWND hwndLV;
	HINSTANCE hOleacc;
	//HANDLE hEndEvent;
	DWORD dwThreadLV;
	BOOL IsAutoClear;
	BOOL IsGetHtmlPsw;
	BOOL bRefreshSignal;
	BOOL bStopSignal;
	BOOL bBusySignal;
	HWND hwndDataList;
	INT iCurColumn;
	BOOL IsAscending;
	HMENU hMenu;
	HIMAGELIST himl;
	INT iImagePos;
	TCHAR szLastModule[MAX_PATH];
	
}PARAM_WRAPPER_PSWLIST;

//////////////////////////////////////////////////////////////////////////

VOID InitImageList(PARAM_WRAPPER_PSWLIST* pW);
BOOL InitListViewColumns(PARAM_WRAPPER_PSWLIST* pW);

INT CALLBACK CompareFuncEx(LPARAM lParam1, LPARAM lParam2, LPARAM iColumn);

BOOL CALLBACK EnumWindowsProc_PSW(HWND hwnd, LPARAM lParam);
BOOL CALLBACK EnumChildWindowsProc_PSW(HWND hwnd, LPARAM lParam);

VOID PasswordView_CheckItems(PARAM_WRAPPER_PSWLIST* pW, BOOL IsRefreshHook);

VOID CALLBACK TimerProc_OnUpdatePasswordList(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID ThreadProc_UpdatePSWView(LPVOID lp);

VOID RefreshWndHookState(HWND hwndTag);

VOID OnShowPswListContextMenu(HWND hwnd, LPARAM lParam, PARAM_WRAPPER_PSWLIST *pW);

//MACRO: GetPSWTextAndUpdateList(2)
#define GetPSWTextAndUpdateList(_hwndTag, _szPswBuffer, _pW) \
GetPSWText(_hwndTag, _szPswBuffer); \
PasswordView_InsertItem(_pW, _hwndTag, 0, _szPswBuffer); \
RefreshWndHookState(_hwndTag)
//MACRO: GetPSWTextAndUpdateList(2) ...END

//////////////////////////////////////////////////////////////////////////

VOID InitImageList(PARAM_WRAPPER_PSWLIST* pW)
{
	pW->iImagePos=0;
	pW->szLastModule[0]=0;
	ImageList_RemoveAll(pW->himl);
}

BOOL InitPasswordListView(PARAM_WRAPPER_PSWLIST* pW) 
{
	LVCOLUMN lvc;
	TCHAR szBuf[SMALL_STRBUF_LEN];

	pW->himl = ImageList_Create(16, 16, ILC_MASK|ILC_COLOR32, 0, 0);
	InitImageList(pW);
	ListView_SetImageList(pW->hwndLV, pW->himl, LVSIL_SMALL);

    ListView_SetExtendedListViewStyle(pW->hwndLV,
		LVS_EX_HEADERDRAGDROP|LVS_EX_FULLROWSELECT|
		LVS_EX_GRIDLINES|LVS_EX_LABELTIP);

	SetWindowTopmost(ListView_GetToolTips(pW->hwndLV), TRUE);
	
	lvc.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM;
	lvc.pszText = szBuf;

	for(int i = 0; i < 1+(IDS_LVHDR_PSW_2-IDS_LVHDR_PSW_0); i++)
	{
        lvc.iSubItem=i;
        if( i == LVPSW_COL_LEN )
		{
			lvc.cx = LVPSW_WIDTH_LEN;
		}
		else
		{
			lvc.cx=(i==LVPSW_COL_HWND?LVPSW_WIDTH_HWND:LVPSW_WIDTH_TEXT);
		}

        LoadString(g_hInstance, IDS_LVHDR_PSW_0 + i, szBuf, SMALL_STRBUF_LEN);
        if (ListView_InsertColumn(pW->hwndLV, i, &lvc) == -1)
		{
            return FALSE;
		}
    }

	EnumWindows(EnumWindowsProc_PSW, (LPARAM)pW);

    return TRUE;
}


INT CALLBACK CompareFuncEx(LPARAM lItem1, LPARAM lItem2, LPARAM lp)  
{

	PARAM_WRAPPER_PSWLIST* pW=(PARAM_WRAPPER_PSWLIST*)lp;
	INT iSign=pW->IsAscending?1:-1;
	TCHAR szText1[MAX_CLASS_NAME], szText2[MAX_CLASS_NAME];

	ListView_FN_GetItemText(pW->hwndLV, lItem1, pW->iCurColumn, szText1, MAX_CLASS_NAME);
	ListView_FN_GetItemText(pW->hwndLV, lItem2, pW->iCurColumn, szText2, MAX_CLASS_NAME);
	
	if( pW->iCurColumn==LVPSW_COL_LEN )
	{
		return iSign*(DecStrToInt(szText1)-DecStrToInt(szText2));
	}
	else
	{
		return iSign*lstrcmpi(szText1, szText2);
	}
}

INT PasswordView_InsertItem(PARAM_WRAPPER_PSWLIST* pW, 
							HWND hwndPassword, INT iSubPswIndex, LPTSTR szPassword)
{
	INT iLen=lstrlen(szPassword);
	HWND hwndLV=pW->hwndLV;
	LVITEM lvi={0};
	TCHAR szSubID[MAX_NUMSTR_LEN];
	TCHAR szText[SMALL_STRBUF_LEN];

	lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_STATE|LVIF_PARAM;
	lvi.lParam=(LPARAM)hwndPassword;
	lvi.cchTextMax=iLen+1;
	lvi.pszText=szText;
	
	wsprintf(szText, HEX_FMT_PREFIX, hwndPassword);
	if(iSubPswIndex)
	{
		wsprintf(szSubID, TEXT("(%d)"), iSubPswIndex);
		lstrcat(szText, szSubID);
	}
	
	LVFINDINFO lfi;
	lfi.flags=LVFI_STRING;
	lfi.psz=szText;

	lvi.iItem=ListView_FindItem(hwndLV, -1, &lfi);
	if(lvi.iItem==-1)
	{
		lvi.iItem=0;
		HICON hIcon;
		TCHAR szModuleName[MAX_PATH];
	
		GetProcessFileNameFromWindow(hwndPassword, szModuleName, MAX_PATH);
		if( 0!=lstrcmpi(szModuleName, pW->szLastModule) )
		{
			lstrcpyn(pW->szLastModule, szModuleName, MAX_PATH);
			if( hIcon=GetIconUsedForFile(szModuleName, TRUE) )
			{
				lvi.iImage=pW->iImagePos=max(0, ImageList_AddIcon(pW->himl, hIcon));
				DestroyIcon(hIcon);
			}
		}
		else
		{
			lvi.iImage=pW->iImagePos;
		}
		lvi.iItem=ListView_InsertItem(hwndLV, &lvi);
	}
	else
	{
		lvi.iSubItem=1;
		lvi.cchTextMax=SMALL_STRBUF_LEN;
		SendMessage(hwndLV, LVM_GETITEMTEXT, lvi.iItem, (LPARAM)&lvi);
		if( 0==lstrcmpi(szText, szPassword) )
		{
			return iLen;
		}
	}
	
	if( lvi.iItem!=-1 )
	{
		lvi.mask=LVIF_TEXT;
		lvi.iSubItem=1;
		lvi.pszText=szPassword;
		ListView_SetItem(hwndLV, &lvi);

		lvi.iSubItem=2;
		lvi.pszText=szText;
		wsprintf(szText, TEXT("%d"), iLen);
		ListView_SetItem(hwndLV, &lvi);
	}

	return iLen;
}

VOID RefreshWndHookState(HWND hwndTag)
{
	if(g_siWinVer>WINVER_WIN9X)
	{
		SendMessageTimeout(hwndTag, 
			WM_GETTEXTLENGTH, 0, 0, SMTO_ABORTIFHUNG, 200, NULL);
	}
	else
	{
		PostMessage(hwndTag, WM_GETTEXTLENGTH, 0, 0);
	}
}


VOID PasswordView_CheckItems(PARAM_WRAPPER_PSWLIST* pW, BOOL IsRefreshHook)
{
	HWND hwndLV=pW->hwndLV;
	LVITEM lvi={0};
	lvi.mask=LVIF_PARAM;
	lvi.iItem=ListView_GetItemCount(hwndLV)-1;
	
	if( lvi.iItem >=0 )
	{
		do
		{
			if( ListView_GetItem(hwndLV, &lvi) )
			{
				if( !IsWindow((HWND)lvi.lParam) || 
					IsIEWindow((HWND)lvi.lParam) )
				{
					ListView_DeleteItem(hwndLV, lvi.iItem);
				}
				else if(IsRefreshHook)
				{
					RefreshWndHookState((HWND)lvi.lParam);
				}
			}
			lvi.iItem--;

		}while( lvi.iItem >=0 );

		if( !ListView_GetItemCount(hwndLV) )
		{
			InitImageList(pW);
		}
	}
	
	HWND hwndCustomPSW;
	INT i=SendMessage(pW->hwndDataList, CB_GETCOUNT, 0, 0);
	while(i)
	{
		hwndCustomPSW=(HWND)SendMessage(pW->hwndDataList, CB_GETITEMDATA, --i, 0);
		if( !IsWindow(hwndCustomPSW) )
		{
			SendMessage(pW->hwndDataList, CB_DELETESTRING, i, 0);
		}
		else if(IsRefreshHook)
		{
			RefreshWndHookState(hwndCustomPSW);
		}
	}

}

BOOL CALLBACK EnumChildWindowsProc_PSW(HWND hwnd, LPARAM lParam)
{
	PARAM_WRAPPER_PSWLIST* pW=(PARAM_WRAPPER_PSWLIST*)lParam;
	INT iHtmlPswCount=0;
	TCHAR szPswBuf[MAX_STRBUF_LEN];
	LPTSTR lp=szPswBuf;

	if( pW->bStopSignal )
	{
		return FALSE;
	}

	pW->bBusySignal=TRUE;

	if( pW->IsGetHtmlPsw )
	{
		iHtmlPswCount=GetHtmlPasswords(GetIEDocInterfaceEx(hwnd, pW->hOleacc), szPswBuf, MAX_STRBUF_LEN);
	}
	if( iHtmlPswCount )
	{
		BOOL IsMultiPsw=(iHtmlPswCount>1);
		do
		{
			lp+=1+PasswordView_InsertItem(pW, 
				hwnd, IsMultiPsw?iHtmlPswCount:0, lp);
			
			iHtmlPswCount--;

		}while((iHtmlPswCount));
	}
	else if( IsPasswordEdit(hwnd) )
	{
		GetPSWTextAndUpdateList(hwnd, szPswBuf, pW);
	}
	
	pW->bBusySignal=FALSE;

	return TRUE;
}

BOOL CALLBACK EnumWindowsProc_PSW(HWND hwnd, LPARAM lParam)
{	
	EnumChildWindows(hwnd, EnumChildWindowsProc_PSW, lParam);
	return TRUE;
}

VOID CALLBACK TimerProc_OnUpdatePasswordList(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	PARAM_WRAPPER_PSWLIST* pW=(PARAM_WRAPPER_PSWLIST*)idEvent;
	
	if( pW->bRefreshSignal )
	{
		pW->bRefreshSignal=FALSE;

		if(pW->IsAutoClear)
		{
			PasswordView_CheckItems(pW, FALSE);
		}
		EnumWindows(EnumWindowsProc_PSW, (LPARAM)pW);

		HWND hwndCustomPSW;
		TCHAR szCustomPSWText[SMALL_STRBUF_LEN];
		szCustomPSWText[0]=0;
		INT i=SendMessage(pW->hwndDataList, CB_GETCOUNT, 0, 0);
		while(i)
		{
			hwndCustomPSW=(HWND)SendMessage(pW->hwndDataList, CB_GETITEMDATA, --i, 0);
			GetPSWTextAndUpdateList(hwndCustomPSW, szCustomPSWText, pW);
		}

	} //if(pW->bRefreshSignal)

} //TimerProc_OnUpdatePasswordList();


VOID ThreadProc_UpdatePSWView(LPVOID lp)
{
	MSG msg;
	HWND hwndList;
	INT idTimer=(INT)lp;
	PARAM_WRAPPER_PSWLIST* pW=(PARAM_WRAPPER_PSWLIST*)lp;

	CoInitialize(NULL);
	
	hwndList=pW->hwndDataList =CreateWindow(WC_COMBOBOX, NULL, 
		WS_CHILD|WS_DISABLED, 0, 0, 0, 0, 
		pW->hwndLV, NULL, g_hInstance, NULL);

	pW->dwThreadLV=GetCurrentThreadId();
	
	SetTimer(hwndList, idTimer, REFRESH_INTERVAL, TimerProc_OnUpdatePasswordList);

	while( GetMessage(&msg, NULL, 0, 0)  )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	KillTimer(hwndList, (UINT_PTR)idTimer);

	CoUninitialize();

	pW->dwThreadLV=NULL;

	_endthread();

} //ThreadProc_UpdatePSWView();

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_PswSniffer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static PARAM_WRAPPER_PSWLIST* pW;

	switch (message)
	{
	case WM_INITDIALOG:
		{
			g_hwnd_PSWsniffer=hwnd;

			pW=(PARAM_WRAPPER_PSWLIST*)
				LocalAlloc(GPTR, sizeof(PARAM_WRAPPER_PSWLIST));
			if(pW==NULL)
			{
				//ShowLastError(g_hwnd);
				DestroyWindow(hwnd);
			}

			CoInitialize(NULL);
			OnInitComCtrl(ICC_LISTVIEW_CLASSES);

			pW->hOleacc=LoadLibrary(TEXT("OLEACC.DLL"));
			pW->hwndLV=GetDlgItem(hwnd, IDC_LIST_PSW);
			
			InitPasswordListView(pW);

			//pW->hEndEvent=CreateEvent(0, FALSE, FALSE, "E_PS");
			//pW->hStopEvent=CreateEvent(0, FALSE, FALSE, "S_PS");

			/*pW->hThreadLV=(HANDLE)*/
			_beginthread(ThreadProc_UpdatePSWView, 0, (LPVOID)pW);
			
			SendMessage(hwnd, WM_SIZE, 0, 0);
			DoSetMainIconAndCenterWnd(hwnd);

			return FALSE;
		}

	case WM_SIZE:
		{
			HWND hwndBuf;
			HDWP hdwp;
			RECT rc;
			RECT rcCtrl;
			POINT pt;
			
#define MARGIN_X_BTN		_HS_GRIP_SIZE
#define MARGIN_Y_BTN		12
#define MARGIN_X_LV			_HS_GRIP_SIZE
#define MARGIN_Y_LV			43

			
			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, (POINT*)&rc);
			ClientToScreen(hwnd, (POINT*)&rc+1);

			hdwp = BeginDeferWindowPos(5);
			
			GetWindowRect(pW->hwndLV, &rcCtrl);
			pt.x=rc.right-rcCtrl.left-MARGIN_X_LV;
			pt.y=rc.bottom-rcCtrl.top-MARGIN_Y_LV;
			hdwp=DeferWindowPos(hdwp, pW->hwndLV, NULL, 
				NULL, NULL, pt.x, pt.y, 
				SWP_NOZORDER|SWP_NOMOVE); //1

			hwndBuf=GetDlgItem(hwnd, IDC_CHK_AUTOCLEAR);
			GetWindowRect(hwndBuf, &rcCtrl);
			pt.x=rcCtrl.left;
			pt.y=rc.bottom-(rcCtrl.bottom-rcCtrl.top)-MARGIN_Y_BTN-2;
			ScreenToClient(hwnd,&pt);
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, NULL, NULL, 
				SWP_NOZORDER|SWP_NOSIZE); //2

			pt.x+=rcCtrl.right-rc.left;
			hwndBuf=GetDlgItem(hwnd, IDC_CHK_GETWEBPSW);
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, NULL, NULL, 
				SWP_NOZORDER|SWP_NOSIZE); //3

			hwndBuf=GetDlgItem(hwnd, IDC_BTN_REFRESH);
			GetWindowRect(hwndBuf, &rcCtrl);
			pt.x=rc.right-(rcCtrl.right-rcCtrl.left)-MARGIN_X_BTN-1;
			pt.y=rc.bottom-(rcCtrl.bottom-rcCtrl.top)-MARGIN_Y_BTN+2;
			ScreenToClient(hwnd,&pt);
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, NULL, NULL, 
				SWP_NOZORDER|SWP_NOSIZE); //4

			hwndBuf=GetDlgItem(hwnd, IDC_SIZEGRIP);
			pt.x= rc.right- _HS_GRIP_SIZE;
			pt.y= rc.bottom- _HS_GRIP_SIZE;
			ScreenToClient(hwnd,&pt);
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, _HS_GRIP_SIZE, _HS_GRIP_SIZE, 
				SWP_NOZORDER); //5

			EndDeferWindowPos(hdwp);
			return FALSE;
		}

	case WM_NOTIFY:
		{
			if(wParam==IDC_LIST_PSW)
			{
				switch( ((LPNMHDR)lParam)->code )
				{
				case LVN_COLUMNCLICK:
					{
						//pW->IsSorting=TRUE;
						pW->IsAscending=!pW->IsAscending;
						pW->iCurColumn=((LPNMLISTVIEW)lParam)->iSubItem;
						ListView_SortItemsEx(pW->hwndLV, CompareFuncEx, (LPARAM)pW);
						break;
					}
				case NM_RCLICK:
					{
						if(pW->hMenu)
						{
							DestroyMenu(pW->hMenu);
						}
						break;
					}
				}
			}
			break;
		}


	case WM_GETMINMAXINFO:
		{
			((LPMINMAXINFO)lParam)->ptMinTrackSize.x = WND_MIN_X;
			((LPMINMAXINFO)lParam)->ptMinTrackSize.y = WND_MIN_Y;
			return FALSE;
		}

	case WM_CONTEXTMENU:
		{
			if( GetDlgCtrlID((HWND)wParam)==IDC_LIST_PSW )
			{
				OnShowPswListContextMenu(pW->hwndLV, lParam, pW);
			}
			return FALSE;
		}
	case WM_COMMAND:
		{
			if( wParam==ID_CMD_ADDHWND )
			{
				INT i;
				TCHAR szBuf[MAX_NUMSTR_LEN];
				wsprintf(szBuf, HEX_FMT_NOPREFIX, lParam);
				i=SendMessage(pW->hwndDataList, CB_FINDSTRING, -1, (LPARAM)szBuf);
				if(i==CB_ERR)
				{
					i=SendMessage(pW->hwndDataList, CB_ADDSTRING, 0, (LPARAM)szBuf);
				}
				SendMessage(pW->hwndDataList, CB_SETITEMDATA, i, lParam);
				pW->bRefreshSignal=TRUE;
				break;
			}

			switch ( LOWORD(wParam) )
			{
			case IDC_CHK_AUTOCLEAR:
				{
					pW->IsAutoClear=(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_AUTOCLEAR);
					break;
				}
			case IDC_CHK_GETWEBPSW:
				{
					pW->IsGetHtmlPsw=(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_GETWEBPSW);
					break;
				}
			case IDC_BTN_REFRESH:
				{
					pW->bRefreshSignal=TRUE;
					break;
				}
			case IDCANCEL:
				{
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				}
			}

			break;
		}//WM_COMMAND

	case WM_CLOSE:
		{
			pW->bStopSignal=TRUE;
			DestroyWindow(hwnd);
			return FALSE;
		}

	case WM_DESTROY:
		{
			if(pW)
			{
				pW->bStopSignal=TRUE;
				
				DoWaitForAWhile(&pW->bBusySignal, MAX_WAITLOOP);
				
				int iMaxTry=10;

				while( pW->dwThreadLV && iMaxTry >0 )
				{
					if( !PostThreadMessage(pW->dwThreadLV, WM_QUIT, 0, 0) )
					{
						iMaxTry--;
						Sleep(100);
					}
				}
				PasswordView_CheckItems(pW, TRUE);
				FreeLibrary(pW->hOleacc);

				ImageList_Destroy(pW->himl);
				CoUninitialize();
				LocalFree((HLOCAL)pW);
			}

			g_hwnd_PSWsniffer=NULL;
			PostMessage(g_hwndTC, WM_MY_TOOL_CLOSE, 0, 0);
			MACRO_TRIM_MEMORY(g_siWinVer>WINVER_WIN9X)			
			return FALSE;
		}

	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
	
} //DlgProc_PswSniffer()

//////////////////////////////////////////////////////////////////////////

#define IDM_HIGHLIGHT	IDM_EDIT_HIGHLIGHT

VOID OnShowPswListContextMenu(HWND hwnd, LPARAM lParam, PARAM_WRAPPER_PSWLIST *pW)
{
	RECT rc;
	POINT pt;
	HMENU hMenuBuf, hMenu;
	HWND hwndPSW;
	INT iHotItem;
	TCHAR szBuf[SMALL_STRBUF_LEN];
	
	pt.x=LOWORD(lParam), pt.y=HIWORD(lParam);
	
	if( IsPtInWnd(&pt, ListView_GetHeader(hwnd)) )
	{
		return;
	}

	iHotItem=ListView_GetSelectedCount(hwnd); //set selected count temporarily...
	if(iHotItem)
	{
		iHotItem=ListView_GetSelectionMark(hwnd);
	}
	else
	{
		iHotItem=-1;
	}
	
	GetWindowRect(hwnd, &rc);
	
	if( !PtInRect(&rc, pt) )
	{
		
		if( iHotItem!=-1 )
		{
			ListView_GetItemRect(hwnd, iHotItem, &rc, LVIR_BOUNDS);
			pt.x=rc.left, pt.y=rc.top;
			ClientToScreen(hwnd, &pt);
		}
		else
		{
			pt.x=rc.left + ((rc.right-rc.left)>>1);
			pt.y=rc.top + ((rc.bottom-rc.top)>>1);
		}
	}
	
	pW->hMenu=LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_EDIT));
	hMenu=GetSubMenu(pW->hMenu, ID_SUBMENU_LV_PSW);
	
	hMenuBuf=GetSubMenu(pW->hMenu, ID_SUBMENU_EDIT_HWNDFILE);
	if( GetMenuString(hMenuBuf, IDM_EDIT_HIGHLIGHT, szBuf, TINY_STRBUF_LEN, MF_BYCOMMAND) )
	{
		InsertMenu(hMenu, 2, MF_BYPOSITION, IDM_HIGHLIGHT, szBuf);
	}
	
	szBuf[0]=0;
	if( iHotItem!=-1 )
	{
		hwndPSW=(HWND)ListView_FN_GetItemData(hwnd, iHotItem);
		if( 0 < ListView_FN_GetItemText(hwnd, iHotItem, 
			LVPSW_COL_TEXT, szBuf, SMALL_STRBUF_LEN) )
		{
			EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND|MF_ENABLED);
		}
	}
	else
	{
		EnableMenuItem(hMenu, IDM_SPYTARGET, MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hMenu, IDM_HIGHLIGHT, MF_BYCOMMAND|MF_GRAYED);
	}
	
	UINT iMenuID = TrackPopupMenu(hMenu, TPM_RETURNCMD|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

	DestroyMenu(pW->hMenu);
	
	switch(iMenuID)
	{
	case IDM_COPY:
		{
			CopyTextToClipBoard(g_hwnd_PSWsniffer, szBuf);
			break;
		}
	case IDM_HIGHLIGHT:
		{
			BringWindowToForeground(GetAncestor(hwndPSW, GA_ROOT)); 
			DoHighlightWindow(hwndPSW, 4);
			break;
		}
	case IDM_SPYTARGET:
		{
			WndSpy_SpyTargetWnd(g_hwndBK, hwndPSW);
			break;
		}
	case IDM_ADD_PSW:
		{
			GetMenuString(hMenu, IDM_ADD_PSW, szBuf, TINY_STRBUF_LEN, MF_BYCOMMAND);
			StrTrim(szBuf, TEXT("(&P)...\0"));
			DialogBoxParam(g_hInstance, MAKEINTRESOURCE(IDD_TOOL_SPYTARGET), 
				g_hwnd_PSWsniffer, DlgProc_SpyTarget, (LPARAM)szBuf);
		}
	}
	
}

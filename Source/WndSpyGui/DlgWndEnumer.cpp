#include "WndSpyGui.h"
#include "DlgWndEnumer.h"

//////////////////////////////////////////////////////////////////////////

#define WND_MIN_X	480
#define WND_MIN_Y	390

//////////////////////////////////////////////////////////////////////////

#define STR_SLIDER_CLASS	TEXT("SpyWmSlider")

#define SLIDER_CX	3

#define IDC_WMSLIDER	5000
#define IDC_STATUSBAR	5001

#define HWND_SYS_ROOT		((HWND)0x0L)

#define I_ICON_OFFSET	(IDI_ICON_W_THREAD-1)

#define REFRESH_INTERVAL	300

//////////////////////////////////////////////////////////////////////////

typedef struct tag_EnmuParam
{
	DWORD dwPID;
	DWORD dwTID;
	HWND hwnd;
	HTREEITEM hTitem;
	HTREEITEM hTitem_WT;
	
} ENUM_PARAM_PARENT_BUFFER;

typedef struct tag_ParamsWrapper_WndEnumr
{
	
	HWND hwndTV;
	HWND hwndTV_WT;
	HWND hwndDataList;
	HWND hwndSB;
	HWND hwndSlider;

	HWND hwndDesktop;
	// for enum proc...
	ENUM_PARAM_PARENT_BUFFER epbParent;

	BOOL bStopSignal;
	BOOL bBusySignal;

	LPTSTR lpszGlBuf;
	HANDLE hThreadSnap;
	HANDLE hProcessSnap;
	
#define I_PROCESS		0
#define I_THREAD		1
#define I_WND			2
#define I_WND_H			3
#define I_WND_S_C		4
#define I_WND_S_C_H		5
#define I_WND_S_HINT	6
#define C_IMAGE_TYPE	7

	INT iDefImage[C_IMAGE_TYPE];
	HIMAGELIST himl;

	TCHAR szTipCount[LABEL_STR_LEN];
	TCHAR szTipDoing[LABEL_STR_LEN];
	TCHAR szTipF5[TINY_STRBUF_LEN]; //for Statusbar hint: F5 refresh..
	TCHAR szTipReady[LABEL_STR_LEN];
	TCHAR szTipWait[LABEL_STR_LEN];
	TCHAR szProcess[LABEL_STR_LEN]; //for loading string resource
	TCHAR szThread[LABEL_STR_LEN];  //for loading string res...
	TCHAR szFindPopTitle[LABEL_STR_LEN]; //find msgbox title...


#define FIND_TYPE_TEXT	0
#define FIND_TYPE_HWND	1
#define FIND_TYPE_PID	2
#define FIND_TYPE_TID	3

	BOOL IsFindReady;
	INT iFindType;
	BOOL IsMatchCase; 
	BOOL IsNumDec;
	HWND hwndTV_Find;
	HTREEITEM hTitem_Find;
	DWORD dwFind;
	TCHAR szFindText[MAX_CLASS_NAME];
	
	//string buffers...
	TCHAR szText[MAX_STRBUF_LEN];
	TCHAR szBuf[MAX_CLASS_NAME];
	TCHAR szTinyBuf[TINY_STRBUF_LEN];
	
} PARAMSWRAPPER_WNDENUMR;

//////////////////////////////////////////////////////////////////////////

HWND CreateWMSildeBar(HWND hwndOwner);
VOID CalcSliderRect(HWND hwndSlider, LPRECT lpRect);

VOID InitImageList(PARAMSWRAPPER_WNDENUMR* pW);
BOOL InitWndTreeView(PARAMSWRAPPER_WNDENUMR* pW);

HTREEITEM ProcessWndTree_InsertSpecProcessItem(PARAMSWRAPPER_WNDENUMR * pW, DWORD dwPID);
HTREEITEM ProcessWndTree_InsertProcessItem(PARAMSWRAPPER_WNDENUMR* pW, PROCESSENTRY32* lpPe32);

HTREEITEM ProcessWndTree_InsertThreadItem(PARAMSWRAPPER_WNDENUMR* pW, THREADENTRY32* lpTe32);
VOID ProcessWndTree_SetThreadItems(PARAMSWRAPPER_WNDENUMR * pW);

HTREEITEM ProcessWndTree_InsertWndItem(PARAMSWRAPPER_WNDENUMR * pW, HWND hwndTag);
BOOL CALLBACK EnumChildWndProc_PWTree(HWND hwnd, LPARAM lParam);

VOID Thread_UpdateWndTreePair(PVOID pvoid);

INT GetItemType(LPTSTR szItemText);
VOID OnRightClick(PARAMSWRAPPER_WNDENUMR* pW, HWND hwndTV);

//////////////////////////////////////////////////////////////////////////

HTREEITEM ProcessWndTree_FindString(PARAMSWRAPPER_WNDENUMR * pW, TVITEM* pTvi);

BOOL CALLBACK DlgProc_Find(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

VOID SetStatusText(PARAMSWRAPPER_WNDENUMR * pW, BOOL IsSetBusy);

//////////////////////////////////////////////////////////////////////////

VOID SetStatusText(PARAMSWRAPPER_WNDENUMR * pW, BOOL IsSetBusy)
{
	if( IsSetBusy )
	{
		SendMessage(pW->hwndSB, SB_SETTEXT, 0, (LPARAM)pW->szTipDoing);
		SendMessage(pW->hwndSB, SB_SETTEXT, 1, (LPARAM)TEXT(""));
		SendMessage(pW->hwndSB, SB_SETTEXT, 2, (LPARAM)pW->szTipWait);
	}
	else
	{
		SendMessage(pW->hwndSB, SB_SETTEXT, 0, (LPARAM)pW->szTipF5);

		wsprintf(pW->szBuf, pW->szTipCount, 
			SendMessage(pW->hwndTV_WT, TVM_GETCOUNT, 0, 0));
		
		SendMessage(pW->hwndSB, SB_SETTEXT, 1, (LPARAM)pW->szBuf);
		SendMessage(pW->hwndSB, SB_SETTEXT, 2, (LPARAM)pW->szTipReady);
	}
}


VOID SendCtrlNotifyMessage(HWND hwndParent, HWND hwndFrom, UINT idFrom, UINT NotifyCode, LPARAM lParam)
{
	NMHDR hdr;

	if( lParam==NULL )
	{
		if( hwndFrom==NULL )
		{
			hwndFrom=GetDlgItem(hwndParent, idFrom);
		}
		if( idFrom==0 )
		{
			idFrom=GetDlgCtrlID(hwndFrom);
		}
		
		hdr.hwndFrom=hwndFrom;
		hdr.idFrom=idFrom;
		hdr.code=NotifyCode;
		
		lParam=(LPARAM)&hdr;
	}

	SendMessage(hwndParent, WM_NOTIFY, (WPARAM)hdr.idFrom, lParam);

}

//////////////////////////////////////////////////////////////////////////

VOID InitImageList(PARAMSWRAPPER_WNDENUMR* pW)
{
	HICON hIcon;
	
	//reset...
	ImageList_RemoveAll(pW->himl);
	
	//I_PROCESS
	ExtractIconEx(TEXT("SHELL32.DLL"), 2, NULL, &hIcon, 1);
	pW->iDefImage[I_PROCESS]=ImageList_AddIcon(pW->himl, hIcon);
	DestroyIcon(hIcon);
	
	// i=0 (I_PROCESS)
	for(int i=1; i<C_IMAGE_TYPE; i++)
	{
		hIcon=(HICON)LoadImage(g_hInstance, 
			MAKEINTRESOURCE(i+I_ICON_OFFSET), IMAGE_ICON, 16, 16, NULL);
		pW->iDefImage[i]=
			ImageList_AddIcon(pW->himl, hIcon);
		DestroyIcon(hIcon);
	}
	
}

BOOL InitWndTreeView(PARAMSWRAPPER_WNDENUMR* pW) 
{

	pW->himl = ImageList_Create(16, 16, ILC_MASK|ILC_COLOR32, 0, 0);
	InitImageList(pW);

	TreeView_SetImageList(pW->hwndTV, pW->himl, TVSIL_NORMAL);
	TreeView_SetImageList(pW->hwndTV_WT, pW->himl, TVSIL_NORMAL);
	SetWindowTopmost(ListView_GetToolTips(pW->hwndTV), TRUE);
	SetWindowTopmost(ListView_GetToolTips(pW->hwndTV_WT), TRUE);
	
	pW->hwndDataList = CreateWindow(
		WC_COMBOBOX, NULL, 
		WS_CHILD|WS_DISABLED, 0, 0, 0, 0, 
		pW->hwndTV, NULL, g_hInstance, NULL);
	
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

HTREEITEM ProcessWndTree_InsertProcessItem(PARAMSWRAPPER_WNDENUMR* pW, PROCESSENTRY32* lpPe32)
{
	
	TVINSERTSTRUCT tvis={0};
	HICON hIcon;
	INT iBuf=pW->iDefImage[I_PROCESS];
	TCHAR szBuf[MAX_PATH];
	
	if( pW->bStopSignal )
	{
		return NULL;
	}
	
	//CharUpper(lpPe32->szExeFile);

	wsprintf(szBuf, TEXT("%s:0x%08X - %s"), 
		pW->szProcess, lpPe32->th32ProcessID, lpPe32->szExeFile);
	
	if( lpPe32->th32ProcessID )
	{
		GetExeFullNameForPE32Ex(lpPe32);
		if( PathFileExists(lpPe32->szExeFile) )
		{
			hIcon=GetIconUsedForFile(lpPe32->szExeFile, TRUE);
			iBuf=ImageList_AddIcon(pW->himl, hIcon);
			DestroyIcon(hIcon);
		}
	}
	
	tvis.hParent=TVI_ROOT;
	tvis.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
	tvis.item.pszText=szBuf;
	tvis.item.cchTextMax=MAX_PATH;
	tvis.item.lParam=lpPe32->th32ProcessID;
	tvis.item.iImage=iBuf;
	tvis.item.iSelectedImage=iBuf;
	
	HTREEITEM hTitem=TreeView_InsertItem(pW->hwndTV, &tvis);
	
	if(hTitem)
	{
		wsprintf(szBuf, HEX_FMT_NOPREFIX, lpPe32->th32ProcessID);
		SendMessage(pW->hwndDataList, CB_SETITEMDATA, 
			(WPARAM)SendMessage(pW->hwndDataList, CB_ADDSTRING, 0, (LPARAM)szBuf), 
			(LPARAM)hTitem);
	}
	
	return hTitem;
	
}

HTREEITEM ProcessWndTree_InsertThreadItem(PARAMSWRAPPER_WNDENUMR* pW, THREADENTRY32* lpTe32)

{
	TVINSERTSTRUCT tvis={0};
	HTREEITEM hTitem=NULL;
	INT iBuf=0;
	TCHAR szBuf[MAX_PATH];
	
	if( pW->bStopSignal )
	{
		return NULL;
	}
	
	wsprintf(szBuf, HEX_FMT_NOPREFIX, lpTe32->th32OwnerProcessID);
	
	iBuf=SendMessage(pW->hwndDataList, CB_FINDSTRING, -1, (LPARAM)szBuf);
	
	if( iBuf!=CB_ERR )
	{
		tvis.hParent=(HTREEITEM)
			SendMessage(pW->hwndDataList, CB_GETITEMDATA, iBuf, 0);
		
		tvis.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
		tvis.item.iImage=tvis.item.iSelectedImage=pW->iDefImage[I_THREAD];
		tvis.item.lParam=lpTe32->th32ThreadID;
		tvis.item.pszText=szBuf;
		tvis.item.cchTextMax=MAX_PATH;
		
		wsprintf(szBuf, TEXT("%s:0x%08X"), pW->szThread, lpTe32->th32ThreadID);
		
		hTitem=TreeView_InsertItem(pW->hwndTV, &tvis);
		
	}
	
	return hTitem;
	
}

HTREEITEM ProcessWndTree_InsertSpecProcessItem(PARAMSWRAPPER_WNDENUMR * pW, DWORD dwPID)
{
	PROCESSENTRY32 pe32={0};
	PROCESS_STRINGS ps={0};
	
	if( pW->bStopSignal )
	{
		return NULL;
	}
	
	GetProcessInfoDirStrs(dwPID, &ps, NULL);
	pe32.dwSize=sizeof(PROCESSENTRY32);
	pe32.th32ProcessID=dwPID;
	lstrcpyn(pe32.szExeFile, ps.szExePath, MAX_PATH);
	
	return ProcessWndTree_InsertProcessItem(pW, &pe32);
	
}

//////////////////////////////////////////////////////////////////////////

HTREEITEM ProcessWndTree_FindString(PARAMSWRAPPER_WNDENUMR * pW, TVITEM* pTvi)
{

	HTREEITEM hTitem=TreeView_GetSelection(pW->hwndTV_Find);
	HTREEITEM hTitemBuf=hTitem;

	if( hTitem == pW->hTitem_Find )
	{
		if( NULL==
			(hTitem=TreeView_FindString(pW->hwndTV_Find, 
				TreeView_GetChild(pW->hwndTV_Find, pW->hTitem_Find), 
				pW->szFindText, pW->IsMatchCase, pTvi)) )
		{
			//find in next..
			if( NULL==
				(hTitem=TreeView_GetNextSibling(pW->hwndTV_Find, pW->hTitem_Find)) )
			{	
				//if reaches the node-end, do backtracking find...
				while( hTitem=TreeView_GetParent(pW->hwndTV_Find, hTitemBuf) )
				{
					hTitemBuf=hTitem;
					hTitem=TreeView_GetNextSibling(pW->hwndTV_Find, hTitem);
					if(hTitem)
					{
						break;
					}
				}
			}
		}
	}

	INT iTry=0;
	
	while( iTry<3 ) //it's a circle...
	{
		while( hTitem)
		{
			hTitemBuf=hTitem;
			hTitem=TreeView_FindString(pW->hwndTV_Find, 
				hTitem, pW->szFindText, pW->IsMatchCase, pTvi);

			if( hTitem )
			{
				return hTitem;
			}
			else
			{
				 while( hTitem=TreeView_GetParent(pW->hwndTV_Find, hTitemBuf) )
				 {
					 hTitemBuf=hTitem;
					 hTitem=TreeView_GetNextSibling(pW->hwndTV_Find, hTitem);
					 if(hTitem)
					 {
						 break;
					 }
				 }
			}
		}

		//here hTitem is null.

		hTitem=hTitemBuf; //restore hTitemBuf to hTitem.
		
		//switch TVs to find...

		pW->hwndTV_Find=(HWND)(pW->hwndTV_Find!=pW->hwndTV?pW->hwndTV:pW->hwndTV_WT);

		//from the top-beginning.
		hTitem=TreeView_GetRoot(pW->hwndTV_Find);
		
		iTry++;
	}

	return NULL;
}


HTREEITEM ProcessWndTree_FindWndThread(PARAMSWRAPPER_WNDENUMR * pW)
{

	HTREEITEM hTitem=NULL;
	TVITEM tvi;
	INT iBuf;

	if( pW->iFindType==FIND_TYPE_PID )
	{
		pW->hwndTV_Find=pW->hwndTV;

		wsprintf(pW->szFindText, HEX_FMT_NOPREFIX, pW->dwFind);
	
		iBuf=SendMessage(pW->hwndDataList, CB_FINDSTRING, -1, (LPARAM)pW->szFindText);
	
		if( iBuf!=CB_ERR )
		{
			hTitem=(HTREEITEM)
				SendMessage(pW->hwndDataList, CB_GETITEMDATA, iBuf, 0);
		}
	}
	else if( pW->iFindType==FIND_TYPE_TID )
	{
		pW->hwndTV_Find=pW->hwndTV;

		HTREEITEM hTitemRoot=TreeView_GetRoot(pW->hwndTV);

		while(hTitemRoot)
		{
			hTitem=TreeView_GetChild(pW->hwndTV, hTitemRoot);

			while(hTitem)
			{
				tvi.mask=TVIF_PARAM|TVIF_HANDLE;
				tvi.hItem=hTitem;
				if( TreeView_GetItem(pW->hwndTV, &tvi) )
				{
					if( tvi.lParam==(LPARAM)pW->dwFind )
					{
						return hTitem;
					}
				}
				
				hTitem=TreeView_GetNextSibling(pW->hwndTV, hTitem);
			}
				
			hTitemRoot=TreeView_GetNextSibling(pW->hwndTV, hTitemRoot);
		}

	}
	else if( pW->iFindType == FIND_TYPE_HWND )
	{
		pW->hwndTV_Find=pW->hwndTV_WT;

		wsprintf(pW->szFindText, HEX_FMT_PREFIX, pW->dwFind);
		
		tvi.mask=TVIF_TEXT|TVIF_CHILDREN|TVIF_HANDLE;
		tvi.pszText=pW->szBuf;
		tvi.cchTextMax=MAX_NUMSTR_LEN;
		pW->hwndTV_Find=pW->hwndTV_WT;
		hTitem=TreeView_FindString(pW->hwndTV_Find, 
			TreeView_GetRoot(pW->hwndTV_Find), pW->szFindText, TRUE, &tvi);
	}
	else
	{
		tvi.mask=TVIF_TEXT|TVIF_CHILDREN|TVIF_HANDLE;
		tvi.pszText=pW->szBuf;
		tvi.cchTextMax=MAX_CLASS_NAME;
		if( pW->hwndTV_Find==NULL )
		{
			pW->hwndTV_Find=pW->hwndTV_WT;
		}
		hTitem=ProcessWndTree_FindString(pW, &tvi);
	}
	
	return hTitem;

}

HTREEITEM TreeView_FindString(HWND hwndTV, HTREEITEM hTitem, LPTSTR szStrFind, BOOL IsMCase, TVITEM* pTvi)
{	
	HTREEITEM hTItemBuf;

	while(hTitem)
	{
		pTvi->hItem=hTitem;

		if( TreeView_GetItem(hwndTV, pTvi) )
		{
			if( IsMCase )
			{
				if( StrStr(pTvi->pszText, szStrFind) )
				{
					return hTitem;
				}
			}
			else
			{
				if( StrStrI(pTvi->pszText, szStrFind) )
				{
					return hTitem;
				}
			}
		}

		if( pTvi->cChildren )
		{
			hTItemBuf=TreeView_FindString(hwndTV, 
				TreeView_GetChild(hwndTV, hTitem), szStrFind, IsMCase, pTvi);

			if( hTItemBuf )
			{
				return hTItemBuf;
			}
		}

		hTitem=TreeView_GetNextSibling(hwndTV, hTitem);
	}

	return NULL;
}


HTREEITEM ProcessWndTree_InsertWndItem(PARAMSWRAPPER_WNDENUMR * pW, HWND hwndTag)
{
	ENUM_PARAM_PARENT_BUFFER epbOrg;
	DWORD dwPID, dwTID;
	INT iImageType=I_WND;
	INT iBuf;
	
	if( pW->bStopSignal )
	{
		return NULL;
	}
	
	// save parent info before any sub-enumproc...
	CopyMemory(&epbOrg, &pW->epbParent, sizeof(epbOrg));
	
	TVITEM tvi={0};
	TVINSERTSTRUCT tvis;
	
	tvis.item.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM;
	tvis.item.cchTextMax=MAX_STRBUF_LEN;
	tvis.item.lParam=0;
	
	wsprintf(pW->szText, HEX_FMT_PREFIX, hwndTag);
	
	
	HTREEITEM hTitemSpecChild=NULL;
	HTREEITEM hTitem=NULL;
	
	dwTID=GetWindowThreadProcessId(hwndTag, &dwPID);	
	
	if( pW->epbParent.hwnd != HWND_SYS_ROOT )
	{
		if( pW->epbParent.hwnd!=pW->hwndDesktop )
		{
			if( pW->epbParent.hTitem && 
				pW->epbParent.dwTID==dwTID && 
				pW->epbParent.dwPID==dwPID )
			{
				hTitem=pW->epbParent.hTitem;
			}
			else
			{
				iImageType=I_WND_S_C;
			}
		}
		
		if( hTitem!=pW->epbParent.hTitem )
		{
			wsprintf(pW->szBuf, 
				TEXT("<0x%08X> ~ %s:0x%08X - %s:0x%08X"), 
				hwndTag, pW->szProcess, dwPID, pW->szThread, dwTID);
			
			//wsprintf(pW->szBuf, TEXT("<%s>"), pW->szText);
			
			tvis.hParent=pW->epbParent.hTitem;
			tvis.item.pszText=pW->szBuf;
			tvis.item.iImage=tvis.item.iSelectedImage=
				pW->iDefImage[I_WND_S_HINT];
			hTitemSpecChild=TreeView_InsertItem(pW->hwndTV, &tvis);
		}
	}
	
	if( hTitem==NULL )
	{
		wsprintf(pW->szTinyBuf, HEX_FMT_NOPREFIX, dwPID);
		
		iBuf=SendMessage(pW->hwndDataList, CB_FINDSTRING, -1, (LPARAM)pW->szTinyBuf);
		if( iBuf!=CB_ERR )
		{
			hTitem=(HTREEITEM)
				SendMessage(pW->hwndDataList, CB_GETITEMDATA, iBuf, 0);
		}
		else
		{
			hTitem=ProcessWndTree_InsertSpecProcessItem(pW, dwPID);
		}
		
		if( !hTitem )
		{
			return NULL;
		}
		
		hTitem=TreeView_GetChild(pW->hwndTV, hTitem);
		
		while(hTitem)
		{
			tvi.mask=TVIF_PARAM|TVIF_HANDLE;
			tvi.hItem=hTitem;
			if( TreeView_GetItem(pW->hwndTV, &tvi) )
			{
				if( tvi.lParam==(LPARAM)dwTID )
				{
					break;
				}
			}
			
			hTitem=TreeView_GetNextSibling(pW->hwndTV, hTitem);
		}
		
		if( !hTitem )
		{
			THREADENTRY32 te32;
			te32.th32OwnerProcessID=dwPID;
			te32.th32ThreadID=dwTID;
			hTitem=ProcessWndTree_InsertThreadItem(pW, &te32);
		}
	}
	
	if( hTitem )
	{
		iBuf=GetClassName(hwndTag, pW->szBuf+1, MAX_CLASS_NAME-1);
		if(iBuf)
		{
			pW->szBuf[0]='[';
			pW->szBuf[iBuf+1]=']';
			pW->szBuf[iBuf+2]=0;
			lstrcat(pW->szText, TEXT(" - "));
			lstrcat(pW->szText, pW->szBuf);
		}
		
		pW->szBuf[0]='\"', pW->szBuf[BIG_STRBUF_LEN]=0;

		SendMessageTimeout(hwndTag, WM_GETTEXT, 
			BIG_STRBUF_LEN, (LPARAM)(pW->szBuf+1), SMTO_ABORTIFHUNG, 200, (DWORD*)&iBuf);
#ifdef UNICODE
		(pW->szBuf+1)[iBuf]=0;
#endif		
		if( iBuf > MINI_STRBUF_LEN )
		{
			pW->szBuf[MINI_STRBUF_LEN]=0;
			SendMessageTimeout(hwndTag, WM_GETTEXT, 
				MINI_STRBUF_LEN, (LPARAM)(pW->szBuf+1), SMTO_ABORTIFHUNG, 200, (DWORD*)&iBuf);
#ifdef UNICODE
			(pW->szBuf+1)[iBuf]=0;
#endif	
			lstrcatchar(pW->szBuf, '.', 6);
		}
		lstrcat(pW->szBuf, TEXT("\""));
		lstrcat(pW->szText, TEXT(" "));
		lstrcat(pW->szText, pW->szBuf);
		
		tvis.hParent=hTitem;
		tvis.item.pszText=pW->szText;
		iImageType+=IsWindowVisible(hwndTag)?0:1;
		tvis.item.iImage=tvis.item.iSelectedImage=pW->iDefImage[iImageType];
		
		pW->epbParent.hTitem=hTitem=TreeView_InsertItem(pW->hwndTV, &tvis);
		pW->epbParent.hwnd=hwndTag;
		pW->epbParent.dwTID=dwTID, pW->epbParent.dwPID=dwPID;
		
		if( hTitemSpecChild )
		{
			tvi.mask=TVIF_PARAM|TVIF_HANDLE;
			tvi.hItem=hTitemSpecChild;
			tvi.lParam=(LPARAM)hTitem;
			TreeView_SetItem(pW->hwndTV, &tvi);
		}
		
		tvis.hParent=pW->epbParent.hTitem_WT;
		tvis.item.lParam=(LPARAM)hTitem;
		tvis.item.iImage=tvis.item.iSelectedImage=
			pW->iDefImage[IsWindowVisible(hwndTag)?I_WND:I_WND_H];
		
		pW->epbParent.hTitem_WT=TreeView_InsertItem(pW->hwndTV_WT, &tvis);
		
		tvi.mask=TVIF_PARAM|TVIF_HANDLE;
		tvi.hItem=hTitem;
		tvi.lParam=(LPARAM)pW->epbParent.hTitem_WT;
		TreeView_SetItem(pW->hwndTV, &tvi);
		
		EnumChildWindows(hwndTag, EnumChildWndProc_PWTree, (LPARAM)pW);
		
	}
	
	// restore parent info...
	CopyMemory(&pW->epbParent, &epbOrg, sizeof(epbOrg));
	return hTitem;
}

BOOL CALLBACK EnumChildWndProc_PWTree(HWND hwnd, LPARAM lParam)
{
	PARAMSWRAPPER_WNDENUMR* pW=(PARAMSWRAPPER_WNDENUMR*)lParam;
	
	if( pW->bStopSignal )
	{
		return FALSE;
	}
	
	if( pW->epbParent.hwnd==GetAncestor(hwnd, GA_PARENT) )
	{
		ProcessWndTree_InsertWndItem(pW, hwnd);
	}
	
	return TRUE;
}


VOID ProcessWndTree_SetThreadItems(PARAMSWRAPPER_WNDENUMR * pW)
{

	if( pW->bStopSignal )
	{
		return;
	}

	HANDLE hThreadSnap;

	pW->hThreadSnap=hThreadSnap=
		CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	
	if( hThreadSnap == INVALID_HANDLE_VALUE )
	{
		return;
	}
	
	THREADENTRY32 te32 = {0};
	te32.dwSize = sizeof(THREADENTRY32);
	
	if( Thread32First(hThreadSnap, &te32) )
	{
		do
		{
			ProcessWndTree_InsertThreadItem(pW, &te32);
		}
		while(!pW->bStopSignal && Thread32Next(hThreadSnap, &te32));
		
    }
	
	CloseHandle(hThreadSnap);
	
}

VOID Thread_UpdateWndTreePair(PVOID pvoid)
{
	PARAMSWRAPPER_WNDENUMR * pW = (PARAMSWRAPPER_WNDENUMR*)pvoid;
	HANDLE hProcessSnap;
	
	pW->bBusySignal=TRUE;
	SetStatusText(pW, TRUE);

	pW->hProcessSnap=hProcessSnap= 
		CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	
	if( hProcessSnap != INVALID_HANDLE_VALUE && 
		!pW->bStopSignal )
	{
		EnableWindow(pW->hwndSlider, FALSE);

		SendMessage(pW->hwndTV, WM_SETREDRAW, FALSE, 0);
		SendMessage(pW->hwndTV_WT, WM_SETREDRAW, FALSE, 0);
		
		TreeView_DeleteAllItems(pW->hwndTV);
		TreeView_DeleteAllItems(pW->hwndTV_WT);
		
		InitImageList(pW);
		SendMessage(pW->hwndDataList, CB_RESETCONTENT, 0, 0);
		
		PROCESSENTRY32 pe32 = {0};
		pe32.dwSize = sizeof(PROCESSENTRY32);
		
		if( Process32First(hProcessSnap, &pe32) )
		{
			do
			{
				ProcessWndTree_InsertProcessItem(pW, &pe32);
			}
			while(!pW->bStopSignal && Process32Next(hProcessSnap, &pe32));
			
		}
		
		CloseHandle(hProcessSnap);
		
		if( !pW->bStopSignal )
		{
			ProcessWndTree_SetThreadItems(pW);
			
			pW->hwndDesktop=GetDesktopWindow();
			
			ZeroMemory(&pW->epbParent, sizeof(ENUM_PARAM_PARENT_BUFFER));
			
			pW->epbParent.hTitem_WT=TVI_ROOT;
			
			ProcessWndTree_InsertWndItem(pW, pW->hwndDesktop);
			ProcessWndTree_InsertWndItem(pW, GetAncestor(g_hwndBK, GA_PARENT));
			
			TreeView_Expand(pW->hwndTV_WT, TreeView_GetRoot(pW->hwndTV_WT), TVE_EXPAND);
			
			SendMessage(pW->hwndTV, WM_SETREDRAW, TRUE,0);
			SendMessage(pW->hwndTV_WT, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(pW->hwndTV, NULL, FALSE);
			InvalidateRect(pW->hwndTV_WT, NULL, FALSE);
			
			EnableWindow(pW->hwndSlider, TRUE);

			pW->hProcessSnap=NULL;
			pW->hThreadSnap=NULL;
			SetStatusText(pW, FALSE);
			Sleep(REFRESH_INTERVAL);
		}
	}
	
	pW->bBusySignal=FALSE;

	_endthread();

}

//////////////////////////////////////////////////////////////////////////


VOID Thread_SaveWndTreePair(PVOID pvoid)
{
	PARAMSWRAPPER_WNDENUMR * pW = (PARAMSWRAPPER_WNDENUMR *)pvoid;
	
	pW->bBusySignal=TRUE;
	SetStatusText(pW, TRUE);
	
	LPTSTR lpszStrBuf=NULL;
	INT iStrLen=MAX_STRBUF_LEN;

	iStrLen+=GetAllTextsFromTreeView(pW->hwndTV, NULL, NULL);
	iStrLen+=GetAllTextsFromTreeView(pW->hwndTV_WT, NULL, NULL);
	
	pW->lpszGlBuf=lpszStrBuf=(LPTSTR)
		VirtualAlloc(NULL, sizeof(TCHAR)*(iStrLen), MEM_COMMIT, PAGE_READWRITE);;
	
	for (int i=0; i<2; i++)
	{
		TCHAR szText[SMALL_STRBUF_LEN];
		lstrcatchar(lpszStrBuf, '=', 20);
		ChangeLine(lpszStrBuf);
		LoadString(g_hInstance, i==0 ? IDS_WTREE_LEFT:IDS_WTREE_RIGHT, szText, MAX_STRBUF_LEN); 
		lstrcat(lpszStrBuf, szText);
		ChangeLine(lpszStrBuf);
		lstrcatchar(lpszStrBuf, '=', 20);
		ChangeLine(lpszStrBuf);
		ChangeLine(lpszStrBuf);
		GetAllTextsFromTreeView(i==0?pW->hwndTV:pW->hwndTV_WT, lpszStrBuf, NULL);
		ChangeLine(lpszStrBuf);
		ChangeLine(lpszStrBuf);
	}

	if( !pW->bStopSignal )
	{
		WriteStrToFile(pW->szBuf, lpszStrBuf, FALSE);
		OnOpenTextFile(pW->szBuf);
	}

	VirtualFree((PVOID)lpszStrBuf, 0, MEM_RELEASE);

	pW->lpszGlBuf=NULL;

	SetStatusText(pW, FALSE);
	pW->bBusySignal=FALSE;
	_endthread();
}

//////////////////////////////////////////////////////////////////////////

VOID OnSaveWndTreePair(HWND hwndOwner, PARAMSWRAPPER_WNDENUMR* pW)
{
	TCHAR szBuf[SMALL_STRBUF_LEN];
	TCHAR szUserPath[MAX_PATH];
	
	MakeFileTypeFromStrRes(g_hInstance, IDS_FILETYPE_TXT, 
		szBuf, NULL, TEXT("*.txt"));
	
	if(	GetSaveFileNameDialog(NULL, hwndOwner, pW->szBuf, 
		NULL, ReadUserPathFromINI(szUserPath, STR_INI_KEY_SAVETEXT), 
		szBuf, TEXT ("txt"), 
		NULL, OFN_EX_NOPLACESBAR, g_siWinVer>WINVER_WIN9X)
		)
	{
		_beginthread(Thread_SaveWndTreePair, 0, pW);
		WriteUserPathToINI(pW->szBuf, FALSE, STR_INI_KEY_SAVETEXT);
	}
}

//////////////////////////////////////////////////////////////////////////


BOOL CALLBACK DlgProc_Find(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PARAMSWRAPPER_WNDENUMR *pW;

	switch (message)
	{
		
	case WM_INITDIALOG:
		{
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)lParam);
			
			pW=(PARAMSWRAPPER_WNDENUMR*)lParam;

			if(pW)
			{
				if( !pW->IsFindReady )
				{
					pW->IsFindReady=TRUE;
					GetWindowText(hwnd, pW->szFindPopTitle, LABEL_STR_LEN);
				}
				
				SendMessage(pW->hwndSB, SB_SETTEXT, 0, (LPARAM)pW->szFindPopTitle);

				if( pW->iFindType!=FIND_TYPE_TEXT )
				{
					wsprintf(pW->szFindText, g_szFormat, pW->dwFind);
				}

				
				SetWindowTextFromStrRes(hwnd, IDC_SBTN_HEXDEC, 
								g_hInstance, pW->IsNumDec?IDS_DEC:IDS_HEX);
				SetDlgItemSubClass(hwnd, IDC_SBTN_HEXDEC, SCP_Static_HandCusror);
						
				if( pW->iFindType==FIND_TYPE_TEXT )
				{
					SetDlgItemText(hwnd, IDC_EDIT_FIND, pW->szFindText);
				}
				
				CheckRadioButton(hwnd, 
					IDC_RB_FIND_TEXT, IDC_RB_FIND_TID, IDC_RB_FIND_TEXT+pW->iFindType);
				
				SendMessage(hwnd, WM_COMMAND, IDC_RB_FIND_TEXT, 0);				
				
				CheckDlgButton(hwnd, IDC_CHK_CASE, (UINT)pW->IsMatchCase);
				SendMessage(hwnd, WM_COMMAND, 0, 0); //see switch-case default bellow...
				SendDlgItemMessage(hwnd, IDC_EDIT_FIND, EM_SETLIMITTEXT, MAX_CLASS_NAME-16, 0);
			
				HWND hwndTT=CreateMyTooltip(hwnd);
				SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, (LPARAM)256);
				AddControlTip(hwndTT, hwnd, IDC_EDIT_FIND, LPSTR_TEXTCALLBACK);
				AddControlTip(hwndTT, hwnd, IDC_SBTN_HEXDEC, MAKEINTRESOURCE(IDS_TIP_HEXDEC));

				for(int i=IDC_RB_FIND_TEXT; i<=IDC_RB_FIND_TID; i++)
				{
					SetDlgItemSubClass(hwnd, i, SCP_ChkBtn_NoFoucsRect);
				}

			}
			
			return FALSE;
		}

	case WM_NOTIFY:
		{
			return OnToolTipNotify(wParam, lParam);
		}

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDOK:
				{
					pW=(PARAMSWRAPPER_WNDENUMR*)
						GetWindowLong(hwnd, GWL_USERDATA);
					if(pW)
					{
						GetDlgItemText(hwnd, IDC_EDIT_FIND, pW->szFindText, MAX_CLASS_NAME);
						pW->IsMatchCase=(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_CASE);
						for(int i=IDC_RB_FIND_TEXT; i<=IDC_RB_FIND_TID; i++)
						{
							if( IsDlgButtonChecked(hwnd, i) )
							{
								pW->iFindType=i-IDC_RB_FIND_TEXT;
								break;
							}
						}
						if( pW->iFindType!=FIND_TYPE_TEXT )
						{
							pW->dwFind=(DWORD)GetWindowInteger(
								GetDlgItem(hwnd, IDC_EDIT_FIND), !pW->IsNumDec);
						}
					}
				}
				//fall through...
			case IDCANCEL:
				{
					EndDialog(hwnd, LOWORD(wParam));
					break;
				}
			case IDC_SBTN_HEXDEC:
				{
					if( STN_CLICKED == HIWORD(wParam) ||
						STN_DBLCLK ==HIWORD(wParam) )
					{
						if( pW=(PARAMSWRAPPER_WNDENUMR*)
							GetWindowLong(hwnd, GWL_USERDATA) )
						{
							pW->IsNumDec=!pW->IsNumDec; //reverse first...
							
							SetWindowTextFromStrRes(hwnd, IDC_SBTN_HEXDEC, 
								g_hInstance, pW->IsNumDec?IDS_DEC:IDS_HEX);

							GetDlgItemText(hwnd, IDC_EDIT_FIND, pW->szBuf, SMALL_STRBUF_LEN);
							if( IsIntStr(pW->szBuf, pW->IsNumDec) )
							{
								//NOTE:
								//some where needs the previous NumFormat to read and set..
								DlgItemPrintf(hwnd, IDC_EDIT_FIND, 
									(pW->IsNumDec?TEXT("%d"):HEX_FMT_PREFIX), 
									pW->IsNumDec?HexStrToInt(pW->szBuf):DecStrToInt(pW->szBuf));
							}
							else
							{
								SendMessage(hwnd, WM_COMMAND, IDC_RB_FIND_TEXT, 0);
							}
						}
					}
					break;
				}
			default:
				{
					pW=(PARAMSWRAPPER_WNDENUMR*)
						GetWindowLong(hwnd, GWL_USERDATA);
					if(pW)
					{
						UINT uBuf=IsDlgButtonChecked(hwnd, IDC_RB_FIND_TEXT);
						if( LOWORD(wParam)!=IDC_EDIT_FIND )
						{
							SetWindowVisible(hwnd, IDC_CHK_CASE, (BOOL)uBuf);
							SetWindowVisible(hwnd, IDC_SBTN_HEXDEC, !(BOOL)uBuf);
						}
						SetWindowEnable(hwnd, IDOK, 
							(BOOL)uBuf || IsWndTextIntStr(GetDlgItem(hwnd, IDC_EDIT_FIND), !pW->IsNumDec));
						SetWindowEnable(hwnd, IDC_CHK_CASE, (BOOL)uBuf);
					}
				}
			}
			break;
		}
		
	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);;
	
} //DlgProc_SndMsgConfirmBox()

//////////////////////////////////////////////////////////////////////////
//#define SB_PART_1	

#define SB_PART_1	108
#define SB_PART_2	64

BOOL CALLBACK DlgProc_WndEnumer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static PARAMSWRAPPER_WNDENUMR * pW;
	INT iSbParts[3];

	switch (message)
	{
	case WM_INITDIALOG:
		{
			g_hwnd_WndEnumEr=hwnd;
			
			OnInitComCtrl(ICC_TREEVIEW_CLASSES);
			
			pW=(PARAMSWRAPPER_WNDENUMR*)
				LocalAlloc(GPTR, sizeof(PARAMSWRAPPER_WNDENUMR));
			if( pW==NULL )
			{
				//ShowLastError(g_hwnd);
				DestroyWindow(hwnd);
			}
			
			LoadString(g_hInstance, IDS_LAB_PROCESS, pW->szProcess, LABEL_STR_LEN);
			LoadString(g_hInstance, IDS_LAB_THREAD, pW->szThread, LABEL_STR_LEN);
			LoadString(g_hInstance, IDS_TIP_F5, pW->szTipF5, TINY_STRBUF_LEN);
			LoadString(g_hInstance, IDS_TIP_READY, pW->szTipReady, LABEL_STR_LEN);
			LoadString(g_hInstance, IDS_TIP_WAIT, pW->szTipWait, LABEL_STR_LEN);
			LoadString(g_hInstance, IDS_TIP_WNDCOUNT, pW->szTipCount, LABEL_STR_LEN);
			LoadString(g_hInstance, IDS_TIP_DOING, pW->szTipDoing, LABEL_STR_LEN);
			
			pW->hwndTV=GetDlgItem(hwnd, IDC_TV_P_WNDTREE);
			pW->hwndTV_WT=GetDlgItem(hwnd, IDC_TV_WNDTREE);	
			InitWndTreeView(pW);
			
			pW->hwndSB=CreateStatusWindow(WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP, pW->szTipF5, hwnd, IDC_STATUSBAR);
			
			pW->hwndSlider=CreateWMSildeBar(hwnd);
			
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDM_REFRESH, 0), 0);
			
			DoSetMainIconAndCenterWnd(hwnd);
			return FALSE;
		}
		
	case WM_SIZE:
		{
			HDWP hdwp;
			RECT rc;
			RECT rcCtrl;
			POINT ptWorkSpace;
			INT iBuf;

			GetWindowRect(pW->hwndSB, &rcCtrl);
			iBuf=(rcCtrl.bottom-rcCtrl.top);
			
			ptWorkSpace.x=LOWORD(lParam);
			ptWorkSpace.y=HIWORD(lParam)-iBuf;
			
			iSbParts[2]=-1, iSbParts[1]=ptWorkSpace.x-SB_PART_2,
				iSbParts[0]=iSbParts[1]-SB_PART_1;

			SendMessage(pW->hwndSB, SB_SETPARTS, 3, (LPARAM)&iSbParts);

			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, (POINT*)&rc);
			ClientToScreen(hwnd, (POINT*)&rc+1);
			
			hdwp = BeginDeferWindowPos(4);
			
			hdwp=DeferWindowPos(hdwp, pW->hwndSB, HWND_TOP, 
				0, ptWorkSpace.y, ptWorkSpace.x, iBuf, NULL); //1
			
			hdwp=DeferWindowPos(hdwp, pW->hwndSlider, HWND_BOTTOM, 
				NULL, NULL, SLIDER_CX, ptWorkSpace.y, SWP_NOMOVE); //2
			
			GetWindowRect(pW->hwndSlider, &rcCtrl);
			ScreenToClient(hwnd, (POINT*)&rcCtrl);
			iBuf=rcCtrl.left;
			hdwp=DeferWindowPos(hdwp, pW->hwndTV, HWND_TOP, 
				0, 0, min(iBuf, ptWorkSpace.x), ptWorkSpace.y, NULL); //3
			
			iBuf+=SLIDER_CX;
			hdwp=DeferWindowPos(hdwp, pW->hwndTV_WT, HWND_TOP, 
				iBuf, 0, ptWorkSpace.x-iBuf, ptWorkSpace.y, NULL); //4
			
			EndDeferWindowPos(hdwp);
			
			return FALSE;
		}
		
	case WM_GETMINMAXINFO:
		{
			((LPMINMAXINFO)lParam)->ptMinTrackSize.x = WND_MIN_X;
			((LPMINMAXINFO)lParam)->ptMinTrackSize.y = WND_MIN_Y;
			return FALSE;
		}
		
	case WM_CONTEXTMENU:
		{
			//here, take hwndSizeBar.IsEnabled as a signal.
			//see below (WM_NOTIFY>>NM_RCLICK>>TrackPopupMenu>>end)...
			return !IsWindowEnabled(pW->hwndSlider);
		}
		
	case WM_NOTIFY:
		{
			if( wParam!=IDC_TV_WNDTREE &&
				wParam!=IDC_TV_P_WNDTREE )
			{
				break;
			}

			TVITEM tvi;
			
			switch( ((LPNMHDR)lParam)->code )
			{
			case NM_DBLCLK:
				{
					pW->hwndTV_Find=((LPNMHDR)lParam)->hwndFrom;

					tvi.mask=TVIF_TEXT|TVIF_PARAM|TVIF_HANDLE;
					tvi.hItem=TreeView_GetSelection(((LPNMHDR)lParam)->hwndFrom);
					tvi.pszText=pW->szBuf;
					tvi.cchTextMax=LABEL_STR_LEN;
					
					if( TreeView_GetItem(((LPNMHDR)lParam)->hwndFrom, &tvi) )
					{
						if( pW->szBuf[0]=='0' && pW->szBuf[1]=='x' )
						{
							TreeView_SelectItem(
								((LPNMHDR)lParam)->hwndFrom==pW->hwndTV?
								pW->hwndTV_WT:pW->hwndTV, (HTREEITEM)tvi.lParam);
						}
						else if( wParam==IDC_TV_P_WNDTREE && pW->szBuf[0]=='<' )
						{
							TreeView_SelectItem(pW->hwndTV, (HTREEITEM)tvi.lParam);
							SendCtrlNotifyMessage(hwnd, pW->hwndTV, NULL, NM_DBLCLK, NULL);
						}
					}

					break;
				}
			case NM_SETFOCUS:
				{
					if(pW)
					{
						pW->hwndTV_Find=((LPNMHDR)lParam)->hwndFrom;
					}
					break;
				}
			case NM_RCLICK:
				{
					OnRightClick(pW, ((LPNMHDR)lParam)->hwndFrom);	
					break;
				}
			}
			
			break;
		}
		
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDM_REFRESH:
				{
					if( !pW->bBusySignal )
					{
						_beginthread(Thread_UpdateWndTreePair, 0, (PVOID)pW);
					}
					break;
				}
			case IDM_SAVE_WNDSTRS:
				{
					if( !pW->bBusySignal )
					{
						OnSaveWndTreePair(hwnd, pW);
					}
					break;
				}
			case IDM_FIND:
				{
					if( pW->bBusySignal ||
						IDOK!=DialogBoxParam(g_hInstance, 
						MAKEINTRESOURCE(IDD_TOOL_WNDENUMER_FIND), 
						hwnd, DlgProc_Find, (LPARAM)pW) )
					{
						SendMessage(pW->hwndSB, SB_SETTEXT, 0, (LPARAM)pW->szTipF5);
						break;
					}

				}//fall through...
			case IDM_FINDNEXT:
				{
					if( !pW->bBusySignal && pW->IsFindReady )
					{
						pW->bBusySignal=TRUE;
						SendMessage(pW->hwndSB, SB_SETTEXT, 0, (LPARAM)pW->szFindPopTitle);
						if( pW->hTitem_Find=ProcessWndTree_FindWndThread(pW) )
						{
							SetFocus(pW->hwndTV_Find);
							TreeView_SelectItem(pW->hwndTV_Find, pW->hTitem_Find);
							if( pW->iFindType!=FIND_TYPE_TEXT )
							{
								SendCtrlNotifyMessage(hwnd, pW->hwndTV_Find, NULL, NM_DBLCLK, NULL);
							}
						}
						else
						{
							MessageBox_MsgFromResStr(g_hInstance, hwnd, 
								IDS_FIND_NOMATCH, pW->szFindPopTitle, MB_ICONINFORMATION|MB_OK);
						}
						SendMessage(pW->hwndSB, SB_SETTEXT, 0, (LPARAM)pW->szTipF5);
						pW->bBusySignal=FALSE;
					}

					break;
				}
			case IDM_CLOSE:
				{
					SendMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				}
			}
			break;
		}
		
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
				DoWaitForAWhile(&pW->bBusySignal, MAX_WAITLOOP); //wait for 15 sec.
				if(pW->lpszGlBuf)
				{
					VirtualFree((PVOID)pW->lpszGlBuf, 0, MEM_RELEASE);
				}
				DoCloseHandle(pW->hProcessSnap);
				DoCloseHandle(pW->hThreadSnap);
				ImageList_Destroy(pW->himl);
				LocalFree((HLOCAL)pW);
			}
			g_hwnd_WndEnumEr=NULL;
			PostMessage(g_hwndTC, WM_MY_TOOL_CLOSE, 0, 0);
			return FALSE;
		}
		
	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
	
} //DlgProc_WndEnumer()

//////////////////////////////////////////////////////////////////////////

#define TYPE_OTHER		0
#define TYPE_WND		1	
#define TYPE_SPEC_WND	2	

INT GetItemType(LPTSTR szItemText)
{
	INT iItemType;
	
	if( szItemText[0]=='0' && szItemText[1]=='x' )
	{
		iItemType=TYPE_WND;
	}
	else if( szItemText[0]=='<' )
	{
		iItemType=TYPE_SPEC_WND;
	}
	else
	{
		iItemType=TYPE_OTHER;
	}
	
	return iItemType;
}

#define IDM_HIGHLIGHT	IDM_EDIT_HIGHLIGHT

VOID OnRightClick(PARAMSWRAPPER_WNDENUMR* pW, HWND hwndTV)
{

	TVHITTESTINFO tvhi;
	POINT pt;

	GetCursorPos(&pt);
	tvhi.pt=pt;
	ScreenToClient(hwndTV, &tvhi.pt);
	
	if( TreeView_HitTest(hwndTV, &tvhi) )
	{
		TreeView_SelectItem(hwndTV, tvhi.hItem);
		
		if( pW->bStopSignal || pW->bBusySignal)
		{
			return;
		}
	
		pW->bBusySignal=TRUE;

		TVITEM tvi;
		tvi.mask=TVIF_TEXT|TVIF_HANDLE|TVIF_STATE|TVIF_CHILDREN;
		tvi.hItem=tvhi.hItem;
		tvi.pszText=pW->szText;
		tvi.cchTextMax=MAX_STRBUF_LEN;
		TreeView_GetItem(hwndTV, &tvi);

		INT iItemType=GetItemType(pW->szText);

		HWND hwndTag=(HWND)HexStrToInt(pW->szText);
			
			HMENU hMenuRes, hMenuBuf, hMenu;
			
			hMenuRes=LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_EDIT));
			
			hMenu=GetSubMenu(hMenuRes, ID_SUBMENU_LV_PSW);		
			EnableMenuItem(hMenu, IDM_COPY, MF_ENABLED);
			DeleteMenu(hMenu, IDM_ADD_PSW, MF_BYCOMMAND);
			if( iItemType!=TYPE_WND )
			{
				DeleteMenu(hMenu, IDM_SPYTARGET, MF_BYCOMMAND);
				DeleteMenu(hMenu, 1, MF_BYPOSITION); //delete the separator.
			}
			else
			{
				hMenuBuf=GetSubMenu(hMenuRes, ID_SUBMENU_EDIT_HWNDFILE);
				if( GetMenuString(hMenuBuf, IDM_EDIT_HIGHLIGHT, pW->szBuf, TINY_STRBUF_LEN, MF_BYCOMMAND) )
				{
					InsertMenu(hMenu, 2, MF_BYPOSITION, IDM_HIGHLIGHT, pW->szBuf);
				}
			}

			//InsertMenu(hMenu, 0, MF_BYPOSITION|MF_SEPARATOR, 0, NULL);

			hMenuBuf=GetSubMenu(hMenuRes, ID_SUBMENU_TV_WTREE);			
			
			if( iItemType!=TYPE_SPEC_WND )
			{
				//See Menu Resource: index 0 expand, index 1 collapse.
				if( GetMenuString(hMenuBuf, 
					(tvi.state&TVIS_EXPANDED)?1:0, pW->szBuf, TINY_STRBUF_LEN, MF_BYPOSITION) )
				{
					InsertMenu(hMenu, 0, MF_BYPOSITION, IDM_EXPAND_COLLAPSE, pW->szBuf);
				}
				
				if( tvi.cChildren==0 )
				{
					EnableMenuItem(hMenu, 0, MF_GRAYED|MF_BYPOSITION);
				}
			}
			else
			{
				if( GetMenuString(hMenuBuf, IDM_SKIPTO, pW->szBuf, TINY_STRBUF_LEN, MF_BYCOMMAND) )
				{
					InsertMenu(hMenu, 0, MF_BYPOSITION, IDM_SKIPTO, pW->szBuf);
				}
			}

			SetMenuDefaultItem(hMenu, 0, MF_BYPOSITION);

			UINT iMenuID = 
				TrackPopupMenu(hMenu, TPM_RETURNCMD|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndTV, NULL);

			DestroyMenu(hMenuRes);
			
			switch(iMenuID)
			{

			case IDM_SKIPTO:
			case IDM_EXPAND_COLLAPSE:
				{
					TreeView_Expand(hwndTV, tvi.hItem, TVE_TOGGLE);
					SendCtrlNotifyMessage(g_hwnd_WndEnumEr, hwndTV, NULL, NM_DBLCLK, NULL);
				}
			case IDM_COPY:
				{
					CopyTextToClipBoard(g_hwnd_WndEnumEr, pW->szText);
					break;
				}
			case IDM_HIGHLIGHT:
				{
					DoHighlightWindow(hwndTag, 2);
					break;
				}
			case IDM_SPYTARGET:
				{
					WndSpy_SpyTargetWnd(g_hwndBK, hwndTag);
					break;
				}
			}
			
			DoDisableCtrlForAMoment(g_hwnd_WndEnumEr, IDC_WMSLIDER, 300);

		pW->bBusySignal=FALSE;	
	}
}

//////////////////////////////////////////////////////////////////////////

VOID CalcSliderRect(HWND hwndSlider, LPRECT lpRect)
{
	RECT rc;
	GetClientRect(hwndSlider, lpRect);
	lpRect->right=lpRect->left+SLIDER_CX;
	
	GetClientRect(g_hwnd_WndEnumEr, &rc);
	WindowToScreenRect(hwndSlider, &rc);
	lpRect->top=rc.top;
	
	GetWindowRect(GetDlgItem(g_hwnd_WndEnumEr, IDC_STATUSBAR), &rc);
	lpRect->bottom=rc.top;
}


LRESULT CALLBACK WndProc_WM_Slider(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rcPre;
	RECT rc;
	
	switch (message)
	{
		
	case WM_MOVING:
		{
			
			CalcSliderRect(hwnd, &rc);
			((LPRECT)lParam)->right=((LPRECT)lParam)->left+4;
			((LPRECT)lParam)->top=rc.top;
			((LPRECT)lParam)->bottom=rc.bottom;
			
			HDC hdc=GetDC(g_hwnd_WndEnumEr);
			HGDIOBJ hSaveDcFont=GetCurrentObject(hdc, OBJ_FONT);
			DrawFocusRect(hdc, &rcPre);
			rcPre=*(LPRECT)lParam;
			rcPre.left++, rcPre.right--, rcPre.top++, rcPre.bottom--;
			ScreenToClient(g_hwnd_WndEnumEr, (POINT*)&rcPre);
			ScreenToClient(g_hwnd_WndEnumEr, (POINT*)&rcPre+1);
			DrawFocusRect(hdc, &rcPre);
			SelectObject(hdc, hSaveDcFont);
			ReleaseDC(g_hwnd_WndEnumEr, hdc);
			
			return 0;
		}
		
	case WM_LBUTTONDOWN:
		{
			PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			break;
		}
		
	case WM_EXITSIZEMOVE:
		{
			InvalidateRect(g_hwnd_WndEnumEr, &rcPre, TRUE);
			GetClientRect(g_hwnd_WndEnumEr, &rc);
			SendMessage(g_hwnd_WndEnumEr, WM_SIZE, 
				(WPARAM)SIZE_MAXSHOW, MAKELPARAM(rc.right-rc.left, rc.bottom-rc.top));
		}//fall through...
	case WM_ENTERSIZEMOVE:
		{
			ZeroMemory(&rcPre, sizeof(RECT));
			break;
		}
		
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDOWN:
		{
			CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
			break;
		}
		
	case WM_NCLBUTTONDOWN:
		{
			if( wParam==HTCAPTION )
			{
				if( g_bIsNcBtnDownDispatched==FALSE )
				{
					if( lParam!=0 )
					{
						BringWindowToTop(g_hwnd_WndEnumEr);
					}
					SetTimer(hwnd, (TIMER_EFFACT_DELAY+message), 
						USER_TIMER_MINIMUM, TIMERPROC_NCBUTTONDOWN_DISPATCH);
					return 0;
				}
				else
				{
					g_bIsNcBtnDownDispatched=FALSE;
				}
			}
			break;
		}
		
	}
	
	return DefWindowProc(hwnd, message, wParam, lParam);

}


ATOM RegisterClass_WM_SlideBar(HINSTANCE hInstance)
{
	WNDCLASSEX wcex={0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = (WNDPROC)WndProc_WM_Slider;
	wcex.cbWndExtra = DLGWINDOWEXTRA;
	wcex.hInstance = hInstance;
	wcex.style = CS_PARENTDC/*|CS_HREDRAW|CS_VREDRAW*/;
	wcex.hCursor = LoadCursor(NULL, IDC_SIZEWE);
	wcex.lpszClassName = STR_SLIDER_CLASS;
	return RegisterClassEx(&wcex);
	
}


HWND CreateWMSildeBar(HWND hwndOwner)
{
	HWND hwndSildeBar;
	RECT rc;
	GetClientRect(hwndOwner, &rc);
	RegisterClass_WM_SlideBar(g_hInstance);
	hwndSildeBar=CreateWindow(STR_SLIDER_CLASS, NULL, 
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, 
		(rc.right-rc.left)*9/20, rc.top, SLIDER_CX, rc.bottom-rc.top, 
		hwndOwner, (HMENU)IDC_WMSLIDER, g_hInstance, NULL);
	
	UpdateWindow(hwndSildeBar);
	return hwndSildeBar;

}

//////////////////////////////////////////////////////////////////////////
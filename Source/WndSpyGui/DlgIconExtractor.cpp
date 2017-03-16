#include "WndSpyGui.h"
#include "DlgIconExtractor.h"

//////////////////////////////////////////////////////////////////////////
#define ID_TIMER_LOCATEDIR		1
#define ID_TIMER_OKBTNSTATE		2
#define ID_TIMER_CHANGEDLGTEXT	3
//////////////////////////////////////////////////////////////////////////
#define IDC_CBDL_RESIDLIST		2500
//////////////////////////////////////////////////////////////////////////
typedef struct tag_IconExtractParam
{
	ICON_ENUM_PARAM *pIEP;
	INT iCountExtract;
	LPTSTR szPath;
	LPTSTR szLocalBuf;
	
}ICON_EXTRACT_PARAM; //local struct...

//////////////////////////////////////////////////////////////////////////

INT DoTestIconExtNeedTime(LPTSTR szModule, INT* pIntIconNum)
{
	INT iTick;
	HICON hIconTest;
	iTick=-(INT)GetTickCount();
	*pIntIconNum=ExtractIconEx(szModule,-1,NULL,NULL,0);
	ExtractIconEx(szModule,0,&hIconTest,NULL,1);
	DestroyIcon(hIconTest);
	iTick+=(INT)GetTickCount();
	return (*pIntIconNum)*iTick;
}

BOOL CALLBACK IconsEnumProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam)	
{

	static LVITEM lvI;
	ICON_ENUM_PARAM *pIEP=(ICON_ENUM_PARAM*)lParam;
	HICON hIcon=NULL;
	TCHAR szBuffer[MAX_PATH];
	
	ExtractIconEx(pIEP->szModule, pIEP->iCount, &hIcon, NULL, 1);
	if(hIcon)
	{
		ImageList_AddIcon(pIEP->himl, hIcon);
		DestroyIcon(hIcon);
		
		if( HIWORD(lpszName) == 0 )
		{
			wsprintf( szBuffer, TEXT("%d"), (DWORD)lpszName );
			SendMessage(pIEP->hwndStrList, CB_ADDSTRING, 0, (LPARAM)szBuffer);
			SendMessage(pIEP->hwndStrList, CB_SETITEMDATA, pIEP->iCount, (LPARAM)lpszName);
		}
		else
		{
			SendMessage(pIEP->hwndStrList, CB_ADDSTRING, 0, (LPARAM)lpszName);
			SendMessage(pIEP->hwndStrList, CB_SETITEMDATA, pIEP->iCount, (LPARAM)0);
		}
		
		wsprintf(szBuffer, TEXT("Icon %d"), pIEP->iCount);
		
		lvI.mask = LVIF_TEXT | LVIF_IMAGE;
		lvI.iItem = pIEP->iCount;
		lvI.iImage = pIEP->iCount;
		lvI.pszText = szBuffer;
		
		if( ListView_InsertItem(pIEP->hwndLV, &lvI) != -1 )
		{
			pIEP->iCount++;	
			SendMessage(pIEP->hwndProgress, PBM_STEPIT, 0, 0); 
		}
		
	}

	return pIEP->IsKillThread? FALSE:TRUE;
}


void EnumIconResToListView(PVOID pvoid)
{
	ICON_ENUM_PARAM*pIEP=(ICON_ENUM_PARAM*)pvoid;
	BOOL isMultiThread=
		(pIEP->hModule? TRUE:FALSE);
	INT iIconNum;
	INT iTick;
	
	ListView_DeleteAllItems(pIEP->hwndLV);
	SendMessage(pIEP->hwndStrList, CB_RESETCONTENT, 0, 0);
	if(pIEP->himl)
	{
		ImageList_RemoveAll(pIEP->himl);
		ImageList_Destroy(pIEP->himl);
	}
	
	pIEP->iCount = 0;
	
	iTick=DoTestIconExtNeedTime(pIEP->szModule, &iIconNum);
	
	if(iIconNum)
	{
		
		pIEP->himl = ImageList_Create(32, 32, ILC_MASK|ILC_COLOR32, 0, 0);
		ListView_SetImageList(pIEP->hwndLV, pIEP->himl, LVSIL_NORMAL);
		pIEP->hModule = LoadLibraryEx(pIEP->szModule, NULL, LOAD_LIBRARY_AS_DATAFILE);
		
		SendMessage(pIEP->hwndProgress, PBM_SETPOS, 0, 0); 
		SendMessage(pIEP->hwndProgress, PBM_SETRANGE32, 0, (LPARAM)iIconNum); 
		SendMessage(pIEP->hwndProgress, PBM_SETSTEP, 1, 0); 
		ShowWindow(pIEP->hwndProgress, iTick>500 || iIconNum>50 ? SW_SHOW:SW_HIDE);
		SetWindowEnable(pIEP->hwndLV, NULL, iTick>500 || iIconNum>50 ? FALSE:TRUE);
		if(IsWindowVisible(pIEP->hwndProgress))
		{
			SetWindowTextFromStrRes(pIEP->hwnd,IDCANCEL, g_hInstance, IDS_ABORT);
			SetWindowTextFromStrRes(pIEP->hwnd,IDC_STATIC_TIP, g_hInstance, IDS_ICONEX_LOADING);
		}
		if(iIconNum>=12)
		{
			SendMessage(pIEP->hwndLV, WM_SETREDRAW, FALSE,0);
		}
		
		EnumResourceNames(pIEP->hModule, RT_GROUP_ICON, IconsEnumProc, (LPARAM)pIEP);
		FreeLibrary(pIEP->hModule);
		
		if(pIEP->iCount>=12 ||pIEP->iCount<0)
		{
			SendMessage(pIEP->hwndLV, WM_SETREDRAW, TRUE,0);
			InvalidateRect(pIEP->hwndLV,NULL,FALSE);
		}
		ShowWindow(pIEP->hwndProgress, SW_HIDE);
	}

	SetWindowEnable(pIEP->hwndLV, NULL, pIEP->iCount ? TRUE:FALSE);
	SetWindowTextFromStrRes(pIEP->hwnd, IDCANCEL, g_hInstance, IDS_CLOSE);
	SetWindowTextFromStrRes(pIEP->hwnd, IDC_STATIC_TIP, g_hInstance, 
		pIEP->iCount ? IDS_ICONEX_SELPLS:IDS_ICONEX_NOICON);
		
	pIEP->hModule=NULL;
	pIEP->IsKillThread=TRUE;
	if(isMultiThread)
	{
		_endthread();
	}
}

void ThreadFuncEnumIcons(ICON_ENUM_PARAM* pIEP)
{
	pIEP->IsKillThread=FALSE;
	if( pIEP->hModule ) //if pviod->hModule is a non-zero value, start a new thread...
	{
		_beginthread(EnumIconResToListView,0,(PVOID)pIEP);
	}
	else //Single thread...
	{
		EnumIconResToListView((PVOID)pIEP);
	}
}

void DoExtractOneIcon(LPTSTR szSaveFileName, ICON_ENUM_PARAM *pIEP, INT iIndex)
{
	TCHAR szBuf[MAX_PATH]={0};
	LPICONRESOURCE lpIR;
	LPTSTR lpID;
	BOOL bRet;
	
	// 	if( IsFileExists(szSaveFileName) )
	// 	{
	// 		anything confirm can be here...
	// 	}
	
	lpID=(LPTSTR)SendMessage(pIEP->hwndStrList, CB_GETITEMDATA, iIndex, 0);
	
	if(lpID==0)
	{
		SendMessage(pIEP->hwndStrList, CB_GETLBTEXT, iIndex, (LPARAM)szBuf);
		lpID=szBuf;
	}
	
	lpIR=ReadIconFromPEFile(pIEP->szModule, lpID);	
	if(lpIR==NULL)
	{
		lpIR=ReadIconFromICOFile(pIEP->szModule);
	}
	if(lpIR)
	{
		bRet=WriteIconToICOFile(lpIR, szSaveFileName);
		DoFreeIconResource(lpIR);
	}
	else
	{
		bRet=FALSE;
	}
	
	if(bRet==FALSE)
	{
		HICON hIcon;
		hIcon=ImageList_GetIcon(pIEP->himl, iIndex, ILD_TRANSPARENT);
		SaveIconToFile(hIcon, szSaveFileName, 32);
		DestroyIcon(hIcon);
	}
	
	SendMessage(pIEP->hwndProgress, PBM_STEPIT, 0, 0);
	
}

void DoExtractMultiIcons(PVOID pvoid)
{
	ICON_EXTRACT_PARAM*pIExP=(ICON_EXTRACT_PARAM*)pvoid;
	INT i;
	INT iTick=0;
	TCHAR szIconName[MAX_PATH];
	TCHAR szBuf[MAX_PATH];
	
	SetWindowEnable(pIExP->pIEP->hwnd, IDOK, FALSE);
	SendMessage(pIExP->pIEP->hwndProgress, PBM_SETPOS, 0, 0); 
	SendMessage(pIExP->pIEP->hwndProgress, PBM_SETRANGE32, 0,(LPARAM)pIExP->iCountExtract); 
	SendMessage(pIExP->pIEP->hwndProgress, PBM_SETSTEP, 1, 0);
	
	for(i=0 ; i < pIExP->pIEP->iCount && !pIExP->pIEP->IsKillThread ; i++)
	{
		if( LVIS_SELECTED == 
			ListView_GetItemState(pIExP->pIEP->hwndLV, i, LVIS_SELECTED) )
		{
			GetFileBaseName(pIExP->pIEP->szModule, szBuf);
			wsprintf(szIconName, TEXT("%s%s[Icon-%d].ico"), pIExP->szPath, szBuf, i);
			
			if(iTick==0)
			{
				iTick=-(INT)GetTickCount();
				DoExtractOneIcon(szIconName, pIExP->pIEP, i);
				iTick+=(INT)GetTickCount()+1;
				iTick*=pIExP->iCountExtract;
				SetWindowEnable(pIExP->pIEP->hwndLV, NULL, iTick>200 ? FALSE:TRUE);
				ShowWindow(pIExP->pIEP->hwndProgress, 
					iTick>200 || pIExP->iCountExtract>50 ? SW_SHOW:SW_HIDE);
				if(IsWindowVisible(pIExP->pIEP->hwndProgress))
				{
					SetWindowTextFromStrRes(pIExP->pIEP->hwnd, 
						IDCANCEL, g_hInstance, IDS_ABORT);
					SetWindowTextFromStrRes(pIExP->pIEP->hwnd, 
						IDC_STATIC_TIP, g_hInstance, IDS_ICONEX_EXTING);
				}
				iTick=pIExP->iCountExtract;
			}
			else
			{
				DoExtractOneIcon(szIconName, pIExP->pIEP, i);
			}
			pIExP->iCountExtract--;
		}
		
	}//for...

	ShowWindow(pIExP->pIEP->hwndProgress, SW_HIDE);
	
	SetWindowTextFromStrRes(pIExP->pIEP->hwnd,IDC_STATIC_TIP, g_hInstance, IDS_ICONEX_SELPLS);
	SetWindowEnable(pIExP->pIEP->hwndLV, NULL, TRUE);
	SetWindowTextFromStrRes(pIExP->pIEP->hwnd,IDCANCEL, g_hInstance, IDS_CLOSE);
	SetWindowEnable(pIExP->pIEP->hwnd, IDOK, TRUE);
	if( IsAutoOpenSavedFile() )
	{
		DoLocateFile(pIExP->szPath, TRUE);
	}
	pIExP->pIEP->IsKillThread=TRUE;
	
	_endthread();//_endthread() will close the thread handle...
}

void ThreadFuncExtractIcons(ICON_EXTRACT_PARAM* pIExP)
{
	
	if(pIExP->iCountExtract > 1)
	{
		pIExP->pIEP->IsKillThread=FALSE;
		_beginthread(DoExtractMultiIcons,0,(PVOID)pIExP);
	}
	
}

//////////////////////////////////////////////////////////////////////////
// 297x302 is the proper dialog-size of iconextractor.
#define ICONEXT_DLG_MIN_SIZE_X	\
(2*GetSystemMetrics(SM_CXFRAME)+297)
#define ICONEXT_DLG_MIN_SIZE_Y	\
(GetSystemMetrics(SM_CYCAPTION)+2*GetSystemMetrics(SM_CYFRAME)+302)


BOOL CALLBACK DlgProc_IconExtractor (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{

	static ICON_EXTRACT_PARAM iexParam;
	static ICON_ENUM_PARAM ieParam;
	INT iVar, i;
	TCHAR szBuf[MAX_PATH];
	TCHAR szPathBuf[MAX_PATH];
	HWND hwndBuf;
	RECT rc;

	switch (message)
	{

	case WM_INITDIALOG :
		{
			g_dwBitFlag|=BOOLEAN_BIT_ICONEXT;
			g_hwnd_TaskModal=g_hwnd_IconExtr=hwnd;
			
			OnInitComCtrl(ICC_LISTVIEW_CLASSES);
			
			hwndBuf=CreateMyTooltip(hwnd);
			AddControlTip(hwndBuf,hwnd,IDC_EDIT_PATH, LPSTR_TEXTCALLBACK);
			AddControlTip(hwndBuf,hwnd,IDC_BTN_BROWSE, NULL);

//#ifdef _THEME_ONDRAWWINDOWSIZEGRIPBOX	
			
			OnInit_OnDrawWindowSizeGripBox(g_hInst.uxtheme_dll);
//#endif
			ieParam.szModule=(TCHAR*)LocalAlloc(LPTR, MAX_PATH*sizeof(TCHAR));
			iexParam.szLocalBuf=(TCHAR*)LocalAlloc(LPTR, MAX_PATH*sizeof(TCHAR));
			lstrcpyn(ieParam.szModule, g_spyinfo_SPI.ProcStrs.szExePath, MAX_PATH);
			ieParam.IsKillThread=TRUE;
			iexParam.pIEP=&ieParam;
			iexParam.iCountExtract=-1;
			ieParam.hwnd=hwnd;
			
			ieParam.hwndLV=GetDlgItem(hwnd, IDC_LV_ICONVIEW);
			ieParam.hwndProgress=GetDlgItem(hwnd, IDC_PROGRESS);
			
			ieParam.hwndStrList=CreateWindow(WC_COMBOBOX, NULL, 
				WS_CHILD|WS_DISABLED, 0, 0, 0, 0, 
				hwnd, (HMENU)IDC_CBDL_RESIDLIST, g_hInstance, NULL);

			ListView_SetIconSpacing(ieParam.hwndLV, 61, 60);
			
			SetWindowTextFromStrRes(hwnd, IDC_STATIC_TIP, g_hInstance, IDS_ICONEX_SELPLS);
			SetWindowTextFromStrRes(hwnd, IDCANCEL, g_hInstance, IDS_CLOSE);
			
			iVar=DoTestIconExtNeedTime(ieParam.szModule, &i);
			SendMessage(hwnd, WM_MY_SETLISTVIEWICONS, 
				(WPARAM)&ieParam, (LPARAM)(iVar > 300 || i > 100));
			
			SetFocus(ieParam.hwndProgress);
			MoveWndToWndCenter(hwnd, g_hwnd);

			return FALSE;
		}
		
	case WM_DROPFILES:
		{
			if(ieParam.IsKillThread)//not busy...
			{
				DragQueryFile((HDROP)wParam, 0, szBuf, MAX_PATH);
				GetFileExtension(szBuf, iexParam.szLocalBuf);
				if(lstrcmpi(iexParam.szLocalBuf, TEXT("lnk"))==0)
				{
					DoResolveShortcut(szBuf,ieParam.szModule, MAX_PATH);
				}
				else
				{
					lstrcpyn(ieParam.szModule, szBuf, MAX_PATH);
				}
				SendMessage(hwnd, WM_MY_SETLISTVIEWICONS, (WPARAM)&ieParam, 1);
			}
			else
			{
				MessageBeep(0);
			}
			return FALSE;
		}
		
	case WM_MY_SETLISTVIEWICONS:
		{
			if(ieParam.IsKillThread)//not busy...
			{
				SetDlgItemText(hwnd, IDC_EDIT_PATH, 
					((ICON_ENUM_PARAM*)wParam)->szModule);

				EnableWindow(GetDlgItem(hwnd,IDOK),FALSE);
				
				((ICON_ENUM_PARAM*)wParam)->hModule=(HINSTANCE)lParam; //hModule here used as multithread switch...
				
				ThreadFuncEnumIcons((ICON_ENUM_PARAM*)wParam);
			}
			return TRUE;
		}

	case WM_SHOWWINDOW:
		{
			if( IS_FLAG_SETTED(g_dwBitFlag, BOOLEAN_BIT_ICONEXT) )
			{
				g_dwBitFlag&=~BOOLEAN_BIT_ICONEXT;

				SetWindowLong(hwnd, GWL_STYLE, 
					GetWindowLong(hwnd,GWL_STYLE)|WS_THICKFRAME);
				SetWindowPos(hwnd, NULL, NULL, NULL, 
					ICONEXT_DLG_MIN_SIZE_X, ICONEXT_DLG_MIN_SIZE_X, 
					SWP_NOZORDER|SWP_NOMOVE);
				//SendMessage(hwnd, WM_SIZE, 0, 0);
			}
			return FALSE;
		}

//#ifdef _THEME_ONDRAWWINDOWSIZEGRIPBOX			

	case WM_PAINT:
		{
			PAINTSTRUCT pst;
			HDC hdc;
			hdc=BeginPaint(hwnd, &pst);
			DrawSizeGripBox_OnPaint(hwnd, hdc);
			EndPaint(hwnd, &pst);
			return FALSE;
		}

//#endif //_THEME_ONDRAWWINDOWSIZEGRIPBOX

	case WM_SIZE:
		{
			HDWP hdwp;
			RECT rcCtrl;
			POINT pt;
			
#define MARGIN_X_BTN		14
#define MARGIN_Y_BTN		12
#define	MARGIN_X_INNER		5
#define MARGIN_X_LV			14
#define MARGIN_Y_LV			44
#define MARGIN_Y_PROGRESS	13

			
			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, (POINT*)&rc);
			ClientToScreen(hwnd, (POINT*)&rc+1);

			// #ifdef _THEME_ONDRAWWINDOWSIZEGRIPBOX
			// the num should be 6, and remove the srcollsize control from res.

			hdwp = BeginDeferWindowPos(6);

			hwndBuf=GetDlgItem(hwnd, IDC_BTN_BROWSE);
			GetWindowRect(hwndBuf, &rcCtrl);
			pt.x=rc.right-(rcCtrl.right-rcCtrl.left)-MARGIN_X_BTN;
			pt.y=rcCtrl.top;
			iVar=pt.x; //save
			ScreenToClient(hwnd,&pt);
			
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, NULL, NULL,
				SWP_NOZORDER|SWP_NOSIZE); //1
			
			hwndBuf=GetDlgItem(hwnd, IDC_EDIT_PATH);
			GetWindowRect(hwndBuf, &rcCtrl);
			pt.x=iVar-rcCtrl.left-MARGIN_X_INNER;
			pt.y=rcCtrl.bottom-rcCtrl.top;
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				NULL, NULL, pt.x, pt.y, 
				SWP_NOZORDER|SWP_NOMOVE); //2
			
			GetWindowRect(ieParam.hwndLV, &rcCtrl);
			pt.x=rc.right-rcCtrl.left-MARGIN_X_LV;
			pt.y=rc.bottom-rcCtrl.top-MARGIN_Y_LV;
			hdwp=DeferWindowPos(hdwp, ieParam.hwndLV, NULL, 
				NULL, NULL, pt.x, pt.y, 
				SWP_NOZORDER|SWP_NOMOVE); //3
			
			GetWindowRect(ieParam.hwndProgress, &rcCtrl);
			pt.x=rcCtrl.left;
			pt.y=rc.bottom-(rcCtrl.bottom-rcCtrl.top)-MARGIN_Y_PROGRESS;
			ScreenToClient(hwnd,&pt);
			hdwp=DeferWindowPos(hdwp, ieParam.hwndProgress, NULL, 
				pt.x, pt.y, NULL, NULL, 
				SWP_NOZORDER|SWP_NOSIZE); //4
			
			hwndBuf=GetDlgItem(hwnd, IDCANCEL);
			GetWindowRect(hwndBuf, &rcCtrl);
			pt.x=rc.right-(rcCtrl.right-rcCtrl.left)-MARGIN_X_BTN;
			pt.y=rc.bottom-(rcCtrl.bottom-rcCtrl.top)-MARGIN_Y_BTN;
			iVar=pt.x, i=pt.y; //save for next ctrl...
			ScreenToClient(hwnd,&pt);
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, NULL, NULL, 
				SWP_NOZORDER|SWP_NOSIZE); //5
			
			hwndBuf=GetDlgItem(hwnd, IDOK);
			GetWindowRect(hwndBuf, &rcCtrl);
			pt.x=iVar-(rcCtrl.right-rcCtrl.left)-MARGIN_X_INNER;
			pt.y=i;
			ScreenToClient(hwnd,&pt);
			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
				pt.x, pt.y, NULL, NULL, 
				SWP_NOZORDER|SWP_NOSIZE); //6

// 			hwndBuf=GetDlgItem(hwnd, IDC_SIZEGRIP);
// 			pt.x= rc.right- _HS_GRIP_SIZE;
// 			pt.y= rc.bottom- _HS_GRIP_SIZE;
// 			ScreenToClient(hwnd,&pt);
// 			hdwp=DeferWindowPos(hdwp, hwndBuf, NULL, 
// 				pt.x, pt.y, NULL, NULL, 
// 				SWP_NOZORDER|SWP_NOSIZE); //7

			EndDeferWindowPos(hdwp);

		}//WM_SIZE end.. but fall though...
	case WM_SIZING:
		{
			DrawSizeGripBox_OnSizing(hwnd);

			return FALSE;
		}
		//size changed finished...

	case WM_GETMINMAXINFO:
		{
			((LPMINMAXINFO)lParam)->ptMinTrackSize.x = ICONEXT_DLG_MIN_SIZE_X;
			((LPMINMAXINFO)lParam)->ptMinTrackSize.y = ICONEXT_DLG_MIN_SIZE_Y;
		}//fall throuth...
	case WM_STYLECHANGING:
		{
			return FALSE;
		}

	case WM_TIMER:
		{
			if( wParam==ID_TIMER_LOCATEDIR )
			{
				KillTimer(hwnd, wParam);
				DoLocateFile(iexParam.szLocalBuf, FALSE);
			}
			else if( wParam==ID_TIMER_OKBTNSTATE )
			{
				KillTimer(hwnd, wParam);
				iVar=ListView_GetSelectedCount(ieParam.hwndLV);
				SetWindowEnable(hwnd,IDOK, iVar>0 ? TRUE:FALSE);
			}
			else if( wParam==ID_TIMER_CHANGEDLGTEXT )
			{
				HWND hwndTemp;
				hwndTemp=GetForegroundWindow();
				if( GetWindow(hwndTemp,GW_OWNER) == hwnd )
				{
					KillTimer(hwnd,wParam);
					SetWindowTextFromStrRes(hwndTemp, NULL, g_hInstance, IDS_SEL_DIR_TITLE);
				}
			}
			break;
		}
		
	case WM_NOTIFY:
		{
			if(!ieParam.IsKillThread)
			{
				break;
			}

			switch( ((LPNMHDR)lParam)->idFrom )
			{
			case IDC_LV_ICONVIEW:
				if(	((LPNMHDR)lParam)->code ==	LVN_ITEMCHANGED)
				{
					SetTimer(hwnd, ID_TIMER_OKBTNSTATE, 20, NULL);
				}
				break;
			}
			
			return OnToolTipNotify(wParam, lParam);
		}
		
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_BTN_BROWSE:
				{
					if(!ieParam.IsKillThread)
					{
						MessageBeep(0);
						break;
					}

					iVar=MakeFileTypeFromStrRes(g_hInstance, 
							IDS_FILETYPE_WIN32PE, iexParam.szLocalBuf, 
							TEXT("*.exe, *.dll ..."), 
							TEXT("*.exe;*.dll;*.ocx;*.scr;*.cpl;*.sfx"));

					iVar+=MakeFileTypeFromStrRes(g_hInstance, 
							IDS_FILETYPE_ICONRES, iexParam.szLocalBuf+iVar, 
							NULL, TEXT("*.res;*.icl"));

					MakeFileTypeFromStrRes(g_hInstance, 
							IDS_FILETYPE_ALL, iexParam.szLocalBuf+iVar, 
							NULL, TEXT("*.*"));

					GetFilePathName(ieParam.szModule, szPathBuf);
					ZeroMemory(szBuf, sizeof(TCHAR)*MAX_PATH);

					if(	GetOpenFileNameDialog(NULL, hwnd, szBuf, 
						NULL, szPathBuf, iexParam.szLocalBuf,
						TEXT("exe"), NULL, OFN_EX_NOPLACESBAR, g_siWinVer>WINVER_WIN9X)
						)
					{
						lstrcpyn(ieParam.szModule, szBuf, MAX_PATH);
						SendMessage(hwnd, WM_MY_SETLISTVIEWICONS, (WPARAM)&ieParam, 1);
					}
					break;
				}
			case IDC_LV_ICONVIEW:
				{
					if(HIWORD(wParam)==NM_CLICK)
					{
						iVar=ListView_GetSelectedCount(ieParam.hwndLV);
						SetWindowEnable(hwnd,IDOK, iVar>0? TRUE:FALSE);
					}
					break;
				}
				
			case IDOK:
				{
					i=ListView_GetSelectedCount(ieParam.hwndLV);
					if(i<=0)
					{
						MessageBeep(0);
						EnableWindow((HWND)lParam, FALSE);
					}
					else if( i==1 )
					{
						MakeFileTypeFromStrRes(g_hInstance, IDS_FILETYPE_ICON, 
							szBuf, NULL, TEXT("*.ico"));
						
						iVar=ListView_GetSelectionMark(ieParam.hwndLV);
						ZeroMemory(iexParam.szLocalBuf, sizeof(TCHAR)*MAX_PATH);
						
						if(	GetSaveFileNameDialog(NULL, hwnd, iexParam.szLocalBuf, 
						NULL, ReadUserPathFromINI(szPathBuf, STR_INI_KEY_SAVEICON), 
							szBuf, TEXT ("ico"), 
							NULL, OFN_EX_NOPLACESBAR, g_siWinVer>WINVER_WIN9X)
							)
						{	
							DoExtractOneIcon(iexParam.szLocalBuf, &ieParam, iVar);
							WriteUserPathToINI(iexParam.szLocalBuf, FALSE, STR_INI_KEY_SAVEICON);
							if( IsAutoOpenSavedFile() )
							{
								SetTimer(hwnd, ID_TIMER_LOCATEDIR, 50, NULL);
							}
						}

					}
					else // i>1 
					{
						SetTimer(hwnd, ID_TIMER_CHANGEDLGTEXT, 50, NULL);
						
						LoadString(g_hInstance, IDS_ICONEX_SEL_DIR ,szBuf, MAX_PATH);

						ReadUserPathFromINI_EX(iexParam.szLocalBuf, STR_INI_KEY_SAVEICON, FALSE);
						
						if(	SelectFolderDialog(hwnd, NULL, iexParam.szLocalBuf, szBuf) )
						{
							WriteUserPathToINI(iexParam.szLocalBuf, FALSE, STR_INI_KEY_SAVEICON);
							iexParam.szPath=iexParam.szLocalBuf;
							iexParam.iCountExtract=i;
							ThreadFuncExtractIcons(&iexParam);
						}

					}
					break;
				}

			case IDCANCEL:
				{
					if(ieParam.IsKillThread)
					{
						SendMessage(hwnd, WM_CLOSE, 0, 0);
					}
					else
					{
						ieParam.IsKillThread=TRUE;
						return TRUE;
					}
					break;
				}
			}//switch @ wm_command
			
			return FALSE;
		}//WM_COMMAND
		
	case WM_CLOSE:
		{
			if(ieParam.IsKillThread)
			{
				EndDialog(hwnd, 0);
			}
			else
			{
				MessageBeep(0);
				return TRUE;
			}
			return FALSE;
		}

	case WM_DESTROY:
		{
			if(ieParam.himl)
			{
				ImageList_RemoveAll(ieParam.himl);
				ImageList_Destroy(ieParam.himl);
			}
			LocalFree((HLOCAL)ieParam.szModule);
			LocalFree((HLOCAL)iexParam.szLocalBuf);
			g_hwnd_TaskModal=g_hwnd_IconExtr=NULL;
			MACRO_TRIM_MEMORY((g_siWinVer>=WINVER_WIN2K))
			return FALSE;
		}

	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////

#include "..\SpyMainDlgTabs.h"

BOOL CALLBACK TabDlgProc_General(HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
		
	case WM_INITDIALOG :
		{
			OnChildDialogInit(hwnd);

			LoadString(g_hInstance, IDS_WINID_HMENU, g_szLabelIdMenu, LABEL_WINID_LEN);
			LoadString(g_hInstance, IDS_WINID_CTRLID, g_szLabelIdCtrl, LABEL_WINID_LEN);
			SetDlgItemText(hwnd, IDC_STATIC_WINID, g_szLabelIdCtrl);
			DoSetEditItemsLayout(hwnd, 1, IDC_EDIT_HWND, IDC_EDIT_CLASS, 1);
			DoSetEditItemsLayout(hwnd, 1, IDC_EDIT_CLASS, IDC_EDIT_WNDRECT, IDC_EDIT_WNDRECT-IDC_EDIT_CLASS);			
			DoSetEditItemsLayout(hwnd, 1, IDC_EDIT_WNDRECT, IDC_EDIT_HINSTANCE, 1);

			SetDlgItemSubClass(hwnd, IDC_EDIT_HWND, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_ICONBTN_BIG, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_ICONBTN_SMALL, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_ICONBTN_MORE, SubClassProcIconExtBtn);
			
			return TRUE;
		}
	
	case WM_SHOWWINDOW:
		{
			if( g_dwBitFlag&BOOLEAN_BIT_TAB_GENERAL )
			{
				g_dwBitFlag&=~BOOLEAN_BIT_TAB_GENERAL;

				SetDlgItemText(hwnd, IDC_STATIC_WINID, 
					(g_spyinfo_SWIex.wi.dwStyle&WS_CHILD)?g_szLabelIdCtrl:g_szLabelIdMenu);

				ExtractIconEx(g_spyinfo_SWIex.szWndModuleName, 0, (HICON*)&g_hTarget, NULL, 1);
				if(g_hTarget)
				{
					lstrcpyn(g_spyinfo_SPI.ProcStrs.szExePath, g_spyinfo_SWIex.szWndModuleName, MAX_PATH);
				}
				else
				{
					ExtractIconEx(g_spyinfo_SPI.pe32.szExeFile, 0, (HICON*)&g_hTarget, NULL, 1);
					lstrcpyn(g_spyinfo_SPI.ProcStrs.szExePath, g_spyinfo_SPI.pe32.szExeFile, MAX_PATH);
				}
				SetWindowVisible(hwnd, IDC_ICONBTN_MORE, g_hTarget?TRUE:FALSE);	
				DestroyIcon((HICON)
					SendDlgItemMessage(hwnd, IDC_ICONBTN_MORE, 
					STM_SETIMAGE, IMAGE_ICON, 
					(LPARAM)CopyImage((HICON)g_hTarget, IMAGE_ICON, 32, 32, NULL)));
				DestroyIcon((HICON)g_hTarget);
			}
			
			break;
		}

	case WM_NOTIFY:
		{
			if( OnWndRectTipNotify(lParam) )
			{
				break;
			}

			return	OnToolTipNotify(wParam, lParam);
		}

	case WM_LBUTTONDOWN:
		{
			PostMessage(g_hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}
		
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_ICONBTN_BIG:
			case IDC_ICONBTN_SMALL:
				{
					if( STN_CLICKED==HIWORD(wParam) )
					{
						g_hTarget=(HANDLE)SendDlgItemMessage(hwnd, LOWORD(wParam), STM_GETIMAGE, IMAGE_ICON,0);
						DialogBox(g_hInstance,MAKEINTRESOURCE(IDD_SAVEICON),g_hwnd,DlgProc_IconColorDepthsSel);
					}
					break;
				}
			case IDC_ICONBTN_MORE:
				{
					if( STN_CLICKED==HIWORD(wParam) )
					{
						if( !IsWindow(g_hwnd_IconExtr) )
						{
							DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ICONEXTRACTOR), g_hwnd, DlgProc_IconExtractor);
						}
					}
					break;
				}
			}
			
			break;
		}//WM_COMMAND
		
	case WM_DESTROY:
		{
			DoUnSetCtrlIcon(hwnd, IDC_ICONBTN_BIG);
			DoUnSetCtrlIcon(hwnd, IDC_ICONBTN_SMALL);
			DoUnSetCtrlIcon(hwnd, IDC_ICONBTN_MORE);

			break;
		}
		
	}//switch message...
	
	return FALSE;

} //TabDlgProc_General()

//////////////////////////////////////////////////////////////////////////

BOOL OnWndRectTipNotify(LPARAM lParam)
{
	TCHAR szBuf[MAX_PATH];
	
	INT iCtlId=GetDlgCtrlID((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom);
	
	if(	((LPNMTTDISPINFO)lParam)->hdr.code==TTN_NEEDTEXT &&
		( iCtlId==IDC_EDIT_WNDRECT || iCtlId==IDC_EDIT_CLIENTRECT) )
	{
		if( GetWindowTextLength((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom) >0 )
		{
			GetWindowText((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom, g_strBuf, MAX_PATH);
			LoadString(g_hInstance, IDS_TIP_COORD, szBuf, MAX_PATH);
			lstrcat(g_strBuf, szBuf);
			((LPNMTTDISPINFO)lParam)->lpszText=g_strBuf;
		}
		return TRUE;
	}

	return FALSE;

} //OnWndRectTipNotify()

//////////////////////////////////////////////////////////////////////////

LRESULT APIENTRY SubClassProcIconExtBtn(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	
	switch (message)
	{

	case WM_RBUTTONUP:
		{
			POINT pt;
			HMENU hMenu;
			UINT iMenuID;
			HICON hIcon;
			LPTSTR szPathSel;

			pt.x=LOWORD(lParam), 
				pt.y=HIWORD(lParam);	
			ClientToScreen(hwnd, &pt);

			wParam=(WPARAM)
				LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_MENU_EDIT));
			hMenu=GetSubMenu((HMENU)wParam, 1);
			
			ExtractIconEx(g_spyinfo_SWIex.szWndModuleName, 0, &hIcon, NULL, 1);
			if(!hIcon)
			{
				EnableMenuItem(hMenu, IDM_ICONEXT_WNDOWNER, MF_GRAYED);
			}
			DestroyIcon(hIcon);
			
			iMenuID = TrackPopupMenu(hMenu, 
				TPM_RETURNCMD|TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
			
			DestroyMenu((HMENU)wParam);

			switch(iMenuID)
			{
			case IDM_ICONEXT_WNDOWNER:
				{
					szPathSel = 
						lstrcpyn(g_spyinfo_SPI.ProcStrs.szExePath, 
						g_spyinfo_SWIex.szWndModuleName, MAX_PATH);
					break;
				}
			case IDM_ICONEXT_EXE:
				{
					szPathSel = 
						lstrcpyn(g_spyinfo_SPI.ProcStrs.szExePath, 
						g_spyinfo_SPI.pe32.szExeFile, MAX_PATH);
					break;
				}
			default:
				szPathSel = NULL;
			}

			ExtractIconEx(g_spyinfo_SPI.ProcStrs.szExePath, 0, &hIcon, NULL, 1);
			DoSetCtrlIcon(hwnd, NULL, hIcon, 32);
			DestroyIcon(hIcon);

			if(szPathSel)
			{
				Sleep(100);
				PostMessage(GetParent(hwnd), WM_COMMAND, 
					MAKEWPARAM(IDC_ICONBTN_MORE, STN_CLICKED), 0);
			}
			break;
		}

	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
		{
			CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
			SetCursor(LoadCursor(NULL,IDC_HAND));
			return 0;
		}

	} //switch(message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
} //SubClassProcIconExtBtn()


//////////////////////////////////////////////////////////////////////////
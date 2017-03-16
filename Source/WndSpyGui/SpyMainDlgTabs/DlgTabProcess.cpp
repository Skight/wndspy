#include "..\SpyMainDlgTabs.h"

BOOL CALLBACK TabDlgProc_Process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch(message)
	{

	case WM_INITDIALOG :
		{
			OnChildDialogInit(hwnd);

			SetDlgItemSubClass(hwnd, IDC_BTN_LOCATE1, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_BTN_LOCATE2, SCP_Static_HandCusror);
			SetDlgItemSubClass(hwnd, IDC_EDIT_WNDMODULE, SCP_HwndFileEdit);
			SetDlgItemSubClass(hwnd, IDC_EDIT_IMAGEFILE, SCP_HwndFileEdit);

			return TRUE;
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
				case IDC_BTN_LOCATE1:
				case IDC_BTN_LOCATE2:
				if( STN_CLICKED==HIWORD(wParam))
				{
					OnLocateFileFromCtrlAsync(hwnd, LOWORD(wParam)-2);
					DoDisableCtrlForAMoment(hwnd, LOWORD(wParam), 500);
				}
				return TRUE;
			}
			break;
		}

	case WM_NOTIFY:
		{
			if(	g_siWinVer > WINVER_WIN9X && 
				((LPNMTTDISPINFO)lParam)->hdr.code==TTN_NEEDTEXT  &&
				( IDC_EDIT_PID==GetDlgCtrlID((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom) ||
				IDC_EDIT_TID==GetDlgCtrlID((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom) ) )
			{
				if( GetWindowTextLength((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom) )
				{
					wsprintf(g_strBuf, TEXT("%u\0"), 
						GetWindowInteger((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom, TRUE));
					((LPNMTTDISPINFO)lParam)->lpszText=g_strBuf;
				}
				break;
			}
			return OnToolTipNotify(wParam, lParam);
		}
		
	}//switch message...
	
	return FALSE;
			 
} //TabDlgProc_Process()

#include "WndSpyGui.h"
#include "DlgSaveIconOption.h"

#define  ID_TIMER_CLOSE_AND_LOCATEDIR	1 //only one, whatever...:)

INT IconOrCursor(HANDLE hImage)
{
	ICONINFO IconInfo={0};
	INT iRet=0;
	if( GetIconInfo((HICON)hImage, &IconInfo) )
	{
		iRet=IconInfo.fIcon? 1:2;
	}
	DeleteObject(IconInfo.hbmMask);
	DeleteObject(IconInfo.hbmColor);
	
	return iRet;
}

#define IsIconImage(_hImage)	( (BOOL)( 1==IconOrCursor((HANDLE)_hImage) ) )
#define IsCursorImage(_hImage)	( (BOOL)( 2==IconOrCursor((HANDLE)_hImage) ) )

BOOL CALLBACK DlgProc_IconColorDepthsSel(HWND hwnd, 
										 UINT message, 
										 WPARAM wParam, 
										 LPARAM lParam)
{
	INT nBit;
	TCHAR szBuf[MAX_PATH];
	TCHAR szPathBuf[MAX_PATH];
	
	switch (message)
		
	{
	case WM_INITDIALOG:
		{

#ifdef DLGPROC_CHECK_ICON_FIRST
			if( !IsIconImage(g_hTarget) )
			{
				MessageBoxFromResStr(g_hInstance, g_hwnd, IDS_MSG_NOTICON, IDS_MSG_NOTE, MB_TOPMOST);
				EndDialog(hwnd, 1);
				break;
			}
#endif //DLGPROC_CHECK_ICON

			SendDlgItemMessage(hwnd, IDC_SICON, 
				STM_SETIMAGE, IMAGE_ICON, 
				(LPARAM)CopyImage((HICON)g_hTarget, IMAGE_ICON, NULL, NULL, NULL));

			CheckRadioButton(hwnd,IDC_RB1, IDC_RB5, IDC_RB5);
			
			SetFocus(GetDlgItem(hwnd,IDOK));
			MoveWndToWndCenter(hwnd, g_hwnd);			
			Sleep(20); //delay feeling...
			return FALSE;
		}

	case WM_TIMER:
		{
			if(ID_TIMER_CLOSE_AND_LOCATEDIR==wParam)
			{
				KillTimer(hwnd,wParam);
				if( IsAutoOpenSavedFile() )
				{
					DoLocateFile(g_strBuf, FALSE);
				}
				EndDialog(hwnd, TRUE);
			}
			break;
		}

	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_RB1:
			case IDC_RB2:
			case IDC_RB3:
			case IDC_RB4:
			case IDC_RB5:
				{
					if(HIWORD(wParam)==BN_CLICKED)
					{
						SetWindowLong(hwnd,GWL_USERDATA,(LONG)LOWORD(wParam));
					}
					break;
				}
				
			case IDOK:
				{
					switch (GetWindowLong(hwnd,GWL_USERDATA))
					{
					case IDC_RB1:
						nBit=1;
						break;
					case IDC_RB2:
						nBit=4;
						break;
					case IDC_RB3:
						nBit=8;
						break;
					case IDC_RB4:
						nBit=24;
						break;
					case IDC_RB5:
					default:
						nBit=32;
					}

					MakeFileTypeFromStrRes(g_hInstance, IDS_FILETYPE_ICON, 
							szBuf, NULL, TEXT("*.ico"));

					if(	GetSaveFileNameDialog(NULL, hwnd, g_strBuf, 
						NULL, ReadUserPathFromINI(szPathBuf, STR_INI_KEY_SAVEICON), 
						szBuf, TEXT ("ico"), 
						NULL, OFN_EX_NOPLACESBAR, g_siWinVer>WINVER_WIN9X)
						)
					{
						ShowWindowAsync(hwnd, SW_HIDE);
						g_hTarget=(HANDLE)SendDlgItemMessage(hwnd, IDC_SICON, STM_GETIMAGE, IMAGE_ICON, 0);
						BringWindowToTop(g_hwnd);
						
						if( !SaveIconToFile((HICON)g_hTarget, g_strBuf, nBit) )
						{
							MessageBoxFromResStr(g_hInstance, g_hwnd, IDS_ERROR_SAVEICON, NULL, MB_TOPMOST);
							EndDialog(hwnd, TRUE);
							break;
						}

						WriteUserPathToINI(g_strBuf, FALSE, STR_INI_KEY_SAVEICON);
						SetTimer(hwnd, ID_TIMER_CLOSE_AND_LOCATEDIR, 20, NULL);
					}
					
					break;
				}

			case IDCANCEL:
				{
					EndDialog(hwnd, FALSE);
					break;
				}

			}//switch @ wm_command end...

			return FALSE;
		}//WM_COMMAND end...

	case WM_DESTROY:
		{
			DoUnSetCtrlIcon(hwnd, IDC_SICON);
			return FALSE;
		}
		
	}//switch message end...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);

}//DlgProc_IconColorDepthsSel() end...
										 

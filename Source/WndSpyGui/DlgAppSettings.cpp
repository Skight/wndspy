#include "WndSpyGui.h"
#include "DlgAppSettings.h"


BOOL CALLBACK DlgProc_APPSetting(HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam)
{
	HWND hwndBuf;
	switch (message)
	{
	case  WM_INITDIALOG:
		{
			g_hwnd_AppSetting=hwnd;

			CheckDlgButton(hwnd, IDC_CHK_MULTIINST, 
				g_option.IsMultiInst? BST_CHECKED: BST_UNCHECKED);
			CheckDlgButton(hwnd, IDC_CHK_PLAYSOUND, 
				g_option.IsPlaySound? BST_CHECKED: BST_UNCHECKED);

			SendDlgItemMessage(hwnd, IDC_HOTKEY_SPYIT, HKM_SETRULES,
						(WPARAM)HKCOMB_NONE|HKCOMB_S|HKCOMB_A|HKCOMB_SCA|HKCOMB_SA|HKCOMB_CA,
						MAKELPARAM(HOTKEYF_CONTROL | HOTKEYF_SHIFT, 0));
			SendDlgItemMessage(hwnd, IDC_HOTKEY_SPYIT, HKM_SETHOTKEY, 
				g_option.SpyHotkey.Hotkey, 0);

			SetDlgItemInt(hwnd, IDC_EDIT_DELAY, g_option.SpyHotkey.wDelay, FALSE);	
			SetDlgItemInt(hwnd, IDC_EDIT_OFFSET_X, g_option.SpyHotkey.ptsOffset.x, TRUE);
			SetDlgItemInt(hwnd, IDC_EDIT_OFFSET_Y, g_option.SpyHotkey.ptsOffset.y, TRUE);		
			
			CheckDlgButton(hwnd, IDC_CHK_HOTKEYSPY, 
				g_option.SpyHotkey.wEnableFlag ? BST_CHECKED: BST_UNCHECKED);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CHK_HOTKEYSPY, 0), 0);
			
			SendDlgItemMessage(hwnd, IDC_EDIT_DELAY, EM_LIMITTEXT, 4, 0);
			SendDlgItemMessage(hwnd, IDC_EDIT_OFFSET_X, EM_LIMITTEXT, 4, 0);
			SendDlgItemMessage(hwnd, IDC_EDIT_OFFSET_Y, EM_LIMITTEXT, 4, 0);

			hwndBuf=CreateMyTooltip(hwnd);
			SendMessage(hwndBuf, TTM_SETMAXTIPWIDTH, 0, (LPARAM)256);
			LoadString(g_hInstance, IDS_TIP_SPYHOTKEY, g_strBuf, MAX_STRBUF_LEN);
			AddControlTip(hwndBuf, hwnd, IDC_CHK_HOTKEYSPY, g_strBuf);

			EnableThemeDialogTexture(hwnd, ETDT_ENABLETAB);
			
			SetDlgItemSubClass(hwnd, IDC_EDIT_OFFSET_X, SCP_Edit_DecNum);
			SetDlgItemSubClass(hwnd, IDC_EDIT_OFFSET_Y, SCP_Edit_DecNum);

			DoSetMainIconAndCenterWnd(hwnd);
			
			return FALSE;
		}

	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_EDIT_DELAY:
			case IDC_EDIT_OFFSET_X:
			case IDC_EDIT_OFFSET_Y:
				{
					if(HIWORD(wParam) == EN_KILLFOCUS)
					{
						if( !IsWndTextDecStr((HWND)lParam) )
						{
							SetWindowText((HWND)lParam, TEXT("0"));
						}
					}
					break;
				}
			case IDC_CHK_HOTKEYSPY:
				{
					DoEnableDlgItems(hwnd, IDC_HOTKEY_SPYIT, IDC_STATIC_SHK4, 1, 
						(BOOL)IsDlgButtonChecked(hwnd,IDC_CHK_HOTKEYSPY));
					break;
				}
			case IDOK:
				{
					g_option.IsMultiInst=(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_MULTIINST);
					g_option.IsPlaySound=(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_PLAYSOUND);

					g_option.SpyHotkey.wEnableFlag=(WORD)IsDlgButtonChecked(hwnd, IDC_CHK_HOTKEYSPY);

					g_option.SpyHotkey.Hotkey=(WORD)
						SendMessage(GetDlgItem(hwnd, IDC_HOTKEY_SPYIT), HKM_GETHOTKEY, 0, 0);

					g_option.SpyHotkey.ptsOffset.x=(SHORT)
						GetDlgItemInt(hwnd, IDC_EDIT_OFFSET_X, FALSE, TRUE);

					g_option.SpyHotkey.ptsOffset.y=(SHORT)
						GetDlgItemInt(hwnd, IDC_EDIT_OFFSET_Y, FALSE, TRUE);

					g_option.SpyHotkey.wDelay=(WORD)GetDlgItemInt(hwnd, IDC_EDIT_DELAY, FALSE, FALSE);

					if( !DoRegisterHotKey(g_hwndBK, ID_HOTKEY_SPYIT, 
						g_option.SpyHotkey.Hotkey, g_option.SpyHotkey.wEnableFlag) )
					{
						g_option.SpyHotkey.wEnableFlag=(WORD)FALSE;
						MessageBoxFromResStr(g_hInstance, hwnd, 
							IDS_ERROR_HOTKEY, 0, MB_TOPMOST);
						SetFocus(GetDlgItem(hwnd,IDC_HOTKEY_SPYIT));
						break;
					}

					WriteAppOption(TRUE);
				}
				//fall through..
			case IDCANCEL:
				{
					EndDialog(hwnd, FALSE);
					break;
				}
			}//switch @ wm_command
			
			break;
		}//WM_COMMAND
	case WM_DESTROY:
		{
			g_hwnd_AppSetting=NULL;
			break;
		}

	default:
		return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);

	}//switch message...
	
	return FALSE;

}//DlgProc_APPSetting()
//////////////////////////////////////////////////////////////////////////

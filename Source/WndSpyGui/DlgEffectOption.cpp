#include "WndSpyGui.h"
#include "DlgColorSpy.h"
#include "DlgEffectOption.h"
//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_EffectOption(HWND hwnd, 
								UINT message, 
								WPARAM wParam, 
								LPARAM lParam)
{

	static COLORREF s_color;

	switch (message)
	{
	case  WM_INITDIALOG:
		{
			g_hwnd_EffectOption=hwnd;

			if(!lpfSetLayeredWindowAttributes)
			{
				DoEnableDlgItems(hwnd, IDC_STATIC_ALPHA_0, IDC_STATIC_ALPHA_3, 1, FALSE);
			}

			SendDlgItemMessage(hwnd, IDC_SLIDER, TBM_SETRANGE, 
				FALSE, MAKELONG(TRANS_ALPHA_MIN, TRANS_ALPHA_MAX));
			SendMessage(GetDlgItem(hwnd, IDC_SPIN), UDM_SETRANGE, 
				0, MAKELONG(TRANS_ALPHA_MIN, TRANS_ALPHA_MAX));

			SetDlgItemSubClass(hwnd,IDC_EDIT_ALPHA, SCP_Edit_DecNum);
			SendDlgItemMessage(hwnd, IDC_EDIT_ALPHA, EM_LIMITTEXT, 3, 0);
			SetDlgItemInt(hwnd, IDC_EDIT_ALPHA, g_option.siAlpha, FALSE);

			SendDlgItemMessage(hwnd, IDC_EDIT_COLOR, EM_LIMITTEXT, 6, 0);
			SetDlgItemSubClass(hwnd,IDC_EDIT_COLOR, SCP_Edit_HexNum);
			s_color=g_option.color;
			DlgItemPrintf(hwnd, IDC_EDIT_COLOR, 
						TEXT("%02X%02X%02X"), 
						GetRValue(s_color), 
						GetGValue(s_color), 
						GetBValue(s_color));

			CheckDlgButton(hwnd, IDC_CHK_LOCKUPDATE, 
				g_option.IsLockUpdate?BST_CHECKED: BST_UNCHECKED);
			
			EnableThemeDialogTexture(hwnd, ETDT_ENABLETAB);
			MoveWndToWndCenter(hwnd, g_hwnd);

			SetDlgItemFocus(hwnd, IDC_STATIC_HEADER);

			return FALSE;
		}

#define COLORBTN_OWNERDRAW
#ifdef COLORBTN_OWNERDRAW
	case WM_DRAWITEM:
		{
			if( wParam == IDC_BTN_COLOR )
			{	
				OnDrawColorBtn(lParam, (COLORREF)s_color);
			}
			break;
		}
#endif //COLORBTN_OWNERDRAW

	case WM_NOTIFY:
		{
			switch( ((LPNMHDR)lParam)->idFrom )
			{
#ifndef COLORBTN_OWNERDRAW
			case IDC_BTN_COLOR:
				{
					HBRUSH hBrush;
					HBRUSH hBrushOrg;
					NMCUSTOMDRAW* lpNCD;
					if( ((LPNMHDR)lParam)->code == NM_CUSTOMDRAW )
					{
						lpNCD=(NMCUSTOMDRAW*)lParam;
						hBrush = CreateSolidBrush((COLORREF)s_color);
						hBrushOrg = (HBRUSH)SelectObject(lpNCD->hdc, hBrush);
						Rectangle(lpNCD->hdc, 
							lpNCD->rc.left, lpNCD->rc.top, 
							lpNCD->rc.right, lpNCD->rc.bottom);
						SelectObject(lpNCD->hdc, hBrushOrg);
						DeleteObject(hBrush);
					}
					return TRUE;
				}
#endif //!COLORBTN_OWNERDRAW
			case IDC_SPIN:
				{
					((LPNMUPDOWN)lParam)->iDelta *= -1;
					return TRUE;
				}
			}
			break;
		}

	case WM_HSCROLL:
		{
 			//if( LOWORD(wParam) == TB_ENDTRACK) 
 			//{ 
				INT iBuf;
				iBuf = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
				SetDlgItemInt(hwnd, IDC_EDIT_ALPHA, min(TRANS_ALPHA_MAX,iBuf), FALSE);
			//}
			break;
		}

	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_EDIT_ALPHA:
				{
					lParam=OnNumEditMaxMinRoutine(hwnd, wParam, lParam, TRANS_ALPHA_MIN, TRANS_ALPHA_MAX);
					if(lParam)
					{
						SendDlgItemMessage(hwnd, IDC_SLIDER, TBM_SETPOS, TRUE, lParam);
					}
					break;
				}
			case IDC_EDIT_COLOR:
				{
					if(HIWORD(wParam) ==EN_CHANGE)
					{
						lParam=GetDlgItemHex(hwnd, IDC_EDIT_COLOR);
						s_color=RGB(
							LOBYTE(HIWORD(lParam)), //R
							HIBYTE(LOWORD(lParam)), //G
							LOBYTE(LOWORD(lParam)) //B
							);

						InvalidateRect(GetDlgItem(hwnd, IDC_BTN_COLOR), NULL, FALSE);

					}
					else if(HIWORD(wParam) ==EN_KILLFOCUS)
					{
						DlgItemPrintf(hwnd, IDC_EDIT_COLOR, 
							TEXT("%02X%02X%02X"), 
							GetRValue(s_color), 
							GetGValue(s_color), 
							GetBValue(s_color));
					}
					break;
				}
			case IDC_BTN_COLOR:
				{
					CHOOSECOLOR cc;
					ChooseColorDialog(hwnd, &cc, (COLORREF)s_color, NULL);
					s_color=(DWORD)cc.rgbResult;
					DlgItemPrintf(hwnd, IDC_EDIT_COLOR, 
						TEXT("%02X%02X%02X"), 
						GetRValue(s_color), 
						GetGValue(s_color), 
						GetBValue(s_color));
					break;
				}
			case IDC_BTN_DEFAULT:
				{
					CheckDlgButton(hwnd, IDC_CHK_LOCKUPDATE, BST_CHECKED);
					SetDlgItemInt(hwnd, IDC_EDIT_ALPHA, TRANS_ALPHA_DEFAULT, FALSE);
					DlgItemPrintf(hwnd, IDC_EDIT_COLOR, 
						TEXT("%02X%02X%02X"), 
						BASE_COLOR_R, BASE_COLOR_G, BASE_COLOR_B);
					break;
				}
			case IDOK:
				{
					g_option.IsLockUpdate=IsDlgButtonChecked(hwnd, IDC_CHK_LOCKUPDATE);
					g_option.siAlpha=(WORD)GetDlgItemInt(hwnd, IDC_EDIT_ALPHA, FALSE, FALSE);
					g_option.color=(COLORREF)s_color;
					OnSetMenuState(GetMenu(g_hwnd));
					WriteAppOption(TRUE);
				}//fall through..
			case IDCANCEL:
				{
					EndDialog(hwnd, FALSE);
					break;
				}
			
			}//switch @ wm_command

			break;
		}//WM_COMMAND

	case WM_SYSCOLORCHANGE:
		{
			SendDlgItemMessage(hwnd, IDC_SLIDER, WM_SYSCOLORCHANGE, 0, 0);
			EnableThemeDialogTexture(hwnd, ETDT_DISABLE);
			break;
		}
		
	case WM_DESTROY:
		{
			g_hwnd_EffectOption=NULL;
			break;
		}

	default:
		return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);

	}//switch message...
	
	return FALSE;

} //DlgProc_EffectOption()

//////////////////////////////////////////////////////////////////////////

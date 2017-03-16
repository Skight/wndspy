#include "WndSpyGui.h"
#include "DlgAbout.h"

//////////////////////////////////////////////////////////////////////////


VOID OnSetLinkWindow(HWND hwnd, BOOL isSupport, UINT IdNormalLink, UINT idLinkStrRes)
{
	TCHAR szBuf[SMALL_STRBUF_LEN+SMALL_STRBUF_LEN];
	TCHAR szSmallBuf[SMALL_STRBUF_LEN];
	HWND hwndLink=GetDlgItem(hwnd, IdNormalLink);
	RECT rc;
	
	LoadString(g_hInstance, idLinkStrRes, szSmallBuf, SMALL_STRBUF_LEN);
	if(isSupport)
	{
		GetWindowRect(hwndLink, &rc);
		ScreenToClient(hwnd, (POINT*)&rc);
		ScreenToClient(hwnd, (POINT*)&rc+1);
		DestroyWindow(hwndLink);
		hwndLink=CreateWindowW(
			WC_LINK, L"", 
			WS_CHILD|WS_VISIBLE, 
			rc.left, rc.top, 
			rc.right-rc.left, rc.bottom-rc.top, 
			hwnd, (HMENU)IdNormalLink, NULL, NULL);
		SendMessage(hwndLink, WM_SETFONT, 
			(WPARAM)SendMessage(hwnd, WM_GETFONT, 0, 0), TRUE);
		wsprintf(szBuf, szSmallBuf, TEXT("<a>"), TEXT("</a>"));
	}
	else
	{
		SetWndSubClass(hwndLink, SCP_Static_HandCusror);
		wsprintf(szBuf, szSmallBuf, TEXT(""), TEXT(""));
	}
	
	SetWindowText(hwndLink, szBuf);
	
} //OnSetLinkWindow()


HFONT OnCreateTitleFont(void)
{
	NONCLIENTMETRICS ncm = {0};
	ncm.cbSize = sizeof(ncm);

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	
	LOGFONT lFont;
	lFont=ncm.lfMessageFont;
	lFont.lfWeight = FW_BOLD;
	lstrcpyn(lFont.lfFaceName, TEXT("Verdana Bold"), LF_FACESIZE);
	
	HDC hdc;	
	hdc = GetDC(NULL);
	lFont.lfHeight = 0 - GetDeviceCaps(hdc, LOGPIXELSY) * 10 / 72 -1;
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));
	ReleaseDC(NULL, hdc);
	
	return CreateFontIndirect(&lFont);

} //OnCreateTitleFont()


//////////////////////////////////////////////////////////////////////////
#define VERSION_FMT		TEXT("%d.%d ( %s Build )")
#define AUTHOR_INFO		TEXT("Skight ( hesung@qq.com )")
#define CREDIT_URL_1	TEXT("http://www.kobik.net/spy_capture.asp")
#define CREDIT_URL_2	TEXT("http://msdn.microsoft.com/en-us/library/ms997538.aspx")


BOOL CALLBACK DlgProc_AboutWS(HWND hwnd, 
							  UINT message, 
							  WPARAM wParam, 
							  LPARAM lParam)
{

	static BOOL isExtended;
	
	switch (message)
	{

	case WM_INITDIALOG:
		{
			g_hwnd_TaskModal=g_hwnd_AboutWS=hwnd;

			BOOL isSupport = OnInitComCtrl(ICC_LINK_CLASS);
			
			OnSetLinkWindow(hwnd, isSupport, IDC_INFO_THANKS_1, IDS_THANKS_1);
			OnSetLinkWindow(hwnd, isSupport, IDC_INFO_THANKS_2, IDS_THANKS_2);
			
			SendDlgItemMessage(hwnd, IDC_INFO_APPNAME, WM_SETFONT, 
				(WPARAM)OnCreateTitleFont(), TRUE);
			SetWindowVisible(hwnd, IDC_INFO_APPNAME, TRUE);
			
			isExtended=FALSE;
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_CREDITS, 0), 0);

			wsprintf(g_strBuf, VERSION_FMT, VER_NUM_MAJOR, VER_NUM_MINOR, VER_CHARSET);
			SetDlgItemText(hwnd, IDC_INFO_VERSION, g_strBuf);
			SetDlgItemText(hwnd, IDC_INFO_AUTHOR, AUTHOR_INFO);

#define _ABOUT_NODEFPUSH
#ifdef _ABOUT_NODEFPUSH

			SetDlgItemSubClass(hwnd, IDC_BTN_ISEE, SCP_Btn_NoDefPush);
			SetDlgItemSubClass(hwnd, IDC_BTN_CREDITS, SCP_Btn_NoDefPush);
#endif
			
			EnableThemeDialogTexture(hwnd, ETDT_ENABLETAB);
			MoveWndToWndCenter(hwnd, g_hwnd);
			
			//SetCursor(LoadCursor(NULL, IDC_ARROW));
			//AnimateWindow(hwnd, 200, AW_CENTER | AW_SLIDE);
			//Sleep(20);
			return TRUE;
		}
		
	case WM_NOTIFY:
		{
			switch( ((LPNMHDR)lParam)->idFrom )
			{
			case IDC_INFO_THANKS_1:
			case IDC_INFO_THANKS_2:
				{
					if(	((LPNMHDR)lParam)->code == NM_CLICK )
					{
						SetDlgItemFocus(hwnd, IDC_LABEL_CREDITS);
						SendMessage(hwnd, WM_COMMAND, 
							MAKEWPARAM(((LPNMHDR)lParam)->idFrom, 0), 0);
					}
					break;
				}
			}
			break;
		}
		
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_INFO_THANKS_1:
			case IDC_INFO_THANKS_2:
				{
					ShellExecute(NULL, TEXT("open"), 
						(IDC_INFO_THANKS_1==LOWORD(wParam))? CREDIT_URL_1:CREDIT_URL_2, 
						NULL, NULL, SW_MAXIMIZE);
					return TRUE;
				}
			case IDC_BTN_CREDITS:
				{
					RECT rect, rc;
					GetWindowRect(hwnd, &rect);
					SetWindowVisible(hwnd, IDC_LABEL_CREDITS, isExtended);
					if(isExtended)
					{
						GetWindowRect(hwnd, &rect);
						GetWindowRect(GetDlgItem(hwnd, IDC_LABEL_CREDITS), &rc);
						SetWindowPos(hwnd, NULL, 0, 0, 
							rect.right-rect.left, rc.bottom + 13 -rect.top, 
							SWP_NOMOVE|SWP_NOZORDER);
						
					}
					else
					{
						GetWindowRect(GetDlgItem(hwnd, IDC_BTN_CREDITS), &rc);
						SetWindowPos(hwnd, NULL, 0, 0, 
							rect.right-rect.left, rc.bottom + 12 -rect.top, 
							SWP_NOMOVE|SWP_NOZORDER);
						
					}
					SendMessage(hwnd, DM_REPOSITION, 0, 0);
					isExtended=!isExtended;
					
					break;
				}
			case IDC_BTN_ISEE:
				{
					EndDialog(hwnd, 0);
					break;
				}
#ifdef _ABOUT_NODEFPUSH
			case IDOK:
				{
					INT iFocusID;
					iFocusID=GetDlgCtrlID(GetFocus());
					if( iFocusID==IDC_BTN_ISEE || 
						 iFocusID==IDC_BTN_CREDITS )
					{
						SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(iFocusID, 0), 0);
					}
					break;
				}
#endif //_ABOUT_NODEFPUSH
			}
			
			return FALSE;
		}//WM_COMMAND
		
	case WM_DESTROY:
		{
			DeleteObject((HFONT)
				SendDlgItemMessage(hwnd, IDC_INFO_APPNAME, WM_GETFONT, 0, 0));

			g_hwnd_AboutWS=NULL;
			g_hwnd_TaskModal=NULL;
			return FALSE;
		}
		
	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
	
} //DlgProc_AboutWS()

//////////////////////////////////////////////////////////////////////////


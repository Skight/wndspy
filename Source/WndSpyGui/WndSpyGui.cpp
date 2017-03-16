///////////////////////////////////////////////////////////////////
#ifdef INTEGRATED_SRC_WNDSPYGUI

#include "DlgPswSniffer.cpp"
#include "DlgWndEnumer.cpp"
#include "DlgWndModifier.cpp"
#include "DlgColorSpy.cpp"
#include "DlgIconExtractor.cpp"
#include "DlgSaveIconOption.cpp"
#include "DlgSndMsg.cpp"
#include "DlgAbout.cpp"
#include "DlgAppSettings.cpp"
#include "EffectOption.cpp"
#include "SpyMainDlg.cpp"
#include "SpyMainDlgTabs.cpp"
#include "SpyMsgWnd.cpp"
#include "SpyMainToolTips.cpp"

#endif //INTEGRATED_SRC_WNDSPYGUI

//////////////////////////////////////////////////////////////////////////

#include "..\WndSpy.h"
#include "WndSpyGui.h"

//////////////////////////////////////////////////////////////////////////

//#define NO_TRANS	255

VOID DoSetWindowTrans(HWND hwndTarget, BYTE byAlpha)
{
	DWORD dwExstyle=GetWindowLong(hwndTarget, GWL_EXSTYLE);

	if(byAlpha==NO_TRANS)
	{
		if( IS_FLAG_SETTED(dwExstyle, WS_EX_LAYERED) )
		{
			SetWindowLong(hwndTarget, GWL_EXSTYLE, dwExstyle& ~WS_EX_LAYERED);
		}
	}
	else
	{
		if( !IS_FLAG_SETTED(dwExstyle, WS_EX_LAYERED) )
		{
			SetWindowLong(hwndTarget, GWL_EXSTYLE, dwExstyle|WS_EX_LAYERED);
		}
	}

	SetLayeredWindowAttributes(hwndTarget, 0, byAlpha, LWA_ALPHA);

} //DoSetWindowTrans();

//////////////////////////////////////////////////////////////////////////

BOOL g_bIsNcBtnDownDispatched=FALSE;

VOID CALLBACK TimerProc_Effect_NCButtonDownDispatch(HWND hwnd, 
													UINT uMsgBuf, 
													UINT_PTR idEvent, 
													DWORD dw_lParamBuf)
{

	KillTimer(hwnd, idEvent);

	if( idEvent==TIMER_NCRBTN_DELAY )
	{
		POINT pt;
		GetCursorPos(&pt);
		dw_lParamBuf=(DWORD)MAKELPARAM(pt.x, pt.y);
	}
	else
	{
		dw_lParamBuf=0;
	}

	g_bIsNcBtnDownDispatched=TRUE;
	PostMessage(hwnd, idEvent-TIMER_EFFACT_DELAY, (WPARAM)HTCAPTION, (LPARAM)dw_lParamBuf);

}

VOID KillTimer_NcBtnDispatch(HWND hwnd)
{
	KillTimer(hwnd, TIMER_NCRBTN_DELAY);
	KillTimer(hwnd, TIMER_NCLBTN_DELAY);
}

BOOL CallSpecWndProcRoutine(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_MENUSELECT:
		{
			if( lParam || !(GetKeyState(VK_LBUTTON)&0x8000) )
			{
				return FALSE;
			}
			//fall through...
		}
	case WM_LBUTTONDOWN:
		{
			PostMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
			break;
		}

	case WM_COMMAND:
	case WM_SYSCOMMAND:
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
		{
			KillTimer(hwnd, TIMER_NCRBTN_DELAY);
			KillTimer(hwnd, TIMER_NCLBTN_DELAY);
			break;
		}

	case WM_NCRBUTTONDOWN:
	case WM_NCLBUTTONDOWN:
		{
			if( wParam==HTCAPTION )
			{
				if( g_bIsNcBtnDownDispatched==FALSE )
				{
					if( lParam!=0 )
					{
						BringWindowToTop(hwnd);
					}
					SetTimer(hwnd, (TIMER_EFFACT_DELAY+message), 
						USER_TIMER_MINIMUM, TimerProc_Effect_NCButtonDownDispatch);

					return TRUE;
				}
				else
				{
					g_bIsNcBtnDownDispatched=FALSE;
				}
			}
			break;
		}

	case WM_GETMINMAXINFO:
		{
			RECT rect;
			GetWindowRect(hwnd, &rect) ;
			((LPMINMAXINFO)lParam)->ptMaxPosition.x = rect.left;
			((LPMINMAXINFO)lParam)->ptMaxPosition.y = rect.top ;
			((LPMINMAXINFO)lParam)->ptMaxSize.x = rect.right - rect.left;
			((LPMINMAXINFO)lParam)->ptMaxSize.y = rect.bottom - rect.top;
			break;
		}

	case WM_SIZE:
		{
			if( SIZE_MINIMIZED == wParam )
			{
				CloseWindow(hwnd);
			}
			else if( SIZE_MAXIMIZED == wParam )
			{
				ShowWindow(hwnd, SW_RESTORE);
			}
			break;
		}

	case WM_STYLECHANGING:
		{
			if( wParam==GWL_STYLE )
			{
				((STYLESTRUCT*)lParam)->styleNew&=~WS_THICKFRAME;
				return TRUE;
			}
			break;
		}
	}

	return FALSE;

} //CallSpecWndProcRoutine()

//////////////////////////////////////////////////////////////////////////

VOID Workaround_BugTrans(HWND hwnd, DWORD dwFlagBit)
{
	if( IsWindowVisible(hwnd) && 
		IsWndInWnd(hwnd, GetDesktopWindow()) )
	{
		g_dwBitFlag|=dwFlagBit;

		// TODO: Put workaround codes here if needed...
		if( !g_option.IsTopMost )
		{
			//MessageBeep(0);
			LockWindowUpdate(hwnd);
			LockWindowUpdate(NULL);
		}
		//MessageBeep(0);
	}

} //Workaround_BugTrans()

//////////////////////////////////////////////////////////////////////////

VOID DoSetMainIconAndCenterWnd(HWND hwnd)
{
	SendMessage(hwnd, WM_SETICON, ICON_BIG, 
		(LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAIN)));
	MoveWndToWndCenter(hwnd, g_hwnd);
}


//////////////////////////////////////////////////////////////////////////

COLORREF GetTinctureColor(VOID)
{
#define CR_COLOR_BLUE	(RGB(51, 96, 169))
#define CR_COLOR_GREEN	(RGB(139,161,105))
#define CR_COLOR_GRAY	(RGB(128,128,128))
//get the tincture for FinderDragIcon's the title bar.
//adapt to 三种主流 schemes of Windows...
//ingore all other color schemes, 
//Koz to be a good programmer you should have mainstream feelings.
	COLORREF color;
	BYTE r, g, b; 
	color=GetSysColor(COLOR_ACTIVECAPTION); 
	//Why not directly use the COLOR_ACTIVECAPTION? 
	//Koz it might have been setted to a !mainstream style. 
	r=GetRValue(color);
	g=GetGValue(color);
	b=GetBValue(color);
	if((b - g) > 32)
	{
		return CR_COLOR_BLUE;
	}
	if((g - b) > 32)
	{
		return CR_COLOR_GREEN;
	}
	return CR_COLOR_GRAY;

} //GetTinctureColor()


//SubClassWndProc : SCP_Param_Static_PickCursorPan
LRESULT APIENTRY SCP_Param_Static_PickCursorPan(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#define HDR_LEFT	4
#define HDR_RIGHT	27
#define HDR_TOP		0
#define HDR_BOTTOM	5

#define PAN_LEFT	1
#define PAN_RIGHT	29
#define PAN_TOP		5
#define PAN_BOTTOM	26


	static HICON hIconPan=LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON_PICKER_PAN));
	static RECT rcPanHeader={HDR_LEFT, HDR_TOP, HDR_RIGHT, HDR_BOTTOM};
	static RECT rcPanSpace={HDR_LEFT, PAN_TOP, PAN_RIGHT, PAN_BOTTOM};

	WND_SUBCLASS_DATA* lpWSD = ((WND_SUBCLASS_DATA*)GetWindowLong(hwnd, GWL_USERDATA));


	switch (message)
	{
	case STM_SETIMAGE:
		{
			return NULL;
		}
	case WM_MY_SETPANCURSOR:
		{
			if(wParam)
			{
				hIconPan=(HICON)wParam;
			}
			lpWSD->lParam=lParam;
			InvalidateRect(hwnd, &rcPanHeader, FALSE);
			InvalidateRect(hwnd, &rcPanSpace, FALSE);
			return TRUE;
		}

	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			HBRUSH hBrush, hOrgBrush;
			
			hdc=BeginPaint(hwnd, &ps);
			DrawIcon(hdc, 0, 0, hIconPan);
			if(lpWSD->lParam)
			{
				DrawIcon(hdc, 0, 0, HICON(lpWSD->lParam));
			}
			hBrush=CreateSolidBrush(GetTinctureColor());
			hOrgBrush=(HBRUSH)SelectObject(hdc, hBrush);
			Rectangle(hdc, 
				rcPanHeader.left, 
				rcPanHeader.top, 
				rcPanHeader.right, 
				rcPanHeader.bottom);
			hBrush=(HBRUSH)SelectObject(hdc, hOrgBrush);
			DeleteObject(hBrush);
			EndPaint(hwnd, &ps);

			return TRUE;
		}
	}
	
	return CallWindowProc(lpWSD->WndProcBuf, hwnd, message, wParam, lParam);
	
} //SCP_Param_Static_PickCursorPan()

//////////////////////////////////////////////////////////////////////////

VOID InitPickPanCtrl(HWND hwndPickPan, WND_SUBCLASS_DATA* pWSCD, HCURSOR hCurInPan)
{
			SetWindowPos(hwndPickPan, NULL, 
				NULL, NULL, PAN_RECT_CX, PAN_RECT_CY, 
				SWP_NOMOVE|SWP_NOZORDER);
			pWSCD->WndProcBuf=SCP_Param_Static_PickCursorPan;
			SetWndSubClassParam(hwndPickPan, pWSCD);
			PickPan_SetCursor(hwndPickPan, hCurInPan);

} //InitPickPanCtrl()
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

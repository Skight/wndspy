#include "WndSpyGui.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include "DlgColorSpy.h"

//////////////////////////////////////////////////////////////////////////

#define SRC_SEL				9
#define SCALE_UNIT			11
#define MAGNIFIER_DIM		(SRC_SEL*SCALE_UNIT)
#define MAGNIFIER_CX		(MAGNIFIER_DIM+2)
#define MAGNIFIER_CY		(MAGNIFIER_DIM+2)
#define MAGNIFIER_CENTER	(MAGNIFIER_DIM>>1)

typedef struct tag_ParamsWrapper_ColorSpy
{
	HDC hdcScreen;
	HWND hwndMagn;
	HWND hwndClrHtml;
	HWND hwndClrWebSafe;
	HWND hwndDragPickerPan;
	BOOL IsDragging;
	COLORREF sClrCurrent;
	COLORREF ClrCurrent;
	HBITMAP hbmMagnOrg, hbmMagn;
	HDC hdcMagnCompat;
	HDC hdcMagn;
	POINT ptCurSel;
	HICON hIconDragTip;
	
}PARAM_WRAPPER_COLORSPY;

//////////////////////////////////////////////////////////////////////////

COLORREF GetMagnifierGridColor(HWND hwndMagn, HDC hdcScreen, POINT pt)
{
	ScreenToClient(hwndMagn, &pt);
	pt.x=(pt.x/SCALE_UNIT)*SCALE_UNIT+(SCALE_UNIT>>1);
	pt.y=(pt.y/SCALE_UNIT)*SCALE_UNIT+(SCALE_UNIT>>1);
	ClientToScreen(hwndMagn, &pt);
	return GetPixel(hdcScreen, pt.x, pt.y);

} //GetMagnifierGridColor();


VOID OnDrawSelFocus(HDC hdc, POINT pt)
{
	RECT rc;
	
	pt.x=(pt.x/SCALE_UNIT)*SCALE_UNIT;
	pt.y=(pt.y/SCALE_UNIT)*SCALE_UNIT;

	rc.left=pt.x;
	rc.top=pt.y;
	rc.right=pt.x+SCALE_UNIT;
	rc.bottom=pt.y+SCALE_UNIT;
	DrawFocusRect(hdc, &rc);
	rc.left+=1;
	rc.right-=1;
	rc.top+=1;
	rc.bottom-=1;
	DrawFocusRect(hdc, &rc);

} //OnDrawSelFocus();

VOID OnUpdateMagnifier(LPARAM lParam, HDC hdcScreen, POINT ptCurSel)
{	
	POINT pt;
	GetCursorPos(&pt);
	
	StretchBlt(((LPDRAWITEMSTRUCT)lParam)->hDC, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.left, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.top, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.right, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom, 
		hdcScreen, pt.x-SRC_SEL/2, pt.y-SRC_SEL/2, SRC_SEL, SRC_SEL, SRCCOPY);

	OnDrawSelFocus(((LPDRAWITEMSTRUCT)lParam)->hDC, ptCurSel);

} //OnUpdateMagnifier();


VOID OnDrawColorBtn(LPARAM lParam, COLORREF clr)
{
	HBRUSH hBrush, hBrushOrg;
	
	hBrush = CreateSolidBrush(clr);
	hBrushOrg = (HBRUSH)
		SelectObject(((LPDRAWITEMSTRUCT)lParam)->hDC, hBrush);
				
	Rectangle(((LPDRAWITEMSTRUCT)lParam)->hDC, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.left, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.top, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.right, 
		((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom);
				
	SelectObject(((LPDRAWITEMSTRUCT)lParam)->hDC, hBrushOrg);
	DeleteObject(hBrush);

} //OnDrawColorBtn();

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_ColorSpy(HWND hwnd, 
							   UINT message, 
							   WPARAM wParam, 
							   LPARAM lParam)
{
	static PARAM_WRAPPER_COLORSPY* pW;
	static WND_SUBCLASS_DATA wsc_DragDropper;
	static HCURSOR hCurDropper=LoadCursor(g_hInstance, MAKEINTRESOURCE(IDI_CUR_DRAGDROP));
	CHOOSECOLOR cc;
	COLORREF clr;
	POINT pt;
	WORD h, s, l;
	BYTE r, g, b;
	BYTE sR, sG, sB;
	
	switch (message)
	{
	case  WM_INITDIALOG:
		{
			g_hwnd_ColorSpy=hwnd;

			pW=(PARAM_WRAPPER_COLORSPY*)
				LocalAlloc(GPTR, sizeof(PARAM_WRAPPER_COLORSPY));

			if(pW==NULL)
			{
				//ShowLastError(g_hwnd);
				DestroyWindow(hwnd);
			}
			
			pW->ClrCurrent=pW->sClrCurrent=GetSysColor(COLOR_BTNFACE);

			pW->hwndClrHtml=GetDlgItem(hwnd, IDC_PAN_HTML);
			pW->hwndClrWebSafe=GetDlgItem(hwnd, IDC_PAN_WEBSAFE);

			pW->hwndMagn=GetDlgItem(hwnd, IDC_MAGNIFIER);
			
			SetWindowPos(pW->hwndMagn, NULL, 
				NULL, NULL, MAGNIFIER_CX, MAGNIFIER_CY, 
				SWP_NOMOVE|SWP_NOZORDER);


			pW->hdcMagn=GetDC(pW->hwndMagn);
			pW->hdcMagnCompat=CreateCompatibleDC(pW->hdcMagn);
			pW->hbmMagn=CreateCompatibleBitmap(pW->hdcMagn, MAGNIFIER_CX, MAGNIFIER_CX);
			pW->hbmMagnOrg=(HBITMAP)SelectObject(pW->hdcMagnCompat, pW->hbmMagn);


			pW->hdcScreen=CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);			

			pW->hwndDragPickerPan=GetDlgItem(hwnd, IDC_ICON_DROPPER);
			InitPickPanCtrl(pW->hwndDragPickerPan, &wsc_DragDropper, hCurDropper);

			HWND hwndTT=CreateMyTooltipEx(hwnd, 
				TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX);
			
			AddControlTipEx(hwndTT, hwnd, IDC_ICON_DROPPER, 
				MAKEINTRESOURCE(IDS_TIP_DROPPER),TTF_SUBCLASS);
			//SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, (LPARAM)190);

			TCHAR szBuf[TINY_STRBUF_LEN];
			pW->hIconDragTip=DoSetIconToolTip(hwndTT, 
				(g_siWinVer >= WINVER_WINXPNEWER), TEXT("SHELL32.DLL"), 161, 
				NULL, g_hInstance, IDS_TIP_DROPPER_TITLE, szBuf, TINY_STRBUF_LEN);
			
			hwndTT=CreateMyTooltip(hwnd);
			AddControlTip(hwndTT, hwnd, IDC_MAGNIFIER, MAKEINTRESOURCE(IDS_TIP_MAGNIFIER));
			AddControlTip(hwndTT, hwnd, IDC_PAN_HTML, MAKEINTRESOURCE(IDS_TIP_COLORDLG));
			
			DoSetMainIconAndCenterWnd(hwnd);

			return FALSE;
		}
		
	case WM_DRAWITEM:
		{
			if( wParam == IDC_PAN_HTML ||
				wParam == IDC_PAN_WEBSAFE )
			{
				SelectObject(((LPDRAWITEMSTRUCT)lParam)->hDC, 
					GetStockObject(NULL_PEN));
				
				clr = (wParam==IDC_PAN_HTML?pW->ClrCurrent:pW->sClrCurrent);
				OnDrawColorBtn(lParam, clr);
			}
			else if( wParam == IDC_MAGNIFIER )
			{
				if( !IS_FLAG_SETTED(g_dwBitFlag, BOOLEAN_BIT_COLORCATCHER) )
				{	
					break;
				}
				if(!pW->IsDragging)
				{
					BitBlt(((LPDRAWITEMSTRUCT)lParam)->hDC, 
						((LPDRAWITEMSTRUCT)lParam)->rcItem.left, 
						((LPDRAWITEMSTRUCT)lParam)->rcItem.top, 
						((LPDRAWITEMSTRUCT)lParam)->rcItem.right, 
						((LPDRAWITEMSTRUCT)lParam)->rcItem.bottom, 
						pW->hdcMagnCompat, 0, 0, SRCCOPY);
					break;
				}
				OnUpdateMagnifier(lParam, pW->hdcScreen, pW->ptCurSel);
			}
			
			return TRUE;
		}

#ifdef _HWND_PEN_DISABLE	//{{{
	
	case WM_LBUTTONDBLCLK:
		{
			GetCursorPos(&pt);
			if( IsPtInWnd(&pt, pW->hwndClrHtml) )
			{
				ChooseColorDialog(hwnd, &cc, (COLORREF)ClrCurrent, NULL);
				SendMessage(hwnd, WM_MY_UPDATECOLORINFO, 
					(WPARAM)cc.rgbResult, 0);
			}
			break;
		}

#endif //_HWND_PEN_DISABLE	//}}}

	case WM_MOUSEMOVE:
		{
			if(pW->IsDragging && IS_FLAG_SETTED(wParam,MK_LBUTTON) )
			{
				InvalidateRect(pW->hwndMagn, NULL, FALSE);
				GetCursorPos(&pt);
				DlgItemPrintf(hwnd, IDC_STATIC_POS, TEXT("%d-%d"), pt.x, pt.y);
				SendMessage(hwnd, WM_MY_UPDATECOLORINFO, 
					(WPARAM)GetPixel(pW->hdcScreen, pt.x, pt.y), 0);
			}
			break;
		}

	case WM_CAPTURECHANGED:
		{
			if(pW->IsDragging)
			{
				SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
			}
			break;
		}

	case WM_LBUTTONUP:
		{
			if(pW->IsDragging)
			{
				pW->IsDragging=FALSE;
				DoPlayEventSound(g_option.IsPlaySound);
				PickPan_SetCursor(pW->hwndDragPickerPan, hCurDropper);
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				ReleaseCapture();
				BitBlt(pW->hdcMagnCompat, 
					0, 0, MAGNIFIER_CX, MAGNIFIER_CX, 
					pW->hdcMagn, 0, 0, SRCCOPY);
			}
			break;
		}
		
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_ICON_DROPPER:
				{
					if( STN_CLICKED == HIWORD(wParam) )
					{
						if( !IS_FLAG_SETTED(g_dwBitFlag, BOOLEAN_BIT_COLORCATCHER) )
						{	
							g_dwBitFlag|=BOOLEAN_BIT_COLORCATCHER;
						}
						pW->ptCurSel.x=pW->ptCurSel.y=MAGNIFIER_CENTER;
						DoPlayEventSound(g_option.IsPlaySound);
						SetFocus(pW->hwndDragPickerPan);
						PickPan_SetCursor(pW->hwndDragPickerPan, NULL);
						SetCursor(hCurDropper);
						SetCapture(hwnd);
						pW->IsDragging=TRUE;
					}					
					break;
				}
			case IDC_MAGNIFIER:
				{
					if( BN_CLICKED == HIWORD(wParam) )
					{
						GetCursorPos(&pt);
						SendMessage(hwnd, WM_MY_UPDATECOLORINFO, 
							(WPARAM)GetMagnifierGridColor((HWND)lParam, pW->hdcScreen, pt), 0);
						OnDrawSelFocus(pW->hdcMagnCompat, pW->ptCurSel);						
						ScreenToClient((HWND)lParam, &pt);
						OnDrawSelFocus(pW->hdcMagnCompat, pt);
						pW->ptCurSel=pt;
						InvalidateRect((HWND)lParam, NULL, FALSE);
						
					}
					break;
				}
			case IDCANCEL:
				{
					ShowWindow(hwnd, SW_MINIMIZE);
					break;
				}

#ifndef _HWND_PEN_DISABLE	//{{{
			case IDC_PAN_HTML:
				{
					if( HIWORD(wParam)==BN_DBLCLK )
					{
						ChooseColorDialog(hwnd, &cc, (COLORREF)pW->ClrCurrent, NULL);
						SendMessage(hwnd, WM_MY_UPDATECOLORINFO, 
							(WPARAM)cc.rgbResult, 0);
					}
					break;
				}
#endif //!_HWND_PEN_DISABLE	//}}}

			}//switch @ wm_command
			
			break;
		}//WM_COMMAND
		
	case WM_MY_UPDATECOLORINFO:
		{
			clr=(COLORREF)wParam;
			if( clr != pW->ClrCurrent)
			{
				pW->ClrCurrent=clr;
				ColorRGBToHLS(clr, &h, &l, &s);
				r=GetRValue(clr), g=GetGValue(clr), b=GetBValue(clr);
				sR=GetWebSafeColorValue(r);
				sG=GetWebSafeColorValue(g);
				sB=GetWebSafeColorValue(b);
				DlgItemPrintf(hwnd, 
					IDC_EDIT_COLORREF, g_szFormat, clr);
				DlgItemPrintf(hwnd, 
					IDC_EDIT_HTML, TEXT("#%02X%02X%02X"), r, g, b);
				DlgItemPrintf(hwnd, 
					IDC_EDIT_WEBSAFE, TEXT("#%02X%02X%02X"), sR, sG, sB);
				DlgItemPrintf(hwnd, IDC_EDIT_R, TEXT("%d"), r);
				DlgItemPrintf(hwnd, IDC_EDIT_G, TEXT("%d"), g);
				DlgItemPrintf(hwnd, IDC_EDIT_B, TEXT("%d"), b);
				DlgItemPrintf(hwnd, IDC_EDIT_HUE, TEXT("%d"), h);
				DlgItemPrintf(hwnd, IDC_EDIT_SAT, TEXT("%d"), s);
				DlgItemPrintf(hwnd, IDC_EDIT_LUM, TEXT("%d"), l);
				
				InvalidateRect(pW->hwndClrHtml, NULL, FALSE);
				if( RGB(sR, sG, sB)!= pW->sClrCurrent)
				{
					pW->sClrCurrent=RGB(sR, sG, sB);
					InvalidateRect(pW->hwndClrWebSafe, NULL, FALSE);
				}
			}
			return TRUE;
		}

	case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return FALSE;
		}
	case WM_DESTROY:
		{
			if(pW)
			{
				ReleaseDC(pW->hwndMagn, pW->hdcMagn);
				SelectObject(pW->hdcMagnCompat, pW->hbmMagnOrg);
				DeleteObject(pW->hbmMagn);
				DeleteDC(pW->hdcMagnCompat);
				SelectObject(pW->hdcScreen, GetStockObject(SYSTEM_FONT));
				DeleteDC(pW->hdcScreen);
				DestroyIcon(pW->hIconDragTip);
				LocalFree((HLOCAL)pW);
			}
			g_dwBitFlag&=~BOOLEAN_BIT_COLORCATCHER;
			g_hwnd_ColorSpy=NULL;
			PostMessage(g_hwndTC, WM_MY_TOOL_CLOSE, 0, 0);
			return FALSE;
		}

	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
	
} //DlgProc_ColorSpy()


//////////////////////////////////////////////////////////////////////////
//
// 	SetROP2(((LPDRAWITEMSTRUCT)lParam)->hDC,R2_NOTXORPEN);
// 	SelectObject(((LPDRAWITEMSTRUCT)lParam)->hDC,GetStockObject(NULL_BRUSH));
// 	MoveToEx(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.left+4, rc.top, NULL);
// 	LineTo(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.right-4, rc.top);
// 	MoveToEx(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.left+4, rc.bottom-1, NULL);
// 	LineTo(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.right-4, rc.bottom-1);
// 	MoveToEx(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.left, rc.top+4, NULL);
// 	LineTo(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.left, rc.bottom-4);
// 	MoveToEx(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.right-1, rc.top+4, NULL);
// 	LineTo(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.right-1, rc.bottom-4);
// 	Rectangle(((LPDRAWITEMSTRUCT)lParam)->hDC, rc.left, rc.top, rc.right, rc.bottom);
//
//////////////////////////////////////////////////////////////////////////


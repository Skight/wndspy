#include "WndSpyGui.h"
#include "DlgWndModifier.h"


//////////////////////////////////////////////////////////////////////////

typedef struct tag_ParamsWrapper_WndMod
{
	HWND hwndFinder;
	HWND hwndMover;
	BOOL IsReady;
	BOOL IsHex;
	BOOL IsMover;
	BOOL IsDragging;
	BOOL IsSliding;
	BOOL IsChild;
	HWND hwndFocus;
	HICON hIconMoverTip;
	INT iScrCx;
	INT iScrCy;
	
}PARAM_WRAPPER_WNDMOD;

//////////////////////////////////////////////////////////////////////////

VOID OnModifyWndStyle(HWND hwnd, WPARAM wParam, HWND hwndTag);
VOID WndMod_OnPrintWndInfo(HWND hwnd, PARAM_WRAPPER_WNDMOD* pW);
VOID OnInitMetrics(HWND hwnd, PARAM_WRAPPER_WNDMOD* pW);
VOID DoAttachThreadInput(HWND hwndTagThread, BOOL IsAttach);

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_WndModifier(HWND hwnd, 
								  UINT message, 
								  WPARAM wParam, 
								  LPARAM lParam)
{
	static WND_SUBCLASS_DATA wsc_DragMover;
	static WND_SUBCLASS_DATA wsc_DragFinder;
	static HCURSOR hCurFinder=LoadCursor(g_hInstance, MAKEINTRESOURCE(IDI_CUR_DRAGFIND));
	static HCURSOR hCurMover=LoadCursor(g_hInstance, MAKEINTRESOURCE(IDI_CUR_DRAGMOVE));
	static PARAM_WRAPPER_WNDMOD* pW=NULL;
	RECT rc;
	POINT pt;
	HDWP hdwp;
	HWND hwndBuf;
	INT iBuf;
	TCHAR szBuf[TINY_STRBUF_LEN];
	
	switch (message)
	{
	case WM_INITDIALOG:
		{
			g_hwnd_WndModEr=hwnd;
			
			pW = (PARAM_WRAPPER_WNDMOD*)
				LocalAlloc(GPTR, sizeof(PARAM_WRAPPER_WNDMOD));
			if(pW==NULL)
			{
				ShowLastError(g_hwnd);
				DestroyWindow(hwnd);
			}

			if(!lpfSetLayeredWindowAttributes)
			{
				DoEnableDlgItems(hwnd, IDC_STATIC_ALPHA_1, IDC_SLIDER, 1, FALSE);
			}
			
			OnInitMetrics(hwnd, pW);
			SendDlgItemMessage(hwnd, IDC_SLIDER, TBM_SETRANGE, FALSE, MAKELONG(32, 255));
			
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_SBTN_HEXDEC, STN_CLICKED), 0);
			
			hwndBuf=CreateMyTooltipEx(hwnd, 
				TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX);
			
			AddControlTipEx(hwndBuf, hwnd, IDC_ICON_DRAG, 
				MAKEINTRESOURCE(IDS_TIP_DRAGFINDER), TTF_SUBCLASS);
			
			LoadString(g_hInstance, IDS_TIP_DRAGFINDERTITLE, szBuf, TINY_STRBUF_LEN);
			SendMessage(hwndBuf, TTM_SETTITLE, 
				(WPARAM)(g_siWinVer>=WINVER_WINXPNEWER?(INT)g_hiconFinder:1), (LPARAM)szBuf);
			
			hwndBuf=CreateMyTooltipEx(hwnd, 
				TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX);
			
			AddControlTipEx(hwndBuf, hwnd, IDC_ICON_MOVER, 
				MAKEINTRESOURCE(IDS_TIP_MOVER), TTF_SUBCLASS);
			
			pW->hIconMoverTip=DoSetIconToolTip(hwndBuf, 
				(g_siWinVer >= WINVER_WINXPNEWER), TEXT("SHELL32.DLL"), 146, 
				NULL, g_hInstance, IDS_TIP_MOVER_TITLE, szBuf, TINY_STRBUF_LEN);
			SendMessage(hwndBuf, TTM_SETDELAYTIME, TTDT_INITIAL, (LPARAM)500);
			SendMessage(hwndBuf, TTM_SETDELAYTIME, TTDT_RESHOW, (LPARAM)500);
			
			hwndBuf=CreateMyTooltipEx(hwnd, 
				TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX|TTS_BALLOON);
			AddControlTip(hwndBuf, hwnd, 
				IDC_SBTN_HEXDEC, MAKEINTRESOURCE(IDS_TIP_HEXDEC));
			AddControlTip(hwndBuf, hwnd, 
				IDC_SLIDER_X, MAKEINTRESOURCE(IDS_TIP_DRAG_RESIZE));
			AddControlTip(hwndBuf, hwnd, 
				IDC_SLIDER_Y, MAKEINTRESOURCE(IDS_TIP_DRAG_RESIZE));

			for(iBuf=IDC_BTN_SET_1; iBuf<=IDC_BTN_SET_3; iBuf+=IDC_BTN_SET_2-IDC_BTN_SET_1)
			{
				AddControlTip(hwndBuf, hwnd, 
					iBuf, MAKEINTRESOURCE(IDS_STYLE_SET));
			}
			for(iBuf=IDC_BTN_UNSET_1; iBuf<=IDC_BTN_UNSET_3; iBuf+=IDC_BTN_UNSET_2-IDC_BTN_UNSET_1)
			{
				AddControlTip(hwndBuf, hwnd, 
					iBuf, MAKEINTRESOURCE(IDS_STYLE_UNSET));
			}
			for(iBuf=IDC_EDIT_1; iBuf<=IDC_EDIT_3; iBuf+=IDC_EDIT_2-IDC_EDIT_1)
			{
				AddControlTip(hwndBuf, hwnd, 
					iBuf, MAKEINTRESOURCE(IDS_STYLE_INPUT));
			}
			
			pW->hwndFinder=GetDlgItem(hwnd, IDC_ICON_DRAG);
			InitPickPanCtrl(pW->hwndFinder, &wsc_DragFinder, hCurFinder);
			
			pW->hwndMover=GetDlgItem(hwnd, IDC_ICON_MOVER);
			InitPickPanCtrl(pW->hwndMover, &wsc_DragMover, hCurMover);
			

			SetDlgItemSubClass(hwnd, IDC_CHK_OPAQUE, SCP_ChkBtn_NoFoucsRect);
			SetDlgItemSubClass(hwnd, IDC_SBTN_HEXDEC, SCP_Static_HandCusror);	
			SetDlgItemSubClass(hwnd, IDC_EDIT_HWND, SCP_HwndFileEdit);	
			
			DoSetMainIconAndCenterWnd(hwnd);
			SetFocus(pW->hwndFinder);
			
			return FALSE;
		}
		
	case WM_DISPLAYCHANGE:
		{
			OnInitMetrics(hwnd, pW);
			return FALSE;
		}
		
	case WM_CTLCOLORSTATIC: 
		{
			iBuf=GetDlgCtrlID((HWND)lParam);
			switch(iBuf)
			{
			case IDC_SBTN_HEXDEC:
			case IDC_STATIC_MOVE:
			case IDC_STATIC_SIZE:
			case IDC_STATIC_ALPHA_1:
				{
					HBRUSH hBrush = GetSysColorBrush(COLOR_BTNFACE);
					SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNFACE));
					//SetBkMode((HDC)wParam, TRANSPARENT);
					SetTextColor((HDC)wParam, (COLORREF)
						GetSysColor(iBuf==IDC_SBTN_HEXDEC?COLOR_HIGHLIGHT:COLOR_ACTIVECAPTION));
					return (LONG)hBrush;
				}
			}
			return FALSE;
		}
		
	case WM_CAPTURECHANGED:
		{
			if(pW->IsDragging)
			{
				SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
			}
			return FALSE;
		}
		
	case WM_TIMER:
		{
			if( wParam==TIMER_MOUSEMOVE )
			{
				if( !GetAsyncKeyState(VK_LBUTTON) || !IsWindow(pW->hwndFocus) )
				{
					KillTimer(hwnd, TIMER_MOUSEMOVE);
					SendMessage(hwnd, WM_LBUTTONUP, 0, 0);
				}
				else if(pW->IsDragging && pW->IsMover )
				{
					GetCursorPos(&pt);
					DlgItemPrintf(hwnd, IDC_STATIC_POS, TEXT("%d-%d"), pt.x, pt.y);
					
					hwndBuf=GetAncestor(pW->hwndFocus, GA_PARENT);
					ScreenToClient(hwndBuf, &pt);
					
					hdwp=BeginDeferWindowPos(1);
					
					hdwp=DeferWindowPos(hdwp, pW->hwndFocus, NULL, 
						pt.x, pt.y, 0, 0, 
						SWP_NOSIZE|SWP_NOZORDER|
						(pW->IsChild?SWP_NOCOPYBITS:NULL));
					
					EndDeferWindowPos(hdwp);
					
				}
				return TRUE;
			}
			else if(wParam==TIMER_CHECK_EXIST)
			{
				if( !IsWindow(pW->hwndFocus) )
				{
					KillTimer(hwnd, wParam);
					SetDlgItemText(hwnd, IDC_EDIT_HWND, TEXT(""));
				}
				return TRUE;
			}
			break;
		}
		
	case WM_MOUSEMOVE:
		{
			if(pW->IsDragging && IS_FLAG_SETTED(wParam,MK_LBUTTON) )
			{
				pt.x=(int)(short)LOWORD(lParam);
				pt.y=(int)(short)HIWORD(lParam);
				
				if(!pW->IsMover)
				{
					ClientToScreen(hwnd, &pt);
					hwndBuf = FindRealWindowFromPointEx(NULL, pt, FINDWINDOW_OPTION);
					
					if(hwndBuf!=pW->hwndFocus)
					{
						RemoveWindowFocus(pW->hwndFocus, g_option.wFlag, g_option.color);
						
						if(	IsWindowInCurrentThreadProcess(hwndBuf, TRUE) )
						{
							pW->hwndFocus=NULL;
							SetDlgItemText(hwnd, IDC_EDIT_HWND, TEXT(""));
							break;
						}
						else
						{
							pW->hwndFocus=hwndBuf;
							DrawWindowFocus(pW->hwndFocus, g_option.wFlag, g_option.color);
							DlgItemPrintf(hwnd, IDC_EDIT_HWND, 
								(pW->IsHex?g_szFormat:TEXT("%d")), pW->hwndFocus);
						}
						
					}
				}
			}
			return FALSE;
		}
		
	case WM_LBUTTONUP:
		{
			if(pW->IsDragging)
			{
				pW->IsDragging=FALSE;
				DoPlayEventSound(g_option.IsPlaySound);
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				ReleaseCapture();
				if(pW->IsMover)
				{
					GetWindowRect(GetDlgItem(hwnd, IDC_ICON_MOVER), &rc);
					SetCursorPos(rc.left+12, rc.top+12);
					BringWindowToTop(hwnd);
					DoAttachThreadInput(pW->hwndFocus, FALSE);
					hwndBuf=GetDlgItem(hwnd, IDC_ICON_MOVER);
					PickPan_SetCursor(hwndBuf, hCurMover);
				}
				else
				{
					RemoveWindowFocus(pW->hwndFocus, g_option.wFlag, g_option.color);
					hwndBuf=GetDlgItem(hwnd, IDC_ICON_DRAG);
					PickPan_SetCursor(hwndBuf, hCurFinder);
				}
				
			}
			return FALSE;
		}
		
	case WM_HSCROLL:
		{
			if(!pW->IsReady)
			{
				break;
			}
			// if( (LOWORD(wParam) == SB_THUMBTRACK || LOWORD(wParam) == TB_ENDTRACK) )
			// {{{
			INT iPos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
			iBuf = GetDlgCtrlID((HWND)lParam);
			
			if( iBuf==IDC_SLIDER )
			{
				DoSetWindowTrans(pW->hwndFocus, (BYTE)iPos);
				SetDlgItemInt(hwnd, IDC_EDIT_ALPHA, min(iPos, NO_TRANS), FALSE);
			}
			else if( iBuf==IDC_SLIDER_X || iBuf==IDC_SLIDER_Y )
			{
				GetWindowRect(pW->hwndFocus, &rc);
				
				if(LOWORD(wParam)==TB_ENDTRACK)
				{
					if(pW->IsSliding)
					{
						pW->IsSliding=FALSE;
						DoAttachThreadInput(pW->hwndFocus, FALSE);
					}
				}
				else if(LOWORD(wParam)==SB_THUMBTRACK)
				{
					if(!pW->IsSliding)
					{
						pW->IsSliding=TRUE;
						DoAttachThreadInput(pW->hwndFocus, TRUE);
					}
				}
				
				if(iBuf==IDC_SLIDER_X)
				{
					pt.x=iPos;
					pt.y=rc.bottom-rc.top;
					SetDlgItemInt(hwnd, IDC_EDIT_X, pt.x, FALSE);
				}
				else
				{
					pt.x=rc.right-rc.left;
					pt.y=iPos;
					SetDlgItemInt(hwnd, IDC_EDIT_Y, pt.y, FALSE);
				}
				
				hdwp=BeginDeferWindowPos(1);
				
				hdwp=DeferWindowPos(hdwp, pW->hwndFocus, NULL, 
					0, 0, pt.x, pt.y,  
					SWP_NOMOVE|SWP_NOZORDER|
					(pW->IsChild?SWP_NOCOPYBITS:SWP_NOACTIVATE));
				
				EndDeferWindowPos(hdwp);
				
			}
			//}}}
			return FALSE;
		}
		
	case WM_COMMAND:
		{
			switch ( LOWORD(wParam) )
			{
			case IDC_ICON_MOVER:
			case IDC_ICON_DRAG:
				{
					if( STN_CLICKED == HIWORD(wParam) )
					{
						SetFocus((HWND)lParam);
						if(LOWORD(wParam)==IDC_ICON_MOVER)
						{
							if( IsValidWindowTarget(pW->hwndFocus) && 
								!IsIconic(GetAncestor(pW->hwndFocus, GA_ROOT)) )
							{
								pW->IsMover=TRUE;
								
								DoAttachThreadInput(pW->hwndFocus, TRUE);
								
								SetCursor(hCurMover);
								GetWindowRect(pW->hwndFocus, &rc);
								SetCursorPos(rc.left, rc.top);
								SetTimer(hwnd, TIMER_MOUSEMOVE, USER_TIMER_MINIMUM, NULL);
							}
							else
							{
								break;
							}
						}
						else
						{
							pW->hwndFocus=NULL;
							SetCursor(hCurFinder);
							pW->IsMover=FALSE;
							
						}
						
						DoPlayEventSound(g_option.IsPlaySound);
						PickPan_SetCursor((HWND)lParam, NULL);
						SetCapture(hwnd);
						pW->IsDragging=TRUE;
					}					
					break;
				}
				
			case IDC_CHK_OPAQUE:
				{
					iBuf=IsDlgButtonChecked(hwnd, IDC_CHK_OPAQUE);
					if( lParam && 
						IsValidWindowTarget(pW->hwndFocus) && !pW->IsChild )
					{
						
						SetWindowEnable(hwnd, IDC_CHK_OPAQUE, TRUE);
						DoEnableDlgItems(hwnd, IDC_EDIT_ALPHA, IDC_SLIDER, 1, 
							!(BOOL)iBuf);
					}
					else
					{
						DoEnableDlgItems(hwnd, IDC_CHK_OPAQUE, IDC_SLIDER, 1, FALSE);
					}
					if(iBuf)
					{
						SendDlgItemMessage(hwnd, IDC_SLIDER, TBM_SETPOS, TRUE, NO_TRANS);
						SendMessage(hwnd, WM_HSCROLL, 
							MAKEWPARAM(SB_THUMBTRACK, NO_TRANS), 
							(LPARAM)GetDlgItem(hwnd, IDC_SLIDER));
					}
					break;
				}

			case IDC_SBTN_HEXDEC:
				{
					if( STN_CLICKED == HIWORD(wParam) ||
						STN_DBLCLK ==HIWORD(wParam) )
					{
						pW->IsHex=!pW->IsHex; //reverse first
						SetWindowTextFromStrRes(hwnd, IDC_SBTN_HEXDEC, 
							g_hInstance, pW->IsHex?IDS_HEX:IDS_DEC);
						if( pW->hwndFocus )
						{
							DlgItemPrintf(hwnd, IDC_EDIT_HWND, 
								(pW->IsHex?g_szFormat:TEXT("%d")), pW->hwndFocus);
						}
					}
					break;
				}
				
			case IDC_EDIT_HWND:
				{
					if( HIWORD(wParam)==EN_UPDATE )
					{
						pW->hwndFocus=(HWND)GetWindowInteger((HWND)lParam, pW->IsHex);
						
						WndMod_OnPrintWndInfo(hwnd, pW);
						
					}
					break;
				}

			case IDC_EDIT_1:
			case IDC_EDIT_2:
			case IDC_EDIT_3:
				{
					if( HIWORD(wParam)==EN_UPDATE ||
						HIWORD(wParam)==EN_SETFOCUS )
					{
						iBuf=(INT)
							(IsValidWindowTarget(pW->hwndFocus) && 
							IsWndTextHexStr((HWND)lParam));
						
						SetWindowEnable(hwnd, LOWORD(wParam)+1, iBuf);
						SetWindowEnable(hwnd, LOWORD(wParam)+2, iBuf);
					}
					break;
				}

			case IDC_BTN_SET_1:
			case IDC_BTN_SET_2:
			case IDC_BTN_UNSET_1:
			case IDC_BTN_UNSET_2:
			case IDC_BTN_SET_3:
			case IDC_BTN_UNSET_3:
				{
					OnModifyWndStyle(hwnd, wParam, pW->hwndFocus);
					return TRUE;
				}
				
			}
			
			return FALSE;
		} //WM_COMMAND end...

#ifdef _EDIT_COLOR		 //{{{
	case WM_CTLCOLOREDIT: 
		{
			iBuf=GetDlgCtrlID((HWND)lParam);
			switch(iBuf)
			{
			case IDC_EDIT_HWND:
			case IDC_EDIT_1:
			case IDC_EDIT_2:
			case IDC_EDIT_3:
				{
					HBRUSH hBrush = GetSysColorBrush(COLOR_INFOBK);
					SetBkColor((HDC)wParam, GetSysColor(COLOR_INFOBK));
					SetTextColor((HDC)wParam, (COLORREF)COLOR_INFOTEXT);
					return (LONG)hBrush;
				}
			}
			return FALSE;
		}
#endif //_EDIT_COLOR	//}}}

	case WM_SYSCOLORCHANGE:
		{
			SendDlgItemMessage(hwnd, IDC_SLIDER_X, WM_SYSCOLORCHANGE, 0, 0);
			SendDlgItemMessage(hwnd, IDC_SLIDER_Y, WM_SYSCOLORCHANGE, 0, 0);
			SendDlgItemMessage(hwnd, IDC_SLIDER, WM_SYSCOLORCHANGE, 0, 0);
			return FALSE;
		}

	case WM_MY_GETHWNDTARGET:
		{
			*(HWND*)lParam=pW->hwndFocus;
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
				DestroyIcon(pW->hIconMoverTip);
				LocalFree((HLOCAL)pW);
			}
			g_hwnd_WndModEr=NULL;
			PostMessage(g_hwndTC, WM_MY_TOOL_CLOSE, 0, 0);
			return FALSE;
		}
		
	}//switch (message) end...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);
	
} //DlgProc_WndModifier()

//////////////////////////////////////////////////////////////////////////

typedef BOOL (WINAPI *LPFGETLAYEREDWINDOWATTRIBUTES)(HWND, COLORREF*, BYTE*, DWORD*);


VOID WndMod_OnClearWndInfo(HWND hwnd)
{
	KillTimer(hwnd, TIMER_CHECK_EXIST);
	SetDlgItemText(hwnd, IDC_STATIC_POS, TEXT(""));
	SetDlgItemText(hwnd, IDC_EDIT_X, TEXT(""));
	SetDlgItemText(hwnd, IDC_EDIT_Y, TEXT(""));
	SendDlgItemMessage(hwnd, IDC_SLIDER_X, TBM_SETPOS, TRUE, 0);
	SendDlgItemMessage(hwnd, IDC_SLIDER_Y, TBM_SETPOS, TRUE, 0);
	SetDlgItemText(hwnd, IDC_EDIT_ALPHA, TEXT(""));
	SendDlgItemMessage(hwnd, IDC_SLIDER, TBM_SETPOS, TRUE, 0);
	//SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CHK_OPAQUE, 0), 0);
}

VOID WndMod_OnPrintWndInfo(HWND hwnd, PARAM_WRAPPER_WNDMOD* pW)
{
	
	RECT rc;
	HWND hwndBuf;
	
	if( IsValidWindowTarget(pW->hwndFocus) )
	{
		SetTimer(hwnd, TIMER_CHECK_EXIST, 1000, NULL);
		pW->IsReady=TRUE;
		pW->IsChild=(BOOL)
			(GetWindowLong(pW->hwndFocus, GWL_STYLE)&WS_CHILD);
		
		GetWindowRect(pW->hwndFocus, &rc);
		DlgItemPrintf(hwnd, IDC_EDIT_X, TEXT("%d"), rc.right-rc.left);
		DlgItemPrintf(hwnd, IDC_EDIT_Y, TEXT("%d"), rc.bottom-rc.top);
		SendDlgItemMessage(hwnd, IDC_SLIDER_X, TBM_SETPOS, TRUE, rc.right-rc.left);
		SendDlgItemMessage(hwnd, IDC_SLIDER_Y, TBM_SETPOS, TRUE, rc.bottom-rc.top);
		
		DlgItemPrintf(hwnd, IDC_STATIC_POS, TEXT("%d-%d"), rc.left, rc.top);
		
		if( lpfSetLayeredWindowAttributes )
		{
			//COLORREF clr=NULL;
			BOOL bSuccess=TRUE;
			DWORD dwFlag=0;
			BYTE byAlpha=NO_TRANS;
			
			static LPFGETLAYEREDWINDOWATTRIBUTES 
				lpGetLayeredWindowAttributes=(LPFGETLAYEREDWINDOWATTRIBUTES)
				GetProcAddress(g_hInst.user32_dll, "GetLayeredWindowAttributes");
			
			if( lpGetLayeredWindowAttributes )
			{
				hwndBuf=GetAncestor(pW->hwndFocus, GA_ROOT);
				
				if( GetWindowLong(hwndBuf, GWL_EXSTYLE)&WS_EX_LAYERED )
				{
					bSuccess=lpGetLayeredWindowAttributes(hwndBuf, NULL, &byAlpha, &dwFlag);
					
				}
			}
			if( bSuccess )
			{
				SendDlgItemMessage(hwnd, IDC_SLIDER, TBM_SETPOS, TRUE, byAlpha);
				SendMessage(hwnd, WM_HSCROLL, 
					MAKEWPARAM(TB_ENDTRACK, 0), (LPARAM)GetDlgItem(hwnd, IDC_SLIDER));
				
				CheckDlgButton(hwnd, IDC_CHK_OPAQUE, (byAlpha<NO_TRANS)?BST_UNCHECKED:BST_CHECKED);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CHK_OPAQUE, 0), dwFlag==LWA_COLORKEY? 0:1);
			}
			else
			{
				SetDlgItemText(hwnd, IDC_EDIT_ALPHA, TEXT(""));
				CheckDlgButton(hwnd, IDC_CHK_OPAQUE, BST_UNCHECKED);
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CHK_OPAQUE, 0), 1);
			}
			
		}
	}
	else
	{
		pW->IsReady=FALSE;
		WndMod_OnClearWndInfo(hwnd);
	}
	
}

//////////////////////////////////////////////////////////////////////////

BOOL IsValidWindowTarget(HWND hwndTag)
{
	return (IsWindow(hwndTag) && 
		!IsWindowInCurrentThreadProcess(hwndTag, TRUE));
}


VOID OnInitMetrics(HWND hwnd, PARAM_WRAPPER_WNDMOD* pW)
{
	pW->iScrCx=GetSystemMetrics(SM_CXSCREEN);
	pW->iScrCy=GetSystemMetrics(SM_CYSCREEN);
	
	SendDlgItemMessage(hwnd, IDC_SLIDER_X, TBM_SETRANGE, TRUE, MAKELONG(0, pW->iScrCx));
	SendDlgItemMessage(hwnd, IDC_SLIDER_Y, TBM_SETRANGE, TRUE, MAKELONG(0, pW->iScrCy));
}


VOID DoAttachThreadInput(HWND hwndTagThread, BOOL IsAttach)
{
	if( IsValidWindowTarget(hwndTagThread) )
	{
		AttachThreadInput(GetCurrentThreadId(), 
			GetWindowThreadProcessId(hwndTagThread, NULL), IsAttach);
	}
}


VOID OnModifyWndStyle(HWND hwnd, WPARAM wParam, HWND hwndTag)
{
	
	RECT rc;
	INT idCtrl=LOWORD(wParam);
	UINT uSetSytleMsg=0;
	UINT uGetSytleMsg=0;
	INT iBuf;
	DWORD dwBuf;
	
	
	TCHAR szBuf[TINY_STRBUF_LEN];
	TCHAR szFormat[MINI_STRBUF_LEN];
	
	
	if(idCtrl==IDC_BTN_SET_1 || idCtrl==IDC_BTN_SET_2 || idCtrl==IDC_BTN_SET_3)
	{
		dwBuf=GetDlgItemHex(hwnd, idCtrl-1);
	}
	else
	{
		dwBuf=GetDlgItemHex(hwnd, idCtrl-2);
	}
	
	if(dwBuf<=0 || !IsWindow(hwndTag) )
	{
		SetWindowEnable(hwnd, idCtrl, FALSE);
		return;
	}
	
	if( idCtrl==IDC_BTN_SET_3 || idCtrl==IDC_BTN_UNSET_3 )
	{
		
		GetClassName(hwndTag, szBuf, TINY_STRBUF_LEN);
		iBuf = (INT)GetKnownClassID(szBuf);
		if(iBuf==KWC_COMBOEX)
		{
			uGetSytleMsg=CBEM_GETEXTENDEDSTYLE,
				uSetSytleMsg=CBEM_SETEXTENDEDSTYLE;
		}
		else if(iBuf==KWC_LISTVIEW)
		{
			uGetSytleMsg=LVM_GETEXTENDEDLISTVIEWSTYLE,
				uSetSytleMsg=LVM_SETEXTENDEDLISTVIEWSTYLE;
		}
		else if( iBuf==KWC_TAB )
		{
			uGetSytleMsg=TCM_GETEXTENDEDSTYLE,
				uSetSytleMsg=TCM_SETEXTENDEDSTYLE;
		}
		else if( iBuf==KWC_TOOLBAR )
		{
			uGetSytleMsg=TB_GETEXTENDEDSTYLE,
				uSetSytleMsg=TB_SETEXTENDEDSTYLE;
		}
		else
		{
			MessageBoxFromResStr(g_hInstance, hwnd, IDS_NOT_SUPPORT, IDS_NOTE, MB_OK);
			return;
		}
		
		SendMessageTimeout(hwndTag, uGetSytleMsg, 0, 0, SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&iBuf);
	}
	
	if( LoadString(g_hInstance, IDS_STYLE_MOD_CONFIRM, szFormat, MINI_STRBUF_LEN) )
	{
		GetDlgItemText(hwnd, idCtrl, szBuf, TINY_STRBUF_LEN);
		if(IDNO==MessageBoxPrintf(hwnd, MB_YESNO, szBuf, szFormat, szBuf, dwBuf))
		{
			return;
		}
	}
	
	
	switch(idCtrl)
	{
	case IDC_BTN_SET_1:
		{
			SetWindowLong(hwndTag, GWL_STYLE, GetWindowLong(hwndTag, GWL_STYLE)|dwBuf);
			break;
		}
	case IDC_BTN_UNSET_1:
		{
			SetWindowLong(hwndTag, GWL_STYLE, GetWindowLong(hwndTag, GWL_STYLE)&~dwBuf);
			break;
		}
	case IDC_BTN_SET_2:
		{
			SetWindowLong(hwndTag, GWL_EXSTYLE, GetWindowLong(hwndTag, GWL_EXSTYLE)|dwBuf);
			break;
		}
	case IDC_BTN_UNSET_2:
		{
			SetWindowLong(hwndTag, GWL_EXSTYLE, GetWindowLong(hwndTag, GWL_EXSTYLE)&~dwBuf);
			break;
		}
	case IDC_BTN_SET_3:
		{
			SendMessageTimeout(hwndTag, uSetSytleMsg, 0, (LPARAM)(((DWORD)iBuf)|dwBuf), 
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, NULL);
			break;
		}
	case IDC_BTN_UNSET_3:
		{
			SendMessageTimeout(hwndTag, uSetSytleMsg, 0, (LPARAM)(((DWORD)iBuf)&~dwBuf),
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, NULL);
			break;
		}
	}
	
	//refresh the target window...
	
	SetWindowPos(hwndTag, NULL, 0, 0, 0, 0, 
		SWP_NOMOVE|SWP_NOSIZE|SWP_NOCOPYBITS|SWP_DRAWFRAME|SWP_NOACTIVATE);
	
	GetClientRect(hwndTag, &rc);
	RedrawWindow(GetAncestor(hwndTag, GA_ROOT), &rc, NULL, 
		RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN);
	
}

//////////////////////////////////////////////////////////////////////////

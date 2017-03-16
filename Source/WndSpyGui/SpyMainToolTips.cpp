#include "WndSpyGui.h"
#include "SpyMainToolTips.h"

//////////////////////////////////////////////////////////////////////////
HICON DoSetIconToolTip(HWND hwndTT, BOOL isIcon, 
					   LPTSTR szIconModule, INT iIconIndex, LPTSTR szTitle,
					   HINSTANCE hInstStrRes, INT iStrId, LPTSTR lpszStrBuf, INT iMaxBufLen)
{
	HICON hIconTitle=NULL;
	HICON hIcon;
	WPARAM wParamIcon;

	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, (LPARAM)20000);
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, (LPARAM)1000);
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_RESHOW, (LPARAM)1000);
	
	if(	isIcon )
	{
		ExtractIconEx(szIconModule, iIconIndex, NULL, &hIcon,1);
		wParamIcon=(LONG)CopyImage(hIcon, IMAGE_ICON, 16, 16, NULL);
		hIconTitle=(HICON)wParamIcon;
		DestroyIcon(hIcon);
	}
	else
	{
		wParamIcon=1;
	}
	if(szTitle)
	{
		SendMessage(hwndTT, TTM_SETTITLE, (WPARAM)wParamIcon, (LPARAM)szTitle);
	}
	else
	{
		LoadString(hInstStrRes, iStrId, lpszStrBuf, iMaxBufLen);
		SendMessage(hwndTT, TTM_SETTITLE, (WPARAM)wParamIcon, (LPARAM)lpszStrBuf);
	}
	return hIconTitle;

} //DoSetIconToolTip()
//////////////////////////////////////////////////////////////////////////

VOID DoCreateIconTooltip(VOID)
{

	BOOL isIconTitle = (g_siWinVer>=WINVER_WINXPNEWER);
	
	int i;
	for(i=0; i<C_MAIN_TAB_PAGES; i++)
	{
		g_hwndTT[i]=CreateMyTooltip(g_TabDlgHdr.CDI[i].hwnd);	
		SendMessage(g_hwndTT[i], TTM_SETMAXTIPWIDTH, 0, (LPARAM)400);
		SendMessage(g_hwndTT[i], TTM_SETDELAYTIME, TTDT_RESHOW, 10000);
	}
	
	for(i=IDC_BTN_OWNER; i<=IDC_BTN_PRE;i++)
	{
		AddControlTip(g_hwndTT[0], 
			g_TabDlgHdr.CDI[0].hwnd, i, 
			MAKEINTRESOURCE(IDS_TIP_SPYIT));
	}
	
	AddControlTip(g_hwndTT[0], g_TabDlgHdr.CDI[0].hwnd, IDC_EDIT_CAPTION, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[0], g_TabDlgHdr.CDI[0].hwnd, IDC_EDIT_CLASS, 
		LPSTR_TEXTCALLBACK);

	AddControlTip(g_hwndTT[1], g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_CAPTION, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[1], g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_CLASS, 
		LPSTR_TEXTCALLBACK);	
	AddControlTip(g_hwndTT[1], g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_WNDRECT, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[1], g_TabDlgHdr.CDI[1].hwnd, IDC_EDIT_CLIENTRECT, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[1], g_TabDlgHdr.CDI[1].hwnd, IDC_ICONBTN_BIG, 
		MAKEINTRESOURCE(IDS_TIP_CLICK_SAVEICON));
	AddControlTip(g_hwndTT[1], g_TabDlgHdr.CDI[1].hwnd, IDC_ICONBTN_SMALL, 
		MAKEINTRESOURCE(IDS_TIP_CLICK_SAVEICON));
	
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_SICON_BIG, 
		MAKEINTRESOURCE(IDS_TIP_CLICK_SAVEICON));
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_SICON_SMALL, 
		MAKEINTRESOURCE(IDS_TIP_CLICK_SAVEICON));
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_CLASS, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HICON, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_SICON_CURSOR, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3],g_TabDlgHdr.CDI[3].hwnd,IDC_EDIT_HCURSOR, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_HBKBRUSH, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_MENUNAME, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_EDIT_MODULEFILE, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_BTN_LOCATE, 
		MAKEINTRESOURCE(IDS_TIP_LOCATE));
#ifdef WS_TIP_CLASSSTYLE
	AddControlTip(g_hwndTT[3], g_TabDlgHdr.CDI[3].hwnd, IDC_CBDL_CS, 
		LPSTR_TEXTCALLBACK);
#endif

	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_BTN_LOCATE1, 
		MAKEINTRESOURCE(IDS_TIP_LOCATE));
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_BTN_LOCATE2, 
		MAKEINTRESOURCE(IDS_TIP_LOCATE));	
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_PID, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_TID, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_CURDIR, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_CMDLINE, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_IMAGEFILE, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[4], g_TabDlgHdr.CDI[4].hwnd, IDC_EDIT_WNDMODULE, 
		LPSTR_TEXTCALLBACK);

	AddControlTip(g_hwndTT[5], g_TabDlgHdr.CDI[5].hwnd, IDC_STATIC_WPARAM, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[5], g_TabDlgHdr.CDI[5].hwnd, IDC_STATIC_LPARAM, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[5], g_TabDlgHdr.CDI[5].hwnd, IDC_EDIT_WPARAM, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[5], g_TabDlgHdr.CDI[5].hwnd, IDC_EDIT_LPARAM, 
		LPSTR_TEXTCALLBACK);
	AddControlTip(g_hwndTT[5], g_TabDlgHdr.CDI[5].hwnd, IDC_SBTN_HEXDEC, 
		MAKEINTRESOURCE(IDS_TIP_HEXDEC));

	
	g_hwndTTFinder=CreateMyTooltipEx(g_TabDlgHdr.CDI[0].hwnd, 
		TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX);
	
	AddControlTipEx(g_hwndTTFinder, 
		g_TabDlgHdr.CDI[0].hwnd, IDC_ICON_DRAG, 
		MAKEINTRESOURCE(IDS_TIP_DRAGFINDER), TTF_SUBCLASS);
	
	g_hiconFinder=DoSetIconToolTip(g_hwndTTFinder, 
		isIconTitle, TEXT("SHELL32.DLL"), 22, 
		NULL, g_hInstance, IDS_TIP_DRAGFINDERTITLE, g_strBuf, MAX_STRBUF_LEN);
	
	
	g_hwndTTCalc=CreateMyTooltipEx(g_TabDlgHdr.CDI[5].hwnd, 
		TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX);
	
	AddControlTipEx(g_hwndTTCalc,g_TabDlgHdr.CDI[5].hwnd,IDC_ICONBTN_CALC,
		MAKEINTRESOURCE(IDS_TIP_CALC),TTF_SUBCLASS);
	
	g_hiconCalc=DoSetIconToolTip(g_hwndTTCalc, 
		isIconTitle, TEXT("calc.exe"), 0, 
		NULL, g_hInstance, IDS_TIP_CALCTITLE, g_strBuf, MAX_STRBUF_LEN);
	
	
	g_hwndTTGetIcon=CreateMyTooltipEx(g_TabDlgHdr.CDI[1].hwnd, 
		TTS_ALWAYSTIP|TTS_NOANIMATE|TTS_NOFADE|TTS_NOPREFIX);
	
	AddControlTipEx(g_hwndTTGetIcon,g_TabDlgHdr.CDI[1].hwnd,IDC_ICONBTN_MORE,
		MAKEINTRESOURCE(IDS_TIP_ICONEX),TTF_SUBCLASS);
	
	g_hiconGetIcon=DoSetIconToolTip(g_hwndTTGetIcon, 
		isIconTitle, TEXT("SHELL32.DLL"), 205, 
		NULL, g_hInstance, IDS_TIP_ICONEXTITLE, g_strBuf, MAX_STRBUF_LEN);
	
#ifdef WNDSPY_MAINDLGTOOLTIP
	// APP_FUNC_ONSETTOOLTIPCOLOR
	// SendMessage(g_hwndBK, WM_MY_COLORSET, 0, 0);
	g_hwndTTMain=CreateMyTooltip(g_hwnd);
	
	AddControlTip (g_hwndTTMain,g_hwnd,IDC_BTN_EXIT, 
		TEXT("Close Window Spy"));
	AddControlTip (g_hwndTTMain,g_hwnd,IDC_CHK_TOPMOST, 
		TEXT("Make Window Spy Topmost (on/off)"));
	AddControlTip (g_hwndTTMain,g_hwnd,IDC_CHK_AUTOHIDE, 
		TEXT("Hide Window Spy While Dragging Finder Cursor (on/off)"));
	
#endif	
	
}//DoCreateIconTooltip()

//////////////////////////////////////////////////////////////////////////

void DoDestroyIconTooltip(VOID)
{

#ifdef WNDSPY_MAINDLGTOOLTIP
	DestroyWindow(g_hwndTTMain);
#endif

	if(g_siWinVer >= WINVER_WINXPNEWER)
	{
		DestroyIcon(g_hiconFinder);
		DestroyIcon(g_hiconCalc);
		DestroyIcon(g_hiconGetIcon);
	}

}//DoDestroyIconTooltip()

//////////////////////////////////////////////////////////////////////////

//#define APP_FUNC_ONSETTOOLTIPCOLOR
#ifdef APP_FUNC_ONSETTOOLTIPCOLOR
void OnSetTooltipColor(void)
{
	#ifdef APP_FUNC_ONSETTOOLTIPCOLOR_ALL
	int i;
	for(i=0; i<C_MAIN_TAB_PAGES; i++)
	{
	SendMessage(g_hwndTT[i], TTM_SETTIPTEXTCOLOR, (WPARAM)GetSysColor(COLOR_BTNTEXT), 0);
  	SendMessage(g_hwndTT[i], TTM_SETTIPBKCOLOR, GetSysColor(COLOR_BTNFACE), 0);
	}
#ifdef WNDSPY_MAINDLGTOOLTIP
	SendMessage(g_hwndTTMain, TTM_SETTIPTEXTCOLOR, (WPARAM)GetSysColor(COLOR_BTNTEXT), 0);
 	SendMessage(g_hwndTTMain, TTM_SETTIPBKCOLOR, GetSysColor(COLOR_BTNFACE), 0);
#endif
	SendMessage(g_hwndTTCalc, TTM_SETTIPTEXTCOLOR, (WPARAM)GetSysColor(COLOR_BTNTEXT), 0);
  	SendMessage(g_hwndTTCalc, TTM_SETTIPBKCOLOR, GetSysColor(COLOR_BTNFACE), 0);
	SendMessage(g_hwndTTFinder, TTM_SETTIPTEXTCOLOR, (WPARAM)GetSysColor(COLOR_BTNTEXT), 0);
  	SendMessage(g_hwndTTFinder, TTM_SETTIPBKCOLOR, GetSysColor(COLOR_BTNFACE), 0);
	#endif

	SendMessage(g_hwndTTGetIcon, TTM_SETTIPTEXTCOLOR, (WPARAM)GetSysColor(COLOR_MENUTEXT), 0);
  	SendMessage(g_hwndTTGetIcon, TTM_SETTIPBKCOLOR, GetSysColor(COLOR_MENU), 0);

}//OnSetTooltipColor()
#endif

//////////////////////////////////////////////////////////////////////////
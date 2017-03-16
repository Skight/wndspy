#include <windows.h>
#include <commctrl.h>
#include "HsLibDef.h"
#include "ModResMem.h"
#include "UxThemeFuncs.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _HS_GETTHEMEPARTSIZE
HSLIB_LPFGETTHEMEPARTSIZE hslib_lpfGetThemePartSize=NULL;
#endif

HSLIB_LPFOPENTHEMEDATA hslib_lpfOpenThemeData=NULL;
HSLIB_LPFCLOSETHEMEDATA hslib_lpfCloseThemeData=NULL;
HSLIB_LPFDRAWTHEMEBACKGROUND hslib_lpfDrawThemeBackground=NULL;
HSLIB_LPENABLETHEMEDIALOGTEXTURE hslib_lpfEnableThemeDialogTexture=NULL;
//////////////////////////////////////////////////////////////////////////


BOOL OnInit_UxThemeBaseDrawFuncs(HINSTANCE h_uxtheme_dll)
{
	if( NULL==h_uxtheme_dll )
	{
		return FALSE;
	}
	if(IsThemeFuncsLoaded)
	{
		return TRUE;
	}
	hslib_lpfOpenThemeData = (HSLIB_LPFOPENTHEMEDATA)
		GetProcAddress(h_uxtheme_dll, "OpenThemeData");
	if(!hslib_lpfOpenThemeData)
	{
		DEBUG_ERROR_MSG(TEXT("hslib_lpfOpenThemeData"))
		return FALSE;
	}
	hslib_lpfCloseThemeData = (HSLIB_LPFCLOSETHEMEDATA)
		GetProcAddress(h_uxtheme_dll, "CloseThemeData");
	if(!hslib_lpfCloseThemeData)
	{
		DEBUG_ERROR_MSG(TEXT("hslib_lpfCloseThemeData"))
		return FALSE;
	}

	hslib_lpfDrawThemeBackground = (HSLIB_LPFDRAWTHEMEBACKGROUND)
		GetProcAddress(h_uxtheme_dll, "DrawThemeBackground");
	if(!hslib_lpfDrawThemeBackground)
	{
		DEBUG_ERROR_MSG(TEXT("hslib_lpfDrawThemeBackground"))
		return FALSE;
	}

#ifdef _HS_GETTHEMEPARTSIZE
	hslib_lpfGetThemePartSize = (HSLIB_LPFGETTHEMEPARTSIZE)
		GetProcAddress(h_uxtheme_dll, "GetThemePartSize");
	if(!hslib_lpfGetThemePartSize)
	{
		DEBUG_ERROR_MSG(TEXT("hslib_lpfGetThemePartSize"))
		return FALSE;
	}
#endif

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
VOID DrawSizeGripBox_OnPaint(HWND hwnd, HDC hdc)
{

	RECT rc;
	HTHEME hTheme=NULL;

	GetClientRect(hwnd, &rc);

#ifndef _HS_GETTHEMEPARTSIZE //fixed...

		rc.left = rc.right- _HS_GRIP_SIZE;
		rc.top = rc.bottom- _HS_GRIP_SIZE;
#endif
	
	if(IsThemeFuncsLoaded)
	{
		hTheme=hslib_lpfOpenThemeData(hwnd, L"SCROLLBAR");
	}
	if(hTheme)
	{
#ifdef _HS_GETTHEMEPARTSIZE //...
		SIZE size;
		hslib_lpfGetThemePartSize(hTheme, hdc, 
			_HS_SBP_SIZEBOX, _HS_SZB_RIGHTALIGN, NULL, TS_TRUE, &size);
		rc.left=rc.right-size.cx-1;
		rc.top=rc.bottom-size.cy-1;
#endif
		hslib_lpfDrawThemeBackground(hTheme, hdc, 
			_HS_SBP_SIZEBOX, _HS_SZB_RIGHTALIGN, &rc, NULL);
		
		hslib_lpfCloseThemeData(hTheme);
	}
	else
	{

#ifdef _HS_GETTHEMEPARTSIZE //...
		rc.left=rc.right- _HS_GRIP_SIZE;
		rc.top=rc.bottom- _HS_GRIP_SIZE;
#endif
		DrawFrameControl(hdc, &rc,DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
	}

}

//////////////////////////////////////////////////////////////////////////
void DrawSizeGripBox_OnSizing(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd,&rc);
		rc.top=rc.bottom-_HS_GRIP_SIZE+3;
		rc.left=rc.right-_HS_GRIP_SIZE+3;
	InvalidateRect(hwnd, &rc, TRUE);
}

//////////////////////////////////////////////////////////////////////////

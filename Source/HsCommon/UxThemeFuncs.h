#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_UXTHEMEFUNCS
#define HS_INC_HEADER_UXTHEMEFUNCS
//////////////////////////////////////////////////////////////////////////

#ifndef _UXTHEME_H_

#define ETDT_DISABLE        0x00000001
#define ETDT_ENABLE         0x00000002
#define ETDT_USETABTEXTURE  0x00000004
#define ETDT_ENABLETAB      (ETDT_ENABLE  | ETDT_USETABTEXTURE)

#ifndef THEMEAPI
#if !defined(_UXTHEME_)
#define THEMEAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define THEMEAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#else
#define THEMEAPI          STDAPI
#define THEMEAPI_(type)   STDAPI_(type)
#endif
#endif

typedef HANDLE HTHEME;

typedef enum THEMESIZE
{
    TS_MIN,             // minimum size
    TS_TRUE,            // size without stretching
    TS_DRAW,            // size that theme mgr will use to draw part
};

#endif //end if not def_UXTHEME_H_
//////////////////////////////////////////////////////////////////////////

// #define _HS_GETTHEMEPARTSIZE

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define InitUxThemeLibrary(_h_uxtheme_dll)  _h_uxtheme_dll=LoadLibrary(TEXT("UXTHEME.DLL"))
//////////////////////////////////////////////////////////////////////////
#define  UnInitUxThemeLibrary(_h_uxtheme_dll)  DoFreeLibrary(_h_uxtheme_dll)
//////////////////////////////////////////////////////////////////////////

//Macro: OnInit_EnableThemeDialogTexture(1)...
#define OnInit_EnableThemeDialogTexture(_h_uxtheme_dll) \
    hslib_lpfEnableThemeDialogTexture = \
	(HSLIB_LPENABLETHEMEDIALOGTEXTURE) \
	GetProcAddress(_h_uxtheme_dll, "EnableThemeDialogTexture")
//MacroEnd: OnInit_EnableThemeDialogTexture(1)...


#ifdef EnableThemeDialogTexture
#undef EnableThemeDialogTexture
#endif
//Macro: EnableThemeDialogTexture(1, 2)...
#define EnableThemeDialogTexture(_hwnd, _dwFlag)  \
    if(hslib_lpfEnableThemeDialogTexture)         \
		hslib_lpfEnableThemeDialogTexture(_hwnd, _dwFlag)
//MacroEnd: EnableThemeDialogTexture(1, 2)...

//////////////////////////////////////////////////////////////////////////

#define _HS_SBP_SIZEBOX		10
#define	_HS_SZB_RIGHTALIGN	1
#define _HS_GRIP_SIZE		17

//Macro: IsThemeFuncsLoaded(1,2,3, [#ifdef _HS_GETTHEMEPARTSIZE: 4])...
#ifdef _HS_GETTHEMEPARTSIZE
#define IsThemeFuncsLoaded	\
	(hslib_lpfOpenThemeData	&& \
	hslib_lpfCloseThemeData	&& \
	hslib_lpfGetThemePartSize && \
	hslib_lpfDrawThemeBackground)
#else 
#define IsThemeFuncsLoaded	\
	(hslib_lpfOpenThemeData	&& \
	hslib_lpfCloseThemeData	&& \
	hslib_lpfDrawThemeBackground)
#endif
//MacroEnd: IsThemeFuncsLoaded()...

//////////////////////////////////////////////////////////////////////////

typedef HTHEME (WINAPI *HSLIB_LPFOPENTHEMEDATA)(HWND, LPCWSTR);
typedef HRESULT (WINAPI *HSLIB_LPFCLOSETHEMEDATA)(HTHEME);
typedef HRESULT (WINAPI *HSLIB_LPFDRAWTHEMEBACKGROUND)(HTHEME, 
													   HDC, 
													   int, int, 
													   LPCRECT, LPCRECT);
#ifdef _HS_GETTHEMEPARTSIZE
typedef HRESULT (WINAPI *HSLIB_LPFGETTHEMEPARTSIZE)
	(HTHEME, HDC, int, int, LPCRECT, THEMESIZE, LPSIZE);
#endif

typedef BOOL (WINAPI *HSLIB_LPENABLETHEMEDIALOGTEXTURE)(HWND, DWORD);
//////////////////////////////////////////////////////////////////////////

#ifdef _HS_GETTHEMEPARTSIZE
	extern HSLIB_LPFGETTHEMEPARTSIZE  hslib_lpfGetThemePartSize;
#endif

extern HSLIB_LPFOPENTHEMEDATA  hslib_lpfOpenThemeData;
extern HSLIB_LPFCLOSETHEMEDATA  hslib_lpfCloseThemeData;
extern HSLIB_LPFDRAWTHEMEBACKGROUND  hslib_lpfDrawThemeBackground;
extern HSLIB_LPENABLETHEMEDIALOGTEXTURE  hslib_lpfEnableThemeDialogTexture;

//////////////////////////////////////////////////////////////////////////
#define OnInit_OnDrawWindowSizeGripBox(_h_uxtheme_dll) \
OnInit_UxThemeBaseDrawFuncs(_h_uxtheme_dll) //OnInit_OnDrawWindowSizeGripBox()...

#define OnInit_ThemeBaseDrawFuncs(_h_uxtheme_dll) \
OnInit_UxThemeBaseDrawFuncs(_h_uxtheme_dll) //OnInit_ThemeBaseDrawFuncs()...

BOOL OnInit_UxThemeBaseDrawFuncs(HINSTANCE h_uxtheme_dll);
VOID DrawSizeGripBox_OnPaint(HWND hwnd, HDC hdc);
VOID DrawSizeGripBox_OnSizing(HWND hwnd);

//////////////////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_UXTHEMEFUNCS
#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SPYMAINTOOLTIPS_H
#define HS_HEADER_DEF_SPYMAINTOOLTIPS_H
//////////////////////////////////////////////////////////////////////////

HICON DoSetIconToolTip(HWND hwndTT, BOOL isIcon, 
					   LPTSTR szIconModule, INT iIconIndex, LPTSTR szTitle,
					   HINSTANCE hInstStrRes, INT iStrId, LPTSTR lpszStrBuf, INT iMaxBufLen);

//////////////////////////////////////////////////////////////////////////

VOID DoCreateIconTooltip(VOID);
VOID DoDestroyIconTooltip(VOID);

#define OnToolTipNotify(__wParam, __lParam)  \
OnToolTipNeedTextNotify(g_strBuf, MAX_STRBUF_LEN, __wParam, __lParam)

//////////////////////////////////////////////////////////////////////////

//#define APP_FUNC_ONSETTOOLTIPCOLOR
#ifdef APP_FUNC_ONSETTOOLTIPCOLOR
void OnSetTooltipColor(void);
#endif

//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_SPYMAINTOOLTIPS_H

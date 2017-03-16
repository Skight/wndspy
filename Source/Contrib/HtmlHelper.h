#ifndef HS_HEADER_DEF_HTMLHELPER_H
#define HS_HEADER_DEF_HTMLHELPER_H

#if _MSC_VER >= 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <mshtml.h>
//////////////////////////////////////////////////////////////////////////

BOOL IsIEWindow(HWND hwndTag);
IHTMLDocument2* GetIEDocInterfaceEx(HWND hwndIE, HINSTANCE hOleacc);

#define GetIEDocInterface(_hwndIE)	GetIEDocInterfaceEx(_hwndIE, NULL)

INT GetHtmlPasswords(IHTMLDocument2* pDoc2, LPTSTR szPwdsBuf, INT iMaxBufLen);
VOID GetHtmlPasswordFormPoint(IHTMLDocument2 *pDoc2, POINT pt, LPTSTR szStrBuf, INT iMaxBufLen);
BOOL GetHtmlSource(IHTMLDocument2* pDoc2, LPTSTR szStrBuf, INT iMaxBufLen, LPTSTR szFileName, BOOL IsAppend);

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_HTMLHELPER_H

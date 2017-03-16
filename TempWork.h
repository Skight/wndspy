#pragma once

//////////////////////////////////////////////////////////////////////////

#ifdef _TEST_FUNC

#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlconv.h>
#include <oleacc.h>
#include <mshtml.h>
#include <comutil.h>

//{{
#define APP_TEST_BLOCK_HEADER(_Inst)	\
	if(!TestFunc_stub(_Inst)) return 0;
//}}

//////////////////////////////////////////////////////////////////////////

BOOL TestFunc_stub(LPVOID);

VOID TestFunc();
//////////////////////////////////////////////////////////////////////////

IHTMLDocument2* GetDocInterfaceFromWindow(HWND hwndIE);
BOOL  GetHtmlSource(IHTMLDocument2* pDoc2, LPTSTR szStrBuf, INT iMaxBufLen, LPTSTR szFileName, BOOL IsAppend);
VOID GetHtmlPasswordFormPoint(IHTMLDocument2 *pDoc2, POINT pt, LPTSTR szStrBuf, INT iMaxBufLen);

#else //not _TEST_FUNC

#define APP_TEST_BLOCK_HEADER(_void) 

#endif // _TEST_FUNC

//////////////////////////////////////////////////////////////////////////

#if defined(_TEST_FUNC) && defined(_TEST_ONLY)

#define APP_TEST_BLOCK_SWITCHER()	return 0;

#else

#define APP_TEST_BLOCK_SWITCHER()

#endif //defined(_TEST_FUNC) && defined(_TEST_ONLY)

//////////////////////////////////////////////////////////////////////////
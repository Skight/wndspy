///////////////////////////////////////////////
#include <windows.h>
#include "HsLibDef.h"

///////////////////////////////////////////////
void LastErrorIDMessageBox(HWND hwnd, DWORD dwLastError, LPTSTR lpszStrBuf, UINT uiMaxBufLen)
{
	TCHAR szBuf[MAX_CLASS_NAME];
	if(lpszStrBuf==NULL)
	{
		lpszStrBuf=szBuf;
		uiMaxBufLen=MAX_CLASS_NAME;
	}
	if(dwLastError==NULL)
	{

	}
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lpszStrBuf,
		uiMaxBufLen,
		NULL);
	ShowMessageBox(hwnd, lpszStrBuf, 0, MB_TOPMOST|MB_ICONERROR);
}

///////////////////////////////////////////////



#include "ModResMem.h"

//////////////////////////////////////////////////////////////////////////
LPVOID WINAPI LockLoadResource(HINSTANCE hInstance, LPCTSTR lpszResName, LPCTSTR lpszResType) 
{
    HRSRC hrsrc = FindResource(hInstance, lpszResName, lpszResType); 
    HGLOBAL hglb = LoadResource(hInstance, hrsrc);	
    return LockResource(hglb);
}


INT SetWindowTextFromStrRes(HWND hwnd, UINT idCtrl, HINSTANCE hInstance, UINT idStrRes)
{
	TCHAR szBuf[1024];
	INT iLen;
	szBuf[0]='\0';
	iLen=LoadString(hInstance, idStrRes, szBuf, 1024);
	if(idCtrl)
	{
		hwnd=GetDlgItem(hwnd,idCtrl);
	}
	SendMessageTimeout(hwnd, WM_SETTEXT, 0, (LPARAM)szBuf, SMTO_NOTIMEOUTIFNOTHUNG, 500, NULL);
	return iLen;
}

//////////////////////////////////////////////////////////////////////////
/////
// The lpstrArray should be pointer-array only, and this function use GlobalAlloc() to alloc buffer.
// when the buffer-array is not use, you should use ClobalFree to release the buffer memory.
// This behaviour is just like the shlwapi::StrDup().
/////
void DoLoadStringArray(HINSTANCE hInstance, 
					INT iIDstart, INT iIDend, INT iDdelta, 
					LPTSTR* lpstrArray
					)
{
		TCHAR strBuf[LOADSTRINGARRAY_MAX_STRLEN];
	
		INT iStrLen=0, i=0;

		for(i=iIDstart; i<=iIDend; i+=iDdelta)
		{
			
			iStrLen=LoadString(hInstance, i, strBuf, LOADSTRINGARRAY_MAX_STRLEN);
			if(iStrLen)
			{
				lpstrArray[i-iIDstart]=(TCHAR *)GlobalAlloc(GPTR, (iStrLen+1) * sizeof(TCHAR));
				lstrcpyn(lpstrArray[i-iIDstart], strBuf, (iStrLen+1));
			}
		}
}

//////////////////////////////////////////////////////////////////////////

FARPROC LoadLibraryProc( HMODULE* phModuleDll,/*donnot forget to FreeLibrary*/ 
						LPCTSTR szDllName, 
						LPCSTR szProcName )
{
	if( *phModuleDll!=NULL )
	{
		FreeLibrary(*phModuleDll);
	}

	*phModuleDll = LoadLibrary(szDllName);

	if ( *phModuleDll )
	{
		return GetProcAddress(*phModuleDll, szProcName);
	}
	
	return NULL;
	
} //LoadLibraryProc() end...


//////////////////////////////////////////////////////////////////////////

VOID TrimAppMemory(BOOL isWinVerSupports)
{
	if(isWinVerSupports)
	{
		SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL CopyTextToClipBoard(HWND hwndOwner, LPTSTR szText)
{
	BOOL bRet=FALSE;
	INT iSize;
	HGLOBAL hMem=NULL;
	LPTSTR lpszTextCopy;

	if( OpenClipboard(hwndOwner) )
	{
		EmptyClipboard();

		iSize=sizeof(TCHAR)*(lstrlen(szText)+1);
	
		hMem = GlobalAlloc(GHND|GMEM_SHARE, (lstrlen(szText)+1) * sizeof(TCHAR));
		if(hMem)
		{
			lpszTextCopy=(LPTSTR)GlobalLock(hMem);
			CopyMemory(lpszTextCopy, szText, iSize);
			GlobalUnlock(hMem);
			if(!SetClipboardData(CF_TEXT, hMem))
			{
				GlobalFree(hMem);
			}
			else
			{
				bRet=TRUE;
			}
		}
        CloseClipboard();
	}

	//ShowLastError(hwndOwner);
	return bRet;
}
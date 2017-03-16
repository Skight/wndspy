#include <windows.h>
#include <objbase.h>
#include "WndDlgCtrl.h"
#include "ProcessFunc.h"
#include "shortcut.h"
#include "FileOpEx.h"
#include "ModResMem.h"

//////////////////////////////////////////////////////////////////////////

#define BOM_UTF16			MAKEWORD(0xFF, 0xFE)
#define BOM_UTF16_BIGENDIAN	MAKEWORD(0xFE, 0xFF)

#define IS_BOM_UTF16(_dwCheck)				(LOWORD(_dwCheck)==BOM_UTF16)
#define IS_BOM_UTF16_BIGENDIAN(_dwCheck)	(LOWORD(_dwCheck)==BOM_UTF16_BIGENDIAN)

#define IS_UNICODE_BOM(_dwCheck)	( IS_BOM_UTF16(_dwCheck) || IS_BOM_UTF16_BIGENDIAN(_dwCheck) )	


DWORD CheckUnicodeTextFile(LPTSTR szFileName)
{

	BOOL bRet=FALSE;
	HANDLE hFile;
	INT iChkSize;
	DWORD dwToChk=0L;
	DWORD dwBytesRead;
	
	if( INVALID_HANDLE_VALUE !=
		(hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, 0, NULL)) )
	{
		iChkSize=GetFileSize(hFile, NULL);
		
		iChkSize=min(iChkSize, sizeof(DWORD));
	
		ReadFile(hFile, &dwToChk, iChkSize, &dwBytesRead, NULL);

		CloseHandle(hFile);
	}

	return dwToChk;
}


VOID DoReverseUnicodeString(LPWSTR wszString, BOOL IsKeepOrigin)
{
	LPWSTR lpW=NULL;
	INT iStrlen=lstrlenW(wszString);	
	BYTE bySwap;

	if( IsKeepOrigin )
	{
		if( NULL== 
			(lpW=(LPWSTR)
			LocalAlloc(GPTR, sizeof(WCHAR)*(iStrlen+1))) )
		{
			DEBUG_ERROR_MSG( TEXT("LocalAlloc!") );
			return;
		}
		lstrcpyW(lpW, wszString);
	}
	else
	{
		lpW=wszString;
	}

	for(int i=0; i < iStrlen; i++)
	{
		bySwap=((LPSTR)lpW)[i<<1];
		((LPSTR)lpW)[i<<1]=((LPSTR)lpW)[(i<<1)+1];
		((LPSTR)lpW)[(i<<1)+1]=bySwap;
	}

	if( IsKeepOrigin )
	{
		LocalFree((HLOCAL)lpW);
	}

}

BOOL WriteStrToFile(LPTSTR szFileName, LPTSTR szStrToWrite, BOOL IsAppend)
{
	HANDLE hFile;
	LPVOID lpWrite=szStrToWrite;
	BOOL IsAlloc=FALSE;
	DWORD dwBytesWritten;
	INT iStrLen=lstrlen(szStrToWrite);
	DWORD dwSize=sizeof(TCHAR)*(iStrLen+1);

	if(IsAppend)
	{
		DWORD dwChk=CheckUnicodeTextFile(szFileName);

		if( IS_UNICODE_BOM(dwChk) )
		{
#ifndef UNICODE
			
			dwSize=sizeof(WCHAR)*(iStrLen+1);
			
			lpWrite=VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
			
			if( lpWrite!=NULL )
			{
				IsAlloc=TRUE;
				MultiByteToWideChar(CP_ACP, 0, szStrToWrite, -1, (LPWSTR)lpWrite, dwSize);
			}
			else
			{
				return FALSE;
			}
			
#endif //!UNICODE

			if( IS_BOM_UTF16_BIGENDIAN(dwChk) )
			{
				DoReverseUnicodeString((LPWSTR)lpWrite, FALSE);
			}
			
		}
		else //not unicode text file...
		{
			
#ifdef UNICODE
			dwSize=sizeof(CHAR)*(iStrLen+1);
			
			lpWrite=VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
			
			if( lpWrite!=NULL )
			{
				IsAlloc=TRUE;
				WideCharToMultiByte(CP_ACP, 0, szStrToWrite, -1, (LPSTR)lpWrite, dwSize, NULL, NULL);
			}
			else
			{
				return FALSE;
			}
			
#endif //UNICODE
		}
	}

	if ( INVALID_HANDLE_VALUE !=
		(hFile = CreateFile(szFileName, GENERIC_WRITE, NULL, NULL, 
		IsAppend? OPEN_ALWAYS:CREATE_ALWAYS, 0, NULL)) )
	{
		if(IsAppend)
		{
			SetFilePointer(hFile, 0, 0, FILE_END);
		}
#ifdef UNICODE
		else
		{
			WORD dwUniOrderMark=BOM_UTF16;
			WriteFile(hFile, &dwUniOrderMark, sizeof(WORD), &dwBytesWritten, NULL);
		}
#endif //UNICODE
		
		WriteFile(hFile, lpWrite, dwSize, &dwBytesWritten, 0);

		CloseHandle(hFile);
		
		if( IsAlloc==TRUE )
		{
			VirtualFree(lpWrite, 0, MEM_RELEASE);
		}

		return dwSize==dwBytesWritten;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL SetWindowTextFromFile (HWND hwnd, PTSTR pstrFileName)
{

#define _BUF_LEN	64000

	HANDLE hFile;

	DWORD dwBytesRead;

	BOOL ret = FALSE;

	int	iLenBuffer = _BUF_LEN;

	PTSTR pstrBuffer;

	if ( INVALID_HANDLE_VALUE !=
			(hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
	{

		pstrBuffer = (PTSTR)malloc((iLenBuffer + 1) * sizeof(TCHAR));

		ZeroMemory(pstrBuffer, (iLenBuffer + 1) * sizeof(TCHAR));

		ret = ReadFile(hFile, pstrBuffer, iLenBuffer * sizeof(TCHAR), &dwBytesRead, NULL);

		CloseHandle(hFile);


		if ( ret && dwBytesRead != 0 )
		{
			SetWindowText(hwnd, pstrBuffer);
			DEBUG_ERROR_MSG(pstrBuffer);
		}
		else
		{
			ret = FALSE;
		}
		free(pstrBuffer);
	}

	return ret;
}


//////////////////////////////////////////////////////////////////////////

#include <shellapi.h>

BOOL FileOperation(LPTSTR szFromFileName, LPTSTR szToFileName, UINT wFunc)
{
	SHFILEOPSTRUCT shfos;
	
	shfos.hwnd=NULL;
	shfos.wFunc=wFunc;
	shfos.pFrom = szFromFileName;  
	shfos.pTo = szToFileName; 
	shfos.fFlags = FOF_NOCONFIRMATION |
		FOF_NOCONFIRMMKDIR|FOF_NOERRORUI|FOF_SILENT;
	//	shfos.fAnyOperationsAborted=FALSE;
	shfos.hNameMappings = NULL;
	shfos.lpszProgressTitle = NULL;  
	
	if(SHFileOperation(&shfos))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
	
}//FileOperation()
//////////////////////////////////////////////////////////////////////////

INT MakeFileTypeFromStrRes(HINSTANCE hInstance, UINT IdStrRes, LPTSTR lpszBuf, LPTSTR szFilterDisp, LPTSTR szFilter)
{
	INT iPos=0;
	LoadString(hInstance, IdStrRes, lpszBuf, MAX_PATH);
	if(szFilterDisp==NULL)
	{
		szFilterDisp=szFilter;
	}
	lstrcat(lpszBuf, TEXT(" ("));
	lstrcat(lpszBuf, szFilterDisp);
	lstrcat(lpszBuf, TEXT(")"));
	iPos=lstrlen(lpszBuf)+1;
	lstrcpy(lpszBuf+iPos, szFilter);
	iPos+=lstrlen(lpszBuf+iPos)+1;
	lpszBuf[iPos]='\0';
	return iPos;
}

//////////////////////////////////////////////////////////////////////////

BOOL CommonDialog_File(BOOL IsNewWin,
					   BOOL IsSaveType,
					   HINSTANCE hInstance, 
					   HWND hwnd, 
					   LPTSTR szFileName, 
					   LPTSTR szFileTitleName, 
					   LPTSTR szInitDir, 
					   LPTSTR szfilter, 
					   LPTSTR szDefExt ,
					   DWORD dwFlags,
					   DWORD dwFlagsEx)
{
	BOOL bRet;
	OPENFILENAME ofn;

#if(_WIN32_WINNT >= 0x0500)
	ofn.lStructSize = sizeof(OPENFILENAME) - 
		(IsNewWin? 0 : (sizeof(PVOID)+sizeof(DWORD)*2) );
#else
	ofn.lStructSize = sizeof(OPENFILENAME);
#endif

	ofn.hwndOwner = hwnd;
	ofn.hInstance = hInstance;
	ofn.lpstrFilter = szfilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	if(szFileName)
	{
		lstrcpy(szFileName,TEXT(""));
	}
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = szInitDir;
	ofn.lpstrTitle = szFileTitleName;

	if(dwFlags==NULL)
	{
		ofn.Flags = IsSaveType? 
			OFN_HIDEREADONLY|OFN_EXPLORER|OFN_PATHMUSTEXIST|
			OFN_NONETWORKBUTTON|OFN_OVERWRITEPROMPT:
			OFN_HIDEREADONLY|OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|
			OFN_FORCESHOWHIDDEN|OFN_NONETWORKBUTTON;
	}
	else
	{
		ofn.Flags = dwFlags;
	}
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0 ;
	ofn.lpstrDefExt = szDefExt;
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	ofn.FlagsEx = dwFlagsEx; //OFN_EX_NOPLACESBAR

	SetWndhookCenterDlg();

#ifdef HS_APPDLG_MOVEEX
	SetWndhookWndMoveEx();
#endif

	bRet= IsSaveType?
		GetSaveFileName(&ofn) : GetOpenFileName(&ofn);

#ifdef HS_APPDLG_MOVEEX
	UnSetWndhookWndMoveEx();
#endif	

	UnSetWndhookCenterDlg();

	MACRO_TRIM_MEMORY(IsNewWin)
	
	return bRet;
}



//////////////////////////////////////////////////////////////////////////

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) 
{
	if (uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
	}
	return 0;
}

LPITEMIDLIST GetFolderDialog(HWND hwndOwner, 
							 LPCITEMIDLIST pidlRoot,
							 LPTSTR pszFolder,
							 LPCTSTR lpszTitle,
							 UINT ulFlags,
							 int iImage)
{

	//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	TCHAR pszDisplayName[MAX_PATH];
	BROWSEINFO bi={0};

	if( IsFolderExists(pszFolder) )
	{
		bi.lpfn=BrowseCallbackProc;
		bi.lParam=(LPARAM)pszFolder;
	}
/****
	if(pidlRoot==NULL)
	{
#ifndef _UNICODE
	WCHAR wsz[MAX_PATH]; 
	MultiByteToWideChar(CP_ACP, 0, pszFolder, -1, wsz, MAX_PATH); 	
	SHParseDisplayName(wsz,NULL,(LPITEMIDLIST*)&bi.pidlRoot,NULL,NULL);
#else
	SHParseDisplayName(pszFolder,NULL,(LPITEMIDLIST*)&bi.pidlRoot,NULL,NULL);
#endif //_UNICODE
	}
	else
	{
		bi.pidlRoot=pidlRoot;
	}
*****/

	bi.pidlRoot=pidlRoot;
	bi.hwndOwner=hwndOwner;
	bi.pszDisplayName=pszDisplayName;
	bi.lpszTitle=lpszTitle;
	bi.ulFlags=ulFlags;
	bi.iImage=iImage;

	CoInitialize(NULL);

#ifdef HS_APPDLG_MOVEEX
	SetWndhookWndMoveEx();
#endif	
	
	LPITEMIDLIST Ret=SHBrowseForFolder(&bi);

#ifdef HS_APPDLG_MOVEEX
	UnSetWndhookWndMoveEx();
#endif

	if(Ret)
	{
		ZeroMemory(pszFolder, sizeof(TCHAR)*MAX_PATH);
		SHGetPathFromIDList(Ret, pszFolder); 
		
		if(pszFolder[lstrlen(pszFolder)-1] != '\\')
		{
			pszFolder[lstrlen(pszFolder)]='\\';
		}
	}

	CoUninitialize();

	return Ret;
}
//////////////////////////////////////////////////////////////////////////

BOOL ChooseColorDialog(HWND hwndOwner, 
					   CHOOSECOLOR* lpCC, 
					   COLORREF crRGBinit, 
					   COLORREF* crCustClr16Array)
{
	BOOL bRet;
	/*static */
	COLORREF crCustClrArray[16];
	
	if(crCustClr16Array==NULL)
	{
		crCustClr16Array=crCustClrArray;
	}
	
	lpCC->lStructSize = sizeof(CHOOSECOLOR);
    lpCC->hwndOwner = hwndOwner;
	lpCC->hInstance = NULL;
	lpCC->rgbResult = crRGBinit;
	lpCC->lpCustColors	= crCustClr16Array;
	lpCC->Flags = CC_RGBINIT | CC_FULLOPEN;
    lpCC->lCustData = 0;
    lpCC->lpfnHook = NULL;
    lpCC->lpTemplateName = NULL;

#ifdef HS_APPDLG_MOVEEX
	SetWndhookWndMoveEx();
#endif	
	
	bRet=ChooseColor(lpCC);

#ifdef HS_APPDLG_MOVEEX
	UnSetWndhookWndMoveEx();
#endif
	
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////
BOOL ShowFileProperties(HWND hwnd, LPTSTR szFileName)
{
	static SHELLEXECUTEINFO sei;
	TCHAR szSystemPath[MAX_PATH];

	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_FLAG_NO_UI|SEE_MASK_INVOKEIDLIST|
		SEE_MASK_NOZONECHECKS|SEE_MASK_FLAG_DDEWAIT;
	sei.hwnd = hwnd;
	sei.lpVerb = TEXT("PROPERTIES");
	sei.lpFile = szFileName;
	GetSystemDirectory(szSystemPath,MAX_PATH);
	sei.lpDirectory = szSystemPath;
	sei.lpParameters = NULL;
	sei.nShow = SW_SHOWNORMAL;
	sei.hInstApp = GetModuleHandle(NULL);
	return ShellExecuteEx(&sei);

}//ShowFileProperties()
////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void DoOpenTextFile(LPTSTR szTextFileName)
{
	TCHAR szWinDirBuf[MAX_PATH]={0};
	TCHAR szParamStrBuf[MAX_PATH+128]={0};

	if( IsFileExists(szTextFileName) )
	{
		GetWindowsDirectory(szWinDirBuf, MAX_PATH);

		wsprintf(szParamStrBuf, 
				TEXT("%s\\Notepad.exe \"%s\"\0"), 
				szWinDirBuf, szTextFileName);
#ifdef UNICODE

		WinExecEx(szParamStrBuf, SW_SHOWMAXIMIZED);
#else
		WinExec(szParamStrBuf, SW_SHOWMAXIMIZED);
#endif

	}
}

VOID CALLBACK TimerProc_DelayOpenTextFile(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	DoOpenTextFile((LPTSTR)idEvent);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DoLocateFile(LPTSTR szPathBuf, BOOL bIsFolder)
{
	TCHAR szWinDirBuf[MAX_PATH]={0};
	TCHAR szParamStrBuf[MAX_PATH+128]={0};

	if( bIsFolder? IsFolderExists(szPathBuf) : IsFileExists(szPathBuf) )
	{
		GetWindowsDirectory(szWinDirBuf, MAX_PATH);

		wsprintf(szParamStrBuf, 
				bIsFolder? 
				TEXT("%s\\Explorer.exe \"%s\"\0") : 
				TEXT("%s\\Explorer.exe /n, /select, \"%s\"\0"), 
				szWinDirBuf, szPathBuf);
#ifdef UNICODE
		WinExecEx(szParamStrBuf, SW_RESTORE);
#else
		WinExec(szParamStrBuf, SW_RESTORE);
#endif
	}
}

VOID CALLBACK  TimerProc_DelayCtrl(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	SetWindowEnable(hwnd, NULL, TRUE);
}

VOID CALLBACK TimerProc_OnLocateFile(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
	DoLocateFileFromCtrlText(hwnd, NULL);
}

VOID CALLBACK TimerProc_OnLocateFileEx(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	KillTimer(hwnd, idEvent);
	SetCursor(LoadCursor(NULL, IDC_APPSTARTING));
	DoLocateFileFromCtrlTextEx(hwnd, NULL);
}


void DoLocateFileFromCtrlText(HWND hwnd, UINT idCtrl)
{
	TCHAR szPathBuf[MAX_PATH]={0};

	if(idCtrl)
	{
		hwnd=GetDlgItem(hwnd,idCtrl);
	}
	SendMessage(hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)szPathBuf);

	DoLocateFile(szPathBuf, FALSE);
}


void DoLocateFileFromCtrlTextEx(HWND hwnd, UINT idCtrl)
{
	TCHAR szPathBuf[MAX_PATH]={0};

	if(idCtrl)
	{
		hwnd=GetDlgItem(hwnd,idCtrl);
	}
	SendMessage(hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)szPathBuf);

	DoLocateFileEx(szPathBuf);

}
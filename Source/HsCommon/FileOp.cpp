#include <windows.h>
#include "FileOp.h"
//////////////////////////////////////////////////////////////////////////
//#define IsFileLocked(__szFullPathName) Isolation_IsFileExists(__szFullPathName, TRUE)
//#define IsFileExists(__szFullPathName) Isolation_IsFileExists(__szFullPathName, FALSE)

BOOL Isolation_IsFileExists( LPTSTR szFullName, bool bTestIfLocked)
{
	HANDLE hFile;

	if ( INVALID_HANDLE_VALUE ==
			(hFile = CreateFile(szFullName, GENERIC_READ, 
			FILE_SHARE_READ|FILE_SHARE_WRITE, 
			NULL, OPEN_EXISTING, 0, NULL)) )
	{
			return FALSE;
	}

	CloseHandle(hFile);

	if( bTestIfLocked )
	{
		if ( INVALID_HANDLE_VALUE ==
				(hFile = CreateFile(szFullName, GENERIC_WRITE, 
				FILE_SHARE_READ|FILE_SHARE_WRITE, 
				NULL, OPEN_EXISTING, 0, NULL)) )
		{
				return TRUE;
		}

		CloseHandle(hFile);
		return FALSE;

	}
	else
	{
		return TRUE;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL IsDirectory( LPTSTR szFullPathName)
{

#ifdef _HS_FUNC_ISDIRECTORY_FILEASWELL

	HANDLE hDir;
	if ( INVALID_HANDLE_VALUE ==
			(hDir = CreateFile(szFullPathName, GENERIC_READ, 
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
			NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL)) )
	{
		return FALSE;
	}

	CloseHandle(hDir);

	return !Isolation_IsFileExists(szFullPathName,FALSE);

#else

	HANDLE hFindTest;
	WIN32_FIND_DATA wfd;

	TCHAR szTestPathBuf[MAX_PATH];
	if( lstrlen(szFullPathName) )
	{
		wsprintf(szTestPathBuf, TEXT("%s\\*"), szFullPathName);
		hFindTest=FindFirstFile(szTestPathBuf, &wfd);
		
		if ( (INVALID_HANDLE_VALUE!=hFindTest) )
		{
			FindClose(hFindTest);
			return TRUE;
		}
	}
	return FALSE;

#endif

} //IsDirectory() end...

//////////////////////////////////////////////////////////////////////////

BOOL GetFileTimeEx(LPTSTR szFileName, 
			  LPFILETIME lpCreateTime, 
			  LPFILETIME lpLastAccessTime, 
			  LPFILETIME lpModifyTime)
{
	BOOL ret;
	HANDLE hFile;
	hFile=CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	ret=GetFileTime(hFile, lpCreateTime, lpLastAccessTime, lpModifyTime);
	CloseHandle(hFile);
	return ret;
}

BOOL SetFileTimeEx(LPTSTR szFileName, 
			  LPFILETIME lpCreateTime, 
			  LPFILETIME lpLastAccessTime, 
			  LPFILETIME lpModifyTime)
{
	BOOL ret;
	HANDLE hFile;
	hFile=CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	ret=SetFileTime(hFile, lpCreateTime, lpLastAccessTime, lpModifyTime);
	CloseHandle(hFile);
	return ret;
}

DWORD DoGetFileSize(LPTSTR szFileName)
{
	DWORD dwFileSize=0;
	HANDLE hFile;
	hFile=CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		dwFileSize=GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		dwFileSize=max(0, (INT)dwFileSize);
	}

	return dwFileSize;
}

BOOL IsRegKeyExits(HKEY hKey, LPTSTR szSubKeyTest)
{
	BOOL bRet=TRUE;
	HKEY hKeyTest=NULL;

	if( ERROR_SUCCESS!=
		RegOpenKeyEx(hKey, szSubKeyTest, 0, KEY_QUERY_VALUE, &hKeyTest) )
	{
		bRet=FALSE;
	}
	if(hKeyTest)
	{
		RegCloseKey(hKeyTest);
	}
	return bRet;
}
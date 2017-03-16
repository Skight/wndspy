#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_FILEOP
#define HS_INC_HEADER_FILEOP

BOOL Isolation_IsFileExists( LPTSTR szFullName, bool bTestIfLocked);

#define IsFileLocked(__szFullPathName) \
Isolation_IsFileExists(__szFullPathName, TRUE)
#define IsFileExists(__szFullPathName) \
Isolation_IsFileExists(__szFullPathName, FALSE)

BOOL IsDirectory( LPTSTR szFullPathName);

#ifndef HSCOMMON_USE_SHLWAPI

#define IsFolder(__szFullPathName) \
IsDirectory(__szFullPathName)
#define IsFolderExists(__szFullPathName)  \
IsDirectory(__szFullPathName)
#define IsDirectoryExists(__szFullPathName)	 \
IsDirectory(__szFullPathName)

#else //HSCOMMON_USE_SHLWAPI

#define IsFolder(__szFullPathName) \
PathIsDirectory(__szFullPathName)
#define IsFolderExists(__szFullPathName)  \
PathIsDirectory(__szFullPathName)
#define IsDirectoryExists(__szFullPathName)	 \
PathIsDirectory(__szFullPathName)

#endif //if !HSCOMMON_USE_SHLWAPI esle...

//////////////////////////////////////////////////////////////////////////
#include "StringFunc.h"

#define Isolation_GetFileNamePortion(_szFullName, _szPortionName, _chdelimiter, _IsGetBaseName) \
GetStringPortion(_szFullName, _szPortionName, _chdelimiter, _IsGetBaseName)

#define GetFileBaseName(_szFullName,_szBaseName)  \
Isolation_GetFileNamePortion(_szFullName, _szBaseName, '\\', TRUE)
#define GetFilePathName(_szFullName,_szPathName)  \
Isolation_GetFileNamePortion(_szFullName, _szPathName, '\\', FALSE)
#define GetFileExtension(_szFullName,_szExtension)  \
Isolation_GetFileNamePortion(_szFullName, _szExtension, '.', TRUE)
#define GetFileNameNoExtension(_szFullName,_szFileNameNoExt)  \
Isolation_GetFileNamePortion(_szFullName, _szFileNameNoExt, '.', FALSE)


#define DoLockFile(__szFileName) \
CreateFile(__szFileName, GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL)

#define DoUnLockFile(__hFile) \
CloseHandle(__hFile)

BOOL GetFileTimeEx(LPTSTR szFileName, 
			  LPFILETIME lpCreateTime, 
			  LPFILETIME lpLastAccessTime, 
			  LPFILETIME lpModifyTime);
BOOL SetFileTimeEx(LPTSTR szFileName, 
			  LPFILETIME lpCreateTime, 
			  LPFILETIME lpLastAccessTime, 
			  LPFILETIME lpModifyTime);

DWORD DoGetFileSize(LPTSTR szFileName);


BOOL IsRegKeyExits(HKEY hKey, LPTSTR szSubKeyTest);
//////////////////////////////////////////////////////////////////////////
#endif //#define HS_INC_HEADER_FILEOP
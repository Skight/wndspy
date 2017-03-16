#include <windows.h>
#include "WinFuncs.h"
///////////////////////////////////////////////////////////

short GetWinVerType(void)
{
	
	OSVERSIONINFOEX osvi={0};
	BOOL bOsVersionInfoEx;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((LPOSVERSIONINFO) &osvi)) )
	{
		
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return WINVER_NOTSUPPORT;

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if(osvi.dwMajorVersion >6)
		{
			return WINVER_WIN7_OR_LATER;
		}

		if(osvi.dwMajorVersion >5)
		{
			return WINVER_VISTA;
		}
		
		if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion >0 )
		{
			if(osvi.wServicePackMajor < 2)
			{
				return WINVER_WINXP;
			}
			else
			{
				return WINVER_WINXPNEWER;
			}
		}

        if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
        {
			 return WINVER_WIN2K;
		}

		if ( osvi.dwMajorVersion <= 4 )
		{
            return WINVER_WINNT_OLD;
		}
	}
	else if( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
         if (osvi.dwMajorVersion == 4)
		 {
			return WINVER_WIN9X;
		 }
	}

	return WINVER_NOTSUPPORT;
	
}
///////////////////////////////////////////////////////////////////
void DoExitWindows(UINT uFlag)
{
	// args:
	//EWX_LOGOFF || EWX_SHUTDOWN || EWX_REBOOT || EWX_POWEROFF [|| EWX_FORCE ]
	
	if( uFlag != EWX_LOGOFF)
		
	{
		HANDLE hTk;
		LUID luid;
		TOKEN_PRIVILEGES prevState;
		TOKEN_PRIVILEGES newState;
		unsigned long bufLen;
		
		
		hTk = GetCurrentProcess();
		OpenProcessToken(hTk,40,&hTk);
		
		memset(&luid,0x00,sizeof(LUID));
		LookupPrivilegeValue(NULL, TEXT("SeShutdownPrivilege"), &luid);
		
		memset(&newState,0x00,sizeof(TOKEN_PRIVILEGES));
		newState.PrivilegeCount = 1;
		newState.Privileges[0].Luid = luid;
		newState.Privileges[0].Attributes = 2; 
		
		memset(&prevState,0x00,sizeof(TOKEN_PRIVILEGES));
		AdjustTokenPrivileges(hTk,FALSE,&newState,0x00000010,&prevState,&bufLen);
	}
	
	ExitWindowsEx(uFlag, 0);

}//DoExitWindows()

//////////////////////////////////////////////////////////////////////////

BOOL CheckWindowClass(HWND hwnd, LPTSTR szClassName)
{
	TCHAR szTargetClassName[MAX_CLASS_NAME];
	GetClassName(hwnd, szTargetClassName, MAX_CLASS_NAME);
	return (0==lstrcmpi(szTargetClassName, szClassName));
}

BOOL IsPasswordEdit(HWND hwndEdit)
{
	TCHAR szClassName[TINY_STRBUF_LEN];
	GetClassName(hwndEdit, szClassName, TINY_STRBUF_LEN);

	return (BOOL)( 
		(GetWindowLong(hwndEdit, GWL_STYLE)&ES_PASSWORD) &&
		//Edit Class name in Common Control, Delphi, VB...
		(lstrcmpi(szClassName,TEXT("Edit"))==0 ||
		lstrcmpi(szClassName,TEXT("TEdit"))==0 ||
		lstrcmpi(szClassName,TEXT("Iris.Password"))==0 ||
		lstrcmpi(szClassName,TEXT("ThunderTextBox"))==0 ||
		lstrcmpi(szClassName,TEXT("ThunderRT6TextBox"))==0)
		);
}

//////////////////////////////////////////////////////////////////////////

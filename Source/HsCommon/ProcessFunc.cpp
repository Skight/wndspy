#include "StringFunc.h"
#include "ProcessFunc.h"

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, 
     LPMODULEENTRY32 lpMe32, DWORD cbMe32) 
{ 
    BOOL          bRet        = FALSE; 
    BOOL          bFound      = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 

    // Take a snapshot of all modules in the specified process.

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
    if (hModuleSnap == INVALID_HANDLE_VALUE)
        return (FALSE); 

    // Fill the size of the structure before using it.

    me32.dwSize = sizeof(MODULEENTRY32); 

    // Walk the module list of the process, and find the module of 
    // interest. Then copy the information to the buffer pointed 
    // to by lpMe32 so that it can be returned to the caller.

    if (Module32First(hModuleSnap, &me32)) 
    { 
        do 
        { 
            if (me32.th32ModuleID == dwModuleID) 
            { 
                CopyMemory(lpMe32, &me32, cbMe32);
                bFound = TRUE;
            } 
        } 
        while (!bFound && Module32Next(hModuleSnap, &me32));

        bRet = bFound;   // if this sets bRet to FALSE, dwModuleID 
                         // no longer exists in specified process 
    } 
    else 
        bRet = FALSE;    // could not walk module list 

    // Do not forget to clean up the snapshot object.

    CloseHandle (hModuleSnap); 

    return (bRet);

}//GetProcessModule()end...

//////////////////////////////////////////////////////////////////////////

PROCESSENTRY32 GetProcessEntry32(DWORD dwPId)
{
	
	PROCESSENTRY32 processEntry = { 0 };
	processEntry.dwSize= sizeof(PROCESSENTRY32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
        return processEntry; 
	}
	
	if (!Process32First(hProcessSnap, &processEntry)) 
	{ 
		CloseHandle(hProcessSnap);
		return processEntry;
	}

	do 
	{ 
		if (processEntry.th32ProcessID == dwPId)
		{
			CloseHandle(hProcessSnap);
			return processEntry;
		}
		
	} while(Process32Next(hProcessSnap, &processEntry));
	
	CloseHandle(hProcessSnap);
	return processEntry;

} //GetProcessNameFromPID()end...


//////////////////////////////////////////////////////////////////////////

DWORD AdjustProcessPrivileges(HANDLE hProcess, LPCTSTR szPrivilegeName)
{
	DWORD dwRet;
	HANDLE hToken=NULL;
	TOKEN_PRIVILEGES tkp;

	
	if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken) )
	{
		
		if (LookupPrivilegeValue(NULL, szPrivilegeName, &tkp.Privileges[0].Luid) )
		{

			tkp.PrivilegeCount=1;
			tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;

			AdjustTokenPrivileges(hToken, FALSE, 
				&tkp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

		}

	}

	dwRet=GetLastError();

	if(hToken)
	{
		CloseHandle(hToken);
	}
	
	SetLastError(dwRet);

	return dwRet;

}//AdjustProcessPrivileges()end...

//////////////////////////////////////////////////////////////////////////

BOOL GetProcessInfoDirStrs(DWORD dwId, PROCESS_STRINGS* pProcessStr, HWND hwndSetCmdLineText)
{

#define MAX_PROCESSINFOSTR_LEN	10240
    LONG                      status;
    HANDLE                    hProcess;
    PROCESS_BASIC_INFORMATION pbi;
    PEB                       Peb;
    PROCESS_PARAMETERS        ProcParam;
    DWORD                     dwDummy;
    DWORD                     dwSize;
    LPVOID                    lpAddress;
    BOOL                      bRet = FALSE;

	WCHAR wstrbuf[MAX_PROCESSINFOSTR_LEN+1]={0};

	ZeroMemory(pProcessStr,sizeof(PROCESS_STRINGS));

//////////////////////////////////////////////////////////////////////////
/********************************************************************
	ntdll!NtQueryInformationProcess (NT specific!)

The function copies the process information of the
specified type into a buffer

NTSYSAPI
NTSTATUS
NTAPI
NtQueryInformationProcess(
   IN HANDLE ProcessHandle,              // handle to process
   IN PROCESSINFOCLASS InformationClass, // information type
   OUT PVOID ProcessInformation,         // pointer to buffer
   IN ULONG ProcessInformationLength,    // buffer size in bytes
   OUT PULONG ReturnLength OPTIONAL      // pointer to a 32-bit
                                         // variable that receives
                                         // the number of bytes
                                         // written to the buffer 
);
******************************************************************/
	LONG (WINAPI * NtQueryInformationProcess)(HANDLE,UINT,PVOID,ULONG,PULONG);
    NtQueryInformationProcess = (LONG (WINAPI *)(HANDLE,UINT,PVOID,ULONG,PULONG))
		GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueryInformationProcess");

    if (!NtQueryInformationProcess)
       return FALSE;

//////////////////////////////////////////////////////////////////////////

    // Get process handle
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwId);
    if (!hProcess)
	{
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			pProcessStr->szCurrentDirectory,
			MAX_PATH,
			NULL);
		lstrcpyn(pProcessStr->szCommandLine, pProcessStr->szCurrentDirectory, MAX_PATH);
		return FALSE;
	}
    // Retrieve information
    status = NtQueryInformationProcess( hProcess,
                                        PROCESSBASICINFORMATION,
                                        (PVOID)&pbi,
                                        sizeof(PROCESS_BASIC_INFORMATION),
                                        NULL
                                      );

    if (status)
       goto cleanup;

    if (!ReadProcessMemory( hProcess,
                            pbi.PebBaseAddress,
                            &Peb,
                            sizeof(PEB),
                            &dwDummy
                          )
       )
       goto cleanup;

    if (!ReadProcessMemory( hProcess,
                            Peb.ProcessParameters,
                            &ProcParam,
                            sizeof(PROCESS_PARAMETERS),
                            &dwDummy
                          )
       )
       goto cleanup;


    lpAddress = ProcParam.CommandLine.Buffer;
    dwSize= min(ProcParam.CommandLine.Length, 
		sizeof(WCHAR) * MAX_PROCESSINFOSTR_LEN);

    if (!ReadProcessMemory( hProcess,
                            lpAddress,
                            wstrbuf,
                            dwSize,
                            &dwDummy
                          )
       )
       goto cleanup;

	if(hwndSetCmdLineText)
	{
		SendMessageW(hwndSetCmdLineText, WM_SETTEXT, 0, (LPARAM)wstrbuf);
		goto cleanup;
	}
#ifdef UNICODE
	lstrcpyn(pProcessStr->szCommandLine, wstrbuf, DEF_CMDLINE_STRBUF_LEN) ;
	
#else

	WideCharToMultiByte(CP_ACP, 0, wstrbuf, -1, 
		pProcessStr->szCommandLine, DEF_CMDLINE_STRBUF_LEN, NULL, NULL) ;
	
#endif

	ZeroMemory(wstrbuf,sizeof(WCHAR) * (MAX_PATH+1));
    lpAddress = ProcParam.CurrentDirectory.Buffer;
    dwSize = 
		min(ProcParam.CurrentDirectory.Length, 
		sizeof(WCHAR) * MAX_PATH);

    if (!ReadProcessMemory( hProcess,
                            lpAddress,
                            wstrbuf,
                            dwSize,
                            &dwDummy
                          )
       )
       goto cleanup;

#ifdef UNICODE

          lstrcpyn(pProcessStr->szCurrentDirectory, wstrbuf, MAX_PATH);
#else
          WideCharToMultiByte(CP_ACP, 0, wstrbuf, -1, 
			  pProcessStr->szCurrentDirectory, MAX_PATH, NULL, NULL);  

#endif

	ZeroMemory(wstrbuf, sizeof(WCHAR)*(MAX_PATH+32));
    lpAddress = ProcParam.ApplicationName.Buffer;
    dwSize = min(ProcParam.ApplicationName.Length, sizeof(WCHAR) * MAX_PATH);

    if (!ReadProcessMemory( hProcess,
                            lpAddress,
                            wstrbuf,
                            dwSize,
                            &dwDummy
                          )
       )
       goto cleanup;

#ifdef UNICODE
	
	lstrcpyn(pProcessStr->szExePath, wstrbuf, MAX_PATH);
	
#else

	WideCharToMultiByte(CP_ACP, 0, wstrbuf, -1, 
		pProcessStr->szExePath, MAX_PATH, NULL, NULL);
#endif

	lstrtrimi(pProcessStr->szExePath, TEXT("\\??\\"));

    bRet = TRUE;

cleanup:

    CloseHandle (hProcess);

     
    return bRet;
}


//////////////////////////////////////////////////////////////////////////

BOOL IsProcessRunning(LPTSTR szFullPathProcessName)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize= sizeof(PROCESSENTRY32);
	MODULEENTRY32 me32  = { 0 };
	HANDLE hProcessSnap = 
		CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
        return FALSE; 
	}
	if (!Process32First(hProcessSnap, &pe32)) 
	{ 
		return FALSE;
	}
	do
	{
		GetProcessModule(pe32.th32ProcessID, 1,
				&me32, sizeof(MODULEENTRY32) );
			if ( lstrcmpi(szFullPathProcessName, me32.szExePath) == 0 )
			{ 
				return TRUE;
			}

	} while(Process32Next(hProcessSnap, &pe32));
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

// typedef BOOL (CALLBACK* LPGUESSPATHOUTPROC)(LPTSTR /*[MAX_PATH]*/, 
// 										  LPARAM/*lParam custom args*/);
INT NtGuessSpecProcessPath(LPTSTR szBaseName, 
						  LPGUESSPATHOUTPROC lpOutProc, /*pass NULL to test proc count */
						  LPARAM lParam
						  )
{
#define MAX_KEY_LENGTH MAX_PATH+1
#define MAX_VALUE_NAME MAX_PATH+1 //max to 16383
//////////////////////////////////////////////////////////////////////////
    INT retCount=0; //return the matched path count
    TCHAR  szNameBuff[MAX_PATH];

	HKEY hKey;
	LONG lRegRetCode; 

	DWORD cValues;              // number of values for key 
    DWORD cbSecurityDescriptor; // size of security descriptor  
    DWORD i;

	TCHAR  szValue[MAX_VALUE_NAME]; 
    DWORD  dwValueNum = MAX_VALUE_NAME;

	if( RegOpenKeyEx( HKEY_CURRENT_USER,
        TEXT("Software\\Microsoft\\Windows\\ShellNoRoam\\MUICache"),
        0, KEY_READ, &hKey) != ERROR_SUCCESS )
	{
		if(hKey) RegCloseKey(hKey);
		return 0;
	}
    // Get the class name and the value count. 
    lRegRetCode = RegQueryInfoKey( hKey, NULL, NULL, NULL, NULL, NULL, 
		NULL, &cValues, NULL, NULL, &cbSecurityDescriptor, NULL);
	
    if (cValues) 
	{
		
        for (i=0, lRegRetCode=ERROR_SUCCESS; i<cValues; i++) 
        { 

			dwValueNum = MAX_VALUE_NAME; 
			szValue[0] = '\0'; 
            lRegRetCode = RegEnumValue(hKey, i, szValue, &dwValueNum, 
				NULL, &cbSecurityDescriptor, NULL, NULL);
			
			if (lRegRetCode == ERROR_SUCCESS ) 
            { 
				if(cbSecurityDescriptor==REG_SZ && 
					 szValue[0]!='@')
				{
					if (!IsFileLocked(szValue))
					{
						continue;
					}
					if( GetFileBaseName(szValue,szNameBuff) <=0 )
					{
						continue;
					}
					if( 0 == lstrcmpi(szNameBuff, szBaseName) )
					{
						retCount++;

						if(lpOutProc)
						{
							if(FALSE==lpOutProc(szValue,lParam) )
							{
								lpOutProc=NULL; //OutProc return FALSE to break OutProc callback¡£
							}
						}
					}
				}
			} 
		}
	}
	
	RegCloseKey(hKey);
	return retCount;
}//GuessSpecProcessPath();...end...

/************************************************************
usage£º
BOOL CALLBACK OutProc(LPTSTR szSpecPath, LPARAM lParam)
{
	HWND hwnd=(HWND)lParam;
	
	SetDlgItemText(hwnd,IDC_EDIT3,szSpecPath);
	return 1;
	
}
**************************************************************/

//////////////////////////////////////////////////////////////////////////
BOOL RunExec(LPTSTR szCommand, UINT uShowWindow, BOOL isWait, DWORD dwMilliseconds)
{
	STARTUPINFO si={0};
	PROCESS_INFORMATION pi={0};

	si.cb = sizeof(si);
	si.wShowWindow=LOWORD(uShowWindow);
 
	
	if( CreateProcess(NULL, szCommand, NULL, NULL, FALSE, NULL, NULL, NULL,  &si, &pi) )
	{
		if(isWait)
		{
			WaitForSingleObject(pi.hProcess, dwMilliseconds);
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL IsWindowInCurrentThreadProcess(HWND hwnd, BOOL IsCheckProcess)
{
	DWORD dwTID;
	DWORD dwPID;

	dwTID=GetWindowThreadProcessId(hwnd, &dwPID);
	return IsCheckProcess?(dwPID==GetCurrentProcessId()):(dwTID==GetCurrentThreadId());

}

//////////////////////////////////////////////////////////////////////////

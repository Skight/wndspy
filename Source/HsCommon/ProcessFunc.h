#ifndef HS_HEADER_DEF_PROCESSFUNC_H
#define HS_HEADER_DEF_PROCESSFUNC_H

#if _MSC_VER >= 1000
#pragma once
#endif


#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>

#define PROCESSBASICINFORMATION 0

typedef struct
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct
{
    ULONG          AllocationSize;
    ULONG          ActualSize;
    ULONG          Flags;
    ULONG          Unknown1;
    UNICODE_STRING Unknown2;
    HANDLE         InputHandle;
    HANDLE         OutputHandle;
    HANDLE         ErrorHandle;
    UNICODE_STRING CurrentDirectory;
    HANDLE         CurrentDirectoryHandle;
    UNICODE_STRING SearchPaths;
    UNICODE_STRING ApplicationName;
    UNICODE_STRING CommandLine;
    PVOID          EnvironmentBlock;
    ULONG          Unknown[9];
    UNICODE_STRING Unknown3;
    UNICODE_STRING Unknown4;
    UNICODE_STRING Unknown5;
    UNICODE_STRING Unknown6;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

typedef struct
{
    ULONG               AllocationSize;
    ULONG               Unknown1;
    HINSTANCE           ProcessHinstance;
    PVOID               ListDlls;
    PPROCESS_PARAMETERS ProcessParameters;
    ULONG               Unknown2;
    HANDLE              Heap;
} PEB, *PPEB;

typedef struct
{
    DWORD ExitStatus;
    PPEB  PebBaseAddress;
    DWORD AffinityMask;
    DWORD BasePriority;
    ULONG UniqueProcessId;
    ULONG InheritedFromUniqueProcessId;
}   PROCESS_BASIC_INFORMATION;

typedef struct
{
#define DEF_CMDLINE_STRBUF_LEN		516
#define DEF_CMDLINE_STR_LEN			512
	TCHAR szCommandLine[DEF_CMDLINE_STRBUF_LEN];
	TCHAR szCurrentDirectory[MAX_PATH];
	TCHAR szExePath[MAX_PATH];
} PROCESS_STRINGS;


BOOL GetProcessInfoDirStrs(DWORD dwId, PROCESS_STRINGS* pProcessStr, HWND hwndSetCmdLineText);

DWORD AdjustProcessPrivileges(HANDLE, LPCTSTR);

BOOL GetProcessModule(DWORD, DWORD, LPMODULEENTRY32, DWORD);
PROCESSENTRY32 GetProcessEntry32(DWORD);

typedef BOOL (CALLBACK* LPGUESSPATHOUTPROC)(LPTSTR, 
											LPARAM);

INT NtGuessSpecProcessPath(LPTSTR szBaseName, 
						  LPGUESSPATHOUTPROC lpOutProc,
						  LPARAM lParam
						  );

BOOL RunExec(LPTSTR szCommand, UINT uShowWindow, BOOL isWait, DWORD dwMilliseconds);

#define WinExecEx(_szCmd, _uShowWnd) RunExec(_szCmd, _uShowWnd, FALSE, 0)

#define WinExecExWait(_szCmd, _uShowWnd, _dwTime) RunExec(_szCmd, _uShowWnd, TRUE, _dwTime)

BOOL IsWindowInCurrentThreadProcess(HWND hwnd, BOOL IsCheckProcess);

#endif //HS_HEADER_DEF_PROCESSFUNC_H

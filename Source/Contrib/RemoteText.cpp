/***************************************************************
Module name: InjCode.cpp
Copyright (c) 2003 Robert Kuster

Notice:	If this code works, it was written by Robert Kuster.
		Else, I don't know who wrote it.

		Use it on your own risk. No responsibilities for
		possible damages of even functionality can be taken.
***************************************************************/

#include <Windows.h>
#include "RemoteText.h"

//---------------------------------------------------------------------
// INJDATA
// Notice: The data structure being injected.
//
typedef LRESULT		(WINAPI *SENDMESSAGE)(HWND,UINT,WPARAM,LPARAM);

typedef struct {
	HWND	hwnd;
	SENDMESSAGE		fnSendMessage;	// pointer to user32!SendMessage

	BYTE	pbText[128 * sizeof(TCHAR)];
} INJDATA, *PINJDATA;


//---------------------------------------------------------------------
// ThreadFunc
// Notice: - the code being injected; 
//		   - the remote copy of this function retrieves the password;
//
//	Return value: password length
//
static DWORD WINAPI ThreadFunc (INJDATA *pData)
{
	// There must be less than a page-worth of local
	// variables used in this function.

	int	nXferred	 = 0;	// number of chars retrieved by WM_GETTEXT

	// Get password
	nXferred = pData->fnSendMessage( pData->hwnd, WM_GETTEXT,
							 sizeof(pData->pbText)/sizeof(TCHAR),
							 (LPARAM)pData->pbText );
	pData->pbText [127 * sizeof(TCHAR)] = __TEXT('\0');	

	// The thread's exit code is the number 
	// of characters retrieved by WM_GETTEXT
	return nXferred;
}

// This function marks the memory address after ThreadFunc.
// int cbCodeSize = (PBYTE) AfterThreadFunc - (PBYTE) ThreadFunc.
static void AfterThreadFunc (void) {
}


//---------------------------------------------------------------------
// GetTextRemote
// Notice: - copies ThreadFunc and INJDATA to the remote process;
//		   - starts the excecution of the remote ThreadFunc;
//
//	Return value: password length;
//
int GetTextRemote (HANDLE hProcess, HWND hWnd, BYTE* pbString, bool fUnicode)
{	
	HINSTANCE	hUser32;
	INJDATA		*pDataRemote;	// the address (in the remote process) where INJDATA will be copied to;
	DWORD		*pCodeRemote;	// the address (in the remote process) where ThreadFunc will be copied to;
	HANDLE		hThread = NULL; // the handle to the thread executing the remote copy of ThreadFunc;
	DWORD		dwThreadId = 0;

	int		  nCharsXferred = 0; // number of chars retrieved by WM_GETTEXT in the remote thread;
	DWORD dwNumBytesXferred = 0; // number of bytes written/read to/from the remote process;
	
	__try {
		hUser32 = GetModuleHandle(__TEXT("USER32.DLL"));
		if (hUser32 == NULL)
			__leave;

		// Initialize INJDATA and then 
		// copy it to the remote process
		INJDATA DataLocal = {
			hWnd,
			(SENDMESSAGE) GetProcAddress(hUser32, fUnicode ? "SendMessageW" : "SendMessageA")			
		};
	
		if( DataLocal.fnSendMessage == NULL )
			__leave;		
		
		// 1. Allocate memory in the remote process for INJDATA
		// 2. Write a copy of DataLocal to the allocated memory
		pDataRemote = (INJDATA*) VirtualAllocEx( hProcess, 0, sizeof(INJDATA), MEM_COMMIT, PAGE_READWRITE );
		if (pDataRemote == NULL)
			__leave;
		WriteProcessMemory( hProcess, pDataRemote, &DataLocal, sizeof(INJDATA), &dwNumBytesXferred );


		// Calculate the number of bytes that ThreadFunc occupies
		const int cbCodeSize = ((LPBYTE) AfterThreadFunc - (LPBYTE) ThreadFunc);

		// 1. Allocate memory in the remote process for the injected ThreadFunc
		// 2. Write a copy of ThreadFunc to the allocated memory
		pCodeRemote = (PDWORD) VirtualAllocEx( hProcess, 0, cbCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE );		
		if (pCodeRemote == NULL)
			__leave;
		WriteProcessMemory( hProcess, pCodeRemote, &ThreadFunc, cbCodeSize, &dwNumBytesXferred );
		

		// Start execution of remote ThreadFunc
		hThread = CreateRemoteThread(hProcess, NULL, 0, 
				(LPTHREAD_START_ROUTINE) pCodeRemote,
				pDataRemote, 0 , &dwThreadId);
		if (hThread == NULL)
			__leave;

		WaitForSingleObject(hThread, INFINITE);

		// Get result (password) back
		ReadProcessMemory( hProcess, pDataRemote, &DataLocal, sizeof(INJDATA), &dwNumBytesXferred);

		if (fUnicode)  wcscpy((LPWSTR) pbString, (LPCWSTR) DataLocal.pbText);
		else		   strcpy((LPSTR)  pbString, (LPCSTR)  DataLocal.pbText);
	}
	__finally {
		if ( pDataRemote != 0 )
			VirtualFreeEx( hProcess, pDataRemote, 0, MEM_RELEASE );

		if ( pCodeRemote != 0 )
			VirtualFreeEx( hProcess, pCodeRemote, 0, MEM_RELEASE );

		if ( hThread != NULL ) {
			GetExitCodeThread(hThread, (PDWORD) &nCharsXferred);
			CloseHandle(hThread);			
		}
	}

	// Return the number of chars retrieved 
	// by WM_GETTEXT in the remote thread.
	return nCharsXferred;
}

///////////////////////////////////////////////////////////////////////////

int GetWindowTextRemoteA (HANDLE hProcess, HWND hWnd, LPSTR	 lpString)
{
	return GetTextRemote (hProcess, hWnd, (BYTE*)lpString, false);
}

int GetWindowTextRemoteW (HANDLE hProcess, HWND hWnd, LPWSTR lpString)
{
	return GetTextRemote (hProcess, hWnd, (BYTE*)lpString, true);
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

BOOL GetRemotePSW(HWND hwndPSW, LPTSTR szPSWbuf)
{
	DWORD dwPID;
	HANDLE hProcess;

	GetWindowThreadProcessId(hwndPSW, &dwPID);
	hProcess = OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|
		PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ, FALSE, dwPID);		

	if( hProcess != NULL )
	{
		GetWindowTextRemote(hProcess, hwndPSW, szPSWbuf);
		CloseHandle(hProcess);
		return TRUE;
	}
	else
	{
		szPSWbuf[0]=0;
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////


//////////////////////// End Of File //////////////////////////////////////
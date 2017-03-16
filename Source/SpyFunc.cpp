#include "WndSpy.h" //Main Header...
#include "GlobalVar.h"
#include "SpyFunc.h"

//////////////////////////////////////////////////////////////////////////

DWORD GetProcessInfoFromWindow(HWND hwnd, SPY_PROCESSINFO* pSPI)
{
	
	ZeroMemory(pSPI,sizeof(SPY_PROCESSINFO));
	
	pSPI->dwThreadID=GetWindowThreadProcessId(hwnd,&pSPI->dwProcessID);
	
	if(pSPI->dwProcessID)
	{
		pSPI->pe32=GetProcessEntry32(pSPI->dwProcessID);
	}
	
#ifdef _HS_ENABLE_ITHREADPRIORITY

	HANDLE hThread;
	if(pSPI->dwThreadID)
	{
		hThread=OpenThread(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, 
			FALSE,pSPI->dwThreadID);
		if(hThread)
		{
			pSPI->iThreadPriority=GetThreadPriority(hThread);
		}
	}

#endif //_HS_ENABLE_ITHREADPRIORITY
	
#if _DEBUG
	return GetLastError();
#else
	return pSPI->dwProcessID;
#endif//!_DEBUG

} //GetProcessInfoFromWindow()

//////////////////////////////////////////////////////////////////////////

VOID GetPSWText(HWND hwndPSW, LPTSTR szPSWbuf)
{

	BOOL bSuccess=FALSE;
	HANDLE hMutex;

	szPSWbuf[0]=0;
	
	if( hMutex=CreateMutex(NULL, FALSE, STR_APP_DLLFILE) )
	{
		if( GetLastError()!=ERROR_ALREADY_EXISTS )
		{
			bSuccess=SpyLib_HookGetPSW(hwndPSW, 
				GetWindowThreadProcessId(hwndPSW, NULL), szPSWbuf);
		}
		CloseHandle(hMutex);
	}

	if(	!bSuccess )
	{
		GetPasswordEditText(hwndPSW, szPSWbuf);
	}
}

//////////////////////////////////////////////////////////////////////////
/**********************
* SpyGetWindowInfo()
* purpose: Get SPY_WININFO for Finder-Tab only.
* The return value indicates target is a Password Edit or not.
* TRUE: Password Edit, FLASE: not...
**********************/
BOOL SpyGetWindowInfo(HWND hwnd, SPY_WININFO* pSWI)
{
	
	DWORD dwResult;
	
	if (IsWindow(hwnd))
	{
		pSWI->hwnd=hwnd;

		ZeroMemory(pSWI->hwndArray, sizeof(HWND)*C_SPYINFO_HWND);

		__try
		{
			pSWI->hwndArray[0]=GetWindow(hwnd,GW_OWNER);
			pSWI->hwndArray[1]=GetAncestor(hwnd,GA_PARENT);
			pSWI->hwndArray[2]=GetWindow(hwnd,GW_CHILD);
			pSWI->hwndArray[3]=GetWindow(hwnd,GW_HWNDNEXT);
			pSWI->hwndArray[4]=GetWindow(hwnd,GW_HWNDPREV);

		}
		__except(1) //EXCEPTION_EXECUTE_HANDLER
		{
			for(int i=0; i<C_SPYINFO_HWND; i++)
			{
				if(	!IsWindow(pSWI->hwndArray[i]) )
				{
					pSWI->hwndArray[i]=NULL;
				}
			}
		}
		
		pSWI->szCaption[0]='\0';

		GetClassName(hwnd,pSWI->szClassName,MAX_CLASS_NAME);
		
		if( (GetWindowLong(hwnd, GWL_STYLE)&ES_PASSWORD) &&
			//Edit Class name in Common Control, Delphi, VB...
			( 
			lstrcmpi(pSWI->szClassName, TEXT("Edit"))==0 ||
			lstrcmpi(pSWI->szClassName, TEXT("TEdit"))==0 ||
			lstrcmpi(pSWI->szClassName, TEXT("Iris.Password"))==0 ||
			lstrcmpi(pSWI->szClassName, TEXT("ThunderTextBox"))==0 ||
			lstrcmpi(pSWI->szClassName, TEXT("ThunderRT6TextBox"))==0 
			) 
			)
		{
			pSWI->isLargeText=FALSE;
			GetPSWText(hwnd, pSWI->szCaption);
			return TRUE;
		}
		else
		{
			SendMessageTimeout(hwnd, WM_GETTEXT, 
				MAX_CAPTION_STRBUF_LEN, (LPARAM)pSWI->szCaption, 
				SMTO_ABORTIFHUNG|SMTO_NOTIMEOUTIFNOTHUNG, 256, &dwResult);
#ifdef UNICODE
			pSWI->szCaption[dwResult]=0;
#endif
			pSWI->isLargeText=(BOOL)
				( dwResult >= MAX_CAPTION_STRBUF_LEN-4 );
		}
	}
	
	return FALSE;
	
} //SpyGetWindowInfo();


//////////////////////////////////////////////////////////////////////////
VOID SpyGetWindowInfoEx(HWND hwnd, SPY_PROCESSINFO* pSPI, SPY_WININFOEX* pSWIex)
{
	
	BOOL IsHooked;
	
	if (IsWindow(hwnd))
	{
		if ( hwnd != pSWIex->swi.hwnd )
		{
			ZeroMemory(&pSWIex->swi,sizeof(SPY_WININFO));
			SpyGetWindowInfo(hwnd, &pSWIex->swi);
		}
		if( !IsWindow(g_hwndBK) )
		{
			g_hwndBK = CreateSpyMsgWnd(g_hInstance);
		}
		_try
		{
			GetProcessInfoFromWindow(hwnd, pSPI);
			
			g_isHookSignal=FALSE;
			
			g_spyinfo_SWIex.IsSelf=(BOOL)
				( pSPI->pe32.th32ProcessID==GetCurrentProcessId() );
			if(	g_spyinfo_SWIex.IsSelf )
			{
				pSPI->pe32.dwFlags=GetPriorityClass(GetCurrentProcess());	
				GetCurrentDirectory(MAX_PATH,pSPI->ProcStrs.szCurrentDirectory);
				lstrcpyn(pSPI->ProcStrs.szCommandLine, GetCommandLine(), 4*MAX_PATH);
				GetModuleFileName(NULL, pSPI->pe32.szExeFile, MAX_PATH);
				SpyLib_BaseGetWndInfoEx(hwnd, pSWIex, g_hwndBK);
				return;
			}
			else
			{
				IsHooked=SpyLib_HookGetWndInfoEx(hwnd, pSPI->dwThreadID, g_hwndBK);
			}
			
			if(IsHooked==FALSE)
			{
				SpyTryGetWndInfoEx(hwnd, pSPI, pSWIex);
			}
			else if (g_isHookSignal==FALSE)
			{
				//MessageBeep(0);
				//UNUSUAL EXCEPTION:
				//Have hooked into target thread successfully,
				//but no signal msg send back in the time-out interval.
				//So see the following...
				if(!IsWindowEnabled(g_hwndBK))
				{
					EnableWindow(g_hwndBK,TRUE);
				}
				SetTimer(g_hwndBK,TIMER_FORWARD_HOOK_SPYWNDINFO_TIMEOUT, 
					g_siWinVer<=WINVER_WIN9X ? 500:100, NULL);
			}
		}
		__except(1) //EXCEPTION_EXECUTE_HANDLER
		{
			MessageBeep(0);
		}
		
	}
	
}//SpyGetWindowInfoEx()


//////////////////////////////////////////////////////////////////////////
VOID SpyTryGetWndInfoEx(HWND hwnd, SPY_PROCESSINFO* pSPI, SPY_WININFOEX* pSWIex)
{
	
	GetProcessInfoDirStrs(pSPI->pe32.th32ProcessID, &pSPI->ProcStrs, NULL);
	
	if( IsFileExists(pSPI->ProcStrs.szExePath) )
	{
		lstrcpyn(pSPI->pe32.szExeFile,pSPI->ProcStrs.szExePath,MAX_PATH);
	}
	else
	{
		GetExeFullNameForPE32Ex(&pSPI->pe32);
	}
	
	SpyLib_BaseGetWndInfoEx(hwnd, pSWIex, g_hwndBK);

} //SpyTryGetWndInfoEx()


//////////////////////////////////////////////////////////////////////////
//GuessProc for NtGuessSpecProcessPath in GetExeFullNameForPE32Ex(1)
BOOL CALLBACK GuessSpecProcessPath_OutProc(LPTSTR szSpecPath, LPARAM lParam)
{
	DEBUG_ERROR_MSG((LPTSTR)lParam);
	lstrcpyn((LPTSTR)lParam, szSpecPath, MAX_PATH);
	return FALSE;
} //GuessSpecProcessPath_OutProc();

BOOL GetExeFullNameForPE32Ex(PROCESSENTRY32* pPe32)
{
	
	BOOL ret=FALSE;
	MODULEENTRY32 me32 = {0};
	
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, 
		FALSE, pPe32->th32ProcessID);
	
	if(hProcess && lpfGetModuleFileNameEx)
	{	
		pPe32->dwFlags=GetPriorityClass(hProcess);
		lpfGetModuleFileNameEx(hProcess,NULL, pPe32->szExeFile,MAX_PATH);
		lstrtrimi(pPe32->szExeFile, TEXT("\\??\\"));
		CloseHandle(hProcess);
		ret=TRUE;
	}
	else
	{
		GetProcessModule(pPe32->th32ProcessID, 1,
			&me32, sizeof(MODULEENTRY32) );
		
		if (IsFileExists(me32.szExePath))
		{
			lstrcpyn(pPe32->szExeFile, 
				me32.szExePath, MAX_PATH);
			ret=TRUE;
		}
		else
		{
			pPe32->th32DefaultHeapID=0;
			pPe32->th32DefaultHeapID = NtGuessSpecProcessPath(
				pPe32->szExeFile, GuessSpecProcessPath_OutProc,
				(LPARAM)pPe32->szExeFile);
			if(pPe32->th32DefaultHeapID>=1)
			{
				ret=TRUE;
			}
		}
		
	}
	
	return ret;

} //GetExeFullNameForPE32Ex()


BOOL GetProcessFileNameFromWindow(HWND hwnd, LPTSTR szFileName, UINT iMaxLen)
{
	PROCESSENTRY32 pe32={0};
	szFileName[0]=0;
	if( IsWindow(hwnd) )
	{
		pe32.dwSize=sizeof(PROCESSENTRY32);
		GetWindowThreadProcessId(hwnd, &pe32.th32ProcessID);
		if( GetExeFullNameForPE32Ex(&pe32) )
		{
			lstrcpyn(szFileName, pe32.szExeFile, iMaxLen);
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "WndSpylib.h"

////////////////////////////////////////////////

#pragma data_seg(".sdata")

SPYAPPDATA	g_SAD={0};
HHOOK		g_hHook=NULL;
HHOOK		g_hHook_Psw=NULL;
HWND		g_hwndTag=NULL;
BOOL		g_bIsOnce=FALSE;
BOOL		g_bIsOnce_Psw=FALSE;
HINT_KSMOD	g_hInstSysMod={NULL};
DWORD		g_dwBuf=0;
WNDPROC		g_wndproc=NULL;
HINSTANCE	g_hWndInstance=NULL;
WINDOWINFO	g_wi={0};
WNDCLASSEX	g_wcex={0};

TCHAR g_szBuf[SPYDLL_LARGE_STR_LEN]={0};
TCHAR g_szSmallBuf[2][SPYDLL_SMALL_STR_LEN]={0};
TCHAR g_szArray[4][MAX_PATH]={0};

#define g_szCmdLine				g_szBuf
#define g_szCurDir				g_szArray[0]
#define g_szExeFileName			g_szArray[1]
#define g_szWndModFileName		g_szArray[2]
#define g_szClsModFileName		g_szArray[3]
#define g_szMenuName			g_szSmallBuf[0]
#define g_szPSWBuf				g_szSmallBuf[1]

#pragma data_seg()
#pragma comment(linker,"/section:.sdata,RWS")

//////////////////////////////////////////////////////////////////////////

VOID wsInitData(SPYAPPDATA* pSAD)
{
	CopyMemory(&g_SAD, pSAD, sizeof(SPYAPPDATA));

}//wsInitData()


HHOOK wsSetWndHook(HWND hwnd, DWORD idThread, int idHook, HOOKPROC lpfn, HHOOK* phHookVar)
{

	HHOOK hHook;

	if( idHook==NULL )
	{
#ifndef UNICODE
		//Set Hook type according to Windows Version.		
		if( g_SAD.siWinVer > WINVER_WIN9X )
		{
			idHook=WH_CALLWNDPROCRET;
		}
		else
		{
			idHook=WH_GETMESSAGE;
		}
#else
		idHook=WH_CALLWNDPROCRET;

#endif //!UNICODE
	}


#ifdef WS_LIB_DATA_DLLNAME
	hHook=SetWindowsHookEx(idHook, lpfn, GetModuleHandle(g_SAD.szDllName), idThread);
#else
	hHook=SetWindowsHookEx(idHook, lpfn, g_SAD.g_hInstWndSpyDll, idThread);
#endif //WS_LIB_DATA_DLLNAME
	
	if( phHookVar )
	{
		//ensure the previous hook to be clear.
		DoEnsureUnHook(phHookVar);
		*phHookVar=hHook;
	}

	if( hHook )
	{
		switch(idHook)
		{
			
		case WH_CALLWNDPROCRET:
			{
				SendMessageTimeout(hwnd, WM_GETTEXTLENGTH, 
					0, 0, SMTO_NOTIMEOUTIFNOTHUNG, 200, NULL);
				break;
			}
#ifndef UNICODE
		case WH_GETMESSAGE:
			{
				PostMessage(hwnd, WM_GETTEXTLENGTH, 0, 0);
				PostMessage(hwnd, WM_GETICON, ICON_BIG, 0);
				Sleep(20); //cause thread switching...
				break;
			}
#endif //!UNICODE
		}
	}

	return hHook;

}//wsSetWndHook()

//////////////////////////////////////////////////////////////////////////

VOID wsUnSetWndHook(HHOOK* phHook)
{
	if( *phHook )
	{
		UnhookWindowsHookEx( *phHook );
		*phHook=NULL;
	}
	
}//wsUnSetWndHook()

//////////////////////////////////////////////////////////////////////////

VOID wsGetWndInfo(HWND hwnd, SPY_WININFOEX* pSWIex, HWND hwndMsgWnd)
{
	g_hwndTag=hwnd;
	LibGetWindowInfo();
	wsReadWndInfo(NULL, pSWIex);
	SendMessageTimeout(hwndMsgWnd, WM_SETTEXT, 
		0, (LPARAM)SPY_BASE_SIGNAL, SMTO_NORMAL, 200 , NULL);

}//wsGetWndInfo()

//////////////////////////////////////////////////////////////////////////

BOOL wsHookGetWndInfo(HWND hwnd, DWORD idThread, HWND hwndMsgWnd)
{

	g_SAD.hwndspyMsg=hwndMsgWnd;
	g_hwndTag=hwnd; //update spy target window...
	g_bIsOnce=FALSE;
	SetHookStub(hwnd, idThread, NULL, HookProc_GetWndInfo, &g_hHook);
	UnSetHook(&g_hHook);
	return g_bIsOnce;

}//wsHookGetWndInfo()

LRESULT CALLBACK HookProc_GetWndInfo(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( g_bIsOnce==FALSE )
	{
		g_bIsOnce=TRUE;
		
		LibGetWindowInfo();
		
		g_dwBuf=GetPriorityClass(GetCurrentProcess());
		
		g_szCurDir[0]='\0', g_szExeFileName[0]='\0', g_szCmdLine[0]='\0';

		GetCurrentDirectory(MAX_PATH, g_szCurDir);
		GetModuleFileName(NULL, g_szExeFileName, MAX_PATH);
		lstrcpyn(g_szCmdLine, GetCommandLine(), SPYDLL_LARGE_STR_LEN);

		SendMessageTimeout(g_SAD.hwndspyMsg, WM_SETTEXT, 0, 
			(LPARAM)SPY_HOOK_SIGNAL, SMTO_NORMAL, 100, NULL);
	}
	
	if( nCode<0 )
	{
		return CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	else
	{
		return 0;
	}

}//HookProc_GetWndInfo()

//////////////////////////////////////////////////////////////////////////

INT wsReadWndInfo(SPY_PROCESSINFO* pSPI, SPY_WININFOEX* pSWIex)
{
	INT iCmdLineLen = 1;

	CopyMemory(&(pSWIex->wi), &g_wi, sizeof(WINDOWINFO));
	CopyMemory(&(pSWIex->wcex), &g_wcex, sizeof(WNDCLASSEX));
	pSWIex->hWndInstance=g_hWndInstance;
	pSWIex->wndproc=g_wndproc;

	lstrcpyn(pSWIex->szClassModuleName, g_szClsModFileName, MAX_PATH);
	lstrcpyn(pSWIex->szWndModuleName, g_szWndModFileName, MAX_PATH);
	lstrcpyn(pSWIex->szClassMenuName, g_szMenuName, SPYDLL_SMALL_STR_LEN);

	if( NULL!=pSPI )
	{
		pSPI->pe32.dwFlags=g_dwBuf;	
		
		iCmdLineLen=lstrlen(g_szCmdLine);

		lstrcpyn(pSPI->ProcStrs.szCommandLine, g_szCmdLine, SPYDLL_CMDLINE_STR_LEN);
		lstrcpyn(pSPI->ProcStrs.szCurrentDirectory, g_szCurDir, MAX_PATH);
		lstrcpyn(pSPI->pe32.szExeFile, g_szExeFileName, MAX_PATH);
	}

 	DoEnsureUnHook(&g_hHook);
	return iCmdLineLen;

}//wsReadWndInfo()


VOID LibGetModuleFileName(HINSTANCE hInst, LPTSTR szFileNameBuf)
{
	DWORD dwErrorCode;
	szFileNameBuf[0]='\0';
	
	if( !GetModuleFileName(hInst, szFileNameBuf,MAX_PATH) )
	{	
		dwErrorCode=GetLastError();
		if(	g_SAD.siWinVer>=WINVER_WIN2K &&
			g_SAD.siWinVer<=WINVER_VISTA )
		{
			if( LibGiveKnownFileName(g_hwndTag, hInst, szFileNameBuf) )
			{
				return;
			}
		}
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			szFileNameBuf, MAX_PATH, NULL);
	}

}//LibGetModuleFileName()


VOID LibGetWindowInfo(VOID)
{
 	ZeroMemory(&g_wi, sizeof(WINDOWINFO));
 	ZeroMemory(&g_wcex, sizeof(WNDCLASSEX));
	
	GetWindowInfo(g_hwndTag, &g_wi);
	GetWindowRect(g_hwndTag, &g_wi.rcWindow);
	GetClientRect(g_hwndTag, &g_wi.rcClient);
	ClientToScreen(g_hwndTag, (POINT*)&g_wi.rcClient);
	ClientToScreen(g_hwndTag, (POINT*)&g_wi.rcClient+1);
	
	if( IsWindowUnicode(g_hwndTag) )
	{
		g_wi.dwStyle=GetWindowLongW(g_hwndTag, GWL_STYLE);
		g_wi.dwExStyle=GetWindowLongW(g_hwndTag, GWL_EXSTYLE);
		g_hWndInstance=(HINSTANCE)GetWindowLongW(g_hwndTag, GWL_HINSTANCE);
		g_wndproc=(WNDPROC)GetWindowLongW(g_hwndTag, GWL_WNDPROC);
		g_wcex.lpszClassName=(LPTSTR)GetClassLongW(g_hwndTag, GCW_ATOM);
		g_wcex.cbClsExtra=(INT)GetClassLongW(g_hwndTag, GCL_CBCLSEXTRA);
		g_wcex.cbWndExtra=(INT)GetClassLongW(g_hwndTag, GCL_CBWNDEXTRA);
		g_wcex.hbrBackground=(HBRUSH)GetClassLongW(g_hwndTag, GCL_HBRBACKGROUND);
		g_wcex.hCursor=(HCURSOR)GetClassLongW(g_hwndTag, GCL_HCURSOR);
		g_wcex.hIcon=(HICON)GetClassLongW(g_hwndTag, GCL_HICON);
		g_wcex.hIconSm=(HICON)GetClassLongW(g_hwndTag, GCL_HICONSM);
		g_wcex.hInstance=(HINSTANCE)GetClassLongW(g_hwndTag, GCL_HMODULE);
		g_wcex.lpszMenuName=(LPTSTR)GetClassLongW(g_hwndTag, GCL_MENUNAME);
		g_wcex.style=(UINT)GetClassLongW(g_hwndTag, GCL_STYLE);
		g_wcex.lpfnWndProc=(WNDPROC)GetClassLongW(g_hwndTag, GCL_WNDPROC);

		if( HIWORD(g_wcex.lpfnWndProc)==0xFFFF )
		{
			g_wcex.lpfnWndProc=(WNDPROC)GetClassLongA(g_hwndTag, GCL_WNDPROC);
		}
		if( g_wcex.lpszMenuName )
		{	
			if( lstrlenW((LPWSTR)g_wcex.lpszMenuName) )
			{
#ifdef UNICODE
				
				lstrcpyn(g_szMenuName, g_wcex.lpszMenuName, SPYDLL_SMALL_STR_LEN);
#else
				WideCharToMultiByte(CP_ACP, NULL, (LPWSTR)g_wcex.lpszMenuName, -1, 
					g_szMenuName, SPYDLL_SMALL_STR_LEN, NULL, NULL);
#endif	
			}
			else //INTRESOURCE...
			{
				wsprintf(g_szMenuName, TEXT("%d"), (DWORD)g_wcex.lpszMenuName);
			}
		}
	}
	else
	{
		g_wi.dwStyle=GetWindowLongA(g_hwndTag, GWL_STYLE);
		g_wi.dwExStyle=GetWindowLongA(g_hwndTag, GWL_EXSTYLE);
		g_hWndInstance=(HINSTANCE)GetWindowLongA(g_hwndTag, GWL_HINSTANCE);
		g_wndproc=(WNDPROC)GetWindowLongA(g_hwndTag, GWL_WNDPROC);
		g_wcex.lpszClassName=(LPTSTR)GetClassLongA(g_hwndTag, GCW_ATOM);
		g_wcex.cbClsExtra=(INT)GetClassLongA(g_hwndTag, GCL_CBCLSEXTRA);
		g_wcex.cbWndExtra=(INT)GetClassLongA(g_hwndTag, GCL_CBWNDEXTRA);
		g_wcex.hbrBackground=(HBRUSH)GetClassLongA(g_hwndTag, GCL_HBRBACKGROUND);
		g_wcex.hCursor=(HCURSOR)GetClassLongA(g_hwndTag, GCL_HCURSOR);
		g_wcex.hIcon=(HICON)GetClassLongA(g_hwndTag, GCL_HICON);
		g_wcex.hIconSm=(HICON)GetClassLongA(g_hwndTag, GCL_HICONSM);
		g_wcex.hInstance=(HINSTANCE)GetClassLongA(g_hwndTag, GCL_HMODULE);
		g_wcex.lpszMenuName=(LPTSTR)GetClassLongA(g_hwndTag, GCL_MENUNAME);
		g_wcex.style=(UINT)GetClassLongA(g_hwndTag, GCL_STYLE);
		g_wcex.lpfnWndProc=(WNDPROC)GetClassLongA(g_hwndTag, GCL_WNDPROC);

		if(	HIWORD(g_wcex.lpfnWndProc)==0xFFFF )
		{
			g_wcex.lpfnWndProc=(WNDPROC)GetClassLongW(g_hwndTag, GCL_WNDPROC);
		}
		if( g_wcex.lpszMenuName )
		{

			if( lstrlenA((LPSTR)g_wcex.lpszMenuName) )
			{

#ifdef UNICODE
				MultiByteToWideChar(CP_ACP, 0, 
					(LPSTR)g_wcex.lpszMenuName, -1, g_szMenuName, SPYDLL_SMALL_STR_LEN);
#else
				lstrcpyn(g_szMenuName, g_wcex.lpszMenuName, SPYDLL_SMALL_STR_LEN);
#endif
			}
			else //INTRESOURCE...
			{
 				wsprintf(g_szMenuName, TEXT("%d"), (DWORD)g_wcex.lpszMenuName);
			}

		}
	}

	if( !g_wcex.lpszMenuName )
	{
		lstrcpyn(g_szMenuName, TEXT("(None)"), SPYDLL_SMALL_STR_LEN);
	}

	LibGetModuleFileName(g_wcex.hInstance, g_szClsModFileName);
	LibGetModuleFileName(g_hWndInstance, g_szWndModFileName);

}//LibGetWindowInfo()
//////////////////////////////////////////////////////////////////////////

BOOL LibGiveKnownFileName(HWND hwndTag, HINSTANCE hInst, LPTSTR szFileNameBuf)
{
	//not actually gain access of these special system modules.
	//so, here just friendly and playfully
	//try to give file name for some known special modules...
	
#define MAX_SPEC_NAME_LEN	32
	
	TCHAR szClassName[MAX_SPEC_NAME_LEN];
	TCHAR szPlayful[MAX_SPEC_NAME_LEN];
	
	GetSystemDirectory(szFileNameBuf, MAX_PATH);
	
	if( szFileNameBuf[lstrlen(szFileNameBuf)-1] != '\\' )
	{
		lstrcat(szFileNameBuf, TEXT("\\"));
	}
	
	if( g_hInstSysMod.win32k_sys==NULL )
	{
		g_hInstSysMod.win32k_sys=(HINSTANCE)
			GetWindowLongW(GetDesktopWindow(), GWL_HINSTANCE);
	}
	
	// here g_hInstSysMod.win32k_sys=hinstance of desktop associated module
	
	if( g_hInstSysMod.win32k_sys==hInst )
	{
		szPlayful[0]='w', szPlayful[1]='i', szPlayful[2]='n', szPlayful[3]='3';
		szPlayful[4]='2', szPlayful[5]='k', szPlayful[6]='.', szPlayful[7]='s';
		szPlayful[8]='y', szPlayful[9]='s', szPlayful[10]='\0';

		lstrcat(szFileNameBuf, szPlayful);
		return TRUE;
	}
	if( g_hInstSysMod.winsrv_dll==NULL )
	{
		GetClassName(hwndTag, szClassName, MAX_SPEC_NAME_LEN);
		
		szPlayful[0]='C', szPlayful[1]='o', szPlayful[2]='n', szPlayful[3]='s'; 
		szPlayful[4]='o', szPlayful[5]='l', szPlayful[6]='e', szPlayful[7]='W'; 
		szPlayful[8]='i', szPlayful[9]='n', szPlayful[10]='d', szPlayful[11]='o';
		szPlayful[12]='w', szPlayful[13]='C', szPlayful[14]='l', szPlayful[15]='a';
		szPlayful[16]='s', szPlayful[17]='s', szPlayful[18]='\0';
		
		if( 0==lstrcmpi(szPlayful, szClassName) )
		{
			g_hInstSysMod.winsrv_dll=hInst;
		}
		else
		{
			return FALSE;
		}
	}//fall through...
	if( hInst && g_hInstSysMod.winsrv_dll==hInst )
	{
		szPlayful[0]='w', szPlayful[1]='i', szPlayful[2]='n', szPlayful[3]='s';
		szPlayful[4]='r', szPlayful[5]='v', szPlayful[6]='.', szPlayful[7]='d';
		szPlayful[8]='l', szPlayful[9]='l', szPlayful[10]='\0';

		lstrcat(szFileNameBuf, szPlayful);
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL wsHookGetWndText(HWND hwnd, DWORD idThread, LPTSTR szPasswordBuf)
{

	DoEnsureUnHook(&g_hHook_Psw);
	g_bIsOnce_Psw=FALSE;
	if( SetHookStub(hwnd, idThread, NULL, HookProc_GetWndText, &g_hHook_Psw) )
	{
		UnSetHook(&g_hHook_Psw);
		lstrcpyn(szPasswordBuf, g_szPSWBuf, SPYDLL_SMALL_STR_LEN);
	}
	return g_bIsOnce_Psw;

}//wsHookGetWndText()

LRESULT CALLBACK HookProc_GetWndText(int nCode, WPARAM wParam, LPARAM lParam)
{
	if( g_bIsOnce_Psw==FALSE )
	{
		g_bIsOnce_Psw=TRUE;

		g_szPSWBuf[0]='\0';
		
		SendMessageTimeout(((LPCWPRETSTRUCT)lParam)->hwnd, 
			WM_GETTEXT, SPYDLL_SMALL_STR_LEN, (LPARAM)g_szPSWBuf, SMTO_NORMAL, 100, NULL);
	}

	if( nCode<0 )
	{
		return CallNextHookEx(g_hHook_Psw, nCode, wParam, lParam);
	}
	else
	{
		return 0;
	}

}//HookProc_GetWndText()

//////////////////////////////////////////////////////////////////////////

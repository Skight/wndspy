//////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG
	#if _MSC_VER <=1200
	#pragma comment(linker, "/OPT:NOWIN98")
	#pragma comment(linker, "/OPT:REF")
	#pragma comment(linker, "/OPT:ICF")
	#endif
#endif

//	#pragma comment(linker, "/VERBOSE")

//////////////////////////////////////////////////////////////////////////

#include "WndSpy.h"
#include "GlobalVar.h"

//////////////////////////////////////////////////////////////////////////

ATOM RegisterClassSpyBackMsgWnd(HINSTANCE hInstance)
{
	WNDCLASSEX wcex={0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = (WNDPROC)WndProc_MsgBackWnd;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = STR_SPYMSGWND_CLASS;
	return RegisterClassEx(&wcex);

} //RegisterClassSpyBackMsgWnd()

ATOM RegisterClassSpyDlgClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	GetClassInfo(NULL, TEXT("#32770"), &wc);
	wc.style = CS_DBLCLKS|CS_SAVEBITS;

#ifndef _WC_HICON_LOADIMAGE

	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));

#else //def _WC_HICON_LOADIMAGE

	wc.hIcon = (HICON)LoadImage(g_hInstance, 
			MAKEINTRESOURCE(IDI_MAIN), IMAGE_ICON, 
			GetSystemMetrics(SM_CXSMICON), 
			GetSystemMetrics(SM_CYSMICON), LR_SHARED);

#endif //!_WC_HICON_LOADIMAGE

	wc.lpszClassName = STR_SPYMAINDLG_CLASS;
	return RegisterClass(&wc);

} //RegisterClassSpyDlgClass()

//////////////////////////////////////////////////////////////////////////

LPTSTR GetInstanceDir(LPTSTR lpStrDirBuf)
{
	TCHAR szBuf[MAX_PATH];
	GetModuleFileName(NULL, szBuf, MAX_PATH);
	GetFilePathName(szBuf, lpStrDirBuf);
	return lpStrDirBuf;

} //GetInstanceDir()


LPTSTR GetAppProfileName(LPTSTR szProfileName, LPTSTR szFileExt)
{
	TCHAR szAppFileName[MAX_PATH];
	GetModuleFileName(NULL, szAppFileName, MAX_PATH);
	GetFileNameNoExtension(szAppFileName, szProfileName);
	return lstrcat(szProfileName, szFileExt);

} //GetAppProfileName()


LPTSTR ReadUserPathFromINI_EX(LPTSTR szDirPathBuf, LPTSTR szIniKey, BOOL isDefInit)
{
	TCHAR szIniFileName[MAX_PATH];
	
	GetPrivateProfileString(STR_INI_SEC_RECENT, szIniKey, NULL, 
		szDirPathBuf, MAX_PATH,  GetIniFileName(szIniFileName));

	if( !IsDirectoryExists(szDirPathBuf) )
	{
		if(isDefInit)
		{
			GetInstanceDir(szDirPathBuf);
		}
		else
		{
			szDirPathBuf[0]='\0';
		}
	}
	return szDirPathBuf;

} //ReadUserPathFromINI_EX()


BOOL WriteUserPathToINI(LPTSTR szFilePath, BOOL IsDir, LPTSTR szIniKey)
{
	TCHAR szIniFileName[MAX_PATH];
	TCHAR szDirPath[MAX_PATH];
	if(!IsDir)
	{
		GetFilePathName(szFilePath, szDirPath);
	}
	else
	{
		lstrcpyn(szDirPath, szFilePath, MAX_PATH);
	}
	return WritePrivateProfileString(STR_INI_SEC_RECENT, 
		szIniKey, szDirPath, GetIniFileName(szIniFileName));

} //WriteUserPathToINI()


BOOL IsAutoOpenSavedFile(void)
{
	TCHAR szIniFileName[MAX_PATH];
	return !GetPrivateProfileInt(STR_INI_SEC_RECENT, 
		TEXT("NoAutoOpen"), 0, GetIniFileName(szIniFileName));

} //IsAutoOpenSavedFile()


VOID LoadAppOption(void)
{
	TCHAR szIniFileName[MAX_PATH];

#ifndef _OPT_SHAREDLL
	g_option.IsShareDll=TRUE;
#endif //!_OPT_SHAREDLL

	if( !IsFileExists(GetIniFileName(szIniFileName)) )
	{

#ifdef _OPT_SHAREDLL
		g_option.IsShareDll=TRUE;
#endif//_OPT_SHAREDLL

		g_option.IsLockUpdate=TRUE;
		g_option.IsShowPosCo=TRUE;
		g_option.IsNoInvisible=TRUE;
		g_option.IsPlaySound=TRUE;
		g_option.IsPrefix=TRUE;
		g_option.siAlpha=TRANS_ALPHA_DEFAULT;
		g_option.SpyHotkey.Hotkey=DEF_SPYHOTKEY;
		g_option.color=BASE_COLOR_RGB;
		
		WriteAppOption(TRUE);
	}
	else
	{

#ifdef _OPT_SHAREDLL
		g_option.IsShareDll=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("ShareDLL"), 1, szIniFileName);
#endif //_OPT_SHAREDLL

		g_option.IsMultiInst=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("MultiInst"), 0, szIniFileName);
		g_option.IsPrefix=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("HexPrefix"), 1, szIniFileName);
		g_option.IsShowPosCo=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("ShowPosCo"), 1, szIniFileName);
		g_option.IsTopMost=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("TopMost"), 0, szIniFileName);
		g_option.IsAutoHide=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("AutoHide"), 0, szIniFileName);
		g_option.IsLockUpdate=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("LockUpdate"), 1, szIniFileName);
		g_option.IsPlaySound=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("PlaySound"), 1, szIniFileName);
		g_option.IsNoInvisible=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("NoInvisible"), 0, szIniFileName);
		g_option.IsDigFind=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("DigFind"), 0, szIniFileName);
		g_option.IsSelfFind=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("SelfFind"), 0, szIniFileName);
		g_option.IsTrans=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("Trans"), 0, szIniFileName);
		g_option.IsShowTrayIcon=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("TrayIcon"), 0, szIniFileName);
		g_option.siAlpha=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("TransAlpha"), 64, szIniFileName);
		g_option.siAlpha=min(g_option.siAlpha, TRANS_ALPHA_MAX);
		g_option.siAlpha=max(g_option.siAlpha, TRANS_ALPHA_MIN);
		
		g_option.wFlag=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("FocusFlag"), 0, szIniFileName);
		g_option.color=
			GetPrivateProfileInt(STR_INI_SEC_SETTING, TEXT("FocusColor"), BASE_COLOR_RGB, szIniFileName);

		GetPrivateProfileStruct(STR_INI_SEC_SETTING, TEXT("SpyHotkey"), 
			&g_option.SpyHotkey, sizeof(SPY_HOTKEY_INFO), szIniFileName);
	}

	g_szFormat=g_option.IsPrefix?g_szVarFormat[1]:g_szVarFormat[0];

} //LoadAppOption()


VOID WriteAppOption(BOOL isAll)
{
	TCHAR szIniFileName[MAX_PATH];
	GetIniFileName(szIniFileName);
	
	if(isAll)
	{
		WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("MultiInst"), 
			MAKEBOOLSTR(g_option.IsMultiInst), szIniFileName);
		WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("PlaySound"), 
			MAKEBOOLSTR(g_option.IsPlaySound), szIniFileName);
	}
	
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("TopMost"), 
		MAKEBOOLSTR(g_option.IsTopMost), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("TrayIcon"),
		MAKEBOOLSTR(g_option.IsShowTrayIcon), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("HexPrefix"), 
		MAKEBOOLSTR(g_option.IsPrefix), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("ShowPosCo"), 
		MAKEBOOLSTR(g_option.IsShowPosCo), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("NoInvisible"), 
		MAKEBOOLSTR(g_option.IsNoInvisible), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("AutoHide"), 
		MAKEBOOLSTR(g_option.IsAutoHide), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("LockUpdate"), 
		MAKEBOOLSTR(g_option.IsLockUpdate), szIniFileName);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("Trans"),
		MAKEBOOLSTR(g_option.IsTrans), szIniFileName);
	
	wsprintf(g_strBuf, TEXT("%d"), g_option.wFlag);
	WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("FocusFlag"), g_strBuf, szIniFileName);	
	
	if(isAll)
	{	
		wsprintf(g_strBuf, TEXT("%d"), g_option.siAlpha);
		WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("TransAlpha"), g_strBuf, szIniFileName);
		
		wsprintf(g_strBuf, TEXT("%d"), g_option.color);
		WritePrivateProfileString(STR_INI_SEC_SETTING, TEXT("FocusColor"), g_strBuf, szIniFileName);
		
		WritePrivateProfileStruct(STR_INI_SEC_SETTING, TEXT("SpyHotkey"), 
			&g_option.SpyHotkey, sizeof(SPY_HOTKEY_INFO), szIniFileName);
	}
	
} //WriteAppOption()

//////////////////////////////////////////////////////////////////////////

void UninitSpyRuntimeLib(LPTSTR szDllName)
{
	if( g_option.IsShareDll )
	{
		return;
	}
	TCHAR szPath[MAX_PATH]={0};
	TCHAR szBuf[MAX_PATH]={0};
	if( lstrlen(szDllName) )
	{
		GetFilePathName(szDllName, szPath);
		FileOperation(szPath, NULL, FO_DELETE);
		GetFilePathName(szPath, szBuf);
		FileOperation(szBuf, NULL, FO_DELETE);
	}

} //UninitSpyRuntimeLib()


BOOL InitSpyRuntimeLib(LPTSTR szDllName)
{

	BOOL isLoaded=FALSE;
	FILETIME ft;
	SPYAPPDATA SAD;
	TCHAR szPath[MAX_PATH]={0};
	TCHAR szBuf[MAX_PATH]={0};

#ifndef WS_LIB_DATA_DLLNAME
#define WS_RT_DLL_NAME	szDllName
#else
#define WS_RT_DLL_NAME	SAD.szDllName
#endif //WS_LIB_DATA_DLLNAME

	SAD.siWinVer=g_siWinVer;
	
	if( g_option.IsShareDll )
	{
		GetModuleFileName(NULL, szBuf, MAX_PATH);
		GetFilePathName(szBuf, szPath);
		wsprintf(WS_RT_DLL_NAME, (TEXT("%s\\")STR_APP_DLLFILE), szPath);

#ifdef WS_CHECK_TIMESTAMP

		FILETIME ftOldDll;
		GetFileTimeEx(szBuf, NULL, NULL, &ft);
		GetFileTimeEx(WS_RT_DLL_NAME, NULL, NULL, &ftOldDll);

		if( CompareFileTime(&ft, &ftOldDll)==0 )
		{
			isLoaded=LoadSpyLibFuncs(WS_RT_DLL_NAME);
		}
		if( !isLoaded )
		{
			DoFreeLibrary(g_hInst.wndspy_dll);
			CreateSpyLibFile(WS_RT_DLL_NAME);

			SetFileTimeEx(WS_RT_DLL_NAME, NULL, NULL, &ft);
			isLoaded=LoadSpyLibFuncs(WS_RT_DLL_NAME);
		}
		
#else //not definded WS_CHECK_TIMESTAMP
		
		if( CHECK_WNDSPY_DLL_SIZE == 
			DoGetFileSize(WS_RT_DLL_NAME) )
		{
			isLoaded=LoadSpyLibFuncs(WS_RT_DLL_NAME);
		}
		if( !isLoaded )
		{
			DoFreeLibrary(g_hInst.wndspy_dll);
			CreateSpyLibFile(WS_RT_DLL_NAME);
			GetFileTimeEx(szBuf, NULL, NULL, &ft);
			SetFileTimeEx(WS_RT_DLL_NAME, NULL, NULL, &ft);

			isLoaded=LoadSpyLibFuncs(WS_RT_DLL_NAME);
		}

#endif //if-else WS_CHECK_TIMESTAMP

	}
	
	if( !isLoaded ) //if loading wndspy.dll failed, try to create new in temp directory...
	{		
		DoFreeLibrary(g_hInst.wndspy_dll);
		g_option.IsShareDll=FALSE;
		GetTempPath(MAX_PATH, szPath);
		GetLongPathName(szPath, szBuf, MAX_PATH);
		if(szBuf[lstrlen(szBuf)-1] != '\\')
		{
			lstrcat(szBuf, TEXT("\\"));
		}
		lstrcat(szBuf, STR_APP_TEMPDIR);
		CreateDirectory(szBuf, NULL);
		
		SYSTEMTIME st;
		GetLocalTime(&st);
		SystemTimeToFileTime(&st, &ft);
		wsprintf(szPath, TEXT("%s\\%x%x"), szBuf, *((WORD*)&ft+2), *(DWORD*)&ft);
		CreateDirectory(szPath, NULL);
		SetFileAttributes(szBuf, FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
		SetFileAttributes(szPath, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
		
		wsprintf(WS_RT_DLL_NAME, (TEXT("%s\\")STR_APP_DLLFILE), szPath);
		CreateSpyLibFile(WS_RT_DLL_NAME);
		isLoaded=LoadSpyLibFuncs(WS_RT_DLL_NAME);
	}

#ifdef WS_LIB_DATA_DLLNAME
	lstrcpyn(szDllName, SAD.szDllName, MAX_PATH);
#else
	SAD.g_hInstWndSpyDll=g_hInst.wndspy_dll;
#endif//WS_LIB_DATA_DLLNAME
	
	if(isLoaded)
	{
		SpyLib_InitData(&SAD);
	}

	return isLoaded;

} //InitSpyRuntimeLib()


BOOL CreateSpyLibFile(LPTSTR szDllName)
{
	HRSRC hRsrc;
	HGLOBAL hGl;
	LPVOID lpRes;
	DWORD dwSize, dwByteWritten;
	HANDLE hFile;
	
	hRsrc=FindResource(NULL, MAKEINTRESOURCE(IDR_WNDSPYDLL), RT_RCDATA);
	if( hRsrc==NULL ) 
	{
		DEBUG_ERROR_MSG(TEXT("FindResource"))
			return FALSE;
	}
	
	dwSize=SizeofResource(NULL, hRsrc); 
	if( dwSize==0 ) 
	{
		DEBUG_ERROR_MSG(TEXT("SizeofResource"))
			return FALSE;
	}
	
	hGl=LoadResource(NULL, hRsrc); 
	if( hGl==NULL ) 
	{
		DEBUG_ERROR_MSG(TEXT("LoadResource"))
			return FALSE;
	}
	
	lpRes=LockResource(hGl); 
	if( lpRes==NULL ) 
	{
		DEBUG_ERROR_MSG(TEXT("LockResource"))
			return FALSE;
	}
	
	hFile=CreateFile(szDllName, GENERIC_WRITE, 
		NULL, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, NULL); 
	
	if( hFile==INVALID_HANDLE_VALUE )
	{
		DEBUG_ERROR_MSG(TEXT("CreateFile"))
		return FALSE;
	}
	
	if ( !WriteFile(hFile, lpRes, dwSize, &dwByteWritten, NULL) ) 
	{
		DEBUG_ERROR_MSG(TEXT("WriteFile"))
	}
	
	CloseHandle(hFile);
	FreeResource(hGl);
	
	if( dwByteWritten!=dwSize )
	{
		DEBUG_ERROR_MSG(TEXT("WriteFile SIZE"))
		return FALSE;
	}
	
	return TRUE;

} //CreateSpyLibFile()

//////////////////////////////////////////////////////////////////////////

BOOL LoadSpyLibFuncs(LPTSTR szDllName)
{
	BOOL bSuccess=TRUE;

	g_hInst.wndspy_dll = LoadLibrary(szDllName);
	
	HS_LOAD_LIBPROC_EX(g_hInst.wndspy_dll, 
		lpfSpyLibInitHookData, 
		LPFSPYLIB_INITDATA, "wsInitData", bSuccess);
	
	HS_LOAD_LIBPROC_EX(g_hInst.wndspy_dll, 
		lpfSpyLibHookGetWndInfoEx, 
		LPFSPYLIB_HOOKGETWNDINFO, "wsHookGetWndInfo", bSuccess);
	
	HS_LOAD_LIBPROC_EX(g_hInst.wndspy_dll, 
		lpfSpyLibBaseGetWndInfoEx, 
		LPFSPYLIB_BASEGETWNDINFO, "wsGetWndInfo", bSuccess);
	
	HS_LOAD_LIBPROC_EX(g_hInst.wndspy_dll, 
		lpfSpyLibReadWndInfoEx, 
		LPFSPYLIB_READWNDINFO, "wsReadWndInfo", bSuccess);
	
	HS_LOAD_LIBPROC_EX(g_hInst.wndspy_dll, 
		lpfSpyLibHookGetPSW, 
		LPFSPYLIB_HOOKGETPSW, "wsHookGetWndText", bSuccess);
	
	return bSuccess;

} //LoadSpyLibFuncs()

//////////////////////////////////////////////////////////////////////////

#define SLOW_LOAD_INTERVAL	300

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, INT iCmdShow)
{

	INT iExitCode;
	HANDLE hMutex;
	HANDLE hSpyDllLock;

	//  {{{
	APP_TEST_BLOCK_HEADER(hInstance)
		APP_TEST_BLOCK_SWITCHER()
		
#ifndef _DEBUG
		SET_MODE_NO_ERROR_MSGBOX();
#endif //!_DEBUG
	
	//  }}}


	LoadAppOption();  // Initialize configuration.
	
	hMutex = CreateMutex(NULL, FALSE, STR_SPYMAINDLG_CLASS);
	
#ifdef APP_ENVENT_PROMPT
	if( hMutex == NULL )
	{
		MessageBeep(MB_ICONEXCLAMATION);
	}
#endif //APP_ENVENT_PROMPT
	
	if( !g_option.IsMultiInst && 
		GetLastError() == ERROR_ALREADY_EXISTS )
	{
		BringWindowToForeground(FindWindow(STR_SPYMAINDLG_CLASS, NULL));
	}
	else
	{
		// begin machine speed checking...

		INT iDelta=-(INT)GetTickCount();

		InitCommonControls();
		g_hInstance=hInstance;
		g_siWinVer=GetWinVerType();
		
		//Set Debug Privilege...

		if( ERROR_SUCCESS != 
			AdjustProcessPrivileges(GetCurrentProcess(), SE_DEBUG_NAME) )
		{
			DEBUG_ERROR_MSG(TEXT("AdjustProcessPrivileges ERROR!"))
		}

		lpfSetLayeredWindowAttributes = (LPFSETLAYEREDWINDOWATTRIBUTES)
			LoadLibraryProc(&g_hInst.user32_dll, TEXT("USER32.DLL"), "SetLayeredWindowAttributes");
		
#ifdef UNICODE 

		lpfGetModuleFileNameEx = (LPFGETMODULEFILENAMEEX)
			LoadLibraryProc(&g_hInst.psapi_dll, TEXT("PSAPI.DLL"), "GetModuleFileNameExW");
#else //!UNICODE
		lpfGetModuleFileNameEx = (LPFGETMODULEFILENAMEEX)
			LoadLibraryProc(&g_hInst.psapi_dll, TEXT("PSAPI.DLL"), "GetModuleFileNameExA");

#endif //+-UNICODE
		
		InitUxThemeLibrary(g_hInst.uxtheme_dll);
		OnInit_EnableThemeDialogTexture(g_hInst.uxtheme_dll);
		
		g_hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL));
		
		if( !RegisterClassSpyDlgClass(hInstance) || 
			!RegisterClassSpyBackMsgWnd(hInstance) )
		{

#ifdef APP_ENVENT_PROMPT

			ShowLastError(NULL);
#endif
			CloseHandle(hMutex);
			return INIT_ERROR;
		}
		
		CoInitialize(NULL);  //Init COM lib, for compatibility...

		MSG msg;
		HWND hwndMainDlg;

		// Create SpyMsgWnd Window...
		// A background window for dispatching message and some internal rountines...
		CreateSpyMsgWnd(hInstance);

		// Create SpyMainDlg Window...
		hwndMainDlg=CreateDialog(hInstance, 
			MAKEINTRESOURCE(IDD_SPYMAIN), NULL, DlgProc_SpyMain);

		// Initialize wndspy.dll...
		InitSpyRuntimeLib(g_szSpyDllName);
		// Lock wndspy.dll image to prevent it from being renamed...
		hSpyDllLock = DoLockFile(g_szSpyDllName);
		
		// Finish speed checking...
		iDelta+=(INT)GetTickCount();
		if( iDelta > SLOW_LOAD_INTERVAL )  // Is too slow?
		{
			// If the machine is in a slow speed,
			// call PlaySound(NULL) to cause some pre-loading...
			PlaySound(NULL,g_hInstance, SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
		}

		ShowWindow(hwndMainDlg, iCmdShow==SW_SHOWMAXIMIZED?SW_RESTORE:iCmdShow);

		//	{{ OPTION_BENGIN
		// OnRefreshFinderDlgEdits(g_TabDlgHdr.CDI[TAB_FINDER].hwnd);
		MACRO_TRIM_MEMORY(g_siWinVer>WINVER_WIN9X)
		//	}} OPTION_END

		while( GetMessage(&msg, NULL, 0, 0) )
		{
			if( g_IsFinding == FALSE && 
				msg.message == WM_KEYDOWN && 
				msg.wParam == VK_TAB && 
				(GetKeyState(VK_CONTROL)&0x8000) ) //process TabControl's switching.
			{

				if( GetKeyState(VK_SHIFT)&0x8000 )
				{
					iDelta = C_MAIN_TAB_PAGES - 1;
				}
				else
				{
					iDelta = 1;
				}

				TabCtrl_SetCurSel(g_TabDlgHdr.hwndTab, 
					(TabCtrl_GetCurSel(g_TabDlgHdr.hwndTab) + iDelta) % C_MAIN_TAB_PAGES);
				OnTabSelChanged();
				
			}
			else if( 
				!TranslateAccelerator(hwndMainDlg, g_hAccel, &msg) && 
				!IsDialogMessage(hwndMainDlg, &msg) )
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
		} //while
		
		iExitCode = msg.wParam; // Set Exit code...

		CoUninitialize();  // Uninitialize COM lib...

		// Save configuration if needed...
		
		if( IS_FLAG_SETTED(g_dwBitFlag, 
			BOOLEAN_BIT_OPTIONUPDATE) )
		{
			WriteAppOption(FALSE);
		}

		// Any other uninitializations...

		DoUnLockFile(hSpyDllLock);
		DoFreeLibrary(g_hInst.wndspy_dll);
		UninitSpyRuntimeLib(g_szSpyDllName);

		DoFreeLibrary(g_hInst.psapi_dll);
		DoFreeLibrary(g_hInst.user32_dll);
		UnInitUxThemeLibrary(g_hInst.uxtheme_dll);

	} //if...else...all...
	
	DoCloseHandle(hMutex);
	return iExitCode;
	
} //WinMain();

//////////////////////////////////////////////////////////////////////////
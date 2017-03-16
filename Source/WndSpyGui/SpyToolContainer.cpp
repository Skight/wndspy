#include "WndSpyGui.h"
#include "SpyToolContainer.h"

//////////////////////////////////////////////////////////////////////////

ATOM RegisterClassSpyToolContainer(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = (WNDPROC)WndProc_ToolContainer;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN));
	wcex.hIconSm = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = STR_SPYTOOLCONTAINNER_CLASS;
	return RegisterClassEx(&wcex);
	
} //RegisterClassSpyToolContainer()

VOID ThreadProcToolContainer(PVOID pFoo)
{
	MSG msg;
	TCHAR szText[TINY_STRBUF_LEN];

	CoInitialize(0);

	LoadString(g_hInstance, IDS_TC_TITLE, szText, TINY_STRBUF_LEN);
	
	g_hwndTC = CreateWindowEx(g_option.IsTopMost? WS_EX_TOPMOST:NULL, 
		STR_SPYTOOLCONTAINNER_CLASS, szText,
		WS_DISABLED|WS_POPUP, -6, -6, 0, 0, NULL, NULL, g_hInstance, NULL);
	
	if(g_hwndTC!=NULL)
	{
		while( GetMessage(&msg, NULL, 0, 0)  )
		{
			if(g_hwnd_ColorSpy)
			{
				if(IsDialogMessage(g_hwnd_ColorSpy, &msg))
				{
					continue;
				}
			}
			if(g_hwnd_PSWsniffer)
			{
				if(IsDialogMessage(g_hwnd_PSWsniffer, &msg))
				{
					continue;
				}
			}
			if(g_hwnd_WndEnumEr)
			{
				if( TranslateAccelerator(g_hwnd_WndEnumEr, g_hAccel, &msg) || 
					IsDialogMessage(g_hwnd_WndEnumEr, &msg))
				{
					continue;
				}
			}
			if(g_hwnd_WndModEr)
			{
				if(IsDialogMessage(g_hwnd_WndModEr, &msg))
				{
					continue;
				}
			}
			
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	CoUninitialize();
	_endthread();
	
} //ThreadProcToolContainer()

HWND GetToolContainerWindow(VOID)
{
	INT iTimeOut=20;
	if( g_hwndTC==NULL )
	{
		RegisterClassSpyToolContainer(g_hInstance);
	}
	if( !IsWindow(g_hwndTC) )
	{
		if(0 < _beginthread(ThreadProcToolContainer, 0, (PVOID)NULL) )
		{
			while( !IsWindow(g_hwndTC) && iTimeOut > 0 )
			{
				Sleep(10);
				iTimeOut--;
			}
		}
	}
	return g_hwndTC;
	
} //GetToolContainerWindow()

BOOL IsToolWindowExists(VOID)
{
	for(int i=0; i<C_TOOLS; i++)
	{
		if( IsWindow(g_hwndTool[i]) )
		{
			return TRUE;
		}
	}

	return FALSE;
	
} //IsToolWindowExists()

VOID DoDestroyToolWindows(VOID)
{
	int i;

	for(i=0; i<C_TOOLS; i++)
	{
		if(g_hwndTool[i])
		{
			PostMessage(g_hwndTool[i], WM_CLOSE, 0, 0);
			ShowWindow(g_hwndTool[i], SW_HIDE);
		}
	}
	for(i=0; i<C_TOOLS; i++)
	{
		if(g_hwndTool[i])
		{
			DestroyWindow(g_hwndTool[i]);
		}
	}
	

} //DoDestroyToolWindows()

VOID OnRunToolCommand(HWND hwndTool, UINT idDialogRes, DLGPROC lpToolDlgProc)
{
	if( !IsWindow(hwndTool) )
	{
		hwndTool=CreateDialog(g_hInstance, 
			MAKEINTRESOURCE(idDialogRes), g_hwndTC, lpToolDlgProc);
		ShowWindow(hwndTool, SW_SHOW);
	}

	SetForegroundWindow(hwndTool);
	
	if( IsMainWndHidden(hwndTool) )
	{
		ShowWindow(hwndTool, SW_RESTORE);
	}

}//OnRunToolCommand()

//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc_ToolContainer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

{
	switch (message)
	{	
		
	case WM_SIZE:
		{
			if(SIZE_MAXIMIZED == wParam)
			{
				ShowWindow(hwnd, SW_HIDE); //Never show..
			}
			break;
		}
		
	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDM_RUN_COLORSPY:
				{
					OnRunToolCommand(g_hwnd_ColorSpy, IDD_TOOL_COLORSPY, DlgProc_ColorSpy);

					break;
				}
			case IDM_RUN_PSWSNIFFER:
				{
					OnRunToolCommand(g_hwnd_PSWsniffer, IDD_TOOL_PSWSNIFFER, DlgProc_PswSniffer);

					break;
				}
			case IDM_RUN_WNDENUMER:
				{
					OnRunToolCommand(g_hwnd_WndEnumEr, IDD_TOOL_WNDENUMER, DlgProc_WndEnumer);

					break;
				}
			case IDM_RUN_WNDMOD:
				{
					OnRunToolCommand(g_hwnd_WndModEr, IDD_TOOL_WNDMODER, DlgProc_WndModifier);

					break;
				}
			}
			break;
		}
		
	case WM_MY_TOOL_CLOSE:
		{
			if( !IsToolWindowExists() )
			{
				DestroyWindow(hwnd);
			}
			return 0;
		}
		
	case WM_CLOSE:
		{
			DoDestroyToolWindows();
			break;
		}
		
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		
	} //switch (message)
	
	return DefWindowProc(hwnd, message, wParam, lParam);
	
} //WndProc_ToolContainer()...

//////////////////////////////////////////////////////////////////////////

BOOL DoWaitForAWhile(BOOL* lpBoolSignal, INT iMaxLoopTime)
{
	while( *lpBoolSignal && iMaxLoopTime >0 )
	{
		iMaxLoopTime--;
		Sleep(150);
	}
	return *lpBoolSignal;
}

//////////////////////////////////////////////////////////////////////////
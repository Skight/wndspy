#include <windows.h>
#include "TaskBarFunc.h"
#include "RectPosRgnEx.h"

INT CheckWndSpecClassAndDoRefresh(HWND hwnd)
{
	TCHAR szClassName[64];
	
	GetClassName(hwnd,szClassName,64);

	if( lstrcmpi(szClassName,TEXT("TrayClockWClass"))==0 )
	{
		PostMessage(hwnd,WM_TIMECHANGE,0,0);
		return 1;
	}
	
	if ( lstrcmpi(szClassName,TEXT("ToolbarWindow32"))==0 )
	{
		InvalidateRect(hwnd, NULL, TRUE);
		return 2;
	}
	
	if ( lstrcmpi(szClassName,TEXT("CiceroUIWndFrame"))==0 )
	{
		PostMessage(hwnd,WM_SYSCOLORCHANGE,0,0);
		return 3;
	}
	
	if( lstrcmpi(szClassName,TEXT("Shell_TrayWnd"))==0 )
	{
		return -1;
	}

	if(lstrcmpi(szClassName,TEXT("TrayNotifyWnd"))==0 )
	{
		return -2;
	}

	return 0;
}

BOOL CALLBACK RefreashWndEnumProc(HWND hwnd, LPARAM lParam)
{
	
	if ( lParam==NULL) //win9x...
	{
		if(CheckWndSpecClassAndDoRefresh(hwnd) == -1 )
		{
		EnumChildWindows(hwnd,RefreashWndEnumProc,(LPARAM)hwnd); 
		return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	// else...lParam!=NULL {} //EnumChildWindowsµÄEnumProc
	if(	IsWndOverWnd(hwnd, (HWND)lParam) )
	{
		CheckWndSpecClassAndDoRefresh(hwnd);
	}
	
	return TRUE;
}


void DoRefreshIfWndIsTaskBar(HWND hwnd)
{
	HWND hwndBuf=hwnd;

	if(hwndBuf==NULL || hwndBuf==GetDesktopWindow())
	{
		EnumWindows(RefreashWndEnumProc, NULL);
		return;
	}
	
	while (hwndBuf)
	{
		if( CheckWndSpecClassAndDoRefresh(hwndBuf) <0 )
		{
			if( CheckWndSpecClassAndDoRefresh(hwnd) <= 0)
			{
				EnumChildWindows(hwndBuf,RefreashWndEnumProc,(LPARAM)hwnd);
			}
			break;
		}

		hwndBuf=GetParent(hwndBuf);
	}

}

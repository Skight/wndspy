#if _MSC_VER >= 1000
#pragma once
#endif
/***************************************************************
Module name: InjCode.h
Copyright (c) 2003 Robert Kuster

Notice:	If this code works, it was written by Robert Kuster.
		Else, I don't know who wrote it.

		Use it on your own risk. No responsibilities for
		possible damages of even functionality can be taken.
***************************************************************/
#if !defined INJCODE_REMOTETEXT_H
#define INJCODE_REMOTETEXT_H

int GetWindowTextRemoteA (HANDLE hProcess, HWND hWnd, LPSTR  lpString);
int GetWindowTextRemoteW (HANDLE hProcess, HWND hWnd, LPWSTR lpString);


#ifdef UNICODE
#define GetWindowTextRemote GetWindowTextRemoteW
#else
#define GetWindowTextRemote GetWindowTextRemoteA
#endif // !UNICODE


BOOL GetRemotePSW(HWND hwndPSW, LPTSTR szPSWbuf);

#endif // !defined(INJCODE_REMOTETEXT_H)


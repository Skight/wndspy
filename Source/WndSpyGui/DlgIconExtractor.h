#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_DLGICONEXTRACTOR_H
#define HS_HEADER_DEF_DLGICONEXTRACTOR_H
//////////////////////////////////////////////////////////////////////////

#define WM_MY_SETLISTVIEWICONS		(WM_USER_FIRST_DLG_ICONEXT + 1)

//////////////////////////////////////////////////////////////////////////

typedef struct tag_IconEnumParam{
	HIMAGELIST himl;
	HWND hwnd; //reserved for extending...
	HWND hwndProgress;
	HWND hwndLV;
	INT iCount;
	HMODULE hModule; //Muiltthread signal...
	LPTSTR szModule;
	HWND hwndStrList;
	BOOL IsKillThread;
}ICON_ENUM_PARAM;

//////////////////////////////////////////////////////////////////////////

INT DoTestIconExtNeedTime(LPTSTR szModule, INT* pIntIconNum);

BOOL CALLBACK IconsEnumProc(HMODULE hModule, 
							LPCTSTR lpszType, 
							LPTSTR lpszName, 
							LONG lParam);

void EnumIconResToListView(PVOID pviod);

void ThreadFuncEnumIcons(ICON_ENUM_PARAM* pviod);

BOOL CALLBACK DlgProc_IconExtractor (HWND hwnd, 
									 UINT message, 
									 WPARAM wParam, 
									 LPARAM lParam);


//////////////////////////////////////////////////////////////////////////
#endif // HS_HEADER_DEF_DLGICONEXTRACTOR_H
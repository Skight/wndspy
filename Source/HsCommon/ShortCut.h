#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_SHORTCUT
#define HS_INC_HEADER_SHORTCUT

///////////////////////////////////////////////////////////


/************************
iShowCmd can be SW_SHOWNORMAL, SW_SHOWMAXIMIZED, SW_SHOWMINNOACTIVE
pwHotkey
Address of the hot key. The virtual key code is in the low-order byte, 
and the modifier flags are in the high-order byte. 
The modifier flags can be a combination of the following values.
HOTKEYF_ALT : ALT key 
HOTKEYF_CONTROL : CTRL key 
HOTKEYF_EXT : Extended key 
HOTKEYF_SHIFT : SHIFT key 
************************/
HRESULT CreateShortcut(BOOL immediately, 
					   LPCTSTR lpszPathLink, 
					   LPCTSTR lpszPathObj, 
					   LPCTSTR lpszArgments, 
					   LPCTSTR lpszCurDir, 
					   INT iShowCmd,
					   WORD pwHotKey,
					   LPCTSTR lpszIcon, INT iIconIndex,
					   LPCTSTR lpszDesc);


HRESULT ResolveShortcut(BOOL immediately,
						HWND hwnd, 
						LPCTSTR lpszLinkFile, 
						LPTSTR lpszPath, 
						int iPathBufferSize);

#define DoCreateShortcut(_szShortcut, _szPathObj) \
CreateShortcut(TRUE, _szShortcut, _szPathObj, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

#define DoResolveShortcut(_szShortcut, _szPathObj, _iPathBufferSize) \
ResolveShortcut(TRUE, NULL, _szShortcut, _szPathObj, _iPathBufferSize)


void DoLocateFileEx(LPTSTR szTarget);
////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_SHORTCUT
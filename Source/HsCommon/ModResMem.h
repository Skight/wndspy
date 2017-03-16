#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_MODRESMEM
#define HS_INC_HEADER_MODRESMEM

//////////////////////////////////////////////////////////////////////////
//MACRO: HS_LOAD_LIBPROC(4)...
//return FALSE if error...
#define HS_LOAD_LIBPROC(_hLibrary, _lpFunc, _typedefined, _szFuncName)  \
    _lpFunc = (_typedefined)GetProcAddress(_hLibrary, _szFuncName);     \
    if(!_lpFunc) return FALSE
//END MACRO: #define HS_LOAD_LIBPROC(4) ...


//MACRO: HS_LOAD_LIBPROC_EX(5)...
#define HS_LOAD_LIBPROC_EX(_hLibrary, _lpFunc, _typedefined, _szFuncName, _IsSuccess)  \
    _lpFunc = (_typedefined)GetProcAddress(_hLibrary, _szFuncName);     \
    if(!_lpFunc) _IsSuccess = FALSE
//END MACRO: #define HS_LOAD_LIBPROC_EX(5) ...
 
//////////////////////////////////////////////////////////////////////////

VOID TrimAppMemory(BOOL isWinVerSupports);

#define _TRIM_MEM
#ifdef _TRIM_MEM
#define MACRO_TRIM_MEMORY(_IsWinSupports)  \
TrimAppMemory(_IsWinSupports);
//if ( _IsWinSupports )  \
//	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
#else 
#define MACRO_TRIM_MEMORY(_void)
#endif //_TRIM_MEM

//////////////////////////////////////////////////////////////////////////

BOOL CopyTextToClipBoard(HWND hwndOwner, LPTSTR szText);

//////////////////////////////////////////////////////////////////////////

#define	DoDeleteObject(_hobj) \
if(_hobj) DeleteObject(_hobj)

#define	DoCloseHandle(_handle) \
if(_handle) CloseHandle(_handle)

#define DoFreeLibrary(_hInstlib)	\
if(_hInstlib) FreeLibrary(_hInstlib) 

//////////////////////////////////////////////////////////////////////////
// phModuleDll 
// call DoFreeLibrary finally
FARPROC LoadLibraryProc( HMODULE* phModuleDll,
						LPCTSTR szDllName, 
						LPCSTR szProcName);

//////////////////////////////////////////////////////////////////////////
LPVOID WINAPI LockLoadResource(HINSTANCE hInstance, LPCTSTR lpszResName,LPCTSTR lpszResType);
#define LockLoadDlgResource(_hInst, _szDlgResName)  \
(DLGTEMPLATE *)LockLoadResource(_hInst, _szDlgResName, RT_DIALOG)

#define LockDlgResource(_szDlgResName)  \
LockLoadDlgResource(NULL, _szDlgResName)
#define LoadDlgResource(_szDlgResName)  \
LockDlgResource(_szDlgResName)

///////////////////////////////////////////////////////////////////////////

INT SetWindowTextFromStrRes(HWND hwnd, UINT idCtrl, HINSTANCE hInstance, UINT idStrRes);

//////////////////////////////////////////////////////////////////////////
/////
// The parameter lpstrArray should be pointer-array only, 
// the function use GlobalAlloc() to alloc string buffers (pointed by the pointers).
// when finished using buffer-array, you should call ClobalFree to release the buffers memory.
// This behaviour is just like the shlwapi::StrDup().
/////
void DoLoadStringArray(HINSTANCE hInstance, 
					INT iIDstart, INT iIDend, INT iDdelta, 
					LPTSTR* lpstrArray
					);


#define LOADSTRINGARRAY_MAX_STRLEN 1024

///////////////////////////////////////////////////////////////////////////

#endif //HS_INC_HEADER_MODRESMEM
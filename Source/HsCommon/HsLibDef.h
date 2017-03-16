#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_HS_COMMON_DEF_AND_MACRO
#define HS_INC_HEADER_HS_COMMON_DEF_AND_MACRO
///////////////////////////////////////////////
#include <windows.h>
#include "_HsCommon.h"

//////////////////////////////////////////////////////////////////////////
#define LABEL_STR_LEN		32
#define TINY_STRBUF_LEN		64
#define MINI_STRBUF_LEN		128
#define SMALL_STRBUF_LEN	MAX_PATH
#define BIG_STRBUF_LEN		512
#define MAX_STRBUF_LEN		2048
#define MAX_STRBUF_SIZE		((MAX_STRBUF_LEN) * sizeof(TCHAR))

#define WNDCLASS_DESKTOP		TEXT("#32769")
#define WNDCLASS_MENU			TEXT("#32768")
#define WNDCLASS_TABSWITCH		TEXT("#32771")
#define WNDCLASS_DLG			TEXT("#32770")

//////////////////////////////////////////////////////////////////////////

#define INIT_ERROR	-1
#define INIT_ERR	-1
//////////////////////////////////////////////////////////////////////////

#define IS_FLAG_MARKED(_fCombo, _f)		(_fCombo&_f)
#define IS_FLAGS_MARKED(_fsCombo, _fs)	((_fsCombo&(_fs))==(_fs))
#define IS_FLAG_SETTED		IS_FLAG_MARKED
#define IS_FLAGS_SETTED		IS_FLAGS_MARKED

#define GetWebSafeColorValue(_byte) \
(((_byte/0x33) + ((_byte%0x33) > 25 ? 1 : 0)) * 0x33)

#define ChangeLine(_string)  lstrcat(_string, TEXT("\r\n"))
//////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
#define A_TCHAR(c) ((WCHAR)MAKEWORD(c,0))
#else
#define A_TCHAR(c) ((CHAR)c)
#endif //UNICODE

#define MAKEBOOLSTR(_boolVar) \
(_boolVar? TEXT("1"):TEXT("0") )

#ifdef _DEBUG
#define DEBUG_ERROR_MSG(__szMsg)  \
MessageBox(NULL, __szMsg, TEXT("Debug"), MB_TOPMOST|MB_TASKMODAL|MB_ICONERROR);
#else
#define DEBUG_ERROR_MSG(__void_only_pop_on_debug_mode)
#endif

#define SET_MODE_NO_ERROR_MSGBOX() \
SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOALIGNMENTFAULTEXCEPT|SEM_NOOPENFILEERRORBOX)

//////////////////////////////////////////////////////////////////////////

void LastErrorIDMessageBox(HWND hwnd, DWORD dwLastError, LPTSTR szStrBuf, UINT uiMaxBufLen);

#define LastErrorMessageBox(_hwnd, _StrBuffer, _iMaxBufferLen) \
LastErrorIDMessageBox(_hwnd, GetLastError(),_StrBuffer, _iMaxBufferLen)


#define ShowLastError(_hwnd) LastErrorMessageBox(_hwnd, NULL, NULL)
///////////////////////////////////////////////
#endif //HS_INC_HEADER_HS_COMMON_DEF_AND_MACRO


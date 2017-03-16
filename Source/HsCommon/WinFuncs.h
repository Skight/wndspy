#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_WINFUNCS
#define HS_INC_HEADER_WINFUNCS
///////////////////////////////////////////////////////////

#define WINVER_NOTSUPPORT		((short)0)
#define WINVER_WIN9X			((short)1)
#define WINVER_WINNT_OLD		((short)2)
#define WINVER_WIN2K			((short)3)
#define WINVER_WINXP			((short)4)
#define WINVER_WINXPNEWER		((short)5)
#define WINVER_VISTA			((short)6)
#define WINVER_WIN7_OR_LATER	((short)7)


short GetWinVerType(void);
///////////////////////////////////////////////////////////////////

// uFlag:
// EWX_LOGOFF || EWX_SHUTDOWN || EWX_REBOOT || EWX_POWEROFF [|| EWX_FORCE ]
void DoExitWindows(UINT uFlag);

//////////////////////////////////////////////////////////////////////////
BOOL CheckWindowClass(HWND hwnd, LPTSTR szClassName);

BOOL IsPasswordEdit(HWND hwndEdit);
////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_WINFUNCS
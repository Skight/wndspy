#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_FILEOPEX
#define HS_INC_HEADER_FILEOPEX
//////////////////////////////////////////////////////////////////////////
#include <shlobj.h>
//////////////////////////////////////////////////////////////////////////

BOOL WriteStrToFile(LPTSTR szFileName, LPTSTR szStrToWrite, BOOL IsAppend);

//////////////////////////////////////////////////////////////////////////
void DoOpenTextFile(LPTSTR szTextFileName);
//////////////////////////////////////////////////////////////////////////
BOOL SetWindowTextFromFile (HWND hwnd, PTSTR pstrFileName);
BOOL FileOperation(LPTSTR szFromFileName, LPTSTR szToFileName, UINT wFunc);
//////////////////////////////////////////////////////////////////////////
INT MakeFileTypeFromStrRes(HINSTANCE hInstance, UINT IdStrRes, 
						   LPTSTR lpszBuf, LPTSTR szFilterDisp, LPTSTR szFilter);
//////////////////////////////////////////////////////////////////////////
LPITEMIDLIST  GetFolderDialog(HWND hwndOwner, 
							 LPCITEMIDLIST pidlRoot,
							 LPTSTR pszFolder,
							 LPCTSTR lpszTitle,
							 UINT ulFlags,
							 int iImage);


#define SelectFolderDialog(_hwnd, _pidlRoot, _szFolder, _szTitle) \
GetFolderDialog(_hwnd, _pidlRoot, _szFolder, _szTitle, \
BIF_DONTGOBELOWDOMAIN|BIF_RETURNONLYFSDIRS, NULL) 

#define SelectFolderDialogNewUI(_hwnd, _pidlRoot, _szFolder, _szTitle) \
GetFolderDialog(_hwnd, _pidlRoot, _szFolder, _szTitle, \
BIF_DONTGOBELOWDOMAIN|BIF_RETURNONLYFSDIRS|BIF_USENEWUI|BIF_UAHINT, NULL) 

//////////////////////////////////////////////////////////////////////////
BOOL CommonDialog_File(BOOL IsNewWin,
					   BOOL IsSaveType,
					   HINSTANCE hInstance, 
						   HWND hwnd, 
						   LPTSTR szFileName, 
						   LPTSTR szFileTitleName, 
						   LPTSTR szInitDir, 
						   LPTSTR szfilter, 
						   LPTSTR szDefExt ,
						   DWORD dwFlags,
						   DWORD dwFlagsEx);

#define GetSaveFileNameDialog(_hInst, _hwnd, _szFileName, \
	_szTitleName, _szInitDir, _szfilter, _szDefExt, _dwFlags, _dwFlagsEx, _IsNewWin) \
CommonDialog_File(_IsNewWin, TRUE, _hInst, _hwnd, _szFileName, \
	_szTitleName, _szInitDir, _szfilter, _szDefExt, _dwFlags, _dwFlagsEx) 
//#define GetSaveFileNameDialog End...

#define GetOpenFileNameDialog(_hInst, _hwnd, _szFileName, \
	_szTitleName, _szInitDir, _szfilter, _szDefExt, _dwFlags, _dwFlagsEx ,_IsNewWin) \
CommonDialog_File(_IsNewWin, FALSE, _hInst, _hwnd, _szFileName, \
	_szTitleName, _szInitDir, _szfilter, _szDefExt, _dwFlags, _dwFlagsEx)
//#define GetOpenFileNameDialog End...
//////////////////////////////////////////////////////////////////////////
BOOL ChooseColorDialog(HWND hwndOwner, CHOOSECOLOR* lpCC, COLORREF crRGBinit, COLORREF* crCustClr16Array);
//////////////////////////////////////////////////////////////////////////
BOOL ShowFileProperties(HWND hwnd, LPTSTR szFileName);
void DoLocateFile(LPTSTR szPathBuf, BOOL bIsFolder);
void DoLocateFileFromCtrlText(HWND hwnd, UINT idCtrl);
void DoLocateFileFromCtrlTextEx(HWND hwnd, UINT idCtrl);

VOID CALLBACK  TimerProc_OnLocateFile(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK  TimerProc_OnLocateFileEx(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID CALLBACK  TimerProc_DelayCtrl(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime);


#define DoDisableCtrlForAMoment(_hDlg, _idCtrl, _Time) \
		SetWindowEnable(_hDlg, _idCtrl, FALSE), \
		SetTimer(GetDlgItem(_hDlg,_idCtrl), (UINT)_hDlg, _Time, TimerProc_DelayCtrl)

#define OnLocateFileFromWndText(_hwndText) \
		SetTimer(_hwndText, (UINT)_hwndText, USER_TIMER_MINIMUM, TimerProc_OnLocateFile)

#define OnLocateFileFromCtrlAsync(_hDlg, _idCtrl) \
		SetTimer(GetDlgItem(_hDlg,_idCtrl), (UINT)_hDlg, USER_TIMER_MINIMUM, TimerProc_OnLocateFile)

#define OnLocateFileFromCtrlExAsync(_hDlg, _idCtrl) \
		SetTimer(GetDlgItem(_hDlg,_idCtrl), (UINT)_hDlg, USER_TIMER_MINIMUM, TimerProc_OnLocateFileEx)

#define OnLocateFileFromCtrl(_hDlg, _idCtrl) \
		DoLocateFileFromCtrlText(_hDlg, _idCtrl)

#define OnLocateFileFromCtrlEx(_hDlg, _idCtrl) \
		DoLocateFileFromCtrlTextEx(_hDlg, _idCtrl)

//////////////////////////////////////////////////////////////////////////
void DoOpenTextFile(LPTSTR szTextFileName);
VOID CALLBACK TimerProc_DelayOpenTextFile(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#define OnOpenTextFile(_TextFileName) \
		DoOpenTextFile(_TextFileName)
#define OnOpenTextFileAsync(_hwndCaller, _g_szTextFileName) \
		SetTimer(_hwndCaller, (UINT)_g_szTextFileName, USER_TIMER_MINIMUM, TimerProc_DelayOpenTextFile)

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_FILEOPEX
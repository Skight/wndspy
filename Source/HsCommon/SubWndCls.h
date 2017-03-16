#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_SUBWNDCLS
#define HS_INC_HEADER_SUBWNDCLS

//////////////////////////////////////////////////////////////////////////
//the VK of CTRL+CHAR is the CHAR's alpha index...

#define VK_CTRL_A	1
#define VK_CTRL_C	3
#define VK_CTRL_V	22
#define VK_CTRL_Z	26

//////////////////////////////////////////////////////////////////////////
typedef struct tag_WndSubClassData
{
	WNDPROC WndProcBuf;
	LPARAM	lParam;

}WND_SUBCLASS_DATA;

LONG SetWndTargetSubClass(HWND hwnd, INT nChildItemID, WNDPROC SubClassWndProc);
LONG SetWndTargetSubClassParam(HWND hwnd, INT nChildItemID, WND_SUBCLASS_DATA* lpSubClassData);

//////////////////////////////////////////////////////////////////////////
#define SetWndSubClass(_hwnd, _SubClassProc) \
SetWndTargetSubClass(_hwnd, NULL, _SubClassProc)

#define SetDlgItemSubClass(_hDlg, _nItemID, _SubClassWndProc) \
SetWndTargetSubClass(_hDlg, _nItemID, _SubClassWndProc)

#define SetWndSubClassParam(_hwnd, _lpSubClassData) \
SetWndTargetSubClassParam(_hwnd, NULL, _lpSubClassData)

#define SetDlgItemSubClassParam(_hDlg, _nItemID, _lpSubClassData) \
SetWndTargetSubClassParam(_hDlg, _nItemID, _lpSubClassData)


//////////////////////////////////////////////////////////////////////////
LRESULT APIENTRY SCP_ChkBtn_NoFoucsRect(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//SubClassWndProc : SCP_Btn_NoDefPush
LRESULT APIENTRY SCP_Btn_NoDefPush(HWND hwnd, 
								   UINT message, 
								   WPARAM wParam, 
								   LPARAM lParam);


//SubClassWndProc : SCP_Static_HandCusror
LRESULT APIENTRY SCP_Static_HandCusror(HWND hwnd, 
									   UINT message, 
									   WPARAM wParam, 
									   LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
//SubClassWndProc : SCP_Edit_HexNum
LRESULT APIENTRY SCP_Edit_HexNum (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//SubClassWndProc : SCP_Edit_DecNum
LRESULT APIENTRY SCP_Edit_DecNum (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

INT OnNumEditMaxMinRoutine(HWND hwnd, WPARAM wParam, LPARAM lParam, INT iMinNum, INT iMaxNum);

//////////////////////////////////////////////////////////////////////////

/*
//SubClassWndProc : SCP_Edit_Num
#define NUM_MIN_VALUE 10 //min
#define NUM_MAX_VALUE 180 //max
#define EDIT_MAX_STR_LENGTH 3 //EM_SETLIMITTEXT
LRESULT APIENTRY SCP_Edit_Num (HWND hwnd, 
										UINT message, 
										WPARAM wParam, 
										LPARAM lParam); */

LRESULT APIENTRY SCP_Edit_SelAll (HWND hwnd, 
								  UINT message, 
								  WPARAM wParam, 
								  LPARAM lParam);

LRESULT APIENTRY SCP_Edit_ReadOnly (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_SUBWNDCLS
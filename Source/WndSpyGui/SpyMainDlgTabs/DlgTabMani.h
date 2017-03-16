#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_MANTAB_5_H
#define HS_HEADER_DEF_MANTAB_5_H

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK TabDlgProc_Mani (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);

LRESULT APIENTRY SubClassProcSndMsgBtn(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void OnRunCalc(HWND hwnd);
void OnInitMsgList(HWND hwndList);

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_MANTAB_5_H

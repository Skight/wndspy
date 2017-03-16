#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_MANTAB_1_H
#define HS_HEADER_DEF_MANTAB_1_H
//////////////////////////////////////////////////////////////////////////


BOOL CALLBACK TabDlgProc_General (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam);
BOOL OnWndRectTipNotify(LPARAM lParam);
LRESULT APIENTRY SubClassProcIconExtBtn(HWND hwnd, 
										UINT message, 
										WPARAM wParam, 
										LPARAM lParam);
//////////////////////////////////////////////////////////////////////////
#endif //HS_HEADER_DEF_MANTAB_1_H


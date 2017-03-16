#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_RECTPOSRGN
#define HS_INC_HEADER_RECTPOSRGN

#include <windows.h>

BOOL IsRectInRect(const RECT*, const RECT*);
BOOL IsRectOverRect(const RECT* pRect1, const RECT* pRect2);

#endif //HS_INC_HEADER_RECTPOSRGN


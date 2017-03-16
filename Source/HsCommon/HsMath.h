#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_MATH
#define HS_INC_HEADER_MATH
///////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////
int hs_intpow(int x, int y);

#define _HS_COMMON_FUN_INTPOW
#ifdef _HS_COMMON_FUN_INTPOW
#define IntPow(_X, _Y)	hs_intpow(_X, _Y)
#else
#define IntPow(_X, _Y)	(int)pow((int)_X, (int)_Y)
#endif

///////////////////////////////////////////////////////////////////////////

#endif // HS_INC_HEADER_MATH
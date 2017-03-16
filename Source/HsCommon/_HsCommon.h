#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_HS_COMMON_SRC
#define HS_INC_HEADER_HS_COMMON_SRC
///////////////////////////////////////////////
#include <windows.h>
#include <tchar.h>

#ifdef HSCOMMON_USE_SHLWAPI
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif


//////////////////////////////////////////////////////////////////////////
#include "HsLibDef.h"
///////////////////////////////////////////////
#include "HsMath.h"
#include "DrawPaintEx.h"
#include "FileOp.h"
#include "FileOpEx.h"
#include "RectPosRgn.h"
#include "ModResMem.h"
#include "ProcessFunc.h"
#include "RectPosRgnEx.h"
#include "StringFunc.h"
#include "StringFuncEx.h"
#include "SubWndCls.h"
#include "TaskBarFunc.h"
#include "WinFuncs.h"
#include "WndDlgCtrl.h"
#include "UxThemeFuncs.h"
#include "Shortcut.h"

///////////////////////////////////////////////
#endif //HS_INC_HEADER_HS_COMMON_SRC


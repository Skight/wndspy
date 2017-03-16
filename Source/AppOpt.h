#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_HESUNG_VCPROJECT_OPTION_H
#define HS_HEADER_DEF_HESUNG_VCPROJECT_OPTION_H

//////////////////////////////////////////////////////////////////////////

#if defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#endif

//////////////////////////////////////////////////////////////////////////

#define MSVC6_DIR	"C:\\Program Files\\Microsoft Visual Studio\\VC98\\Lib"

//////////////////////////////////////////////////////////////////////////

#if _MSC_VER ==1200 && _DEBUG //--------------------------------

#pragma comment (linker, "/nodefaultlib:shell32.lib")
#pragma comment (linker, "/nodefaultlib:uuid.lib")
#pragma comment (lib, MSVC6_DIR"\\shell32.lib")
#pragma comment (lib, MSVC6_DIR"\\uuid.lib")

#else //--------------------------------------------------------

#pragma comment (lib, "uuid.lib" )

#endif //_MSC_VER ==1200 && _DEBUG //---------------------------

//////////////////////////////////////////////////////////////////////////
//#define _TEST_ONLY
//#define _TEST_FUNC
#include "..\TempWork.h"

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_HESUNG_VCPROJECT_OPTION_H
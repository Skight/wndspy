//////////////////////////////////////////////////////////////////////////
#ifdef _TEST_FUNC

//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
// #include <atlbase.h>
// #include <atlconv.h>
// #include <oleacc.h>
// #include <mshtml.h>
// #include <comdef.h>
// #include <comutil.h>

//#include "Source\WndSpy.h"
//#include "Source\GlobalVar.h"
//#include "TempWork.h"

//////////////////////////////////////////////////////////////////////////
#define STR_TEST	TEXT("Test")
#define STR_TEST_START TEXT("Press [YES] to start test...")
#define STR_TEST_END TEXT("Do you want to run main application nextly?")

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

BOOL TestFunc_stub(LPVOID pTestParam)
{
	g_hInstance=(HINSTANCE)pTestParam;
	InitCommonControls();

	if( IDYES==MessageBox(NULL, STR_TEST_START, STR_TEST, MB_YESNO|MB_TOPMOST) )
	{
		//TestFunc();
	}
	
	if( IDYES==MessageBox(NULL, STR_TEST_END, STR_TEST, MB_YESNO|MB_TOPMOST) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
#endif //#ifdef _TEST_FUNC

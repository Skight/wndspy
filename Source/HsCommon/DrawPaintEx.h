#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_DRAWPAINTEX
#define HS_INC_HEADER_DRAWPAINTEX

//////////////////////////////////////////////////////////////////////////
#define DWR_FLAG_FOCUS_RECT		0x0000
#define DWR_FLAG_VITREOUS_PEN	0x0001
#define DWR_FLAG_CUSTOM_PEN		0x0002
#define DWR_FLAG_FOCUS_BLOCK	0x0003

#define DWR_FLAG_TOPMOST		0x1000
#define DWR_FLAG_LOCKUPATE		0x2000
#define DWR_VAR_PIXEL_COUNT 3 
#define DWR_VAR_PEN_WIDTH	3 

/************************************************************************ 
	#define DWR_FLAG_FOCUS_RECT		0x0000
	#define DWR_FLAG_CUSTOM_PEN		0x0001
	#define DWR_FLAG_FOCUS_BLOCK	0x0002
	#define DWR_FLAG_VITREOUS_PEN	0x0003
	#define DWR_FLAG_TOPMOST		0x1000
	#define DWR_FLAG_LOCKUPATE		0x2000
	
	#define DWR_VAR_PIXEL_COUNT 3
	#define DWR_VAR_PEN_WIDTH	3
*************************************************************************/

BOOL DrawWindowRect(HWND hwnd, WORD wFlag, COLORREF dwColor);
#define DrawWindowFocus(_hwnd, _wFlag, _color) \
DrawWindowRect(_hwnd, _wFlag, _color)
#define RemoveWindowFocus(_hwnd, _wFlag, _color) \
DrawWindowRect(_hwnd, _wFlag, _color)

//////////////////////////////////////////////////////////////////////////
#endif //HS_INC_HEADER_DRAWPAINTEX
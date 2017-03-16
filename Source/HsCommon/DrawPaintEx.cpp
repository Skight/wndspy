#include <windows.h>
#include "DrawPaintEx.h"

/************************************************************************ 
	#define DWR_FLAG_FOCUS_RECT		0x0000
	#define DWR_FLAG_CUSTOM_PEN		0x0001
	#define DWR_FLAG_FOCUS_BLOCK	0x0002
	#define DWR_FLAG_TOPMOST		0x1000
	#define DWR_FLAG_LOCKUPATE		0x2000
	
	#define DWR_VAR_PIXEL_COUNT 3
	#define DWR_VAR_PEN_WIDTH	3
*************************************************************************/
BOOL DrawWindowRect(HWND hwnd, WORD wFlag, COLORREF dwColor)
{

	HWND hwndDraw=hwnd;
	int iRgnType=ERROR;
	HRGN hrgn=NULL;
	RECT rect;
	RECT rcTemp;
	HDC hdc;
	HGDIOBJ hPrev, hSaveDcFont;

	if( !IsWindow(hwnd) || 
		( LOBYTE(wFlag)==DWR_FLAG_CUSTOM_PEN && dwColor==RGB(0xFF,0xFF,0xFF) )
		)
	{
		return FALSE;
	}

	GetWindowRect(hwnd,&rect);

	if( !(GetWindowLong(hwnd,GWL_STYLE)&WS_CHILD) ) //top window
	{
		if(IsZoomed(hwnd))
		{
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
			hwndDraw = GetDesktopWindow();
		}
		else
		{
			if( IsWindowVisible(hwnd) )
			{
				hrgn=CreateRectRgn(0, 0, 0, 0);
				iRgnType=GetWindowRgn(hwnd,hrgn);
			}

			if(iRgnType!=ERROR)
			{
				GetRgnBox(hrgn,&rect);
			}
			else
			{
				hwndDraw = 
				(wFlag & DWR_FLAG_TOPMOST)==DWR_FLAG_TOPMOST ? 
				GetDesktopWindow() : hwnd;
			}

		}
	}
	else //Child Window
	{
		if ( (rect.bottom-rect.top) <= 2*DWR_VAR_PIXEL_COUNT )
		{
			rect.top-=DWR_VAR_PIXEL_COUNT;
			rect.bottom+=DWR_VAR_PIXEL_COUNT;
		}
		if ( (rect.right-rect.left) <= 2*DWR_VAR_PIXEL_COUNT )
		{
			rect.left-=DWR_VAR_PIXEL_COUNT;
			rect.right+=DWR_VAR_PIXEL_COUNT;
		}

		hwndDraw = (wFlag & DWR_FLAG_TOPMOST)==DWR_FLAG_TOPMOST ?
			GetDesktopWindow() : GetAncestor(hwnd,GA_ROOT);
	}

	if(iRgnType==ERROR)
	{
		GetWindowRect(hwndDraw,&rcTemp);
		rect.right-=rect.left;
		rect.bottom-=rect.top;
		rect.left-=rcTemp.left;
		rect.top-=rcTemp.top;
		rect.right+=rect.left;
		rect.bottom+=rect.top;
	}

	//starting drawfocus..
	if( (wFlag & DWR_FLAG_TOPMOST)==DWR_FLAG_TOPMOST  ||
		(wFlag & DWR_FLAG_LOCKUPATE)==DWR_FLAG_LOCKUPATE )
	{
		hdc=GetDCEx(hwndDraw, NULL, DCX_WINDOW| DCX_CACHE|
			((wFlag & DWR_FLAG_LOCKUPATE)==DWR_FLAG_LOCKUPATE ? (DCX_LOCKWINDOWUPDATE):0) );
	}
	else
	{
		hdc=GetWindowDC(hwndDraw);
	}

	hSaveDcFont=GetCurrentObject(hdc, OBJ_FONT);
	switch ( LOBYTE(wFlag) )
	{

	case DWR_FLAG_VITREOUS_PEN:
		dwColor = RGB(
			GetRValue(dwColor)%200, 
			GetGValue(dwColor)%200, 
			GetBValue(dwColor) );
		//fall through...
	case DWR_FLAG_CUSTOM_PEN:
	case DWR_FLAG_FOCUS_BLOCK:
		{
			SetROP2(hdc,R2_NOTXORPEN);
			
			SelectObject(hdc, 
				GetStockObject(LOBYTE(wFlag) == 
				DWR_FLAG_FOCUS_BLOCK ? NULL_PEN:NULL_BRUSH ) );

			hPrev=SelectObject(hdc, 
				LOBYTE(wFlag) == DWR_FLAG_FOCUS_BLOCK ?
				(HGDIOBJ)CreateSolidBrush(RGB(0,0,0)) :
				(HGDIOBJ)CreatePen(PS_DOT, DWR_VAR_PEN_WIDTH, dwColor) );
				
			if( LOBYTE(wFlag) != DWR_FLAG_FOCUS_BLOCK )
			{
				if( LOBYTE(wFlag) == DWR_FLAG_VITREOUS_PEN && 
					!IsRectEmpty(&rect) )
				{
					PatBlt(hdc, 
						rect.left, rect.top, 
						rect.right - rect.left, 
						DWR_VAR_PEN_WIDTH, 
						DSTINVERT);
					PatBlt(hdc, 
						rect.left, rect.bottom - DWR_VAR_PEN_WIDTH, 
						DWR_VAR_PEN_WIDTH,
						-(rect.bottom - rect.top - 2 * DWR_VAR_PEN_WIDTH), 
						DSTINVERT);
					PatBlt(hdc, 
						rect.right - DWR_VAR_PEN_WIDTH, rect.top + DWR_VAR_PEN_WIDTH, 
						DWR_VAR_PEN_WIDTH,
						rect.bottom - rect.top - 2 * DWR_VAR_PEN_WIDTH, 
						DSTINVERT);
					PatBlt(hdc, 
						rect.right, rect.bottom - DWR_VAR_PEN_WIDTH, 
						-(rect.right - rect.left),
						DWR_VAR_PEN_WIDTH, 
						DSTINVERT);
				}

				if( ERROR!=iRgnType )
				{
					if( LOBYTE(wFlag) == DWR_FLAG_CUSTOM_PEN )
					{
						hPrev=SelectObject(hdc,hPrev);
						FrameRgn(hdc, hrgn, (HBRUSH)hPrev,
							GetSystemMetrics(SM_CXBORDER)*DWR_VAR_PEN_WIDTH,
							GetSystemMetrics(SM_CYBORDER)*DWR_VAR_PEN_WIDTH);
						DeleteObject(hPrev);
						break;
					}
					rect.top+=2;
					rect.left+=2;
					rect.right-=2;
					rect.bottom-=2;
				} 
			}

			Rectangle(hdc, 
					rect.left, rect.top, 
					rect.right, rect.bottom);


			DeleteObject(SelectObject(hdc,hPrev));
			break;
		}
		
	case DWR_FLAG_FOCUS_RECT: //default
	default:
		{
			int i;
			DrawFocusRect(hdc,&rect);
			for(i=1; i<DWR_VAR_PIXEL_COUNT; i++)
			{
				rect.bottom -= 1;
				rect.right -= 1;
				rect.top += 1;
				rect.left += 1;
				DrawFocusRect(hdc,&rect);
			}
			if(iRgnType==COMPLEXREGION)
			{
				rcTemp.left=rect.left+1;
				rcTemp.top=rect.top+1;
				rcTemp.right=rect.left+DWR_VAR_PIXEL_COUNT;
				rcTemp.bottom=rect.top+DWR_VAR_PIXEL_COUNT;
				DrawFocusRect(hdc,&rcTemp);
				
				rcTemp.left=rect.right-DWR_VAR_PIXEL_COUNT;
				rcTemp.right=rect.right-1;
				DrawFocusRect(hdc,&rcTemp);

			}	
			break;
		}

	}//switch (uFlag)

	SelectObject(hdc,hSaveDcFont);
	
	if(hrgn) DeleteObject(hrgn);
	ReleaseDC(hwndDraw,hdc);
	return TRUE;
	
} //DrawWindowRect()
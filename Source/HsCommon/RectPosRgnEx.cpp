#include <windows.h>
#include "RectPosRgnEx.h"
#include "RectPosRgn.h"

//////////////////////////////////////////////////////////////////////////

VOID ScreenToWindowRect(HWND hwnd, RECT *pRect)
{
	RECT rectBuffer;
	GetWindowRect(hwnd,&rectBuffer);

	pRect->right-=pRect->left;
	pRect->bottom-=pRect->top;
	
	pRect->left-=rectBuffer.left;
	pRect->top-=rectBuffer.top;
	pRect->right+=pRect->left;
	pRect->bottom+=pRect->top;
}

//////////////////////////////////////////////////////////////////////////

VOID WindowToScreenRect(HWND hwnd, RECT *pRect)
{

	RECT rectBuffer;
	GetWindowRect(hwnd,&rectBuffer);
	
	pRect->right-=pRect->left;
	pRect->bottom-=pRect->top;
	
	pRect->left+=rectBuffer.left;
	pRect->top+=rectBuffer.top;

	pRect->right+=pRect->left;
	pRect->bottom+=pRect->top;

}

//////////////////////////////////////////////////////////////////////////

BOOL IsWndOverWnd(HWND hwnd1, HWND hwnd2)
{
	RECT rc, rcWnd1, rcWnd2;

	GetWindowRect(hwnd1,&rcWnd1);
	GetWindowRect(hwnd2,&rcWnd2);
	IntersectRect(&rc,&rcWnd1,&rcWnd2);
				
	return !IsRectEmpty(&rc);

}

//////////////////////////////////////////////////////////////////////////

BOOL IsWndInWnd(HWND hwnd1, HWND hwnd2)
{
	RECT rc1, rc2;

	if(GetWindowRect(hwnd1, &rc1) && 
		GetWindowRect(hwnd2, &rc2) )
	{
		return IsRectInRect(&rc1, &rc2);

	}
	return FALSE;

}

//////////////////////////////////////////////////////////////////////////

BOOL IsPtInWnd(POINT* pPt, HWND hwnd)
{
	RECT rc;
	if(GetWindowRect(hwnd, &rc) )
	{
		return PtInRect(&rc,*pPt);
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL IsWndRectEmpty(HWND hwnd)
{
	RECT rc;

	if(!IsWindow(hwnd))
	{
		return TRUE;
	}
	GetWindowRect(hwnd, &rc);

	return IsRectEmpty(&rc);
}

//////////////////////////////////////////////////////////////////////////

HWND FindNextRealWindowFromPoint(HWND hwndStart, POINT pt)
{
	HWND hwndFind=NULL;
	HWND hwndTest=hwndStart;
	RECT rcTest;
	
	do 
	{
		hwndTest=GetNextWindow(hwndTest, GW_HWNDNEXT);
		
		if ( IsWindowVisible(hwndTest) && 
			GetWindowRect(hwndTest, &rcTest) )
		{
			if ( PtInRect(&rcTest, pt) )
			{
				hwndFind=hwndTest;
				//MessageBoxPrintf(NULL, MB_TOPMOST, " ", TEXT("%X"), hwndFind);
				break; //okay...
			}
		}
		
	} while(hwndTest);
	
	if( hwndFind==NULL ) //no other window found in this level...
	{
		return NULL;
	}
	
	//Find real child window...

	while(hwndTest) //now hwndTest==hwndFind
	{
		hwndTest=GetTopWindow(hwndFind);
		
		while(hwndTest)
		{
			if( IsWindowVisible(hwndTest) && 
				GetWindowRect(hwndTest, &rcTest) )
			{
				if(	PtInRect(&rcTest, pt) )
				{
					hwndFind=hwndTest;
					break;
				}
			}
			
			hwndTest=GetNextWindow(hwndTest,GW_HWNDNEXT);	
		}
	}

	return hwndFind;

}


//////////////////////////////////////////////////////////////////////////

#define IsRealChildWindow(_hwnd)  \
(GetParent(_hwnd) && !GetWindow(_hwnd, GW_OWNER) && \
WS_CAPTION!=(GetWindowLong(_hwnd, GWL_STYLE)&WS_CAPTION))

//////////////////////////////////////////////////////////////////////////

HWND DescendantWindowFromPoint(HWND hwndChild, POINT pt, BOOL IsDeepFind)
{
	HWND hwndFindPre=NULL;
	HWND hwndTest;
	RECT rcFind;
	RECT rcTest;

	if( IsRealChildWindow(hwndChild) )
	{
		//ensure finding any disable window...
		
		hwndTest=GetWindow(hwndChild, GW_HWNDFIRST);

		GetWindowRect(hwndChild, &rcFind);	
		
		while(hwndTest)
		{
			while(hwndTest)
			{
				if( (IsDeepFind||IsWindowVisible(hwndTest)) )
				{
					GetWindowRect(hwndTest, &rcTest);
					if( PtInRect(&rcTest, pt) &&
						IsRectInRect(&rcTest, &rcFind) )
					{
						hwndChild=hwndTest;
						rcFind=rcTest;
					}
				}

				hwndTest=GetWindow(hwndTest, GW_HWNDNEXT);	
			}
			
			if( hwndFindPre!=hwndChild )
			{
				hwndFindPre=hwndChild;
				hwndTest=GetWindow(hwndChild, GW_CHILD);
			}
		}
	}

	return hwndChild;

}

//////////////////////////////////////////////////////////////////////////

#define FW_IS_DEEPDIG			( LOWORD(dwFlag) )
#define FW_IS_FIND_INVISIBLE	( !HIWORD(dwFlag) )
#define FW_IS_NEEDLASTFIND	( !dwFlag ) // (FW_IS_FIND_INVISIBLE && !FW_IS_DEEPDIG)

HWND FindRealWindowFromPointEx(HWND hwndSkip, POINT pt, DWORD dwFlag)
{
	HWND hwndFind;
	HWND hwndTest;
 	RECT rcTest;
	POINT ptTest=pt;

	// Firstly, we use WindowFromPoint() to find.
	
	if( NULL==(hwndFind=WindowFromPoint(pt)) )
	{
		return NULL; //if error...
	}
	
	// Why we call the following GetAncestor? For 2 purposes:
	// [1] For Checking if hwndSkip...
	// [2] For Checking if a top window disabled by a model window...
	// If [1] || [2] , we have to get first toplevel child window ourself...
	
	hwndTest=GetAncestor(hwndFind, GA_ROOT);

	// If it is hwndSkip, find the next window (contains pt) in hwndSkip's Z-level.

	if( (hwndTest==hwndSkip) && hwndSkip )
	{
		do
		{
			hwndTest=GetWindow(hwndTest, GW_HWNDNEXT);
			
			if ( IsWindowVisible(hwndTest) )
			{
				GetWindowRect(hwndTest, &rcTest);
				if ( PtInRect(&rcTest, pt) )
				{
					hwndFind=hwndTest;
					break; //okay...
				}
			}
			
		} while(hwndTest);
		
		if( hwndTest==NULL ) //no other window found in this level...
		{
			return NULL;
		}

	}
	
	// (hwndTest==hwndFind) indicates hwndFind now is a toplevel window.
	
	if( hwndTest==hwndFind )
	{
		// Get it's first toplevel (deepest-level) child (contains pt)...

		while(hwndTest)
		{
			hwndTest=GetWindow(hwndFind, GW_CHILD);
			
			while(hwndTest)
			{
				if ( IsWindowVisible(hwndTest) )
				{
					GetWindowRect(hwndTest, &rcTest);
					if(	PtInRect(&rcTest, pt) )
					{
						hwndFind=hwndTest;
						break; //only find first visible child (contains pt)..
					}
				}
				
				hwndTest=GetWindow(hwndTest, GW_HWNDNEXT);	
			}
		}
	}

	// Then, find the deepin layout if now hwndFind is a real child...
	
	hwndFind=DescendantWindowFromPoint(hwndFind, pt, FW_IS_DEEPDIG);

	// We call ChildWindowFromPointEx to ensure finding the child 
	// (if any) which is not entirely placed in it's parent window's rect. 
	
	ScreenToClient(hwndFind, &ptTest);

	hwndTest=ChildWindowFromPointEx(hwndFind, 
		ptTest, FW_IS_NEEDLASTFIND?CWP_ALL:CWP_SKIPINVISIBLE);
	
	if( hwndTest )
	{
		hwndFind=hwndTest;
	}

	// Lastly, Find out any invisable descendant (contains pt) if needed...

	if( FW_IS_NEEDLASTFIND )
	{

		hwndFind=DescendantWindowFromPoint(hwndFind, pt, TRUE);

	}

	return hwndFind;

}

////////////////////////////////////////////////////////////////////////


RECT* AdjustRectInScreen(RECT * pRect)
{
	RECT rect, rc;
	int iX, iY;

	CopyMemory(&rect,pRect,sizeof(RECT));

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

	iX = rc.right - rc.left  - rect.right + rect.left;
	iY = rc.bottom - rc.top - rect.bottom + rect.top;
	
	if ( pRect->top > iY )
		pRect->top = iY, pRect->bottom = rc.bottom - rc.top;
	
	if ( pRect->top < 0 )
		pRect->top = 0, pRect->bottom = rect.bottom - rect.top;
	
	if ( pRect->left > iX )
		pRect->left = iX, pRect->right = rc.right - rc.left;

	if ( pRect->left < 0 )
		pRect->left = 0, pRect->right = rect.right - rect.left;
	
	return pRect;
}

//////////////////////////////////////////////////////////////////////////

void MoveWndToWndCenter(HWND hwndMoving, HWND hwndDest)
{

	RECT rc; 
	RECT rect;
	
	if(IsIconic(hwndDest) || 
		!IsWindowVisible(hwndDest))
	{
		hwndDest=GetDesktopWindow();
	}
	GetWindowRect(hwndDest, &rc);
	GetWindowRect(hwndMoving, &rect);

	rc.left = (rc.right + rc.left - rect.right + rect.left) >> 1;
	rc.top = (rc.bottom + rc.top - rect.bottom + rect.top) >> 1;
	rc.right = rc.left + (rect.right - rect.left);
	rc.bottom = rc.top + (rect.bottom - rect.top);

	AdjustRectInScreen(&rc);

	SetWindowPos(hwndMoving, NULL, 
		rc.left, rc.top, 
		0, 0, SWP_NOZORDER | SWP_NOSIZE) ;

}


///////////////////////////////////////////////////////////

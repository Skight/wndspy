#include <windows.h>
#include "RectPosRgn.h"


BOOL IsRectOverRect(const RECT* pRect1, const RECT* pRect2)
{
	//Is Rect1 in Rect2
	
	return ( /*bottom-left*/ 
		(pRect1->bottom<pRect2->bottom) && 
		(pRect1->bottom>pRect2->top) && 
		(pRect1->left>pRect2->left) && 
		(pRect1->left<pRect2->right) 
		
		|| /*bottom-right*/
		(pRect1->bottom<pRect2->bottom) &&
		(pRect1->bottom>pRect2->top) &&
		(pRect1->right>pRect2->left) &&
		(pRect1->right<pRect2->right) 
		
		|| /*up-left*/
		(pRect1->top>pRect2->top) && 
		(pRect1->top<pRect2->bottom) && 
		(pRect1->left>pRect2->left) && 
		(pRect1->left<pRect2->right) 
		
		|| /*up-right*/
		(pRect1->top>pRect2->top) && 
		(pRect1->top<pRect2->bottom) && 
		(pRect1->right>pRect2->left) && 
		(pRect1->right<pRect2->right) 
		);
}

BOOL IsRectInRect(const RECT* pRect1, const RECT* pRect2)
{
	//Is Rect1 in Rect2 
	
	return ( 
			/*not larger*/
		!((pRect1->top < pRect2->top) || 
		(pRect1->left < pRect2->left) ||
		(pRect1->bottom > pRect2->bottom) ||
		(pRect1->right > pRect2->right)) 
			&&
		/*and not equal*/
		(pRect1->left != pRect2->left ||
		pRect1->right != pRect2->right ||
		pRect1->top != pRect2->top ||
		pRect1->bottom != pRect2->bottom)
		);
}

//////////////////////////////////////////////////////////////////////////
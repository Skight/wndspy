#include <windows.h>
#include "subwndcls.h"
//////////////////////////////////////////////////////////////////////////


LONG SetWndTargetSubClass(HWND hwnd, INT nChildItemID, WNDPROC SubClassWndProc)
{
	if(nChildItemID)
	{
		hwnd=GetDlgItem(hwnd, nChildItemID);
	}
	return SetWindowLong(hwnd, GWL_USERDATA, 
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)SubClassWndProc));
}


LONG SetWndTargetSubClassParam(HWND hwnd, INT nChildItemID, WND_SUBCLASS_DATA* lpSubClassData)
{
	if(nChildItemID)
	{
		hwnd=GetDlgItem(hwnd, nChildItemID);
	}
	lpSubClassData->WndProcBuf=(WNDPROC)
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)lpSubClassData->WndProcBuf);
	return SetWindowLong(hwnd, GWL_USERDATA, (LONG)lpSubClassData);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//SubClassWndProc : SCP_Btn_NoDefPush
LRESULT APIENTRY SCP_ChkBtn_NoFoucsRect(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	switch (message)
	{

	case WM_SETFOCUS:
		return 0;	
		
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}


////////////////////////////////////////////////////////////////////////////////////////////////
//SubClassWndProc : SCP_Btn_NoDefPush
LRESULT APIENTRY SCP_Btn_NoDefPush(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
#define VK_CTRL_A	1
#define VK_CTRL_C	3
	
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	switch (message)
	{

	case BM_SETSTYLE:
		wParam&=~wParam;
		return 0;

	case WM_STYLECHANGING:
		if(wParam==GWL_STYLE)
		{
			((STYLESTRUCT*)lParam)->styleNew&=~BS_DEFPUSHBUTTON;
		}
		return 0;

	case WM_DESTROY:
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)OriginWndProc);
		break; //WM_DESTROY		
		
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
//SubClassWndProc : SCP_Static_HandCusror
LRESULT APIENTRY SCP_Static_HandCusror(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
//	STYLESTRUCT ss;
	switch (message)
	{

	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
		CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
		SetCursor(LoadCursor(NULL,IDC_HAND));
		return 0;


	case WM_DESTROY:
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)OriginWndProc);
		
		break; //WM_DESTROY		
		
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//SubClassWndProc : SCP_Edit_HexNum
LRESULT APIENTRY SCP_Edit_HexNum (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	LPTSTR szCheck;
	switch (message)
	{
	case WM_DESTROY:
		{
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG)OriginWndProc);
			break; //WM_DESTROY
		}

	case WM_CHAR:
		
		if (wParam == VK_CTRL_A)
		{
			SendMessage (hwnd, EM_SETSEL, 0, -1) ;
			return 0;
		}
		else if (wParam == VK_BACK || wParam == VK_CTRL_C
			|| wParam == VK_CTRL_V || wParam == VK_CTRL_Z )
		{
			break;
		}
		else if ( IsInCharSet((TCHAR)wParam, TEXT("0123456789ABCDEF\0"), FALSE) )
		{
			break;
		}

		MessageBeep(0);
		return 0;
		//WM_CHAR
		
	case WM_PASTE:
		{
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			OpenClipboard(hwnd);
			szCheck = (TCHAR *)GetClipboardData(CF_TEXT);
	
			CloseClipboard();
			if(IsCharSetString(szCheck,TEXT("0123456789ABCDEF\0"), FALSE))
			{
				break;
			}
		}
		MessageBeep(0);
		return 0;
		} // if..else..
		
	case WM_SETTEXT:
			if(IsCharSetString((LPTSTR)lParam,TEXT("0123456789ABCDEF\0"), FALSE))
			{
				break;
			}
		MessageBeep(0);
		return 0; //WM_SETTEXT
	
	default:
		break;
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}


INT OnNumEditMaxMinRoutine(HWND hwnd, WPARAM wParam, LPARAM lParam, INT iMinNum, INT iMaxNum)
{
	if(HIWORD(wParam) ==EN_CHANGE)
	{
		lParam=GetDlgItemInt(hwnd, LOWORD(wParam), FALSE, FALSE);
		if(lParam > iMaxNum)
		{
			SetDlgItemInt(hwnd, LOWORD(wParam), iMaxNum, FALSE);
			SendDlgItemMessage(hwnd, LOWORD(wParam), EM_SETSEL, 0, -1);
		}
		return lParam;
	}
	else if(HIWORD(wParam) ==EN_KILLFOCUS)
	{
		lParam=max(iMinNum, (INT)GetDlgItemInt(hwnd, LOWORD(wParam), FALSE, FALSE));
		SetDlgItemInt(hwnd, LOWORD(wParam), lParam, FALSE);
		return lParam;
	}

	return 0;
}

//SubClassWndProc : SCP_Edit_DecNum
LRESULT APIENTRY SCP_Edit_DecNum (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	LPTSTR szCheck;
	switch (message)
	{
	case WM_DESTROY:
		{
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG)OriginWndProc);
			break; //WM_DESTROY
		}

	case WM_CHAR:
		
		if (wParam == VK_CTRL_A)
		{
			SendMessage (hwnd, EM_SETSEL, 0, -1) ;
			return 0;
		}
		else if (wParam == VK_BACK || wParam == VK_CTRL_C
			|| wParam == VK_CTRL_V || wParam == VK_CTRL_Z )
		{
			break;
		}
		else if ( IsInCharSet((TCHAR)wParam, TEXT("0123456789-\0"), FALSE) )
		{
			break;
		}
		MessageBeep(0);
		return 0;
		//WM_CHAR
		
	case WM_PASTE:
		{
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			OpenClipboard(hwnd);
			szCheck = (TCHAR *)GetClipboardData(CF_TEXT);
			CloseClipboard();
			if(IsCharSetString(szCheck,TEXT("0123456789-F\0"), FALSE))
			{
				break;
			}
		}
		MessageBeep(0);
		return 0;
		} // if..else..
		
	case WM_SETTEXT:
			if(IsCharSetString((LPTSTR)lParam,TEXT("0123456789-\0"), FALSE))
			{
				break;
			}
		MessageBeep(0);
		return 0; //WM_SETTEXT
	
	default:
		break;
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}

//////////////////////////////////////////////////////////////////////////
/*
//SubClassWndProc : SCP_Edit_Num
LRESULT APIENTRY SCP_Edit_Num (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{

// 适用于处理32bit整数的编辑框,比ES_NUMBER风格的Edit用途广...

	TCHAR * szCheck;
	TCHAR szNum[EDIT_MAX_STR_LENGTH + 1];
	
	int i, iLength, iMaxLen , iNum;
	int iStartSel, iEndSel;
	
	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	
	switch (message)
	{
		
	case WM_CREATE:
		
		CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
		SendMessage(hwnd, EM_LIMITTEXT, (WPARAM)EDIT_MAX_STR_LENGTH, 0);
		
		return 0;
		
	case WM_CHAR:
		
		if (wParam == VK_CTRL_A)
		{
			SendMessage (hwnd, EM_SETSEL, 0, -1) ;
			return 0;
		}
		else if (wParam == VK_BACK || wParam == VK_CTRL_C
			|| wParam == VK_CTRL_V || wParam == VK_CTRL_Z )
		{
			break;
		}
		else if ( (TCHAR)wParam >= '0' && (TCHAR)wParam <= '9' )
		{
			
			SendMessage(hwnd, EM_GETSEL, 
				(WPARAM)(&iStartSel), (LPARAM)(&iEndSel));
			
			if( (iEndSel - iStartSel) < 0 )
			{
				break;
			}
			
			iMaxLen = (EDIT_MAX_STR_LENGTH + 1 );
			iLength = GetWindowText(hwnd, szNum, iMaxLen);
			szNum[EDIT_MAX_STR_LENGTH]='\0';
			
			if( iLength - (iEndSel - iStartSel) > iMaxLen)
			{
				wsprintf(szNum, TEXT("%d\0"), NUM_MAX_VALUE);
				SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)szNum);
				
				return 0;
			}
			
			szNum[iStartSel]=(TCHAR)wParam;
			
			for (i=0; i< (iLength-iEndSel); i++)
			{
				szNum[iStartSel+1+i] = szNum[iEndSel+i];
			}
			
			szNum[iStartSel+1+i]='\0';
			
			iLength = lstrlen(szNum);
			
			for (i=0,iNum=0 ; i < iLength; i++)
			{
				iNum += 
					(int)((szNum[iLength - 1 - i] - '0') 
					* 
					(int)pow((double)10, i));
			}
			
			if (iNum > NUM_MAX_VALUE )
			{
				wsprintf(szNum, TEXT("%d\0"), NUM_MAX_VALUE);
				SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)szNum);
				
				return 0;
			}
			
			break;
		}
		else
		{
			MessageBeep(0xFFFFFFFF);
			return 0;
		} //WM_CHAR
		
	case WM_PASTE:
		
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			OpenClipboard(hwnd) ;
			
			szCheck = (TCHAR *)GetClipboardData(CF_TEXT);
			
			CloseClipboard() ;
		}
		else
		{
			return 0;
		}
		
		SendMessage (hwnd, EM_GETSEL, 
			(WPARAM)(&iStartSel), (LPARAM)(&iEndSel)) ;
		
		iMaxLen = (iEndSel - iStartSel) < 0 ? 
			(EDIT_MAX_STR_LENGTH)
			: 
		( (iEndSel - iStartSel) + 
			EDIT_MAX_STR_LENGTH - GetWindowTextLength(hwnd) ) ;
		
		iLength= lstrlen(szCheck);
		
		if ( iLength > iMaxLen)
		{
			MessageBeep(0xFFFFFFFF);
			return 0;
		}
		else
		{
			
			for (i = 0, iNum=0; i < iLength ;i++)
			{
				if ( szCheck[iLength - 1 - i] > '9' || 
					szCheck[iLength - 1 - i] < '0')
				{
					MessageBeep(0xFFFFFFFF);
					return 0;
				}
				
				iNum += 
					(int)((szCheck[iLength - 1 - i] - '0') * 
					(int)pow((double)10, i));
			}
			
			if (iNum > NUM_MAX_VALUE )
			{
				wsprintf(szNum, TEXT("%d\0"), NUM_MAX_VALUE);
				SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)szNum);
				return 0;
			}
			
			CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
			
			GetWindowText(hwnd, szNum, EDIT_MAX_STR_LENGTH + 1);
			SendMessage(hwnd, WM_SETTEXT, 
				(WPARAM)(iStartSel+iLength), (LPARAM)szNum);
			
			return 0;
			
		} // if..else..
		
		break; //WM_PASTE
		
		
	case WM_SETTEXT:
		szCheck = (TCHAR *)lParam;
		iMaxLen = EDIT_MAX_STR_LENGTH +1;
		iLength = lstrlen(szCheck);
		
		if (iLength > iMaxLen)
		{
			wsprintf(szNum, TEXT("%d\0"), NUM_MAX_VALUE);
			SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)szNum);
			SendMessage (hwnd, EM_SETSEL, 0, -1) ;
			return 0;
		}
		else
		{
			for (i = 0, iNum=0; i < iLength ;i++)
			{
				if ( szCheck[iLength - 1 - i] > '9' || 
					szCheck[iLength - 1 - i] < '0')
				{
					MessageBeep(0xFFFFFFFF);
					return 0;
				}
				iNum += 
					(int)((szCheck[iLength - 1 - i] - '0') 
					* 
					(int)pow((double)10, i));
			}
			
			if (iNum > NUM_MAX_VALUE )
			{
				wsprintf(szNum, TEXT("%d\0"), NUM_MAX_VALUE);
				SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)szNum);
				SendMessage (hwnd, EM_SETSEL, 0, -1) ;
				return 0;
			}
			
		}
		
		CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
		if(wParam != NULL)
		{
			SendMessage (hwnd, EM_SETSEL, wParam, (LPARAM)wParam) ;
		}
		else
		{
			SendMessage (hwnd, EM_SETSEL, 0, -1) ;
		}
		return 0; //WM_SETTEXT
		
		
	case WM_KILLFOCUS:
		
		iMaxLen = EDIT_MAX_STR_LENGTH +1;
		iLength = GetWindowText(hwnd, szNum, iMaxLen);

		for (i = 0, iNum = 0; i < iLength; i++)
		{
			iNum += 
				(int)((szNum[iLength - 1 - i] - '0') 
				* 
				(int)pow((double)10, i));
		}
		
		wsprintf(szNum, TEXT("%d\0"), iNum); 
		
		if (iNum < NUM_MIN_VALUE)
		{
			wsprintf(szNum, TEXT("%d\0"), NUM_MIN_VALUE);
			
		}
		
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)szNum);
		SendMessage (hwnd, EM_SETSEL, 0, 0) ;
		break; //WM_KILLFOCUS
		
	case WM_DESTROY:
		
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)OriginWndProc);
		break; //WM_DESTROY
		
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
} **/

//////////////////////////////////////////////////////////////////////////
//SubClassWndProc : SCP_Edit_SelAll
LRESULT APIENTRY SCP_Edit_SelAll (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{

	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	
	switch (message)
	{
	case WM_CHAR:
		if (wParam == VK_CTRL_A)
		{
			SendMessage(hwnd, EM_SETSEL, 0, -1) ;
			return 0;
		}
		break;
		
	case WM_DESTROY:
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)OriginWndProc);
		break; //WM_DESTROY
		
	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}

//////////////////////////////////////////////////////////////////////////

//SubClassWndProc : SCP_Edit_ReadOnly
LRESULT APIENTRY SCP_Edit_ReadOnly (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{

	WNDPROC	OriginWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_USERDATA);
	
	switch (message)
	{
	case WM_CHAR:
		if (wParam == VK_CTRL_A)
		{
			SendMessage(hwnd, EM_SETSEL, 0, -1) ;
		}//fall through...
	case WM_PASTE:
		{
			return 0;
		}

	} //switch (message)
	
	return CallWindowProc(OriginWndProc, hwnd, message, wParam, lParam);
	
}
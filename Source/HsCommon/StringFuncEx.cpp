#include <windows.h>
#include "stringfunc.h"
#include "stringfuncEx.h"

//////////////////////////////////////////////////////////////////////////
#define MIN_TIMEOUT	128

#define END_PROGRESS_RANGE	32
//////////////////////////////////////////////////////////////////////////



INT GetWindowInteger(HWND hwnd, BOOL bIsHex)
{
	DWORD dwRes=0;
	TCHAR szStrInt[MAX_STRLEN_LONGINT];
	
	szStrInt[0]='\0', szStrInt[MAX_STRLEN_LONGINT-1]='\0';

	SendMessageTimeout(hwnd, WM_GETTEXT, 
		MAX_STRLEN_LONGINT, (LPARAM)szStrInt, 
		SMTO_NOTIMEOUTIFNOTHUNG, MIN_TIMEOUT, &dwRes);
	if( !dwRes )
	{
		return 0;
	}

	return bIsHex ? HexStrToInt(szStrInt) : DecStrToInt(szStrInt);
}

BOOL IsWndTextIntStr(HWND hwnd, BOOL bIsHex)
{
	TCHAR szBuf[TINY_STRBUF_LEN];
	szBuf[0]=0;
	
	SendMessageTimeout(hwnd, WM_GETTEXT, 
		TINY_STRBUF_LEN, (LPARAM)szBuf, 
		SMTO_NOTIMEOUTIFNOTHUNG, MIN_TIMEOUT, NULL);

	return IsIntStr(szBuf, bIsHex);
}

INT CopyWndTextToWnd(HWND hwndFrom, HWND hwndTo, INT iMaxLen, 
					 LPTSTR szSuffix, HINSTANCE hInstance, UINT uStrResID)
{

#define MAX_SUFFIX_LEN		64
#define MAX_CHECK_TIRM		16
#define MAX_COPY_BUF_LEN	(MAX_STRBUF_LEN+64)

	INT iLen=0;
	TCHAR szCaption[MAX_COPY_BUF_LEN];
	
	iMaxLen=min(MAX_STRBUF_LEN-MAX_SUFFIX_LEN, iMaxLen);
	
	//add terminating char before WM_GETTEXT...
	szCaption[iMaxLen+MAX_SUFFIX_LEN]='\0';
	if( !SendMessageTimeout(hwndFrom, WM_GETTEXT, 
				(iMaxLen+MAX_SUFFIX_LEN), (LPARAM)szCaption, 
				SMTO_NOTIMEOUTIFNOTHUNG, 
				200, NULL/*(DWORD*)&iLen*/)
				)
	{
		szCaption[0]='\0'; 
	}
	
	//better to use lstrlen() for non-ASCII text..
	iLen=lstrlen(szCaption);
	if( iLen > iMaxLen )
	{
		szCaption[iMaxLen]='\0';
		if( !SendMessageTimeout(hwndFrom, WM_GETTEXT, 
			iMaxLen, (LPARAM)szCaption, 
			SMTO_NOTIMEOUTIFNOTHUNG, 
			200, NULL/*(DWORD*)&iLen*/)
			)
		{ 
			szCaption[0]='\0'; 
		}
		else
		{
			iLen=lstrlen(szCaption);
			for(int i=iLen; i> iLen-MAX_CHECK_TIRM; i--)
			{
				if( (szCaption[i]==' '  ||
					szCaption[i]=='\n') && 
					szCaption[i-1] < '\0xFF' )
				{
					if(szCaption[i-1]=='\r')
					{
						i--;
					}
					szCaption[i]='\0';
					break;
				}
			}
			
			if(szSuffix)
			{
				lstrcpyn(szCaption+lstrlen(szCaption), szSuffix, MAX_SUFFIX_LEN);
			}
			else if( hInstance && uStrResID )
			{
				TCHAR szBuf[MAX_SUFFIX_LEN];
				LoadString(hInstance, uStrResID, szBuf, MAX_SUFFIX_LEN);
				lstrcat(szCaption, szBuf);
			}
			else
			{
				lstrcat(szCaption, TEXT("..."));
			}
		}
	}

	SendMessageTimeout(hwndTo, WM_SETTEXT, 
				0, (LPARAM)szCaption, 
				SMTO_NOTIMEOUTIFNOTHUNG, 200, NULL);

	return lstrlen(szCaption);
}

////////////////////////////////////////////////////////////////////////

INT GetPasswordEditText(HWND hwndPWEdit, LPTSTR szPassword)
{
	
	DWORD uCharOrigin;
	if(IsWindowUnicode(hwndPWEdit))
	{
		SendMessageTimeoutW(hwndPWEdit, EM_GETPASSWORDCHAR, 0, 0,
			SMTO_NOTIMEOUTIFNOTHUNG, 100, &uCharOrigin);
		PostMessageW(hwndPWEdit, EM_SETPASSWORDCHAR, 0, 0);
		Sleep(20); //等待PostMessage操作...
		SendMessageTimeout(hwndPWEdit, WM_GETTEXT, MAX_PATH, (LPARAM)szPassword, 
			SMTO_NOTIMEOUTIFNOTHUNG, 100, NULL);
		PostMessageW(hwndPWEdit, EM_SETPASSWORDCHAR, (WPARAM)uCharOrigin, 0);
	}
	else
	{
		SendMessageTimeoutA(hwndPWEdit, EM_GETPASSWORDCHAR, 0, 0, 
			SMTO_NOTIMEOUTIFNOTHUNG, 100, &uCharOrigin);
		PostMessageA(hwndPWEdit, EM_SETPASSWORDCHAR, 0, 0);
		Sleep(20); //等待PostMessage操作...
		SendMessageTimeout(hwndPWEdit, WM_GETTEXT, MAX_PATH, (LPARAM)szPassword, 
			SMTO_NOTIMEOUTIFNOTHUNG, 100, NULL);
		PostMessageA(hwndPWEdit, EM_SETPASSWORDCHAR, (WPARAM)uCharOrigin, 0);
	}
	
	return lstrlen(szPassword);
	
} //GetPasswordEditText();

//////////////////////////////////////////////////////////////////////////

INT GetAllTextsFromListWnd(LPTSTR lpsTextBuf, INT iMaxBufLen, 
							HWND hwndList, UINT uMsgGetCount, UINT uMsgGetText, UINT uMsgGetTextLen)
{
	LPTSTR lpszItemBuf=NULL;
	INT iItemBufLen=MAX_STRBUF_LEN;
	INT i, iCount, iBuf;

	if(lpsTextBuf==NULL) //test str len...
	{
		iMaxBufLen=0;
	}
	else
	{
		lpszItemBuf=(LPTSTR)
			LocalAlloc(LPTR, sizeof(TCHAR)*iItemBufLen);
		if(NULL==lpszItemBuf)
		{
			return 0;
		}
	}

	iCount=SendMessage(hwndList, uMsgGetCount, 0, 0);
	if(iCount)
	{
		for(i=0; i< iCount; i++)
		{
			SendMessageTimeout(hwndList, uMsgGetTextLen, i, 0, 
				SMTO_NOTIMEOUTIFNOTHUNG, 200, (DWORD*)&iBuf);

			if(lpsTextBuf==NULL)
			{
				iMaxBufLen+=iBuf+2;
				continue;
			}
			if(iItemBufLen<iBuf)
			{
				lpszItemBuf=(LPTSTR)
					LocalReAlloc((HLOCAL)lpszItemBuf, 
					sizeof(TCHAR)*iItemBufLen, LMEM_ZEROINIT);
			}

			SendMessageTimeout(hwndList, uMsgGetText, i, (LPARAM)lpszItemBuf, 
				SMTO_NOTIMEOUTIFNOTHUNG, 200, (DWORD*)&iBuf);

			if( iMaxBufLen > iBuf+2 )
			{
				lstrcat(lpsTextBuf, lpszItemBuf);
				lstrcat(lpsTextBuf, TEXT("\r\n"));
				iMaxBufLen-=iBuf+2;
				// iMaxBufLen - Catenated str len each time.
			}
			else
			{
				LocalFree((HLOCAL)lpszItemBuf);
				return 0;
			}
		}
	}

	if(lpsTextBuf!=NULL)
	{
		LocalFree((HLOCAL)lpszItemBuf);
		lpsTextBuf[lstrlen(lpsTextBuf)-2]='\0'; //tirm last "\r\n"
		iMaxBufLen=lstrlen(lpsTextBuf);
	}
	return iMaxBufLen;

} //GetAllTextsFromListWnd()

//////////////////////////////////////////////////////////////////////////

INT GetAllTextsFromStatusBar(HWND hwndSB, LPTSTR lpszTextBuf) 
{
	DWORD dwPID;
	HANDLE hProcess=NULL;
	int i;
	INT iPartCount=0;
	INT iTextLen=0;
	INT iBuf;

	TCHAR szPartText[MAX_STRBUF_LEN];
	LPVOID pVMPartStr=NULL; // lvi.lpText pointer in target address space.

	GetWindowThreadProcessId(hwndSB, &dwPID);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|
		PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, dwPID);
	if(hProcess==NULL)
	{
		return -1;
	}

	SendMessageTimeout(hwndSB, SB_GETPARTS, 0, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&iPartCount);

	if(lpszTextBuf)
	{
		pVMPartStr = VirtualAllocEx(hProcess, NULL, 
			MAX_STRBUF_SIZE, MEM_COMMIT, PAGE_READWRITE);
	}

	for(i=0; i<iPartCount; i++)
	{

		if(lpszTextBuf)
		{
			SendMessageTimeout(hwndSB, SB_GETTEXT, i, (LPARAM)pVMPartStr, 
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, NULL);

			if( ReadProcessMemory(hProcess, 
				pVMPartStr, szPartText, MAX_STRBUF_SIZE, NULL) )
			{
				lstrcat(lpszTextBuf, szPartText);
				lstrcat(lpszTextBuf, TEXT("\r\n"));
			}

		}
		else
		{
			SendMessageTimeout(hwndSB, SB_GETTEXTLENGTH, i, 0, 
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&iBuf);
			if( LOWORD(iBuf) >0 )
			{
				iTextLen+=LOWORD(iBuf)+2;
			}
		}
	}

	if(pVMPartStr)
	{
		VirtualFreeEx(hProcess, pVMPartStr, 0, MEM_RELEASE);
	}

	CloseHandle(hProcess);

	return lpszTextBuf ? lstrlen(lpszTextBuf) : iTextLen;

} //GetAllTextsFromStatusBar()

//////////////////////////////////////////////////////////////////////////

INT GetAllTextsFromListViewCtrl(HWND hwndLV, LPTSTR lpszTextBuf, 
								HWND hwndMsgOnwer, HWND hwndProgress, BOOL isGrouping) 
{
	DWORD dwPID;
	HANDLE hProcess=NULL;
	LVITEM lvi={0};
	int i, j;
	INT iCounter;
	HWND hwndHeader=GetWindow(hwndLV, GW_CHILD);
	INT nItemCount, nSubItemCount;
	LPVOID pVMItemData=NULL; //LVITEM pointer in target address space.
	LPVOID pVMItemStr=NULL;  //lvi.lpText pointer in target address space.
	TCHAR szBuf[MAX_STRBUF_LEN]; //receive buffer for lvi.lpText.
	BOOL bSuccess=TRUE;

	
	GetWindowThreadProcessId(hwndLV, &dwPID);
	hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, dwPID);
	if(hProcess==NULL)
	{
		return -1;
	}
	
	SendMessageTimeout(hwndLV, LVM_GETITEMCOUNT, 0, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&nItemCount);
	SendMessageTimeout(hwndHeader, HDM_GETITEMCOUNT, 0, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&nSubItemCount);

	iCounter = nItemCount * max(nSubItemCount, 1);
	if(lpszTextBuf==NULL || iCounter==0)
	{
		DoCloseHandle(hProcess);
		return (MAX_STRBUF_LEN * 
			(iCounter+(nSubItemCount?1:0))); //no less
	}

	if(hwndProgress)
	{
		SendMessage(hwndProgress, PBM_SETPOS, 0, 0);
		SendMessage(hwndProgress, PBM_SETRANGE32, 0, iCounter+END_PROGRESS_RANGE);
		SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
	}

	pVMItemData = VirtualAllocEx(hProcess, NULL, 
		sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
	
	pVMItemStr = VirtualAllocEx(hProcess, NULL, 
		MAX_STRBUF_SIZE, MEM_COMMIT, PAGE_READWRITE);
	
	lvi.iItem=0;
	lvi.iSubItem=0;
	lvi.pszText=(LPTSTR)pVMItemStr;
	lvi.cchTextMax=MAX_STRBUF_LEN;

	if(nSubItemCount)
	{
		GetAllTextsFromHeaderCtrl(hProcess, hwndHeader, TEXT("*"), lpszTextBuf, isGrouping);
		lstrcat(lpszTextBuf, TEXT("\r\n"));
	}
	
	nSubItemCount=max(nSubItemCount, 1);

	for(i = 0; i < nItemCount; i++)
	{
		if(isGrouping)
		{
			lstrcatchar(lpszTextBuf, '-', 20);
		}
		lstrcat(lpszTextBuf, TEXT("\r\n"));
		for(j=0; j<nSubItemCount; j++)
		{
			lvi.iSubItem=j;
			if( !WriteProcessMemory(hProcess, 
				pVMItemData, &lvi, sizeof(LVITEM), NULL) )
			{
				bSuccess=FALSE;
				goto get_lv_text_error;
			}
			SendMessageTimeout(hwndLV, LVM_GETITEMTEXT, 
				(WPARAM)i, (LPARAM)pVMItemData, 
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, NULL);
			
			if( !ReadProcessMemory(hProcess, 
				pVMItemStr, szBuf, MAX_STRBUF_SIZE, NULL) )
			{
				bSuccess=FALSE;
				goto get_lv_text_error;
			}
			
			lstrcat(lpszTextBuf, szBuf);
			if(isGrouping)
			{
				lstrcat(lpszTextBuf, TEXT("\r\n"));
			}
			else if(j!=nSubItemCount-1)
			{
				lstrcat(lpszTextBuf, TEXT("\t"));
			}
			if(hwndProgress)
			{
				SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
			}
		}
		lstrcat(lpszTextBuf, TEXT("\r\n"));
	}
	
	goto get_lv_text_free;

get_lv_text_error:
	EnableWindow(GetParent(hwndProgress), FALSE);
	LastErrorMessageBox(hwndMsgOnwer, szBuf, MAX_STRBUF_LEN);
	lstrcat(lpszTextBuf, szBuf);

get_lv_text_free:
	
	VirtualFreeEx(hProcess, pVMItemData, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, pVMItemStr, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	if(bSuccess==TRUE)
	{
		return lstrlen(lpszTextBuf);
	}
	return -1;

}//GetAllTextsFromListViewCtrl()

INT GetAllTextsFromHeaderCtrl(HANDLE hProcess, HWND hwndHeader, LPTSTR szPrefix, LPTSTR lpszTextBuf, BOOL isGrouping) 
{
	BOOL bSuccess=TRUE;
	DWORD dwPID=0;
	HDITEM hdi={0};
	INT nItemCount;
	LPVOID pVMItemData=NULL;
	LPVOID pVMItemStr=NULL;
	TCHAR szBuf[MAX_STRBUF_LEN]; //receive buffer for lvi.lpText.

	if(NULL==hProcess)
	{
		GetWindowThreadProcessId(hwndHeader, &dwPID);
		hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, dwPID);
		if(hProcess==NULL)
		{
			return -1;
		}
	}
	
	SendMessageTimeout(hwndHeader, HDM_GETITEMCOUNT, 0, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&nItemCount);
	
	if(lpszTextBuf==NULL || nItemCount==0)
	{
		if(dwPID)
		{
			DoCloseHandle(hProcess);
		}
		return (MAX_STRBUF_LEN * nItemCount);
	}

	pVMItemData = VirtualAllocEx(hProcess, NULL, 
		sizeof(LPHDITEM), MEM_COMMIT, PAGE_READWRITE);
	
	pVMItemStr = VirtualAllocEx(hProcess, NULL, 
		MAX_STRBUF_SIZE, MEM_COMMIT, PAGE_READWRITE);

	hdi.mask=HDI_TEXT;
	hdi.pszText=(LPTSTR)pVMItemStr;
	hdi.cchTextMax = MAX_STRBUF_LEN;

	if( WriteProcessMemory(hProcess, pVMItemData, &hdi, sizeof(HDITEM), NULL) )
	{
		for(int i = 0; i < nItemCount; i++)
		{
			SendMessageTimeout(hwndHeader, HDM_GETITEM, 
				(WPARAM)i, (LPARAM)pVMItemData, 
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, NULL);
			
			if( !ReadProcessMemory(hProcess, 
				pVMItemStr, szBuf, MAX_STRBUF_SIZE, NULL) )
			{
				bSuccess=FALSE;
				break;
			}
			if(szPrefix)
			{
				lstrcat(lpszTextBuf, szPrefix);
				lstrcat(lpszTextBuf, TEXT(" "));
			}
			lstrcat(lpszTextBuf, szBuf);
			lstrcat(lpszTextBuf, isGrouping?TEXT("\r\n"):TEXT("\t"));
		}
	}

	VirtualFreeEx(hProcess, pVMItemData, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, pVMItemStr, 0, MEM_RELEASE);
	if(dwPID)
	{
		CloseHandle(hProcess);
	}
	if(bSuccess==TRUE)
	{
		return lstrlen(lpszTextBuf);
	}
	return -1;

}//GetAllTextsFromHeaderCtrl()

//////////////////////////////////////////////////////////////////////////

INT GetAllTextsFromTabCtrl(HWND hwndTab, LPTSTR lpszTextBuf) 
{
	BOOL bSuccess=TRUE;
	HANDLE hProcess=NULL;
	DWORD dwBuf=0;
	TCITEM tci={0};
	INT nItemCount;
	LPVOID pVMItemData=NULL;
	LPVOID pVMItemStr=NULL;
	TCHAR szBuf[MAX_STRBUF_LEN]; //receive buffer for lvi.lpText.


	GetWindowThreadProcessId(hwndTab, &dwBuf);
	hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, dwBuf);
		if(hProcess==NULL)
		{
			return -1;
		}
	
	SendMessageTimeout(hwndTab, TCM_GETITEMCOUNT, 0, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&nItemCount);
	
	if(lpszTextBuf==NULL || nItemCount==0)
	{
		DoCloseHandle(hProcess);
		return (MAX_STRBUF_LEN * nItemCount);
	}

	pVMItemData = VirtualAllocEx(hProcess, NULL, 
		sizeof(LPHDITEM), MEM_COMMIT, PAGE_READWRITE);
	
	pVMItemStr = VirtualAllocEx(hProcess, NULL, 
		MAX_STRBUF_SIZE, MEM_COMMIT, PAGE_READWRITE);

	tci.mask=TCIF_TEXT;
	tci.pszText=(LPTSTR)pVMItemStr;
	tci.cchTextMax = MAX_STRBUF_LEN;

	if( WriteProcessMemory(hProcess, pVMItemData, &tci, sizeof(TCITEM), NULL) )
	{
		for(int i = 0; i < nItemCount; i++)
		{
			SendMessageTimeout(hwndTab, TCM_GETITEM, 
				(WPARAM)i, (LPARAM)pVMItemData, 
				SMTO_NOTIMEOUTIFNOTHUNG, 1000, &dwBuf);
			
			if( !ReadProcessMemory(hProcess, 
				pVMItemStr, szBuf, MAX_STRBUF_SIZE, NULL) )
			{
				bSuccess=FALSE;
				break;
			}
			lstrcat(lpszTextBuf, szBuf);
			lstrcat(lpszTextBuf, TEXT("\r\n"));
		}
	}

	VirtualFreeEx(hProcess, pVMItemData, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, pVMItemStr, 0, MEM_RELEASE);

	CloseHandle(hProcess);

	if(bSuccess==TRUE)
	{
		return lstrlen(lpszTextBuf);
	}
	return -1;

}//GetAllTextsFromTabCtrl()

//////////////////////////////////////////////////////////////////////////

VOID GetAllTextsFromTreeItem(HANDLE hProcess, HWND hwndTV, HTREEITEM hTreeItem, 
							LPVOID pVMItemData, LPVOID pVMItemStr, 
							INT ilevel, LPTSTR lpszTextBuf, LPTSTR szBuf, HWND hwndProgress)
{
	TVITEM tvi={0};

	tvi.mask=TCIF_TEXT;
	tvi.cchTextMax = MAX_STRBUF_LEN;
	tvi.pszText=hProcess?(LPTSTR)pVMItemStr:szBuf;;
	
	while(hTreeItem)
	{
		tvi.hItem=hTreeItem;

		if( hProcess )
		{
			if( WriteProcessMemory(hProcess, pVMItemData, &tvi, sizeof(TVITEM), NULL) )
			{
				SendMessageTimeout(hwndTV, TVM_GETITEM, 
				0, (LPARAM)pVMItemData, SMTO_ABORTIFHUNG, 500, NULL);
				ReadProcessMemory(hProcess, 
					pVMItemStr, szBuf, MAX_STRBUF_SIZE, NULL);
			}
		}
		else
		{
			SendMessageTimeout(hwndTV, TVM_GETITEM, 
				0, (LPARAM)&tvi, SMTO_ABORTIFHUNG, 500, NULL);
		}
		
		lstrcatchar(lpszTextBuf, ' ', ilevel*4);
		//lstrcatchar(lpszTextBuf, '\t', ilevel);
		lstrcat(lpszTextBuf, szBuf);
		lstrcat(lpszTextBuf, TEXT("\r\n"));

		if(hwndProgress)
		{
			SendMessage(hwndProgress, PBM_STEPIT, 0, 0);
		}

		SendMessageTimeout(hwndTV, TVM_GETNEXTITEM, 
			TVGN_CHILD, (LPARAM)tvi.hItem, 
			SMTO_NOTIMEOUTIFNOTHUNG, 500, (DWORD*)&hTreeItem);
		if(hTreeItem)
		{
			GetAllTextsFromTreeItem(hProcess, hwndTV, hTreeItem, 
				pVMItemData, pVMItemStr, ilevel+1, lpszTextBuf, szBuf, hwndProgress);
		}
		
		SendMessageTimeout(hwndTV, TVM_GETNEXTITEM, 
			TVGN_NEXT, (LPARAM)tvi.hItem, 
			SMTO_NOTIMEOUTIFNOTHUNG, 500, (DWORD*)&hTreeItem);
	}

} //GetAllTextsFromTreeItem()


INT GetAllTextsFromTreeView(HWND hwndTV, LPTSTR lpszTextBuf, HWND hwndProgress) 
{
	HANDLE hProcess=NULL;
	DWORD dwPID=0;
	HTREEITEM hTreeItem=NULL;
	INT nItemCount;
	LPVOID pVMItemData=NULL;
	LPVOID pVMItemStr=NULL;
	TCHAR szBuf[MAX_STRBUF_LEN];

	GetWindowThreadProcessId(hwndTV, &dwPID);

	if( GetCurrentProcessId()!=dwPID )
	{
		hProcess = OpenProcess(PROCESS_VM_OPERATION|
			PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, dwPID);
		if(hProcess==NULL)
		{
			return -1;
		}
	}
	
	SendMessageTimeout(hwndTV, TVM_GETCOUNT, 0, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&nItemCount);
	
	if( lpszTextBuf==NULL || nItemCount==0 )
	{
		DoCloseHandle(hProcess);
		return (MAX_STRBUF_LEN * nItemCount);
	}

	if( hProcess )
	{
		pVMItemData = VirtualAllocEx(hProcess, NULL, sizeof(TVITEM), MEM_COMMIT, PAGE_READWRITE);
		pVMItemStr = VirtualAllocEx(hProcess, NULL, MAX_STRBUF_SIZE, MEM_COMMIT, PAGE_READWRITE);
	}

	if(hwndProgress)
	{
		SendMessage(hwndProgress, PBM_SETPOS, 0, 0);
		SendMessage(hwndProgress, PBM_SETRANGE32, 0, nItemCount+END_PROGRESS_RANGE);
		SendMessage(hwndProgress, PBM_SETSTEP, 1, 0);
	}

	SendMessageTimeout(hwndTV, TVM_GETNEXTITEM, TVGN_ROOT, 0, 
		SMTO_NOTIMEOUTIFNOTHUNG, 1000, (DWORD*)&hTreeItem);

	if(hTreeItem)
	{
		GetAllTextsFromTreeItem(hProcess, hwndTV, hTreeItem, 
			pVMItemData, pVMItemStr, 0, lpszTextBuf, szBuf, hwndProgress);
	}

	if( hProcess )
	{
		VirtualFreeEx(hProcess, pVMItemData, 0, MEM_RELEASE);
		VirtualFreeEx(hProcess, pVMItemStr, 0, MEM_RELEASE);
		CloseHandle(hProcess);
	}

	return lstrlen(lpszTextBuf);


}//GetAllTextsFromTreeView()


//////////////////////////////////////////////////////////////////////////

HFONT DoCreateDefaultFont(LPTSTR szFontName)
{
	LOGFONT lFont;
	lFont.lfHeight = -11; // size 8
	lFont.lfWidth = 0;
	lFont.lfEscapement = 0;
	lFont.lfOrientation = 0;
	lFont.lfWeight = FW_BOLD;
	lFont.lfItalic = 0;
	lFont.lfUnderline = 0;
	lFont.lfStrikeOut = 0;
	lFont.lfCharSet = 0;
	lFont.lfOutPrecision = OUT_STRING_PRECIS;
	lFont.lfClipPrecision = CLIP_STROKE_PRECIS;
	lFont.lfQuality = DEFAULT_QUALITY;
	lFont.lfPitchAndFamily	= FF_SWISS|VARIABLE_PITCH;
	lstrcpyn(lFont.lfFaceName, szFontName, LF_FACESIZE);
	
	return CreateFontIndirect(&lFont);

} //DoCreateDefaultFont()

//////////////////////////////////////////////////////////////////////////
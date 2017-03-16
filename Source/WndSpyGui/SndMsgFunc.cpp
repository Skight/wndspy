#include "WndSpyGui.h"
#include "SndMsgFunc.h"

//////////////////////////////////////////////////////////////////////////
// Global Variable ///////////////////////////////////////////////////////

SENDMSG_HEADER g_smHdr= {0};

//////////////////////////////////////////////////////////////////////////

INT GetDataAlign(DWORD dwSndMsgFlag)
{
	switch( SndMsgOpt_GetDataAlign(dwSndMsgFlag) )
	{

	case SNDMSG_FLAG_ALIG_BYTE:
		return sizeof(BYTE);

	case SNDMSG_FLAG_ALIG_WORD:
		return sizeof(WORD);

	default:
	//case SNDMSG_FLAG_ALIG_DWORD:
		return sizeof(DWORD);
	}

} //GetDataAlign();


BOOL CheckParamNumString(LPTSTR szString, BOOL isHex)
{
	if( isHex )
	{
		lstrtrimi(szString, TEXT("0x"));
		return IsCharSetString(szString, TEXT("0123456789ABCDEF- \t\r\n"), FALSE);
				//TEXT("0123456789ABCDEF-, \t\r\n")
	}
	else
	{
		return IsCharSetString(szString, TEXT("0123456789- \t\r\n"), FALSE);
				//TEXT("0123456789-, \t\r\n")
	}

} //CheckParamNumString();

void LoadSndMsgParam(LPTSTR szString, SENDMSG_HEADER* pSMH, INT iParamIndex)
{

	INT iAlign=GetDataAlign(pSMH->SMO.dwFlagSMH);
	BOOL IsNumStr=TRUE;
	BOOL IsHex=SndMsgOpt_IsHex(pSMH->SMO.dwFlagSMH);
	WORD wTypeCheck=SndMsgParamOpt_GetDataType(pSMH->SMO.wFlagParam[iParamIndex]);
	
	TCHAR szBuf[MAX_SNDMSG_STR_LEN];
	DWORD dwData[MAX_SNDMSG_PARAM_LEN];
	dwData[0]=0;
	
	//Make a copy...
	lstrcpyn(szBuf, szString, MAX_SNDMSG_STR_LEN); 

	if( SndMsgParamOpt_GetDataFor(pSMH->SMO.wFlagParam[iParamIndex])==SNDMSG_FLAG_IN )
	{
			if( wTypeCheck == NULL )
			{
				IsNumStr=CheckParamNumString(szBuf, IsHex);
			}

			if( !IsNumStr || wTypeCheck == SNDMSG_FLAG_STRING)
			{

				if( wTypeCheck != SNDMSG_FLAG_STRING && 
					szString[0]=='\"' && szString[lstrlen(szString)-1]=='\"' )
				{
					//如果是自动判断型字符串，去掉引号...
					lstrcpyn((LPTSTR)dwData, szString+1, lstrlen(szString)-1);
				}
				else
				{
					//明确设定的字符串...
					lstrcpyn((LPTSTR)dwData, szString, MAX_SNDMSG_STR_LEN);
				}

				SndMsgParamOpt_SetDataType(pSMH->SMO.wFlagParam[iParamIndex], SNDMSG_FLAG_STRING);
				pSMH->SMO.wBufSize[iParamIndex]=(WORD)lstrlen((LPTSTR)dwData)*sizeof(TCHAR)+1;
			}
			else //不是字符串...
			{
				lstrtrimi(szBuf, TEXT("0x")); //for better...
				pSMH->SMO.wBufSize[iParamIndex]=(WORD) 
					NumStrToStruct(szBuf, (PVOID)&dwData, iAlign, IsHex, MAX_SNDMSG_PARAM_LEN);
				
				pSMH->SMO.wBufSize[iParamIndex]=max((WORD)sizeof(DWORD), pSMH->SMO.wBufSize[iParamIndex]);
				
				if( wTypeCheck == NULL)
				{
					SndMsgParamOpt_SetDataType(pSMH->SMO.wFlagParam[iParamIndex], 
						(pSMH->SMO.wBufSize[iParamIndex] == sizeof(DWORD)?SNDMSG_FLAG_DWORD:SNDMSG_FLAG_STRUCT));
				}
			}	

		//全局内存区，用于临时存放参数数据..
		//Here reference to GetSndMsgHeader() call, 
		//it's the reason why ReleaseSndMsgHeader Must be called.
		pSMH->lpData[iParamIndex] =(LPVOID)GlobalAlloc(GPTR, pSMH->SMO.wBufSize[iParamIndex]);
		CopyMemory(pSMH->lpData[iParamIndex], &dwData, pSMH->SMO.wBufSize[iParamIndex]);
	}

} //LoadSndMsgParam();

HWND GetTargetHwndForSMH(HWND hwnd, SENDMSG_HEADER* pSMH)
{
	INT iBuf=DoSelComboBoxString(GetDlgItem(hwnd,IDC_CBDL_HWND));
	
	if(iBuf==CB_ERR)
	{
		HWND hwndEdit=GetWindow(GetDlgItem(hwnd,IDC_CBDL_HWND), GW_CHILD);

		pSMH->hwndTarget=(HWND)GetWindowInteger(hwndEdit, SndMsgOpt_IsHex(pSMH->SMO.dwFlagSMH));

	}
	else if(iBuf==0)
	{
		pSMH->hwndTarget=HWND_BROADCAST;
		return HWND_BROADCAST;
	}

	return IsWindow(pSMH->hwndTarget) ? pSMH->hwndTarget:NULL;

} //GetTargetHwndForSMH();

VOID GetParamsForSMH(HWND hwnd, SENDMSG_HEADER* pSMH, LPTSTR szReadyWPARAM, LPTSTR szReadyLPARAM)
{
	TCHAR szWPARAM[MAX_SNDMSG_STR_LEN];
	TCHAR szLPARAM[MAX_SNDMSG_STR_LEN];
	LPTSTR lpszWPARAM, lpszLPARAM;

	if( szReadyWPARAM )
	{
		lpszWPARAM=szReadyWPARAM;
	}
	else
	{
		GetDlgItemText(hwnd, IDC_EDIT_WPARAM, szWPARAM, MAX_SNDMSG_STR_LEN);
		lpszWPARAM=szWPARAM;
	}

	if( szReadyLPARAM )
	{
		lpszLPARAM=szReadyLPARAM;
	}
	else
	{
		GetDlgItemText(hwnd, IDC_EDIT_LPARAM, szLPARAM, MAX_SNDMSG_STR_LEN);
		lpszLPARAM=szLPARAM;
	}

	LoadSndMsgParam(lpszWPARAM, pSMH, 0);
	LoadSndMsgParam(lpszLPARAM, pSMH, 1);

} //GetParamsForSMH();


INT GetMessageForSMH(HWND hwnd, SENDMSG_HEADER* pSMH)
{

	INT iLen;
	BOOL isHex=SndMsgOpt_IsHex(pSMH->SMO.dwFlagSMH);	
	TCHAR szBuf[MAX_MSG_STR_LEN];

	pSMH->iMsgIndex=DoSelComboBoxString(GetDlgItem(hwnd,IDC_COMBO_MSG));
	
	if(pSMH->iMsgIndex==CB_ERR)
	{
		iLen=GetDlgItemText(hwnd, IDC_COMBO_MSG, szBuf, MAX_MSG_STR_LEN);

		if( IsIntStr(szBuf, isHex) )
		{
			pSMH->uMsg=isHex? HexStrToInt(szBuf):DecStrToInt(szBuf);
		}
		else
		{
			pSMH->uMsg=0;
		}
		if(	pSMH->uMsg<=0 || pSMH->uMsg > 0xFFFF )
		{
			return CB_ERR;
		}
		return (-1-pSMH->uMsg); //for special checking..
	}
	else
	{
		pSMH->uMsg=SendDlgItemMessage(hwnd,IDC_COMBO_MSG, CB_GETITEMDATA, pSMH->iMsgIndex, 0);
		return pSMH->iMsgIndex;
	}

} //GetMessageForSMH();

// Remarks:
// After every GetSndMsgHeader() call, must give a corresponding call to ReleaseSndMsgHeader().
// ReleaseSndMsgHeader() ensures pSMH->lpData(GlobalAlloc memory) to be freed.
BOOL GetSndMsgHeader(HWND hwnd, SENDMSG_HEADER* pSMH, BOOL bExcludeParam)
{

	if( !SndMsgOpt_IsBusyNow(pSMH->SMO.dwFlagSMH) )
	{
		SndMsgOpt_SetBusy(pSMH->SMO.dwFlagSMH);
	}
	else
	{
		return FALSE;
	}
	
	GetTargetHwndForSMH(hwnd, pSMH);
	GetMessageForSMH(hwnd, pSMH);

	if(!bExcludeParam)
	{
		GetParamsForSMH(hwnd, pSMH, NULL, NULL);
	}

	return TRUE;

} //GetSndMsgHeader();

VOID ReleaseSndMsgHeader(SENDMSG_HEADER* pSMH)
{
	BOOL IsFixedOpt=SndMsgOpt_IsProfileFixed(pSMH->SMO.dwFlagSMH);

	for(int i=0; i<2; i++)
	{
		if(pSMH->lpData[i])
		{
			GlobalFree((HGLOBAL)pSMH->lpData[i]);
			pSMH->lpData[i]=NULL;
		}

		if( !IsFixedOpt )
		{
			SndMsgParamOpt_SetDataType(pSMH->SMO.wFlagParam[i], NULL);
			SndMsgParamOpt_SetDataFor(pSMH->SMO.wFlagParam[i], NULL);
			pSMH->SMO.wBufSize[i]=0;
		}

	}

	SndMsgOpt_SetIdle(pSMH->SMO.dwFlagSMH);

} //GetSndMsgHeader();

//////////////////////////////////////////////////////////////////////////

VOID DoSetParamEditText(HWND hwndEdit, SENDMSG_HEADER* pSMH, INT iParamIndex, INT iAlign, BOOL isHex)
{

	TCHAR szBuf[MAX_STRBUF_LEN];

	if ( SndMsgParamOpt_GetDataFor(pSMH->SMO.wFlagParam[iParamIndex]) == SNDMSG_FLAG_IN &&
		SndMsgParamOpt_GetDataType(pSMH->SMO.wFlagParam[iParamIndex]) != SNDMSG_FLAG_STRING )
	{
		GetWindowText(hwndEdit, szBuf, MAX_SNDMSG_STR_LEN);
		if( IsWindowEnabled(hwndEdit) && CheckParamNumString(szBuf, TRUE) )
		{
			NumStrPrintf(szBuf, pSMH->lpData[iParamIndex], 
				(INT)pSMH->SMO.wBufSize[iParamIndex], 8, iAlign, isHex, g_option.IsPrefix);
			SetWindowText(hwndEdit, szBuf);
			SendMessage(hwndEdit, EM_SETSEL, MAX_SNDMSG_STR_LEN, MAX_SNDMSG_STR_LEN);
		}
	}

} //DoSetParamEditText();

VOID DoSwitchHexDec(HWND hwnd, BOOL isToHex, SENDMSG_HEADER* pSMH, BOOL isConvParam)
{

	INT iBuf;
	INT iAlign=GetDataAlign(pSMH->SMO.dwFlagSMH);
	TCHAR szBuf[SMALL_STRBUF_LEN];

	//reverse hex <--> dec
	if( isToHex )
	{
		SndMsgOpt_SetHex(pSMH->SMO.dwFlagSMH);
	}
	else
	{
		SndMsgOpt_SetDec(pSMH->SMO.dwFlagSMH);
	}

	if( pSMH->hwndTarget && pSMH->hwndTarget!=HWND_BROADCAST )
	{
		wsprintf(szBuf, isToHex? g_szFormat: TEXT("%u"), pSMH->hwndTarget);
		SetDlgItemText(hwnd, IDC_CBDL_HWND, szBuf);
	}

	if( isConvParam )
	{
		iBuf=SendDlgItemMessage(hwnd,IDC_COMBO_MSG, CB_GETCURSEL,0,0);
		if(iBuf==CB_ERR && pSMH->uMsg)
		{
			wsprintf(szBuf, isToHex? TEXT("0x%04X"): TEXT("%u"), pSMH->uMsg);
			SetDlgItemText(hwnd, IDC_COMBO_MSG, szBuf);
		}
		DoSetParamEditText(GetDlgItem(hwnd, IDC_EDIT_WPARAM), pSMH, 0, iAlign, isToHex);
		DoSetParamEditText(GetDlgItem(hwnd, IDC_EDIT_LPARAM), pSMH, 1, iAlign, isToHex);
	}

	iBuf=(INT)(BOOL)SndMsgOpt_IsProfileFixed(pSMH->SMO.dwFlagSMH); //save state.
	SndMsgOpt_SetProfileFixed(pSMH->SMO.dwFlagSMH); //don't clear params' types.
	ReleaseSndMsgHeader(pSMH);
	if(!iBuf)
	{
		SndMsgOpt_UnSetProfileFixed(pSMH->SMO.dwFlagSMH); //restore state.
	}

	SetWindowTextFromStrRes(hwnd, IDC_SBTN_HEXDEC, g_hInstance, isToHex?IDS_HEX:IDS_DEC);

} //DoSwitchHexDec();

//////////////////////////////////////////////////////////////////////////

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

BOOL CheckAndProcessStructParam(LPTSTR szParamString, BOOL isHex, WORD* pwFlagParam)
{
	TCHAR szBufCopy[MAX_SNDMSG_STR_LEN];
	WORD wType=SndMsgParamOpt_GetDataType(*pwFlagParam);

	//make a string copy for checking
	lstrcpyn(szBufCopy, szParamString, MAX_SNDMSG_STR_LEN);
	
	if( wType!=NULL && 
		wType!=SNDMSG_FLAG_STRUCT)
	{
		return FALSE;
	}

	StrTrim(szBufCopy, TEXT(" \t\r\n\0"));
	
	if( wType==NULL )
	{
		if(szBufCopy[0]!='{' || 
			szBufCopy[lstrlen(szBufCopy)-1]!='}') //auto check -> not struct
		{
			return FALSE;
		}
		// else auto check -> struct. set type flag.
		SndMsgParamOpt_SetDataType(*pwFlagParam, SNDMSG_FLAG_STRUCT);
	}
	//else now setting is struct.

	//make a uniqe format.
	StrTrim(szBufCopy, TEXT("{}\0"));
	wsprintf(szParamString, TEXT("{%s}\0"), szBufCopy);

 	return TRUE;

} //CheckAndProcessStructParam();


BOOL CALLBACK DlgProc_SndMsgConfirmBox(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		
	case  WM_INITDIALOG:
		{
			g_hwnd_TaskModal=hwnd;
			if( g_siWinVer<WINVER_VISTA )
			{
				DestroyWindow(GetDlgItem(hwnd, IDC_RECT));
			}

			//SetWindowLong(hwnd, GWL_HINSTANCE, (LONG)g_hInst.user32_dll);
			SendDlgItemMessage(hwnd, IDC_SICON, 
				STM_SETIMAGE, IMAGE_ICON, 
				(LPARAM)LoadIcon(NULL, MAKEINTRESOURCE(IDI_QUESTION)));

			MessageBeep(MB_ICONINFORMATION);
			
			SetWindowTextFromStrRes(hwnd, NULL, g_hInstance, IDS_CONFIRM); 
			
			SetDlgItemSubClass(hwnd, IDC_CHK_NOPOP, SCP_ChkBtn_NoFoucsRect);
			SetDlgItemFocus(hwnd, IDYES);
			
			MoveWndToWndCenter(hwnd, g_hwnd);
			
			return FALSE;
		}

	case WM_CTLCOLORSTATIC: 
		{
			if(g_siWinVer>=WINVER_VISTA)
			{
				switch(GetDlgCtrlID((HWND)lParam))
				{
				case IDC_STATIC_TEXT:
				case IDC_SICON:
					{
						HBRUSH hBrush = GetSysColorBrush(COLOR_WINDOW);
						//SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
						SetBkMode((HDC)wParam, TRANSPARENT);
						SetTextColor((HDC)wParam, (COLORREF)GetSysColor(COLOR_BTNTEXT));
						return (LONG)hBrush;
					}
				}
			}
			break;
		}

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDYES:
			case IDNO:
			case IDCANCEL:
				{
					g_smHdr.bNoPopConfirm=(BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_NOPOP);
					EndDialog(hwnd, LOWORD(wParam));
				}
			}
			
			return FALSE;
		}

	case WM_DESTROY:
		{
			g_hwnd_TaskModal=NULL;
			return FALSE;
		}
		
	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);;
	
} //DlgProc_SndMsgConfirmBox()

//////////////////////////////////////////////////////////////////////////

typedef struct tag_SndMsgRetInfo
{
	SENDMSG_HEADER*	pSMH;
	SENDMSG_OPT smo;
	LPTSTR lpszInfo;

	MAP_DA_CHAIN mdc[2];
	BOOL IsMappedString[2];
	LONG_PTR lpVmData[2], lpVmStr[2];
	TCHAR szBufStr[4][MAX_SNDMSG_STR_LEN];
	DWORD dwBufData[4][MAX_SNDMSG_PARAM_LEN]; //first 2 for input, last 2 for returned.
	INT iParamID[2];
	TCHAR szPARAM[2][MAX_SNDMSG_STR_LEN];
	DWORD dwData[2][MAX_SNDMSG_PARAM_LEN];
	DWORD dwParam[2];

	LONG lRetValue;
	INT iAlign;
	BOOL isHex;
	WORD wFor[2];
	WORD wType[2];

}SENDMSG_RETINFO;

//////////////////////////////////////////////////////////////////////////

LPTSTR GetParamMappedDataString(LPTSTR szGetBuf, SENDMSG_RETINFO* pSMRET, INT iParamIndex, BOOL isAfterCall)
{
	INT i;
	INT iOffsetStr, iOffsetData;
	TCHAR szBuf[MAX_STRBUF_LEN<<1];
	TCHAR szSmallBuf[SMALL_STRBUF_LEN];
	MAP_DA_NODE* pCurrent;

	pCurrent=pSMRET->mdc[iParamIndex].pTail;
	iOffsetStr=pSMRET->mdc[iParamIndex].iOffsetStr; 
	iOffsetData=pSMRET->mdc[iParamIndex].iOffsetData;
	if(isAfterCall)
	{
		iOffsetStr+=(INT)pSMRET->szBufStr[iParamIndex]-(INT)pSMRET->szBufStr[iParamIndex+2];
		iOffsetData+=(INT)pSMRET->dwBufData[iParamIndex]-(INT)pSMRET->dwBufData[iParamIndex+2];
		i=IDS_SEND_AFTER;

	}
	else
	{
		i=IDS_SEND_PRE;
	}

	LoadString(g_hInstance, i, szSmallBuf, SMALL_STRBUF_LEN);
	lstrcat(pSMRET->lpszInfo, szSmallBuf);
	
	__try
	{
		for(i=pSMRET->mdc[iParamIndex].iNodeCount; i>1; i--)
		{
			
			if( pCurrent->isString )
			{
				lstrcpyn(szBuf, (LPTSTR)((INT)pCurrent->lpAddress-iOffsetStr), (INT)pCurrent->uSize);
			}
			else
			{
				NumStrPrintf(szBuf, (LPVOID)((INT)pCurrent->lpAddress-iOffsetData), 
					(INT)pCurrent->uSize, 0, pSMRET->iAlign, pSMRET->isHex, TRUE);
			}
			wsprintf(szSmallBuf, TEXT("[0x%08X] = "), pCurrent->lpAddress);
			ChangeLine(szGetBuf);
			lstrcat(szGetBuf, szSmallBuf);
			lstrcat(szGetBuf, szBuf);
			pCurrent=pCurrent->pPre;
		}
	}
	__except(1)
	{
		szGetBuf[0]=0;
	}
	return szGetBuf;

} //GetParamMappedDataString();

LPTSTR GetParamTypeString(LPTSTR szGetTypeBuf, WORD wFor, WORD wType)
{
	TCHAR szFor[TINY_STRBUF_LEN];
	TCHAR szType[TINY_STRBUF_LEN];
	TCHAR szSmallBuf[SMALL_STRBUF_LEN];

	INT iStrResID;
	BOOL isInUse=TRUE;

	switch( wFor )
	{

	case SNDMSG_FLAG_UNUSE:
		{
			isInUse=FALSE;
			iStrResID=IDS_FOR_3;
			break;
		}
	case SNDMSG_FLAG_OUT:
		{
			iStrResID=IDS_FOR_2;
			break;
		}
	
	default://case SNDMSG_FLAG_IN:
		{
			iStrResID=IDS_FOR_1;
			break;
		}
	}

	LoadString(g_hInstance, iStrResID, szFor, TINY_STRBUF_LEN);
	wsprintf(szSmallBuf, TEXT(" (%s)"), szFor);

	if( isInUse )
	{
		switch( wType )
		{
		case SNDMSG_FLAG_STRUCT:
			{
				iStrResID=IDS_TYPE_3;
				break;
			}
		case SNDMSG_FLAG_STRING:
			{
				iStrResID=IDS_TYPE_2;
				break;
			}
		case SNDMSG_FLAG_DWORD:
			{
				iStrResID=IDS_TYPE_1;
				break;
			}
		default:
			iStrResID=IDS_AUTO;

		}

		LoadString(g_hInstance, iStrResID, szType, TINY_STRBUF_LEN);
		wsprintf(szGetTypeBuf, TEXT(" [%s]"), szType);
	}
	else
	{
		szGetTypeBuf[0]=0;
	}

	lstrcat(szGetTypeBuf, szSmallBuf);
	return szGetTypeBuf;

} //GetParamTypeString();


VOID SndMsgDetailResultPrintf(HWND hwnd, SENDMSG_RETINFO* pSMRET)
{
	INT i;
	BOOL isBroadCasted=FALSE;
	HWND hwndEdit[2];
	TCHAR szBuf[MAX_STRBUF_LEN];
	TCHAR szSmallBuf[SMALL_STRBUF_LEN];

	pSMRET->lpszInfo[0]=0;

	lstrcat(pSMRET->lpszInfo, TEXT("HWND : "));
	if ( pSMRET->pSMH->hwndTarget==HWND_BROADCAST )
	{
		GetDlgItemText(hwnd, IDC_CBDL_HWND, szSmallBuf, SMALL_STRBUF_LEN);
		isBroadCasted=TRUE;
	}
	else
	{
		wsprintf(szSmallBuf, HEX_FMT_PREFIX, pSMRET->pSMH->hwndTarget);
	}
 	lstrcat(pSMRET->lpszInfo, szSmallBuf);
 	ChangeLine(pSMRET->lpszInfo);

	lstrcat(pSMRET->lpszInfo, TEXT("MSG : "));
	
	if( 0 < GetSndMsgMessageSymbol(hwnd, pSMRET->pSMH, szBuf, MAX_CLASS_NAME, FALSE) )
	{
		lstrcat(pSMRET->lpszInfo, szBuf);
		lstrcat(pSMRET->lpszInfo, TEXT(" "));
	}
	wsprintf(szSmallBuf, TEXT("[0x%04X(%u)]"), pSMRET->pSMH->uMsg, pSMRET->pSMH->uMsg);
	lstrcat(pSMRET->lpszInfo, szSmallBuf);
	ChangeLine(pSMRET->lpszInfo);

	for(i=0; i<2; i++)
	{
		ZeroMemory(pSMRET->szPARAM[i], MAX_SNDMSG_STR_LEN*sizeof(TCHAR));
		hwndEdit[i]=GetDlgItem(hwnd, pSMRET->iParamID[i]);
		GetWindowText(hwndEdit[i], pSMRET->szPARAM[i], MAX_SNDMSG_STR_LEN);

		lstrcat(pSMRET->lpszInfo, (i==0 ? TEXT("WPARAM"):TEXT("LPARAM")) );
		lstrcat(pSMRET->lpszInfo, TEXT(" : "));
		lstrcat(pSMRET->lpszInfo, pSMRET->szPARAM[i]);
		ChangeLine(pSMRET->lpszInfo);
	}

	if(isBroadCasted)
	{
		return;
	}

	ChangeLine(pSMRET->lpszInfo);

	for(i=0; i<2; i++)
	{
		lstrcatchar(pSMRET->lpszInfo, '-', 40);
		ChangeLine(pSMRET->lpszInfo);
		lstrcat(pSMRET->lpszInfo, (i==0 ? TEXT("WPARAM"):TEXT("LPARAM")) );
		
		if(pSMRET->wFor[i]!=SNDMSG_FLAG_UNUSE)
		{
			lstrcat(pSMRET->lpszInfo, TEXT(" = "));
			wsprintf(szSmallBuf, HEX_FMT_PREFIX, pSMRET->dwParam[i], pSMRET->dwParam[i]);
			lstrcat(pSMRET->lpszInfo, szSmallBuf);
		}
		
		lstrcat(pSMRET->lpszInfo, GetParamTypeString(szSmallBuf, pSMRET->wFor[i], pSMRET->wType[i]));
		ChangeLine(pSMRET->lpszInfo);
		
		if(pSMRET->lpVmData[i] || pSMRET->lpVmStr[i])
		{
			ChangeLine(pSMRET->lpszInfo);
			GetParamMappedDataString(pSMRET->lpszInfo, pSMRET, i, FALSE);
			ChangeLine(pSMRET->lpszInfo);
			GetParamMappedDataString(pSMRET->lpszInfo, pSMRET, i, TRUE);
			ChangeLine(pSMRET->lpszInfo);
		}
		else 
		{
			wsprintf(szSmallBuf, TEXT("[0x%08X] = "), pSMRET->dwParam[i]);
			
			if( pSMRET->wType[i]==SNDMSG_FLAG_STRING )
			{
				pSMRET->dwData[i][MAX_SNDMSG_PARAM_LEN-1]=0;
				lstrcat(pSMRET->lpszInfo, szSmallBuf);
				lstrcat(pSMRET->lpszInfo, (LPTSTR)pSMRET->dwParam[i]);
			}
			else if( pSMRET->wFor[i]==SNDMSG_FLAG_OUT )
			{
				NumStrPrintf(szBuf, (LPVOID)pSMRET->dwParam[i], 
					min(pSMRET->pSMH->SMO.wBufSize[i], MAX_SNDMSG_PARAM_LEN*sizeof(DWORD)), 
					0, pSMRET->iAlign, pSMRET->isHex, TRUE);
				lstrcat(pSMRET->lpszInfo, szSmallBuf);
				lstrcat(pSMRET->lpszInfo, szBuf);
			}
			ChangeLine(pSMRET->lpszInfo);
		}
	}

} //SndMsgDetailResultPrintf();


//////////////////////////////////////////////////////////////////////////


VOID OnApplySndMsgOptions(HWND hwnd, SENDMSG_HEADER* pNewSMH, BOOL isAutoConvParam)
{
	
	WORD wFor[2], wType[2];
	DWORD dwAlginOld, dwAlginNew;
	BOOL isHexOld, isHexNew;
	HWND hwndEdit, hwndEditCopy;
	INT iParamID[2]={IDC_EDIT_WPARAM, IDC_EDIT_LPARAM};
	TCHAR szBuf[MAX_CLASS_NAME];
	
	if( GetSndMsgHeader(g_TabDlgHdr.CDI[TAB_MANI].hwnd, &g_smHdr, FALSE) )
	{
		int i; 
		for(i=0; i<2; i++)
		{
			wFor[i]=SndMsgParamOpt_GetDataFor(pNewSMH->SMO.wFlagParam[i]);
			wType[i]=SndMsgParamOpt_GetDataType(pNewSMH->SMO.wFlagParam[i]);
			
			hwndEdit=GetDlgItem(hwnd, iParamID[i]);
			hwndEditCopy=GetDlgItem(hwnd, iParamID[i]+80);
			if(wFor[i]!=SNDMSG_FLAG_IN)
			{
				if( IsWindowEnabled(hwndEdit) )
				{
					GetWindowText(hwndEdit, szBuf, MAX_SNDMSG_STR_LEN);
					SetWindowText(hwndEditCopy, szBuf);
				}
				GetParamTypeString(szBuf, wFor[i], wType[i]);
				SetWindowText(hwndEdit, szBuf);
				SetWindowEnable(hwndEdit, NULL, FALSE);
			}
			else if( !IsWindowEnabled(hwndEdit) )
			{
				GetWindowText(hwndEditCopy, szBuf, MAX_SNDMSG_STR_LEN);
				SetWindowText(hwndEdit, szBuf); 
				SetWindowText(hwndEditCopy, TEXT(""));
				SetWindowEnable(hwndEdit, NULL, TRUE);
			}
		}
		
		dwAlginOld=SndMsgOpt_GetDataAlign(g_smHdr.SMO.dwFlagSMH);
		dwAlginNew=SndMsgOpt_GetDataAlign(pNewSMH->SMO.dwFlagSMH);
		isHexOld=SndMsgOpt_IsHex(g_smHdr.SMO.dwFlagSMH);
		isHexNew=SndMsgOpt_IsHex(pNewSMH->SMO.dwFlagSMH);
		
		if( dwAlginOld!=dwAlginNew || isHexOld!=isHexNew )
		{				
			SndMsgOpt_SetDataAlign(g_smHdr.SMO.dwFlagSMH, dwAlginNew);
			DoSwitchHexDec(g_TabDlgHdr.CDI[TAB_MANI].hwnd, isHexNew, &g_smHdr, isAutoConvParam);
		}
		else
		{
			ReleaseSndMsgHeader(&g_smHdr);
		}
		
		g_smHdr.SMO.dwFlagSMH=pNewSMH->SMO.dwFlagSMH;
		
		for(i=0; i<2; i++)
		{
			g_smHdr.SMO.wBufSize[i]=pNewSMH->SMO.wBufSize[i];
			SndMsgParamOpt_SetDataFor(g_smHdr.SMO.wFlagParam[i], wFor[i]);
			SndMsgParamOpt_SetDataType(g_smHdr.SMO.wFlagParam[i], wType[i]);
		}
		
	}
	
} //OnApplySndMsgOptions();

/****
* The szMsgBuf must has at least 128 tchar space, or the function will always fail.
* and iMaxBufLen >= 128
****/
INT GetSndMsgMessageSymbol(HWND hwnd, SENDMSG_HEADER* pSMH, 
							LPTSTR szMsgBuf, INT iMaxBufLen, BOOL IsFmtUnKnownMsg)
{

#define FORMAT_STR_UNKNOWN_MSG	TEXT("%s_MSG%04X")
#define FORMAT_STR_KNOWN_MSG	TEXT("%s")

	int iMsgIndex;
	TCHAR szBuf[MAX_CLASS_NAME];

	szMsgBuf[0]=0;
	iMsgIndex=GetMessageForSMH(hwnd, pSMH);

	if( iMsgIndex <= CB_ERR ) //unknownMsg or error...
	{
		GetTargetHwndForSMH(hwnd, pSMH);
		if(	pSMH->uMsg<=0 || 
			!IsWindow(pSMH->hwndTarget) )
		{
			return 0;
		}
		else if(IsFmtUnKnownMsg)
		{
			if( iMaxBufLen < 128 )
			{
				return -1;
			}
			// leave (64 tchar) enough buffer space for next processing.
			iMaxBufLen=iMaxBufLen-64;
			szBuf[iMaxBufLen]='\0';
			GetClassName(pSMH->hwndTarget, szBuf, iMaxBufLen);
			wsprintf(szMsgBuf, FORMAT_STR_UNKNOWN_MSG, szBuf, pSMH->uMsg);
		}
	}
	else //known Msg List....
	{
		SendDlgItemMessage(hwnd, IDC_COMBO_MSG, CB_GETLBTEXT, iMsgIndex, (LPARAM)szBuf);
		wsprintf(szMsgBuf, FORMAT_STR_KNOWN_MSG, szBuf);
	}
	return pSMH->uMsg;

} //GetSndMsgMessageSymbol();


BOOL GetSndMsgOptProfile(LPTSTR szIniFileName, LPTSTR szMsgSymbol, 
						 SENDMSG_HEADER* pSMH, LPTSTR szMemoBuf, INT iMaxBufLen)
{
	TCHAR szBuf[MAX_CLASS_NAME];

	if(szMsgSymbol==NULL)
	{
		szMsgSymbol=szBuf;
		if(	CB_ERR == GetSndMsgMessageSymbol(
			g_TabDlgHdr.CDI[TAB_MANI].hwnd, 
			pSMH, szMsgSymbol, MAX_CLASS_NAME, TRUE) )
		{
			return FALSE;
		}
	}

	if(pSMH!=NULL)
	{
		if( GetPrivateProfileStruct(STR_INI_SEC_MSGOPT, 
			szMsgSymbol, &pSMH->SMO, sizeof(SENDMSG_OPT), szIniFileName) )
		{
			SndMsgOpt_SetProfileFixed(pSMH->SMO.dwFlagSMH);
		}

	}
	if(szMemoBuf!=NULL)
	{
		GetPrivateProfileString(STR_INI_SEC_MSGMEMO, 
			szMsgSymbol, TEXT(""), szMemoBuf, iMaxBufLen, szIniFileName);
		DeCodeChangeLineChars(szMemoBuf);
	}
	return TRUE;

} //GetSndMsgOptProfile();

BOOL WriteSndMsgOptProfile(LPTSTR szIniFileName, LPTSTR szMsgSymbol, 
						   SENDMSG_HEADER* pSMH, LPTSTR szMsgMemo)
{

	if( WritePrivateProfileStruct(STR_INI_SEC_MSGOPT, 
		szMsgSymbol, &pSMH->SMO, sizeof(SENDMSG_OPT), szIniFileName) )
	{
		if(szMsgMemo!=NULL)
		{
			if(lstrlen(szMsgMemo)>0)
			{
				TCHAR szMsgMemoCovBuf[MAX_MSGMEMO_BUF_LEN];
				lstrcpyn(szMsgMemoCovBuf, szMsgMemo, MAX_MSGMEMO_STR_LEN);
				CodeChangeLineChars(szMsgMemoCovBuf);
				WritePrivateProfileString(STR_INI_SEC_MSGMEMO, 
					szMsgSymbol, szMsgMemoCovBuf, szIniFileName);
			}
		}
		return TRUE;
	}
	
	return FALSE;

} //WriteSndMsgOptProfile();

VOID CALLBACK TimerProc_LoadSndMsgProfile(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	TCHAR szIniFilaName[MAX_PATH];
	SENDMSG_HEADER smh={0};
	
	KillTimer(hwnd, idEvent);
	if( GetSndMsgOptProfile(GetCfgFileName(szIniFilaName), NULL, &smh, NULL, NULL) )
	{
		OnApplySndMsgOptions(g_TabDlgHdr.CDI[TAB_MANI].hwnd, &smh, FALSE);
	}
	
} //TimerProc_SetCalcWndPos();

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_SndMsgDetailResult(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
	
	case  WM_INITDIALOG:
		{
			g_hwnd_TaskModal=hwnd;
			
			SENDMSG_RETINFO* pSMRET=(SENDMSG_RETINFO*)lParam;

			SetDlgItemText(hwnd, IDC_EDIT_RESULT, pSMRET->lpszInfo);

			HFONT hFont=(HFONT)SendMessage(g_hwnd, WM_GETFONT, 0, 0);

			SendDlgItemMessage(hwnd, IDC_STATIC_RET, WM_SETFONT, 
				(WPARAM)hFont, TRUE);

			SendDlgItemMessage(hwnd, IDC_EDIT_RETVAL, WM_SETFONT, 
				(WPARAM)hFont, TRUE);
			
			DlgItemPrintf(hwnd, IDC_EDIT_RETVAL, 
				TEXT("0x%08X (%d)"), 
				pSMRET->lRetValue, 
				pSMRET->lRetValue); 

			SetDlgItemSubClass(hwnd, IDC_EDIT_RESULT, SCP_Edit_ReadOnly);
			
			OnInit_OnDrawWindowSizeGripBox(g_hInst.uxtheme_dll);

			SendMessage(hwnd, WM_SETICON, ICON_BIG, 
				(LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_MAIN)));

			MoveWndToWndCenter(hwnd, g_hwnd);

			SendDlgItemMessage(hwnd, IDC_EDIT_RESULT, EM_SETSEL, MAX_STRBUF_LEN, MAX_STRBUF_LEN);
			
			return FALSE;
		}

	case WM_PAINT:
		{
			PAINTSTRUCT pst;
			HDC hdc;
			hdc=BeginPaint(hwnd, &pst);
			DrawSizeGripBox_OnPaint(hwnd, hdc);
			EndPaint(hwnd, &pst);
			return FALSE;
		}

	case WM_SIZE:
		{

			RECT rc;
			GetClientRect(hwnd, &rc);
			ClientToScreen(hwnd, (POINT*)&rc);
			ClientToScreen(hwnd, (POINT*)&rc+1);

			HDWP hdwp = BeginDeferWindowPos(1);

			hdwp = DeferWindowPos(hdwp, 
				GetDlgItem(hwnd, IDC_EDIT_RESULT), 
				NULL, 
				NULL, NULL, 
				rc.right-rc.left-(_HS_GRIP_SIZE+16), 
				rc.bottom-rc.top-(_HS_GRIP_SIZE+34), 
				SWP_NOZORDER|SWP_NOMOVE); 
			
			EndDeferWindowPos(hdwp);
			
		}//WM_SIZE end.. but fall though...
	case WM_SIZING:
		{
			DrawSizeGripBox_OnSizing(hwnd);
			return FALSE;
		}
		//size changed finished...

	case WM_GETMINMAXINFO:
		{
			((LPMINMAXINFO)lParam)->ptMinTrackSize.x = 256;
			((LPMINMAXINFO)lParam)->ptMinTrackSize.y = 128;
			return FALSE;
		}

	case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDCANCEL:
				{
					EndDialog(hwnd, LOWORD(wParam));
				}
			}
			return FALSE;
		}

	case WM_DESTROY:
		{
			g_hwnd_TaskModal=NULL;

			return FALSE;
		}
		
	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);;
	
} //DlgProc_SndMsgConfirmBox()

//////////////////////////////////////////////////////////////////////////

VOID OnSendMessageToTarget(HWND hwnd, SENDMSG_HEADER* pSMH, BOOL isPostMsg)
{	
	INT i;
	HWND hwndTarget=NULL;
	HANDLE hProcess=NULL;
	DWORD dwBuf=0;
	BOOL IsError=FALSE;
	DWORD dwLastError;
	SENDMSG_RETINFO smRet;


	if( !GetSndMsgHeader(hwnd, pSMH, TRUE) )
	{
		return;
	}
	
	if( IsWindow(pSMH->hwndTarget) && 
		( pSMH->bNoPopConfirm || 
		IDYES==DialogBox(g_hInstance, 
		MAKEINTRESOURCE(IDD_ALERT_SNDMSG), g_hwnd, DlgProc_SndMsgConfirmBox)) )
	{	
		hwndTarget=pSMH->hwndTarget;
	}
	else if( pSMH->hwndTarget == HWND_BROADCAST )
	{
		if( SndMsgOpt_IsNotAllowBC(pSMH->SMO.dwFlagSMH) )
		{
			MessageBoxFromResStr(g_hInstance, g_hwnd, 
				IDS_MSG_FORBID_BC, IDS_NOTE, MB_ICONEXCLAMATION);
		}
		else if( IDYES==
			MessageBoxFromResStr(g_hInstance, g_hwnd, 
			IDS_MSG_BROADCAST, IDS_CONFIRM, MB_YESNO|MB_DEFBUTTON2|MB_ICONEXCLAMATION) )
		{
			isPostMsg=TRUE;
			hwndTarget=HWND_BROADCAST;
		}
	}
	
	if( hwndTarget )
	{
		ZeroMemory(&smRet, sizeof(SENDMSG_RETINFO));
		smRet.iAlign=GetDataAlign(pSMH->SMO.dwFlagSMH);
		smRet.isHex=SndMsgOpt_IsHex(pSMH->SMO.dwFlagSMH);
		smRet.iParamID[0]=IDC_EDIT_WPARAM, smRet.iParamID[1]=IDC_EDIT_LPARAM;

		for(i=0; i<2; i++)
		{
			smRet.wFor[i]=SndMsgParamOpt_GetDataFor(pSMH->SMO.wFlagParam[i]);
			smRet.wType[i]=SndMsgParamOpt_GetDataType(pSMH->SMO.wFlagParam[i]);
			if( smRet.wFor[i]!=SNDMSG_FLAG_IN )
			{
				continue;
			}

			GetDlgItemText(hwnd, smRet.iParamID[i], smRet.szPARAM[i], MAX_SNDMSG_STR_LEN);
			
			if ( hwndTarget!=HWND_BROADCAST && smRet.wType[i]!=SNDMSG_FLAG_STRING &&
				CheckAndProcessStructParam(smRet.szPARAM[i], smRet.isHex, &pSMH->SMO.wFlagParam[i]) )
			{

				if( !hProcess)
				{
					GetWindowThreadProcessId(hwndTarget, &dwBuf);
					hProcess = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|PROCESS_VM_WRITE, FALSE, dwBuf);
				}
				
				if( hProcess)
				{
					smRet.lpVmData[i] = (LONG_PTR)
						VirtualAllocEx(hProcess, NULL, 
						MAX_SNDMSG_PARAM_LEN*sizeof(DWORD), /*MEM_RESERVE|*/MEM_COMMIT, PAGE_READWRITE);

					smRet.mdc[i].iOffsetData=(INT)
						((INT)(BYTE*)(smRet.lpVmData[i])-(INT)(BYTE*)smRet.dwBufData[i]);
					
					smRet.lpVmStr[i] = (LONG_PTR)
						VirtualAllocEx(hProcess, NULL, 
						MAX_SNDMSG_STR_LEN*sizeof(TCHAR), /*MEM_RESERVE|*/MEM_COMMIT, PAGE_READWRITE);

					smRet.mdc[i].iOffsetStr=(INT)
						((INT)(LPTSTR)(smRet.lpVmStr[i])-(INT)(LPTSTR)smRet.szBufStr[i]);
					
					if(smRet.lpVmData[i] && smRet.lpVmStr[i])
					{
						InitDataAdrChain(&smRet.mdc[i]);
						
						smRet.IsMappedString[i]=(BOOL)
							MapStringToAdr(&smRet.mdc[i], smRet.szPARAM[i], 
							smRet.szBufStr[i], 
							smRet.mdc[i].iOffsetStr, 0, MAX_SNDMSG_STR_LEN);
						
						dwBuf=MapStructToAdr(&smRet.mdc[i], smRet.szPARAM[i], 
							'{', '}', smRet.isHex, smRet.iAlign, 
							(BYTE*)smRet.dwBufData[i], 
							smRet.mdc[i].iOffsetData, 0, MAX_SNDMSG_PARAM_LEN*sizeof(DWORD));
						
						//check whether the structure has only a string buffer.
						smRet.IsMappedString[i]=(dwBuf<=sizeof(DWORD))&&smRet.IsMappedString[i];
						
						WriteProcessMemory(hProcess, (PVOID)smRet.lpVmStr[i], 
							smRet.szBufStr[i], MAX_SNDMSG_STR_LEN*sizeof(TCHAR), NULL);
						
						WriteProcessMemory(hProcess, (PVOID)smRet.lpVmData[i], 
							smRet.dwBufData[i], MAX_SNDMSG_PARAM_LEN*sizeof(DWORD), NULL);
					}
				}
				
			}
			
		}//for

		GetParamsForSMH(hwnd, pSMH, smRet.szPARAM[0], smRet.szPARAM[1]);
		
		CopyMemory(&smRet.smo, &pSMH->SMO, sizeof(SENDMSG_OPT));
		
		for(i=0; i<2; i++)
		{
			//update dwType here...
			smRet.wType[i]=SndMsgParamOpt_GetDataType(pSMH->SMO.wFlagParam[i]);

			if(smRet.wFor[i]==SNDMSG_FLAG_IN)
			{
				CopyMemory(smRet.dwData[i], pSMH->lpData[i], pSMH->SMO.wBufSize[i]);
			}

			if(smRet.IsMappedString[i])
			{
				smRet.dwParam[i]=(DWORD)smRet.lpVmStr[i];
			}
			else
			{
				smRet.dwParam[i] = 
					( smRet.wFor[i]!=SNDMSG_FLAG_IN || 
					smRet.wType[i]==SNDMSG_FLAG_STRING ) ? 
					(DWORD)smRet.dwData[i]:(DWORD)smRet.dwData[i][0];
			}
		}
		
		__try
		{
			if(!isPostMsg)
			{
				SendMessageTimeout(hwndTarget, pSMH->uMsg, 
					smRet.dwParam[0], smRet.dwParam[1], 
					SMTO_NORMAL, 1000, (DWORD*)&smRet.lRetValue);
			}
			else
			{
				if(hwndTarget==HWND_BROADCAST)
				{
					BroadcastSystemMessage(BSF_FORCEIFHUNG|BSF_POSTMESSAGE|BSF_IGNORECURRENTTASK, NULL, 
						pSMH->uMsg, smRet.dwParam[0], smRet.dwParam[1]);
				}
				else
				{
					PostMessage(hwndTarget, pSMH->uMsg, smRet.dwParam[0], smRet.dwParam[1]);
				}
			}
		}
		__except(1)
		{
			IsError=TRUE;
			dwLastError=GetLastError();
		}
		
		if( hProcess )
		{
			if(isPostMsg)
			{
				//wait target window for processing posted msg...
				MessageBoxFromResStr(g_hInstance, g_hwnd, 
					IDS_PSTMSG_WAIT, IDS_NOTE, MB_TOPMOST);
			}
			for(i=0; i<2; i++)
			{
				if( smRet.lpVmData[i] )
				{
					ReadProcessMemory(hProcess, (PVOID)smRet.lpVmData[i], 
						smRet.dwBufData[i+2], MAX_SNDMSG_PARAM_LEN*sizeof(DWORD), NULL);
					VirtualFreeEx(hProcess, (PVOID)smRet.lpVmData[i], 0, MEM_RELEASE);
				}
				if( smRet.lpVmStr[i] )
				{
					ReadProcessMemory(hProcess, (PVOID)smRet.lpVmStr[i], 
						smRet.szBufStr[i+2], MAX_SNDMSG_STR_LEN*sizeof(TCHAR), NULL);
					VirtualFreeEx(hProcess, (PVOID)smRet.lpVmStr[i], 0, MEM_RELEASE);
				}
			}
			
			CloseHandle(hProcess);
		}
		
	} //end if (hwndTarget is a value of TRUE).

	ReleaseSndMsgHeader(pSMH); //release Mem and set idle...
		
	if(hwndTarget)
	{
		if(IsError)
		{
			LastErrorIDMessageBox(g_hwnd, dwLastError, g_strBuf, MAX_STRBUF_LEN);
		}
		else if(!isPostMsg && SndMsgOpt_IsShowRetDetail(pSMH->SMO.dwFlagSMH) )
		{
			smRet.pSMH=pSMH;
			smRet.lpszInfo=(LPTSTR)
				GlobalAlloc(GPTR, sizeof(TCHAR)*(MAX_STRBUF_LEN<<2));

			SndMsgDetailResultPrintf(hwnd, &smRet);
			
			DialogBoxParam(g_hInstance, 
				MAKEINTRESOURCE(IDD_MSG_RETURN), 
				g_hwnd, DlgProc_SndMsgDetailResult, (LPARAM)&smRet);
			
			GlobalFree((HGLOBAL)smRet.lpszInfo);

		}
	}

	if(hProcess)
	{
		UninitDataAdrChain(&smRet.mdc[0]);
		UninitDataAdrChain(&smRet.mdc[1]);
	}
	
	if( !SndMsgOpt_IsProfileFixed(pSMH->SMO.dwFlagSMH) )
	{
		SndMsgOpt_GoToLoadProfile(hwnd, USER_TIMER_MINIMUM);
	}

} //OnSendMessageToTarget();// A BIG...

//////////////////////////////////////////////////////////////////////////

#define INI_MEMO				TEXT("Memo")
#define INI_IS_DEC				TEXT("Dec")
#define INI_TYPE_ALGIN			TEXT("Align")
#define INI_IS_NOPOPRET			TEXT("NoRet")
#define INI_TYPE_WPARAM			TEXT("wParamType")
#define INI_FOR_WPARAM			TEXT("wParamFor")
#define INI_SIZE_WPARAM			TEXT("wParamSize")
#define INI_TYPE_LPARAM			TEXT("lParamType")
#define INI_FOR_LPARAM			TEXT("lParamFor")
#define INI_SIZE_LPARAM			TEXT("lParamSize")
#define INI_FORBID_BROADCAST	TEXT("ForbidBC")


VOID ImportSndMsgProfile(LPTSTR szBackupFile)
{
#define IMPORT_BIG_SIZE	1024
#define MAX_INI_IMPORT_BUFFER_LEN	51200
#define MAX_INI_IMPORT_BUFFER_SIZE	( MAX_INI_IMPORT_BUFFER_LEN * sizeof(TCHAR) )

	HWND hwndPB=NULL;
	SENDMSG_OPT smo;
	INT i, j, n, iBuf;

	LPTSTR lpszKeys=(LPTSTR)
		GlobalAlloc(GPTR, MAX_INI_IMPORT_BUFFER_SIZE);

	TCHAR szCfgFileName[MAX_PATH];
	TCHAR szKey[MINI_STRBUF_LEN];
	TCHAR szValue[MAX_STRBUF_LEN];
	
	if(lpszKeys)
	{
		if( n=GetPrivateProfileSectionNames(
			lpszKeys, MAX_INI_IMPORT_BUFFER_LEN, szBackupFile) )
		{

			if( n > IMPORT_BIG_SIZE )
			{
				LoadingBar_Open();
				hwndPB=LoadingBar_GetProgressBar();
				SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0,n));
				SendMessage(hwndPB, PBM_SETPOS, 0, 0);
			}

			i=0;
			
			while( i<n && (BOOL)(j=lstrlen(lpszKeys+i)) )
			{

				lstrcpyn(szKey, lpszKeys+i, MINI_STRBUF_LEN);
				StrTrim(szKey, TEXT("\t \0"));

				if( INIT_ERR!=
					(iBuf=GetPrivateProfileInt(szKey, 
					INI_FOR_WPARAM, INIT_ERR, szBackupFile)) ) //if wParamType does not exists, invalid...
				{		
					ZeroMemory(&smo, sizeof(SENDMSG_OPT));
					
					SndMsgParamOpt_SetDataFor(smo.wFlagParam[0], (WORD)((WORD)iBuf<<12));
					
					if( INIT_ERR!=
						(iBuf=GetPrivateProfileInt(szKey, 
						INI_FOR_LPARAM, INIT_ERR, szBackupFile)) ) //if lParamType does not exists, invalid...
					{
						SndMsgParamOpt_SetDataFor(smo.wFlagParam[1], (WORD)((WORD)iBuf<<12));
						
						SndMsgParamOpt_SetDataType(smo.wFlagParam[0], 
							(WORD)GetPrivateProfileInt(szKey, INI_TYPE_WPARAM, 0, szBackupFile));
						
						SndMsgParamOpt_SetDataType(smo.wFlagParam[1], 
							(WORD)GetPrivateProfileInt(szKey, INI_TYPE_LPARAM, 0, szBackupFile));
						
						smo.wBufSize[0]=(WORD)GetPrivateProfileInt(szKey, INI_SIZE_WPARAM, 0, szBackupFile);
						smo.wBufSize[1]=(WORD)GetPrivateProfileInt(szKey, INI_SIZE_LPARAM, 0, szBackupFile);
						
						if( GetPrivateProfileInt(szKey, INI_IS_NOPOPRET, 0, szBackupFile) )
						{
							SndMsgOpt_UnSetShowRetDetail(smo.dwFlagSMH);
						}
						
						if( GetPrivateProfileInt(szKey, INI_FORBID_BROADCAST, 0, szBackupFile) )
						{
							SndMsgOpt_UnSetAllowBC(smo.dwFlagSMH);
						}
						
						if( GetPrivateProfileInt(szKey, INI_IS_DEC, 0, szBackupFile) )
						{
							SndMsgOpt_SetDec(smo.dwFlagSMH);
						}

						iBuf=GetPrivateProfileInt(szKey, INI_TYPE_ALGIN, 0, szBackupFile);
						
						SndMsgOpt_SetDataAlign(smo.dwFlagSMH, MAKELONG(0,iBuf));
						
						WritePrivateProfileStruct(STR_INI_SEC_MSGOPT, szKey, 
							&smo, sizeof(SENDMSG_OPT), GetCfgFileName(szCfgFileName)); 
						
						if( GetPrivateProfileString(szKey, 
							INI_MEMO, NULL, szValue, MAX_STRBUF_LEN, szBackupFile) )
						{
							WritePrivateProfileString(STR_INI_SEC_MSGMEMO, szKey, szValue, szCfgFileName);
						}

					}
				}


				i+=j+1;
				SendMessage(hwndPB, PBM_SETPOS, i, 0);
			}
		}

		GlobalFree((HGLOBAL)lpszKeys);

		if(hwndPB)
		{
			LoadingBar_Close();
		}
	}

}

VOID ExportSndMsgProfile(LPTSTR szBackupFile)
{
#define EXPORT_BIG_SIZE	4096
#define MAX_INI_EXPORT_BUFFER_LEN	102400
#define MAX_INI_EXPORT_BUFFER_SIZE	( MAX_INI_EXPORT_BUFFER_LEN * sizeof(TCHAR) )
#define INI_COMMENT		"; Message-Settings backup file for Window Spy...\r\n"

	HWND hwndPB=NULL;
	SENDMSG_OPT smo;
	INT i, j, n;
	
	CHAR szHeadLine[]=INI_COMMENT;

	HANDLE hFile=CreateFile(szBackupFile, GENERIC_WRITE, 
		NULL, NULL, CREATE_ALWAYS, 0, NULL);
	
	if( INVALID_HANDLE_VALUE != hFile )
	{
		DWORD dwBytesWritten;
		WriteFile(hFile, szHeadLine, (lstrlenA(szHeadLine)+1)*sizeof(CHAR), &dwBytesWritten, NULL);
		CloseHandle(hFile);
	}
	else
	{
		return;
	}

	LPTSTR lpszKeys=(LPTSTR)
		GlobalAlloc(GPTR, MAX_INI_EXPORT_BUFFER_SIZE);

	TCHAR szCfgFileName[MAX_PATH];
	TCHAR szKey[MINI_STRBUF_LEN];
	TCHAR szValue[MAX_STRBUF_LEN];
	
	if(lpszKeys)
	{
		if( n=GetPrivateProfileSection(STR_INI_SEC_MSGOPT, 
			lpszKeys, MAX_INI_EXPORT_BUFFER_LEN, 
			GetCfgFileName(szCfgFileName)) )
		{
			i=0;
			
			if( n > EXPORT_BIG_SIZE )
			{
				LoadingBar_Open();
				hwndPB=LoadingBar_GetProgressBar();
				SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0,n));
				SendMessage(hwndPB, PBM_SETPOS, 0, 0);
			}

			while( i<n && (BOOL)(j=lstrlen(lpszKeys+i)) )
			{
				GetStringPortion(lpszKeys+i, szKey, '=', FALSE);
				StrTrim(szKey, TEXT("\t \0"));
				
				if( GetPrivateProfileStruct(STR_INI_SEC_MSGOPT, 
					szKey, &smo, sizeof(SENDMSG_OPT), szCfgFileName) )
				{
					lstrcpy(szValue, TEXT("\r\n;"));
					lstrcatchar(szValue, '-', 30);
					ChangeLine(szValue);
					ChangeLine(szValue);
					WriteStrToFile(szBackupFile, szValue, TRUE);

					WritePrivateProfileString(szKey, INI_IS_DEC, 
						MAKEBOOLSTR(SndMsgOpt_IsDec(smo.dwFlagSMH)), szBackupFile);

					wsprintf(szValue, TEXT("%d"), GetDataAlign(smo.dwFlagSMH));
					WritePrivateProfileString(szKey, INI_TYPE_ALGIN, szValue, szBackupFile);

					if(SndMsgOpt_IsNotShowRetDetail(smo.dwFlagSMH))
					{
						WritePrivateProfileString(szKey, INI_IS_NOPOPRET, TEXT("1"), szBackupFile);
					}
					if(SndMsgOpt_IsNotAllowBC(smo.dwFlagSMH))
					{
						WritePrivateProfileString(szKey, INI_FORBID_BROADCAST, TEXT("1"), szBackupFile);
					}
					wsprintf(szValue, TEXT("%d"), SndMsgParamOpt_GetDataFor(smo.wFlagParam[0])>>12);
					WritePrivateProfileString(szKey, INI_FOR_WPARAM, szValue, szBackupFile);
					wsprintf(szValue, TEXT("%d"), SndMsgParamOpt_GetDataType(smo.wFlagParam[0]));
					WritePrivateProfileString(szKey, INI_TYPE_WPARAM, szValue, szBackupFile);
					wsprintf(szValue, TEXT("%d"), smo.wBufSize[0]);
					WritePrivateProfileString(szKey, INI_SIZE_WPARAM, szValue, szBackupFile);

					wsprintf(szValue, TEXT("%d"), SndMsgParamOpt_GetDataFor(smo.wFlagParam[1])>>12);
					WritePrivateProfileString(szKey, INI_FOR_LPARAM, szValue, szBackupFile);
					wsprintf(szValue, TEXT("%d"), SndMsgParamOpt_GetDataType(smo.wFlagParam[1]));
					WritePrivateProfileString(szKey, INI_TYPE_LPARAM, szValue, szBackupFile);
					wsprintf(szValue, TEXT("%d"), smo.wBufSize[1]);
					WritePrivateProfileString(szKey, INI_SIZE_LPARAM, szValue, szBackupFile);

					if( GetPrivateProfileString(STR_INI_SEC_MSGMEMO, 
					szKey, NULL, szValue, MAX_STRBUF_LEN, szCfgFileName))
					{
						WritePrivateProfileString(szKey, INI_MEMO, szValue, szBackupFile);
					}

				}

				i+=j+1;
				SendMessage(hwndPB, PBM_SETPOS, i, 0);
			}
		}

		GlobalFree((HGLOBAL)lpszKeys);

		if(hwndPB)
		{
			LoadingBar_Close();
		}
	}
}


//////////////////////////////////////////////////////////////////////////
#include "WndSpyGui.h"
#include "SndMsgFunc.h"
#include "DlgSndMsg.h"

//////////////////////////////////////////////////////////////////////////

BOOL OnOptTipNotify(LPARAM lParam);
VOID OnInitSndMsgSettingDlg(HWND hwnd);
VOID OnShowSndMsgSettings(HWND hwnd);
VOID OnCheckSndMsgSettings(HWND hwnd);

//////////////////////////////////////////////////////////////////////////

BOOL CALLBACK DlgProc_SndMsgSetting(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
#define INT_CTRLID_DELTA	 (IDC_CBDL_2_FOR - IDC_CBDL_1_FOR)
#define NUM_SENDMSG_STRING	8

#define SEL_INPUT		0 //for IN
#define SEL_OUTPUT		1 //for OUT
#define SEL_UNUSE		2 //for UNUSE

#define SEL_32BITVAL	0 //type DWORD
#define SEL_STRING		1 //type STRING
#define SEL_STRUCT		2 //type STRUCT
#define SEL_AUTO		3 //type Auto

	switch (message)
	{

	case WM_INITDIALOG:
		{
			OnInitSndMsgSettingDlg(hwnd);
			return FALSE;
		}
		
	case WM_CTLCOLOREDIT: 
		{
			if(GetDlgCtrlID((HWND)lParam)==IDC_EDIT_MSGCOMMENT)
			{
				HBRUSH hBrush;
				hBrush = GetSysColorBrush(COLOR_WINDOW);
				SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));
				SetTextColor((HDC)wParam, (COLORREF)GetSysColor(COLOR_GRAYTEXT));
				return (LONG)hBrush;
			}
			return FALSE;
		}
		
	case WM_NOTIFY:
		{
			if( !OnOptTipNotify(lParam) )
			{
				OnToolTipNotify(wParam, lParam);
			}
			break;
		}
		
	case WM_COMMAND:
		{
			int i , j, n, m;
			
			switch ( LOWORD(wParam) )
			{
				
			case IDC_CBDL_1_FOR:
			case IDC_CBDL_2_FOR:
				{
					if(CBN_SELCHANGE == HIWORD(wParam) )
					{
					
						i=SendDlgItemMessage(hwnd, LOWORD(wParam), CB_GETCURSEL, 0, 0);
						
						if(i==SEL_INPUT)
						{
							n=SendDlgItemMessage(hwnd, LOWORD(wParam)+1, CB_GETCOUNT, 0, 0);
							if( n <= SEL_AUTO )
							{
								TCHAR szBuf[TINY_STRBUF_LEN];
								LoadString(g_hInstance, IDS_AUTO, szBuf, TINY_STRBUF_LEN); 
								SendDlgItemMessage(hwnd, LOWORD(wParam)+1, 
									CB_INSERTSTRING, SEL_AUTO, (LPARAM)szBuf);
								m= LOWORD(wParam)==IDC_CBDL_1_FOR ? 0:1;
								if( SndMsgParamOpt_GetDataType(g_smHdr.SMO.wFlagParam[m])==NULL )
								{
									SendDlgItemMessage(hwnd, LOWORD(wParam)+1, 
										CB_SETCURSEL, SEL_AUTO, 0);
								}
								
							}
						}
						else
						{
							SendDlgItemMessage(hwnd, LOWORD(wParam)+1, CB_DELETESTRING, SEL_AUTO, 0);
						}
						if(i==SEL_UNUSE)
						{
							DoEnableDlgItems(hwnd, LOWORD(wParam)+1, 
								LOWORD(wParam)+IDC_EDIT_1_SIZE-IDC_CBDL_1_FOR, 1, FALSE);
						}
						else
						{
							SetWindowEnable(hwnd, LOWORD(wParam)+1, TRUE);
						}
					
						SendMessage(hwnd,WM_COMMAND, MAKEWPARAM(LOWORD(wParam)+1,CBN_SELCHANGE), 0);
					}
					return TRUE;
				}
				
			case IDC_CBDL_1_TYPE:
			case IDC_CBDL_2_TYPE:
				{
					if( CBN_SELCHANGE == HIWORD(wParam) )
					{
						i=SendDlgItemMessage(hwnd, LOWORD(wParam), CB_GETCURSEL, 0, 0);
						
						j=SendDlgItemMessage(hwnd, LOWORD(wParam)-1, CB_GETCURSEL, 0, 0);

						if(j==SEL_UNUSE)
						{
							SetDlgItemInt(hwnd, LOWORD(wParam)+1, 0, 0);
							SendDlgItemMessage(hwnd, LOWORD(wParam), CB_SETCURSEL, CB_ERR, 0);
						}
						else
						{
							if( i==CB_ERR && IsWindowVisible(hwnd) )
							{
								i=SEL_32BITVAL;
								SendDlgItemMessage(hwnd, LOWORD(wParam), CB_SETCURSEL, SEL_32BITVAL, 0);
							}
							
							if(i==SEL_32BITVAL)
							{
								SetDlgItemInt(hwnd, LOWORD(wParam)+1, sizeof(UINT_PTR), 0);
							}
							else
							{
								
								if(j==SEL_INPUT)
								{
									SetWindowTextFromStrRes(hwnd, LOWORD(wParam)+1, g_hInstance, IDS_AUTO);
								}
								else // if(j==SEL_OUTPUT)
								{
									//MessageBeep(0);
									m= LOWORD(wParam)==IDC_CBDL_1_TYPE ? 0:1;
									if( g_smHdr.SMO.wBufSize[m] < 4 )
									{
										n= MAX_SNDMSG_PARAM_LEN-4;
									}
									else
									{
										n= (INT)g_smHdr.SMO.wBufSize[m];
									}
									SetDlgItemInt(hwnd,  LOWORD(wParam)+1, n, 0);
								}
								
							}
						}
						SetWindowEnable(hwnd, LOWORD(wParam)+1, (j==SEL_OUTPUT&&i!=SEL_32BITVAL));

					}
					return FALSE;
				}

			case IDC_EDIT_1_SIZE:
			case IDC_EDIT_2_SIZE:
				{
					return OnNumEditMaxMinRoutine(hwnd, wParam, lParam, sizeof(DWORD), MAX_SNDMSG_PARAM_LEN);
				}

			case IDC_BTN_OK:
				{
					OnCheckSndMsgSettings(hwnd);

				}//fall through...

			case IDCANCEL:
				{
					EndDialog(hwnd,0);
					return TRUE;
				}

			}//switch @ wm_command
			
			return FALSE;

		}//WM_COMMAND
		
	}//switch message...
	
	return CallSpecWndProcRoutine(hwnd, message, wParam, lParam);

} //DlgProc_SndMsgSetting();

//////////////////////////////////////////////////////////////////////////

VOID OnShowSndMsgSettings(HWND hwnd)
{
	INT i, j, iCtrlID;
	TCHAR szBuf[SMALL_STRBUF_LEN];

	CheckRadioButton(hwnd, IDC_RB_HEX, IDC_RB_DEC, 
		SndMsgOpt_IsHex(g_smHdr.SMO.dwFlagSMH)? IDC_RB_HEX:IDC_RB_DEC);
	
	switch(SndMsgOpt_GetDataAlign(g_smHdr.SMO.dwFlagSMH))
	{
	case SNDMSG_FLAG_ALIG_BYTE:
		i=IDC_RB_BYTE;
		break;
	case SNDMSG_FLAG_ALIG_WORD:
		i=IDC_RB_WORD;
		break;
	//case SNDMSG_FLAG_ALIG_DWORD:
	default:
		i=IDC_RB_DWORD;
	}
	CheckRadioButton(hwnd, IDC_RB_DWORD, IDC_RB_BYTE, i);

	CheckDlgButton(hwnd, IDC_CHK_POPDETAIL, 
		SndMsgOpt_IsShowRetDetail(g_smHdr.SMO.dwFlagSMH)? BST_CHECKED: BST_UNCHECKED);

	CheckDlgButton(hwnd, IDC_CHK_NOTALLOW_BC, 
		SndMsgOpt_IsAllowBC(g_smHdr.SMO.dwFlagSMH)? BST_UNCHECKED:BST_CHECKED);

	//CheckDlgButton(hwnd, IDC_CHK_SAVESETTING, 
	//	SndMsgOpt_IsProfileFixed(g_smHdr.SMO.dwFlagSMH)? BST_CHECKED: BST_UNCHECKED);

	CheckDlgButton(hwnd, IDC_CHK_SAVESETTING, BST_UNCHECKED);	
	
	for(i=0; i<2; i++)
	{
		iCtrlID = (i==0?IDC_CBDL_1_TYPE:IDC_CBDL_2_TYPE);

		SendDlgItemMessage(hwnd, iCtrlID, CB_SETDROPPEDWIDTH, 150, 0);

		for(j=IDS_TYPE_1; j<=IDS_TYPE_3; j++)
		{
			LoadString(g_hInstance, j, szBuf, SMALL_STRBUF_LEN);
			SendDlgItemMessage(hwnd, iCtrlID, CB_ADDSTRING, 0, (LPARAM)szBuf);
		}

		switch( SndMsgParamOpt_GetDataType(g_smHdr.SMO.wFlagParam[i]) )
		{

		case SNDMSG_FLAG_STRUCT:
			j=SEL_STRUCT;
			break;

		case SNDMSG_FLAG_STRING:
			j=SEL_STRING;
			break;

		case SNDMSG_FLAG_DWORD:
			j=SEL_32BITVAL;
			break;

		default:
			j=SEL_AUTO;

		}

		SendDlgItemMessage(hwnd, iCtrlID, CB_SETCURSEL, j, 0);
	}

	for(i=0; i<2; i++)
	{
		iCtrlID = (i==0?IDC_CBDL_1_FOR:IDC_CBDL_2_FOR);
		
		for(j=IDS_FOR_1; j<=IDS_FOR_3; j++)
		{
			LoadString(g_hInstance, j, szBuf, SMALL_STRBUF_LEN);
			SendDlgItemMessage(hwnd, iCtrlID, CB_ADDSTRING, 0, (LPARAM)szBuf);
		}

		switch( SndMsgParamOpt_GetDataFor(g_smHdr.SMO.wFlagParam[i]) )
		{

		case SNDMSG_FLAG_UNUSE:
			j=SEL_UNUSE;
			break;
		
		case SNDMSG_FLAG_OUT:
			j=SEL_OUTPUT;
			break;
		
		default://case SNDMSG_FLAG_IN:
			j=SEL_INPUT;
		}

		SendDlgItemMessage(hwnd, iCtrlID, CB_SETCURSEL, j, 0);
		SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(iCtrlID, CBN_SELCHANGE), 0);
	}

} //OnShowSndMsgSettings();

VOID OnCheckSndMsgSettings(HWND hwnd)
{
	INT i, j;

	SENDMSG_HEADER smh={0};

	if( IsDlgButtonChecked(hwnd, IDC_RB_BYTE) == BST_CHECKED )
	{
		SndMsgOpt_SetDataAlign(smh.SMO.dwFlagSMH, SNDMSG_FLAG_ALIG_BYTE);
	}
	else if( IsDlgButtonChecked(hwnd, IDC_RB_WORD) == BST_CHECKED )
	{
		SndMsgOpt_SetDataAlign(smh.SMO.dwFlagSMH, SNDMSG_FLAG_ALIG_WORD);
	}
	else
	{
		SndMsgOpt_SetDataAlign(smh.SMO.dwFlagSMH, SNDMSG_FLAG_ALIG_DWORD);
	}

	
	if( (BOOL)(IsDlgButtonChecked(hwnd, IDC_RB_HEX) == BST_CHECKED) )
	{
		SndMsgOpt_SetHex(smh.SMO.dwFlagSMH);
	}
	else
	{
		SndMsgOpt_SetDec(smh.SMO.dwFlagSMH);
	}

	if( IsDlgButtonChecked(hwnd, IDC_CHK_POPDETAIL) == BST_CHECKED )
	{
		SndMsgOpt_SetShowRetDetail(smh.SMO.dwFlagSMH);
	}
	else
	{
		SndMsgOpt_UnSetShowRetDetail(smh.SMO.dwFlagSMH);
	}
	
	if( IsDlgButtonChecked(hwnd, IDC_CHK_NOTALLOW_BC) == BST_CHECKED )
	{
		SndMsgOpt_UnSetAllowBC(smh.SMO.dwFlagSMH);
	}
	else
	{
		SndMsgOpt_SetAllowBC(smh.SMO.dwFlagSMH);
	}

	for(i=0; i<2; i++)
	{	
		j=SendDlgItemMessage(hwnd, i==0 ? IDC_CBDL_1_FOR:IDC_CBDL_2_FOR, CB_GETCURSEL, 0, 0);

		if(j==SEL_UNUSE)
		{
			SndMsgParamOpt_SetDataFor(smh.SMO.wFlagParam[i], SNDMSG_FLAG_UNUSE);
			continue;
		}
		else if(j==SEL_OUTPUT)
		{
			smh.SMO.wBufSize[i]=(WORD)
				GetDlgItemInt(hwnd, i==0? IDC_EDIT_1_SIZE: IDC_EDIT_2_SIZE, NULL, FALSE);
			SndMsgParamOpt_SetDataFor(smh.SMO.wFlagParam[i], SNDMSG_FLAG_OUT);
		}
		else
		{
			SndMsgParamOpt_SetDataFor(smh.SMO.wFlagParam[i], SNDMSG_FLAG_IN);
		}
		
		j=SendDlgItemMessage(hwnd, i==0 ? IDC_CBDL_1_TYPE:IDC_CBDL_2_TYPE, CB_GETCURSEL, 0, 0);
		
		if(j==SEL_STRUCT)
		{
			SndMsgParamOpt_SetDataType(smh.SMO.wFlagParam[i], SNDMSG_FLAG_STRUCT);
		}
		else if(j==SEL_STRING)
		{
			SndMsgParamOpt_SetDataType(smh.SMO.wFlagParam[i], SNDMSG_FLAG_STRING);
		}
		else if(j==SEL_32BITVAL)
		{
			SndMsgParamOpt_SetDataType(smh.SMO.wFlagParam[i], SNDMSG_FLAG_DWORD);
		}
		else
		{
			SndMsgParamOpt_SetDataType(smh.SMO.wFlagParam[i], NULL);
		}
		
	}
	
	if( (BOOL)IsDlgButtonChecked(hwnd, IDC_CHK_SAVESETTING) )
	{
		SndMsgOpt_SetProfileFixed(smh.SMO.dwFlagSMH);
		TCHAR szBuf[SMALL_STRBUF_LEN];
		TCHAR szMemo[MAX_MSGMEMO_STR_LEN];
		GetDlgItemText(hwnd, IDC_EDIT_MSGCOMMENT, szMemo, MAX_MSGMEMO_STR_LEN);
		GetDlgItemText(hwnd, IDC_EDIT_MSGNAME, g_strBuf, MAX_STRBUF_LEN);
		WriteSndMsgOptProfile(GetCfgFileName(szBuf), g_strBuf, &smh, szMemo);
	}
	else
	{
		SndMsgOpt_UnSetProfileFixed(smh.SMO.dwFlagSMH);
	}

	OnApplySndMsgOptions(g_TabDlgHdr.CDI[TAB_MANI].hwnd, &smh, TRUE);

} //OnCheckSndMsgSettings();

VOID OnInitSndMsgSettingDlg(HWND hwnd)
{

	TCHAR szBuf[MAX_PATH];
	TCHAR szMinBuf[MINI_STRBUF_LEN];
	TCHAR szSmallBuf[SMALL_STRBUF_LEN];
	TCHAR szMemoBuf[MAX_MSGMEMO_BUF_LEN];
	
	if(	0 >= GetSndMsgMessageSymbol(
		g_TabDlgHdr.CDI[TAB_MANI].hwnd, 
		&g_smHdr, szSmallBuf, SMALL_STRBUF_LEN, TRUE) )
	{
		EndDialog(hwnd, INIT_ERROR);
	}
	
	GetSndMsgOptProfile(GetCfgFileName(szBuf), szSmallBuf, 
		NULL, szMemoBuf, MAX_MSGMEMO_BUF_LEN);

	SetDlgItemText(hwnd, IDC_EDIT_MSGNAME, szSmallBuf);
	SetDlgItemText(hwnd, IDC_EDIT_MSGCOMMENT, szMemoBuf);
	GetWindowText(hwnd, szMinBuf, MINI_STRBUF_LEN);
	wsprintf(g_strBuf, TEXT("[%s]%s"), szSmallBuf, szMinBuf);
	SetWindowText(hwnd, g_strBuf);
	
	HWND hwndTT;
	hwndTT=CreateMyTooltip(hwnd);
	SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, (LPARAM)500);
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_RESHOW, 100);

	for(int i=IDC_RB_DWORD; i<=IDC_RB_BYTE; i++)
	{
		AddControlTip(hwndTT, hwnd, i, 
			MAKEINTRESOURCE(IDS_TIP_DATA_ALIGN));
	}

	AddControlTip(hwndTT, hwnd, 
		IDC_CHK_SAVESETTING, LPSTR_TEXTCALLBACK);

	SendDlgItemMessage(hwnd, IDC_EDIT_MSGCOMMENT, EM_SETLIMITTEXT, MAX_MSGMEMO_STR_LEN-16, 0);

	hwndTT=CreateMyTooltip(hwnd);
	SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, (LPARAM)500);
 	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 1000);
	SendMessage(hwndTT, TTM_SETTIPBKCOLOR, GetSysColor(COLOR_INFOBK), 0);
	SendMessage(hwndTT, TTM_SETTIPTEXTCOLOR, GetSysColor(COLOR_HIGHLIGHT), 0);
	AddControlTip(hwndTT, hwnd, IDC_EDIT_MSGCOMMENT, LPSTR_TEXTCALLBACK);
		
	EnableThemeDialogTexture(hwnd, ETDT_ENABLETAB);
	MoveWndToWndCenter(hwnd, g_hwnd);

	OnShowSndMsgSettings(hwnd);
	
	SetDlgItemSubClass(hwnd, IDC_EDIT_MSGCOMMENT, SCP_Edit_SelAll);
	SetDlgItemSubClass(hwnd, IDC_CHK_POPDETAIL, SCP_ChkBtn_NoFoucsRect);
	SetDlgItemSubClass(hwnd, IDC_CHK_NOTALLOW_BC, SCP_ChkBtn_NoFoucsRect);
	SetDlgItemSubClass(hwnd, IDC_CHK_SAVESETTING, SCP_ChkBtn_NoFoucsRect);
	SetDlgItemFocus(hwnd, IDCANCEL);

} //OnInitSndMsgSettingDlg();

BOOL OnOptTipNotify(LPARAM lParam)
{
	INT iCtlId;
	iCtlId=GetDlgCtrlID((HWND)((LPNMTTDISPINFO)lParam)->hdr.idFrom);
	if(	((LPNMTTDISPINFO)lParam)->hdr.code==TTN_NEEDTEXT && 
		iCtlId==IDC_CHK_SAVESETTING )
	{
		LoadString(g_hInstance, IDS_TIP_SAVEMSGSET, g_strBuf, MAX_STRBUF_LEN);
		((LPNMTTDISPINFO)lParam)->lpszText=g_strBuf;
		return TRUE;
	}
	return FALSE;

} //OnWndRectTipNotify();

//////////////////////////////////////////////////////////////////////////
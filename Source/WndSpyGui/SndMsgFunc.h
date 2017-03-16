#if _MSC_VER >= 1000
#pragma once
#endif
#ifndef HS_HEADER_DEF_SNDMSGFUCN_H
#define HS_HEADER_DEF_SNDMSGFUCN_H

//////////////////////////////////////////////////////////////////////////

#define MAX_MSG_STR_LEN			64
#define MAX_HWNDTAG_STR_LEN		32

typedef struct tag_SndMsgOpt{
	
	DWORD dwFlagSMH;
	WORD wFlagParam[2];
	WORD wBufSize[2];
	
} SENDMSG_OPT;

typedef struct tag_SndMsgHeader{
	
	SENDMSG_OPT SMO;
	LPVOID lpData[2];
	HWND hwndTarget;
	UINT uMsg;
	INT	iMsgIndex;
	BOOL bNoPopConfirm;

}SENDMSG_HEADER;

extern SENDMSG_HEADER g_smHdr;

//////////////////////////////////////////////////////////////////////////
#define MAX_SNDMSG_STR_LEN		MAX_PATH
#define MAX_SNDMSG_PARAM_LEN	MAX_SNDMSG_STR_LEN
#define MAX_MSGMEMO_STR_LEN		512
#define MAX_MSGMEMO_BUF_LEN		(MAX_MSGMEMO_STR_LEN+MAX_MSGMEMO_STR_LEN)

#define SNDMSG_FLAG_FOR_MASK		0x0FFF
#define SNDMSG_FLAG_IN				0x0000 //DEFAULT
#define SNDMSG_FLAG_OUT				0x1000
#define SNDMSG_FLAG_UNUSE			0x2000
#define SNDMSG_FLAG_TYPE_MASK		0xFFF0
#define SNDMSG_FLAG_DWORD			0x0001
#define SNDMSG_FLAG_STRUCT			0x0002
#define SNDMSG_FLAG_STRING			0x0004
// #define SNDMSG_FLAG_STRUCT_SPECIAL	0x0012 //reserved...

#define SNDMSG_FLAG_NOPOPUPRET		0x01000000
#define SNDMSG_FLAG_FORBID_BC		0x02000000
#define SNDMSG_FLAG_FIXED			0x00001000
#define SNDMSG_FLAG_BUSY			0x000000FF

//#define SNDMSG_FLAG_VALUE_HEX		0x00000000 //DEFAULT
#define SNDMSG_FLAG_VALUE_DEC		0x08000000

#define SNDMSG_FLAG_ALIGN_MASK		0xFFF0FFFF
#define SNDMSG_FLAG_ALIG_DWORD		0x00040000
#define SNDMSG_FLAG_ALIG_WORD		0x00020000
#define SNDMSG_FLAG_ALIG_BYTE		0x00010000


#define SndMsgOpt_IsDec(_dwFlag)  \
(IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_VALUE_DEC))

#define SndMsgOpt_IsHex(_dwFlag)  \
(!IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_VALUE_DEC))

#define SndMsgOpt_SetHex(_dwFlag)  \
(_dwFlag&=~SNDMSG_FLAG_VALUE_DEC)

#define SndMsgOpt_SetDec(_dwFlag)  \
(_dwFlag|=SNDMSG_FLAG_VALUE_DEC)

#define SndMsgOpt_IsBusyNow(_dwFlag) \
IS_FLAGS_MARKED(_dwFlag, SNDMSG_FLAG_BUSY)

#define SndMsgOpt_SetBusy(_dwFlag) \
(_dwFlag|=SNDMSG_FLAG_BUSY)

#define SndMsgOpt_SetIdle(_dwFlag) \
(_dwFlag&=~SNDMSG_FLAG_BUSY)

#define SndMsgOpt_IsProfileFixed(_dwFlag)  \
IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_FIXED)

#define SndMsgOpt_SetProfileFixed(_dwFlag)  \
(_dwFlag|=SNDMSG_FLAG_FIXED)

#define SndMsgOpt_UnSetProfileFixed(_dwFlag)  \
(_dwFlag&=~SNDMSG_FLAG_FIXED)

#define SndMsgOpt_IsShowRetDetail(_dwFlag)  \
!IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_NOPOPUPRET)

#define SndMsgOpt_IsNotShowRetDetail(_dwFlag)  \
IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_NOPOPUPRET)

#define SndMsgOpt_SetShowRetDetail(_dwFlag)  \
(_dwFlag&=~SNDMSG_FLAG_NOPOPUPRET)

#define SndMsgOpt_UnSetShowRetDetail(_dwFlag)  \
(_dwFlag|=SNDMSG_FLAG_NOPOPUPRET)


#define SndMsgOpt_IsAllowBC(_dwFlag)  \
!IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_FORBID_BC)

#define SndMsgOpt_IsNotAllowBC(_dwFlag)  \
IS_FLAG_SETTED(_dwFlag, SNDMSG_FLAG_FORBID_BC)

#define SndMsgOpt_SetAllowBC(_dwFlag)  \
(_dwFlag&=~SNDMSG_FLAG_FORBID_BC)

#define SndMsgOpt_UnSetAllowBC(_dwFlag)  \
(_dwFlag|=SNDMSG_FLAG_FORBID_BC)

#define SndMsgOpt_GetDataAlign(_dwFlag) \
(_dwFlag&~SNDMSG_FLAG_ALIGN_MASK)

#define SndMsgOpt_SetDataAlign(_dwFlag, _bitsFlag) \
( _dwFlag= ((_dwFlag&SNDMSG_FLAG_ALIGN_MASK)|_bitsFlag) )

#define SndMsgParamOpt_GetDataFor(_wFlag)	\
(_wFlag&~SNDMSG_FLAG_FOR_MASK)

#define SndMsgParamOpt_SetDataFor(_wFlag, _bitsFlag)	\
( _wFlag= ((_wFlag&SNDMSG_FLAG_FOR_MASK)|_bitsFlag) )

#define SndMsgParamOpt_GetDataType(_wFlag)	\
(_wFlag&~SNDMSG_FLAG_TYPE_MASK)

#define SndMsgParamOpt_SetDataType(_wFlag, _bitsFlag)	\
( _wFlag= ((_wFlag&SNDMSG_FLAG_TYPE_MASK)|_bitsFlag) )


//////////////////////////////////////////////////////////////////////////

HWND GetTargetHwndForSMH(HWND hwnd, SENDMSG_HEADER* pSMH);
INT GetMessageForSMH(HWND hwnd, SENDMSG_HEADER* pSMH);
BOOL GetSndMsgHeader(HWND hwnd, SENDMSG_HEADER* pSMH, BOOL bExcludeParam);

VOID DoSetParamEditText(HWND hwndEdit, SENDMSG_HEADER* pSMH, 
						INT iParamIndex, INT iAlign, BOOL isHex);
VOID DoSwitchHexDec(HWND hwnd, BOOL isToHex, SENDMSG_HEADER* pSMH, BOOL isConvParam);


//////////////////////////////////////////////////////////////////////////

VOID OnSendMessageToTarget(HWND hwnd, SENDMSG_HEADER* pSMH, BOOL isPostMsg);
BOOL CALLBACK DlgProc_SndMsgConfirmBox(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc_SndMsgDetailResult(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////

LPTSTR GetParamTypeString(LPTSTR szGetTypeBuf, WORD wFor, WORD wType);

/****
* The szMsgBuf must has at least 128 tchar space, or the function will always fail.
* and iMaxBufLen >= 128
****/
INT GetSndMsgMessageSymbol(HWND hwnd, SENDMSG_HEADER* pSMH, 
							LPTSTR szMsgBuf, INT iMaxBufLen, BOOL IsFmtUnKnownMsg);

BOOL GetSndMsgOptProfile(LPTSTR szIniFileName, LPTSTR szMsgSymbol, 
						 SENDMSG_HEADER* pSMH, LPTSTR szMemoBuf, INT iMaxBufLen);

BOOL WriteSndMsgOptProfile(LPTSTR szIniFileName, LPTSTR szMsgSymbol, 
						   SENDMSG_HEADER* pSMH, LPTSTR szMsgMemo);

VOID OnApplySndMsgOptions(HWND hwnd, SENDMSG_HEADER* pNewSMH, BOOL isAutoConvParam);

//////////////////////////////////////////////////////////////////////////

VOID ExportSndMsgProfile(LPTSTR szBackupFile);
VOID ImportSndMsgProfile(LPTSTR szBackupFile);

//////////////////////////////////////////////////////////////////////////

#define WM_MY_LOAD_MSG_PROFILE (WM_USER_FIRST_SENDMSG+1)

VOID CALLBACK TimerProc_LoadSndMsgProfile(HWND hwnd,UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

#define SndMsgOpt_GoToLoadProfile(_hwndHoldTimer, _timeInterval)	\
SetTimer(_hwndHoldTimer, WM_MY_LOAD_MSG_PROFILE, _timeInterval, TimerProc_LoadSndMsgProfile);

//////////////////////////////////////////////////////////////////////////

#endif //HS_HEADER_DEF_SNDMSGFUCN_H
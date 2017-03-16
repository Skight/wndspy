#include "WndSpylib.h"
#include "WndSpylibOpt.h"

//////////////////////////////////////////////////////////////////////////
#ifdef WS_LIB_DLLENTRYPOINT

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(DLL_PROCESS_ATTACH==fdwReason)
	{
#define _NO_THREAD_NOTIFY
#ifdef _NO_THREAD_NOTIFY
		DisableThreadLibraryCalls(hinstDLL);
#endif
	}
	return TRUE;
}

#endif //WS_LIB_DLLENTRYPOINT


#ifdef WS_LIB_MAP_MSG_PARAM
//////////////////////////////////////////////////////////////////////////
extern SPYAPPDATA 	g_SAD;
extern HHOOK		g_hHook;
extern HWND			g_hwndTag;
extern BOOL			g_bIsOnce;
extern DWORD		g_dwBuf;
extern WNDPROC		g_wndproc;
extern HINSTANCE	g_hWndInstance;
extern WINDOWINFO	g_wi;
extern WNDCLASSEX	g_wcex;
extern TCHAR 		g_szBuf[SPYDLL_LARGE_STR_LEN];
extern TCHAR 		g_szSmallBuf[SPYDLL_SMALL_STR_LEN];
extern TCHAR 		g_szArray[4][MAX_PATH];

#define g_iAlign		g_wi.cxWindowBorders
#define g_IsHex			g_wi.cyWindowBorders
#define g_iStrPos		g_wcex.cbClsExtra
#define g_iDataPos		g_wcex.cbWndExtra
#define g_DataBuf		g_szBuf
#define g_szWParam		g_szArray[0]
#define g_szLParam		g_szArray[1]
#define g_StrBuf		g_szArray[2]
//////////////////////////////////////////////////////////////////////////

void DoMapDataString(LPTSTR szString)
{
	if( lstrlen(szString) )
	{
		g_iStrPos=Lib_MapStringToAdr(szString, g_StrBuf, g_iStrPos, MAX_PATH-(INT)g_iStrPos);
		g_iDataPos=Lib_MapStructToAdr(szString, '[', ']', g_IsHex, g_iAlign, 
			(BYTE*)g_DataBuf, g_iDataPos, SPYDLL_LARGE_STR_LEN-(INT)g_iDataPos);
	}
}


void LibMapDataString(void)
{
	g_iStrPos=0;
	g_iDataPos=0;
	ZeroMemory(g_DataBuf, SPYDLL_LARGE_STR_LEN*sizeof(TCHAR));
	ZeroMemory(g_StrBuf, MAX_PATH*sizeof(TCHAR));

	DoMapDataString(g_szWParam);
	DoMapDataString(g_szLParam);
}


LRESULT CALLBACK HookProc_MapData(int nCode,
								WPARAM wParam,
								LPARAM lParam )
{
	if(!g_bIsOnce) //只hook一次...
	{
		g_bIsOnce=TRUE;
		LibMapDataString();
	}

	if(nCode<0)
	{
		return CallNextHookEx(g_hHook,nCode,wParam,lParam);
	}
	else
	{
		return 0;
	}
}

BOOL wsMapDataToTarget(HWND hwnd, BOOL IsHex, INT iAlign, LPTSTR szWParam, LPTSTR szLParam)
{
	BOOL ret = TRUE;
	DWORD dwPID=0;
	DWORD dwTID=0;

	g_IsHex=IsHex;
	g_iAlign=iAlign;
	g_szWParam[0]='\0';
	g_szLParam[0]='\0';
	if(szWParam)
	{
		lstrcpyn(g_szWParam, szWParam, MAX_SNDMSG_STR_LEN);
	}
	if(szLParam)
	{
		lstrcpyn(g_szLParam, szLParam, MAX_SNDMSG_STR_LEN);
	}

	dwTID=GetWindowThreadProcessId(hwnd, &dwPID);

	if( GetCurrentProcessId()!=dwPID )
	{
		ret=wsSetWndHookStub(hwnd,dwTID,NULL,HookProc_MapData, FALSE);
		//We should keep hook until sndmsg is finished.
	}
	else
	{
		LibMapDataString();
	}
	
	//Wait another 100ms to ensure data-mapping to be done...
	Sleep(100);

	if(szWParam)
	{
		lstrcpyn(szWParam, g_szWParam, MAX_SNDMSG_STR_LEN);
	}
	if(szLParam)
	{
		lstrcpyn(szLParam, g_szLParam, MAX_SNDMSG_STR_LEN);
	}

	return ret;
}


INT Lib_MapStringToAdr(LPTSTR szString, LPTSTR szGlobalBuf, INT iAdrPos, INT iMaxGloBufLen)
{
	int i,j;
	int iStart;
	int iLen=lstrlen(szString);
	TCHAR szBuf[MAX_SNDMSG_STR_LEN]={0};
	TCHAR szNum[MAX_STRLEN_NUM]={0};

	for(i=0, j=0; i<iLen; i++,j++)
	{
		if( szString[i]=='\"' )
		{
			iStart=i+1;
			do
			{
				i++;
				if( szString[i]=='\"')
				{
					lstrcpyn(szGlobalBuf+iAdrPos, 
						szString+iStart, 
						min(i-iStart+1, iMaxGloBufLen-iAdrPos) );

					wsprintf(szNum, TEXT(" %X# "), 
						(DWORD)(szGlobalBuf+iAdrPos) );

					lstrcat(szBuf, szNum);
					j+=lstrlen(szNum)-1;
					iAdrPos+=i-iStart+1;
					break;
				}

			}while(szString[i]!='\0');

		}
		else
		{
			szBuf[j]=szString[i];
		}
	}

	lstrcpyn(szString, szBuf, MAX_SNDMSG_STR_LEN);
	return iAdrPos;
}


INT Lib_MapStructToAdr(LPTSTR szString, 
					 TCHAR chStart, TCHAR chEnd, BOOL isHex, INT iAlign, 
					 BYTE* pGlobalDataBuf, INT iAdrPos, INT iMaxGloBufSize)
{
	INT i=0;
	TCHAR szTempBuf[MAX_SNDMSG_STR_LEN]={0};
	TCHAR szNum[MAX_STRLEN_NUM]={0};

	while(szString[i]!='\0')
	{
		if(szString[i]==chStart)
		{
			lstrcpyn(szTempBuf, szString+i+1, MAX_SNDMSG_STR_LEN);
			lstrcpy(szString+i, szTempBuf);

			iAdrPos=Lib_MapStructToAdr(szString+i, 
				chStart, chEnd, isHex, iAlign, 
				pGlobalDataBuf, iAdrPos, iMaxGloBufSize-iAdrPos);

		}
		else if(szString[i]==chEnd)
		{
			wsprintf(szNum, TEXT(" %X# "),
				(DWORD)(pGlobalDataBuf+iAdrPos) );
			
			lstrcpyn(szTempBuf, szString, i+1);
			iAdrPos+=Lib_NumStrToStruct(szTempBuf, 
					pGlobalDataBuf+iAdrPos, iAlign, isHex, iMaxGloBufSize-iAdrPos);

			lstrcpy(szTempBuf, szNum);
			lstrcat(szTempBuf, szString+i+1);
			lstrcpy(szString, szTempBuf);
			break;
		}

		i++;
	}

	return iAdrPos;

}

#ifndef _HS_COMMON_FUN_STR2INT
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#define _HS_COMMON_FUN_INTPOW
#include "..\HsCommon\HsMath.cpp"

INT Lib_HexStrToInt(LPTSTR szHex)
{
	int i, iLen;
	TCHAR chNum;
	INT iNum;
	LPTSTR pstr=szHex;
	DWORD dwInt=0;
	INT iSign=1;

	if(szHex[0]=='-')
	{
		pstr++;
		iSign=-1;
	}

	iLen=lstrlen(pstr);

	if(iLen > 8) //DWORD为8个16进制字符...
	{
		iLen=8;
	}
	
	CharUpper(pstr);
 	
	for (i = 0, dwInt = 0; i < iLen; i++)
	{
		chNum = *(pstr + iLen -1 - i );

		if('0'<=chNum && chNum <= '9')
		{
			iNum=(int)(chNum-'0');
		}
		else if( 'A'<=chNum && chNum <='F' )
		{
			iNum=(int)(chNum-'A' + 10);
		}
		else
		{
			return 0;
		}

		dwInt += (DWORD)( iNum * IntPow(16, i) );
	}

	return iSign*dwInt;
}


INT Lib_DecStrToInt(LPTSTR szDecNum)
{
	int i, iLen;
	TCHAR chNum;
	INT iNum;
	INT iInt=0;
	INT iSign=1;
	LPTSTR pstr=szDecNum;
	
	if(szDecNum[0]=='-')
	{
		pstr++;
		iSign=-1;
	}

	iLen=lstrlen(pstr);

	if(iLen > 10)
	{
		iLen=10;
	}

	for (i = 0, iInt = 0; i < iLen; i++)
	{
		chNum = *(pstr + iLen -1 - i );

		if('0'<=chNum && chNum <= '9')
		{
			iNum=(int)(chNum-'0');
		}
		else
		{
			return 0;
		}

		iInt += (DWORD)( iNum * IntPow(10, i) );
	}

	return iSign*iInt;
}

#endif //_HS_COMMON_FUN_STR2INT


INT Lib_NumStrToStruct(LPTSTR szDataStr, PVOID lpData, INT iAlign, BOOL IsHex, INT iMaxByte)
{
	int i;
	int iStart, iEnd;
	int iWidth;
	DWORD dwBuf;
	INT iDelta;
	int iLen=lstrlen(szDataStr);
	TCHAR szNum[MAX_STRLEN_NUM];

	switch (iAlign)
	{
	case 1: case 2: case 4:
		break;
	default:
		iAlign=4;
	}

	for(i=0, iStart=0, iDelta=0; i<=iLen && iDelta<=iMaxByte; i++)
	{

		if( szDataStr[i]==' ' || 
			szDataStr[i]=='\t' || 
			szDataStr[i]=='\r' || 
			szDataStr[i]=='\n' ||
			szDataStr[i]=='#' ||
			szDataStr[i]=='\0' )
		{

			if(i>iStart)
			{
				iEnd=i;
				iWidth=min(iEnd-iStart, IsHex? 8:MAX_STRLEN_NUM);

#ifndef _HS_COMMON_FUN_STR2INT

				if(IsHex)
				{
					lstrcpyn(szNum, TEXT("0x"), 3);
					lstrcpyn(szNum+2, szDataStr+iEnd-iWidth, iWidth+1);
				}
				else
				{
					lstrcpyn(szNum, szDataStr+iEnd-iWidth, iWidth+1);
				}
				StrToIntEx(szNum, STIF_SUPPORT_HEX, (INT*)&dwBuf);
#else

				lstrcpyn(szNum, szDataStr+iEnd-iWidth, iWidth+1);
				dwBuf=(IsHex||szDataStr[i]=='#') ? 
					Lib_HexStrToInt(szNum) : Lib_DecStrToInt(szNum);
#endif
				*(DWORD*)((BYTE*)lpData+iDelta)=dwBuf;

				if(iEnd==iLen && iDelta==0)
				{
					iDelta=sizeof(DWORD);
					break;
				}

				if(szNum[0]=='-')
				{
					iWidth=1;
					dwBuf*=-1;
				}
				else
				{
					iWidth=0;
				}

				wsprintf(szNum,TEXT("%X"),dwBuf);

				if(iWidth==1 && szNum[0]<'8') //负数且乘-1未溢出，初始长度归零...
				{
					iWidth=0;
				}

				iWidth+=lstrlen(szNum);

				if(IsHex) //十六进制数长度处理
				{
					iWidth=max(iWidth,iEnd-iStart);
				}

				if( iWidth>4 || iAlign==4 ||
					szDataStr[i]=='#')
				{
					iDelta+=sizeof(DWORD);
				}
				else if(iWidth>2 || iAlign==2)
				{
					iDelta+=sizeof(WORD);
				}
				else
				{
					iDelta+=sizeof(BYTE);
				}

			}
			iStart=i+1;
		}

	}

	return iDelta;
}

#endif //End... WS_LIB_MAP_MSG_PARAM
//////////////////////////////////////////////////////////////////////////



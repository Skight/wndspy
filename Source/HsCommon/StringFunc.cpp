#include <windows.h>
#include "HsMath.h"
#include "StringFunc.h"

///////////////////////////////////////////////////////////////////////////
#ifdef _HS_COMMON_FUN_STR2INT

int HexStrToInt(LPTSTR szIntStr)
{	
	int iResult;
	int iSign=1;
	int i, iLen;
	int iNum;
	
	TCHAR chNum;
	LPTSTR pstr=szIntStr;
	
	if(*pstr=='-')
	{
		pstr++;
		iSign=-1;
	}
	
	iLen=lstrlen(pstr);
	
	if(iLen > 8) //DWORD 8 bits char...
	{
		iLen=8;
	}
	
	CharUpper(pstr);
	
	for (i = 0, iResult = 0; i < iLen; i++)
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
		
		iResult += (int)( iNum * IntPow(16, i) );
	}
	return iSign*iResult;
}

int DecStrToInt(LPTSTR szIntStr)
{
	int iResult;
	int iSign=1;
	int i, iLen;
	int iNum;
	
	TCHAR chNum;
	LPTSTR pstr=szIntStr;
	
	if(szIntStr[0]=='-')
	{
		pstr++;
		iSign=-1;
	}
	
	iLen=lstrlen(pstr);
	
	if(iLen > 10)
	{
		iLen=10;
	}
	
	for (i = 0, iResult = 0; i < iLen; i++)
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
		
		iResult += (DWORD)( iNum * IntPow(10, i) );
	}
	
	return iSign*iResult;
}

#else

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

int HexStrToInt(LPTSTR szIntStr)
{
	int iResult=0;
	int iSign=1;
	TCHAR szHexStr[MAX_STRLEN_LONGINT];	
	TCHAR szIntStrBuf[MAX_STRLEN_LONGINT];	//int str copyn;
	LPTSTR pstr=szIntStr;
	
	if( *pstr=='-' )
	{
		pstr++;
		iSign=-1;
	}
	
	if( FALSE == (*pstr=='0' && 
		(*(pstr+1)=='x' || *(pstr+1)=='X')) )
	{
		lstrcpyn(szIntStrBuf, pstr, MAX_STRLEN_LONGINT-2);
		wsprintf(szHexStr, TEXT("0x%s"), szIntStrBuf);
	}
	else
	{
		lstrcpyn(szHexStr, pstr, MAX_STRLEN_LONGINT);
	}
	
	StrToIntEx(szHexStr, STIF_SUPPORT_HEX, (INT*)&iResult);
	
	return iSign*iResult;
}

int DecStrToInt(LPTSTR szIntStr)
{
	int iResult=0;
	
	StrToIntEx(szIntStr, STIF_DEFAULT,(INT*)&iResult);
	
	return iResult;
}

#endif //_HS_COMMON_FUN_STR2INT

//////////////////////////////////////////////////////////////////////////

INT Isolation_GetStringPortion(LPTSTR szString, 
							   LPTSTR szPortion, 
							   TCHAR chdelimiter,
							   bool IsGetTail)
{
	int i=0, iLen;
	LPTSTR pstr;
	
	iLen=lstrlen(szString);
	
	if(iLen)
	{
		pstr = szString + iLen-1;
		
		for(i=0; i<iLen; i++)
		{
			if( *pstr == chdelimiter)
			{
				pstr++;
				break;
			}
			
			pstr--;
		}
		if(i<iLen)
		{
			if(IsGetTail)
			{
				lstrcpyn(szPortion,pstr,i+1);
			}
			else
			{
				lstrcpyn(szPortion,szString,iLen-i);
				i=iLen-i;
			}
		}
		else
		{
			i=0;
		}
	}
	
	*(szPortion+i)='\0';
	
	return i;
	
}

//////////////////////////////////////////////////////////////////////////

TCHAR Hs_GetCharCase(TCHAR chTest, BOOL isUpperCase)
{
	TCHAR chSetChar[2];
	chSetChar[0]=chTest;
	chSetChar[1]=0;
	if(isUpperCase)
	{
		CharUpper(chSetChar);
	}
	else
	{
		CharLower(chSetChar);
	}
	
	return chSetChar[0];
}

//////////////////////////////////////////////////////////////////////////

BOOL Hs_CharCmp(TCHAR char1, TCHAR char2, BOOL isCaseSen)
{
	TCHAR ch1[2];
	TCHAR ch2[2];
	ch1[1]=ch2[1]=0;
	ch1[0]=char1;
	ch2[0]=char2;
	
	if(!isCaseSen)
	{
		CharUpper(ch1);
		CharUpper(ch2);
	}
	
	return ch1[0]-ch2[0];
}

//////////////////////////////////////////////////////////////////////////

BOOL IsIntStr(LPTSTR szString, BOOL isHex)
{
	int n, m;
	TCHAR szStrCopy[MAX_NUMSTR_LEN];
	
	n=lstrlen(szString);
	if( n==0 || n > MAX_NUMSTR_LEN-1 )
	{
		return FALSE;
	}
	
	lstrcpy(szStrCopy, szString);
	
	m=lstrtrim(szStrCopy, TEXT("-"));
	if( n-m > 1)
	{
		return FALSE;
	}
	n=lstrtrimi(szStrCopy, TEXT("0x"));
	if( m-n > 2 )
	{
		return FALSE;
	}
	if( !IsCharSetString(szStrCopy, 
		isHex? TEXT("0123456789ABCDEF"):TEXT("0123456789"), FALSE) )
	{
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL IsInCharSet(TCHAR chTest, TCHAR* szCharSetArray, BOOL IsCaseSen)
{
	int i, iArrayLen;
	
	iArrayLen=lstrlen(szCharSetArray);
	
	for(i=0; i< iArrayLen; i++)
	{
		if(	0==CharCompare(chTest, szCharSetArray[i], IsCaseSen) )
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL IsCharSetString(LPCTSTR szString, TCHAR* szCharSetArray, BOOL IsCaseSen)
{
	int i, iLen;
	
	iLen=lstrlen(szString);
	
	for (i = 0; i < iLen; i++)
	{
		if( !IsInCharSet(szString[i], szCharSetArray, IsCaseSen) )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}


INT MakeSameCharString(LPTSTR szString, TCHAR cChar, INT iRepeat)
{
	int i;
	for (i = 0; i < iRepeat; i++)
	{
		szString[i]=cChar;
	}
	szString[i]='\0';
	return i;
}

INT StringPatternReplace(LPTSTR szString, LPTSTR szPat, LPTSTR szReplace, BOOL bIsCaseSensitive)
{
	TCHAR* pTchar = szString;
	TCHAR* szBuf=NULL;
	
#define BUILDIN_STR_BUF_LEN	 32
	
	int iLenLeft;
	int iLen;
	int iLenPat;
	int iLenReplace;
	int iCmp;
	BOOL isBufAlloc=FALSE;
	TCHAR szQuickBuf[BUILDIN_STR_BUF_LEN];
	
	iLenLeft = iLen = lstrlen(szString);
	iLenPat = lstrlen(szPat);
	iLenReplace=lstrlen(szReplace);
	
	if( iLen < iLenPat )
	{
		return iLen;
	}
	
	if( iLenPat < BUILDIN_STR_BUF_LEN )
	{
		szBuf=szQuickBuf;
	}
	else
	{
		if( NULL == ( szBuf = 
			(TCHAR *) LocalAlloc(GPTR, (iLenPat + 1) * sizeof(TCHAR)) )
			) //if
		{
			return iLen;
		}
		isBufAlloc=TRUE;
	}
	
	while(iLenLeft >= iLenPat)
	{
		lstrcpyn(szBuf, pTchar, iLenPat + 1 );
		
		if(bIsCaseSensitive)
		{
			iCmp = lstrcmp(szPat,szBuf);
		}
		else
		{
			iCmp = lstrcmpi(szPat,szBuf);
		}
		if( iCmp == 0 )
		{
			MoveMemory(pTchar+iLenReplace, pTchar+iLenPat, iLenLeft * sizeof(TCHAR) );
			CopyMemory(pTchar, szReplace, iLenReplace * sizeof(TCHAR));
			pTchar+=iLenReplace;
			iLenLeft -= iLenPat;
		}
		else
		{
			pTchar++;
			iLenLeft--;
		}
	}
	
	if(isBufAlloc)
	{
		LocalFree( (HLOCAL)szBuf );
	}
	
	*(pTchar+iLenLeft) = 0;
	
	return lstrlen(szString);
	
}

VOID DeCodeChangeLineChars(LPTSTR szString)
{

	INT iLen=lstrlen(szString)+1;
	LPTSTR lpszBuf;
	TCHAR szCPH_n[3]={0x01, 0x05, 0}; //casting place-holder.
	TCHAR szCPH_r[3]={0x02, 0x06, 0}; //casting place-holder.
	TCHAR szMaxBuf[MAX_STRBUF_LEN];	

	if( iLen > MAX_STRBUF_LEN )
	{
		LPTSTR lpszBuf=(LPTSTR)LocalAlloc(GPTR, (iLen<<2)*sizeof(TCHAR));
	}
	else
	{
		lpszBuf=szMaxBuf;
	}
	if(lpszBuf)
	{
		__try
		{
			lstrcpy(lpszBuf, szString);
			lstrreplace(lpszBuf, TEXT("\\\\r"), szCPH_r);
			lstrreplace(lpszBuf, TEXT("\\\\n"), szCPH_n);
			lstrreplace(lpszBuf, TEXT("\\r"), TEXT("\r"));
			lstrreplace(lpszBuf, TEXT("\\n"), TEXT("\n"));
			lstrreplace(lpszBuf, szCPH_r, TEXT("\\r"));
			lstrreplace(lpszBuf, szCPH_n, TEXT("\\n"));
			lstrcpy(szString, lpszBuf);
		}
		__finally
		{
			if(iLen > MAX_STRBUF_LEN)
			{
				LocalFree((HLOCAL)lpszBuf);
			}
		}
	}
	
}

VOID CodeChangeLineChars(LPTSTR szString)
{
	INT iLen=lstrlen(szString)+1;
	LPTSTR lpszBuf;
	TCHAR szMaxBuf[MAX_STRBUF_LEN];

	if( iLen > MAX_STRBUF_LEN )
	{
		LPTSTR lpszBuf=(LPTSTR)LocalAlloc(GPTR, (iLen<<1)*sizeof(TCHAR));
	}
	else
	{
		lpszBuf=szMaxBuf;
	}

	if(lpszBuf)
	{
		__try
		{
			lstrcpy(lpszBuf, szString);
			lstrreplace(lpszBuf, TEXT("\\r"), TEXT("\\\\r"));
			lstrreplace(lpszBuf, TEXT("\\n"), TEXT("\\\\n"));
			lstrreplace(lpszBuf, TEXT("\r"), TEXT("\\r"));
			lstrreplace(lpszBuf, TEXT("\n"), TEXT("\\n"));
			lstrcpy(szString, lpszBuf);
		}
		__finally
		{
			if(iLen > MAX_STRBUF_LEN)
			{
				LocalFree((HLOCAL)lpszBuf);
			}
		}
	}

}
//////////////////////////////////////////////////////////////////////////

INT AppendString(LPTSTR szString, 
				 LPTSTR szPrefix, LPTSTR szSuffix, 
				 LPTSTR szStrBuf, INT MaxBufLen)
{

#define APPENDSTRING_MAXLEN	 4096

	TCHAR szBuf[APPENDSTRING_MAXLEN];
	LPTSTR lpszBuf=szStrBuf;
	
	if( szStrBuf==NULL )
	{
		lpszBuf=szBuf;
		MaxBufLen=APPENDSTRING_MAXLEN;
	}
	
	lstrcpyn(lpszBuf, szString, MaxBufLen);
	wsprintf(szString, TEXT("%s%s%s"), szPrefix, lpszBuf, szSuffix);
	return lstrlen(szString);
}


//////////////////////////////////////////////////////////////////////


MAP_DA_NODE* InitDataAdrChain(MAP_DA_CHAIN* pMDaChain)
{
	pMDaChain->pTail=pMDaChain->pHead=(MAP_DA_NODE*)
		GlobalAlloc(GPTR, sizeof(MAP_DA_NODE));
	pMDaChain->pHead->pNext=pMDaChain->pTail;
	pMDaChain->pTail->pPre=pMDaChain->pHead;
	pMDaChain->iNodeCount=1;
	return pMDaChain->pHead;
}

VOID UninitDataAdrChain(MAP_DA_CHAIN* pMDaChain)
{
// 	__try
// 	{
		while(pMDaChain->iNodeCount>1)
		{
			DecreaseDataAdrNode(pMDaChain);
		}
		GlobalFree((HGLOBAL)pMDaChain->pHead);
		ZeroMemory(pMDaChain, sizeof(MAP_DA_CHAIN));
//	}
// 	__except(1)
// 	{
// 		//...
// 	}
}

MAP_DA_NODE* IncreaseDataAdrNode(MAP_DA_CHAIN* pMDaChain, MAP_DA_NODE* pMdaNodeData)
{
	pMDaChain->pTail->pNext=(MAP_DA_NODE*)
		GlobalAlloc(GPTR, sizeof(MAP_DA_NODE));
	if(pMDaChain->pTail->pNext)
	{
		CopyMemory(pMDaChain->pTail->pNext, pMdaNodeData, sizeof(MAP_DA_NODE));
		pMDaChain->pTail->pNext->pPre=pMDaChain->pTail;
		pMDaChain->pTail=pMDaChain->pTail->pNext;

		pMDaChain->iNodeCount++;
		return pMDaChain->pTail;
	}
	return NULL;
}

INT DecreaseDataAdrNode(MAP_DA_CHAIN* pMDaChain)
{
	if( pMDaChain->pTail )
	{
		pMDaChain->pTail=pMDaChain->pTail->pPre;
		GlobalFree((HGLOBAL)(pMDaChain->pTail->pNext));
		pMDaChain->pTail->pNext=NULL;
		pMDaChain->iNodeCount--;
	}

	return pMDaChain->iNodeCount;
}

INT MapStringToAdr(MAP_DA_CHAIN* pMDC, LPTSTR szString, LPTSTR szGlobalBuf, 
				   INT iOffset, INT iAdrPos, INT iMaxGloBufLen)
{
	int i,j;
	int iStart;
	int iLen=lstrlen(szString);
	TCHAR szBuf[MAX_STRBUF_LEN]={0};
	TCHAR szNum[MAX_STRLEN_NUM]={0};
	MAP_DA_NODE mdn;
	mdn.isString=TRUE;
	
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

					//MessageBox(NULL, szGlobalBuf+iAdrPos, 0, 0);
					mdn.lpAddress=(LONG_PTR)((INT)(szGlobalBuf+iAdrPos)+iOffset);
					wsprintf(szNum, TEXT(" %X# "), (DWORD)mdn.lpAddress);
					
					lstrcat(szBuf, szNum);
					j+=lstrlen(szNum)-1;
					mdn.uSize=i-iStart+1;
					iAdrPos+=mdn.uSize;
					if(pMDC)
					{
						IncreaseDataAdrNode(pMDC, &mdn);
					}
					break;
				}
				
			}while(szString[i]!='\0');
			
		}
		else
		{
			szBuf[j]=szString[i];
		}
	}
	
	lstrcpyn(szString, szBuf, MAX_STRBUF_LEN);
	return iAdrPos;
}

//////////////////////////////////////////////////////////////////////////

INT MapStructToAdr(MAP_DA_CHAIN* pMDC, LPTSTR szString, 
				   TCHAR chStart, TCHAR chEnd, BOOL isHex, INT iAlign, 
				   BYTE* pGlobalDataBuf, INT iOffset, INT iAdrPos, INT iMaxGloBufSize)
{
	INT i=0;
	TCHAR szTempBuf[MAX_STRBUF_LEN]={0};
	TCHAR szNum[MAX_STRLEN_NUM]={0};
	MAP_DA_NODE mdn;
	mdn.isString=FALSE;
	
	while(szString[i]!='\0')
	{
		if(szString[i]==chStart)
		{
			lstrcpyn(szTempBuf, szString+i+1, MAX_STRBUF_LEN);
			lstrcpy(szString+i, szTempBuf);
			
			iAdrPos=MapStructToAdr(pMDC, szString+i, 
				chStart, chEnd, isHex, iAlign, 
				pGlobalDataBuf, iOffset, iAdrPos, iMaxGloBufSize-iAdrPos);
			
		}
		else if(szString[i]==chEnd)
		{

			mdn.lpAddress=(LONG_PTR)((INT)(pGlobalDataBuf+iAdrPos)+iOffset);
			wsprintf(szNum, TEXT(" %X# "), (DWORD)mdn.lpAddress);
			
			lstrcpyn(szTempBuf, szString, i+1);

			mdn.uSize=NumStrToStruct(szTempBuf, 
				pGlobalDataBuf+iAdrPos, iAlign, isHex, iMaxGloBufSize-iAdrPos);
			
			iAdrPos+=mdn.uSize;

			lstrcpy(szTempBuf, szNum);
			lstrcat(szTempBuf, szString+i+1);
			lstrcpy(szString, szTempBuf);
			
			if(pMDC)
			{
				IncreaseDataAdrNode(pMDC, &mdn);
			}

			break;
		}
		
		i++;
	}
	
	return iAdrPos;
	
}

//////////////////////////////////////////////////////////////////////////

INT NumStrToStruct(LPTSTR szDataStr, PVOID lpData, INT iAlign, BOOL IsHex, INT iMaxByte)
{
	int i;
	int iStart;
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
	
	lstrtrimi(szDataStr, TEXT("0x"));
	
	for(i=0, iStart=0, iDelta=0; i<=iLen && iDelta<=iMaxByte; i++)
	{
		if( szDataStr[i]==' ' || 
			szDataStr[i]=='\t' || 
			szDataStr[i]=='\r' || 
			szDataStr[i]=='\n' ||
			szDataStr[i]==',' ||
			szDataStr[i]=='#' ||
			szDataStr[i]=='\0' )
		{
			
			// if the previous char is in div set, iStart will always == i. 
			// so, only if i> iStart, a new number string is gotten...
			
			if(i>iStart)
			{
				//get num string...
				iWidth=min(i-iStart, MAX_STRLEN_NUM);
				lstrcpyn(szNum, szDataStr+i-iWidth, iWidth+1);
				
				//convert num string into dword num.
				dwBuf=(IsHex||szDataStr[i]=='#') ? 
					HexStrToInt(szNum) : DecStrToInt(szNum);
				
				 //write into data buffer...
				*(DWORD*)((BYTE*)lpData+iDelta)=dwBuf;
				
				// calculate delta, 3 steps...
				// step 1:
				if(szNum[0]=='-')
				{
					iWidth=1;
					dwBuf*=-1;
				}
				else
				{
					iWidth=0;
				}

				//step 2:
				wsprintf(szNum,TEXT("%X"),dwBuf);
				
				if(iWidth==1 && szNum[0]<'8') 
				{
					iWidth=0;
				}
				
				iWidth+=lstrlen(szNum);
				
				if(IsHex)
				{
					iWidth=max(iWidth, i-iStart);
				}

				//step 3:
				if( iWidth>4 || iAlign==4 || szDataStr[i]=='#')
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
			
			iStart=i+1; //update iStart here(important), see if() comment above...
		}
		
	}
	
	return iDelta;
}


VOID NumStrPrintf(LPTSTR szBuf, LPVOID lpData, INT iSize, INT iWidth, INT iAlign, BOOL isHex, BOOL isPrefix)
{
	INT i,j;
	INT iBuf=0;
	TCHAR szNum[MAX_NUMSTR_LEN];
	
#ifndef _DEBUG 
	if(lpData==NULL)
	{
		return;
	}
#endif //!_DEBUG

	szBuf[0]=0; //initialize buffer

	if( iSize<=sizeof(DWORD) )
	{
		CopyMemory(&iBuf, lpData, iSize);

		if(iBuf==0)
		{
			wsprintf(szNum, TEXT("0\0"));
		}
		else if(isHex)
		{
			if(isPrefix)
			{
				lstrcat(szBuf, TEXT("0x"));
			}
			wsprintf(szNum, TEXT("%08X"), iBuf);
		}
		else
		{
			wsprintf(szNum, TEXT("%u"), iBuf);
		}
		lstrcat(szBuf, szNum);
	}
	else
	{
		
		for(i=0, j=0, iBuf=0; i < iSize; i+=iAlign,j++)
		{
			if(isHex)
			{
				if(i>0)
				{
					iBuf=0;
					lstrcat(szBuf, (iWidth==0||j%(iWidth/iAlign))?(TEXT(" ")):TEXT("\r\n"));
				}
				if(isPrefix)
				{
					lstrcat(szBuf, TEXT("0x"));
				}
				
				if(iAlign==4)
				{
					wsprintf(szNum, TEXT("%08X"), *(INT*)((BYTE*)(lpData)+i) );
				}
				else if(iAlign==2)
				{
					wsprintf(szNum, TEXT("%08X"), *(SHORT*)((BYTE*)(lpData)+i));
				}
				else if(iAlign==1)
				{
					wsprintf(szNum, TEXT("%08X"), *(signed char*)((BYTE*)(lpData)+i));
				}
				lstrcat(szBuf, szNum+(8-iAlign*2));
				
			}
			else
			{
				if(i>0)
				{
					iBuf=0;
					lstrcat(szBuf, (iWidth==0||j%(iWidth*3/iAlign))? (TEXT(" ")):TEXT("\r\n"));
				}
				if(iAlign==4)
				{
					wsprintf(szNum, TEXT("%d"), *(DWORD*)((BYTE*)(lpData)+i));
				}
				else if(iAlign==2)
				{
					wsprintf(szNum, TEXT("%d"), *(WORD*)((BYTE*)(lpData)+i));
				}
				else if(iAlign==1)
				{
					wsprintf(szNum, TEXT("%d"), *(BYTE*)((BYTE*)(lpData)+i));
				}
				lstrcat(szBuf, szNum);
			}
			
			iBuf++;
		}
		
	}
	
} //NumStrPrintf()
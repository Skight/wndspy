#if _MSC_VER > 1000
#pragma once
#endif

#ifndef HS_INC_HEADER_STRINGFUNC
#define HS_INC_HEADER_STRINGFUNC

//////////////////////////////////////////////////////////////////////////
#include <tchar.h>

#define MAX_STRLEN_NUM		11
#define MAX_NUMSTR_LEN		MAX_STRLEN_NUM
#define MAX_STRLEN_LONGINT	MAX_STRLEN_NUM
//////////////////////////////////////////////////////////////////////////

#define InitStrBuf(_lpszBuf) _lpszBuf[0]='\0'

#define GetUpperChar(_char) Hs_GetCharCase(_char, TRUE)
#define GetLowerChar(_char) Hs_GetCharCase(_char, FALSE)

#define CharCompare(_char1, _char2, _IsCaseSensitive) \
Hs_CharCmp(_char1, _char2, _IsCaseSensitive)

TCHAR Hs_GetCharCase(TCHAR chTest, BOOL isUpperCase);
BOOL Hs_CharCmp(TCHAR char1, TCHAR char2, BOOL isCaseSen);
BOOL IsInCharSet(TCHAR chTest, TCHAR* szCharSetArray, BOOL IsCaseSen);
BOOL IsCharSetString(LPCTSTR szString, TCHAR* szCharSetArray, BOOL IsCaseSen);

//crude checking...
#define IsNumberStr(_szString)  \
	IsCharSetString(_szString, TEXT("0123456789ABCDEF-X"), FALSE)

//////////////////////////////////////////////////////////////////////////
BOOL IsIntStr(LPTSTR szString, BOOL isHex);

//////////////////////////////////////////////////////////////////////////

/*********************************
In order to maximize use of shlwapi.lib,
the following 2 functions are rewritten to be stubs.
int HexStrToInt(LPTSTR szHexStr);
int DecStrToInt(LPTSTR szDecStr);
To enable original hscommon lib functions:
#define _HS_COMMON_FUN_STR2INT
**********************************/
int HexStrToInt(LPTSTR szHexStr);
int DecStrToInt(LPTSTR szDecStr);

#define GetStringPortion(_szString, _szPortion, _chdelimiter, _IsGetTail) \
Isolation_GetStringPortion(_szString, _szPortion, _chdelimiter, (bool)(_IsGetTail!=0))

INT Isolation_GetStringPortion(LPTSTR szString, 
								 LPTSTR szPortion, 
								 TCHAR chdelimiter,
								 bool IsGetTail);


//////////////////////////////////////////////////////////////////////////

VOID NumStrPrintf(LPTSTR szBuf, LPVOID lpData, INT iSize, INT iWidth, INT iAlign, BOOL isHex, BOOL isPrefix);

INT NumStrToStruct(LPTSTR szDataStr, PVOID lpData, INT iAlign, BOOL IsHex, INT iMaxByte);


typedef struct tag_DataAdrNode
{
	BOOL isString;
	LONG_PTR lpAddress;
	UINT uSize;
	tag_DataAdrNode* pPre;
	tag_DataAdrNode* pNext;

}MAP_DA_NODE;

typedef struct tag_DataAdrChain
{
	MAP_DA_NODE* pHead;
	MAP_DA_NODE* pTail;
	INT iNodeCount;
	INT iOffsetStr;
	INT iOffsetData;

}MAP_DA_CHAIN;


MAP_DA_NODE* InitDataAdrChain(MAP_DA_CHAIN* pMDaChain);
VOID UninitDataAdrChain(MAP_DA_CHAIN* pMDaChain);
MAP_DA_NODE* IncreaseDataAdrNode(MAP_DA_CHAIN* pMDaChain, MAP_DA_NODE* pMdaNodeData);
INT DecreaseDataAdrNode(MAP_DA_CHAIN* pMDaChain);

INT MapStringToAdr(MAP_DA_CHAIN* pMDataChain, LPTSTR szString, LPTSTR szGlobalStringBuf, 
				   INT iOffset, INT iAdrPos, INT iMaxGloBufLen);
INT MapStructToAdr(MAP_DA_CHAIN* pMDataChain, LPTSTR szString, 
				   TCHAR chStart, TCHAR chEnd, BOOL isHex, INT iAlign, 
				   BYTE* pGlobalDataBuf, INT iOffset, INT iAdrPos, INT iMaxGloBufSize);
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
INT MakeSameCharString(LPTSTR szString, TCHAR cChar, INT iRepeat);

#define lstrcatchar(_lpstr, _char, _iRepeat)  \
MakeSameCharString(_lpstr+lstrlen(_lpstr), _char, _iRepeat)

//////////////////////////////////////////////////////////////////////////

/****
Alert: 
To use this function, 
The first argument szString, 
must be !big enough! to hold the result string and the closing '\0', 
otherwise a buffer overrun may occur. 
****/
INT StringPatternReplace(LPTSTR szString_Big_Enough_Buffer, 
						 LPTSTR szPattern, 
						 LPTSTR szReplace, 
						 BOOL bIsCaseSensitive);

/*_szBigEnoughStringBuf must be big enough space to hold result string */
#define lstrreplace(_szBigEnoughStringBuf, _szPattern, _szReplace) \
StringPatternReplace(_szBigEnoughStringBuf, _szPattern, _szReplace, TRUE)
/*_szBigEnoughStringBuf must be big enough space to hold result string */
#define lstrreplacei(_szBigEnoughStringBuf, _szPattern, _szReplace) \
StringPatternReplace(_szBigEnoughStringBuf, _szPattern, _szReplace, FALSE)


#define StringTrimPattern(_szBigEnoughStringBuf, _szPattern, _IsCaseSensitive) \
	StringPatternReplace(_szBigEnoughStringBuf, _szPattern, TEXT(""), _IsCaseSensitive)

#define lstrtrim(_szString, _szPattern)  \
StringTrimPattern(_szString, _szPattern, TRUE)

#define lstrtrimi(_szString, _szPattern)  \
StringTrimPattern(_szString, _szPattern, FALSE)

/***
Max Len of the string buf built in function is 4096 bytes...
If you wanna larger buffer, you can set Param4: szStrBuf and Param5:MaxBufLen...
***/
INT AppendString(LPTSTR szString, 
			 LPTSTR szPrefix, LPTSTR szSuffix, 
			 LPTSTR szStrBuf, INT MaxBufLen);


VOID DeCodeChangeLineChars(LPTSTR szString);
VOID CodeChangeLineChars(LPTSTR szString);
//////////////////////////////////////////////////////////////////////////
#endif //#define HS_INC_HEADER_STRINGFUNC
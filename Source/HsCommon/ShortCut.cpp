#include <shobjidl.h>
#include <objidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <objbase.h>

#include "shortcut.h"

HRESULT CreateShortcut(BOOL immediately, 
					   LPCTSTR lpszPathLink, 
					   LPCTSTR lpszPathObj, 
					   LPCTSTR lpszArgments, 
					   LPCTSTR lpszCurDir, 
					   INT iShowCmd, 
					   WORD pwHotKey,
					   LPCTSTR lpszIcon, INT iIconIndex, 
					   LPCTSTR lpszDesc) 
					  
{ 
    HRESULT hres;
    IShellLink* psl; 
	
	if(immediately)
	{
		CoInitialize(NULL);
	}

    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target and add the description. 
        psl->SetPath(lpszPathObj);
		if(lpszCurDir)
		psl->SetWorkingDirectory(lpszCurDir);
		if(lpszArgments)
		psl->SetArguments(lpszArgments);
		if(iShowCmd)
        psl->SetShowCmd(iShowCmd);
		if(pwHotKey)
        psl->SetHotkey(pwHotKey);
		if(lpszIcon)
		psl->SetIconLocation(lpszIcon, iIconIndex);
		if(lpszDesc)
        psl->SetDescription(lpszDesc);
 
        // Query IShellLink for the IPersistFile interface for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
 
        if (SUCCEEDED(hres)) 
        { 

#ifndef UNICODE
            WCHAR wsz[MAX_PATH]; 

            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
			
            // TODO: Check return value from MultiByteWideChar to ensure 
             //        success.
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE); 
#else
			hres = ppf->Save(lpszPathLink, TRUE); 
#endif //!UNICODE

            ppf->Release(); 
        } 
        psl->Release(); 
    } 

	if(immediately)
	{
		CoUninitialize();
	}

    return hres; 
}


//////////////////////////////////////////////////////////////////////////
HRESULT ResolveShortcut(BOOL immediately,
						HWND hwnd, 
						LPCTSTR lpszLinkFile, 
						LPTSTR lpszPath, 
						int iPathBufferSize) 
{ 

    HRESULT hres;
	IShellLink* psl; 
    TCHAR szGotPath[MAX_PATH]; 
    TCHAR szDescription[MAX_PATH]; 
    WIN32_FIND_DATA wfd; 

	if(immediately)
	{
		CoInitialize(NULL);
	}

    *lpszPath = 0; // assume failure 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
        // Get a pointer to the IPersistFile interface. 
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
		
        if (SUCCEEDED(hres)) 
        {

#ifndef UNICODE
            WCHAR wsz[MAX_PATH]; 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH); 
            // TODO: Check return value from MultiByteWideChar to ensure success.
 
            // Load the shortcut. 
            hres = ppf->Load(wsz, STGM_READ); 
#else      
			hres = ppf->Load(lpszLinkFile, STGM_READ); 
#endif //!UNICODE
			
            if (SUCCEEDED(hres)) 
            { 
                // Resolve the link. 
                hres = psl->Resolve(hwnd, 0); 

                if (SUCCEEDED(hres)) 
                { 
                    // Get the path to the link target. 
                    hres = psl->GetPath(szGotPath, 
                                        MAX_PATH, 
                                        (WIN32_FIND_DATA*)&wfd, 
                                        SLGP_SHORTPATH); 

                    if (SUCCEEDED(hres)) 
                    { 
                        // Get the description of the target. 
                        hres = psl->GetDescription(szDescription, MAX_PATH); 

                        if (SUCCEEDED(hres)) 
                        {
                            if (GetLongPathName(szGotPath,lpszPath,MAX_PATH) <=0)
                            {
                                lstrcpyn(lpszPath, lpszLinkFile, iPathBufferSize);
                            }

                        }
                    }
                } 
            } 

            // Release the pointer to the IPersistFile interface. 
            ppf->Release(); 
        } 

        // Release the pointer to the IShellLink interface. 
        psl->Release(); 
    } 

	if(immediately)
	{
		CoUninitialize();
	}
	
    return hres; 
}


//////////////////////////////////////////////////////////////////////////

HRESULT GetShellFolderViewDual(ITEMIDLIST* pidl, IShellFolderViewDual** ppIShellFolderViewDual)
{
	HRESULT hr;
	IWebBrowserApp* pIWebBrowserApp;
	IDispatch* pDoc;
	HWND hwnd;

	HINSTANCE hShdocvw;
	HRESULT (WINAPI*lpfSHGetIDispatchForFolder)
		(ITEMIDLIST* pidl, IWebBrowserApp** ppIWebBrowserApp);
	
	*ppIShellFolderViewDual = NULL;
	pIWebBrowserApp=NULL;

	hShdocvw = LoadLibrary(TEXT("SHDOCVW.DLL"));
	if (hShdocvw == NULL)
	return FALSE;

	lpfSHGetIDispatchForFolder = (HRESULT (WINAPI*)(ITEMIDLIST*, IWebBrowserApp**)) 
		GetProcAddress(hShdocvw, "SHGetIDispatchForFolder");

	if (lpfSHGetIDispatchForFolder == NULL)
	{
		FreeLibrary(hShdocvw);
		return FALSE;
	}

	if (SUCCEEDED(lpfSHGetIDispatchForFolder(pidl, &pIWebBrowserApp))) 
	{
		if (SUCCEEDED(pIWebBrowserApp->get_HWND((long*)&hwnd))) 
		{
			SetForegroundWindow(hwnd);
			ShowWindow(hwnd, SW_SHOWNORMAL);
		}

		if (SUCCEEDED(hr = pIWebBrowserApp->get_Document(&pDoc)))
		{
			pDoc->QueryInterface(IID_IShellFolderViewDual, (void**) ppIShellFolderViewDual);
			pDoc->Release();
		}
		pIWebBrowserApp->Release();
	}

	FreeLibrary(hShdocvw);
	return TRUE;
}



ITEMIDLIST* GetFilePidl(ITEMIDLIST* pidl)
{
	ITEMIDLIST* pIdlFile = pidl;
    USHORT cb;
	if (pIdlFile) 
	{
		while (cb = pidl->mkid.cb) 
		{
			pIdlFile = pidl;
			pidl = (ITEMIDLIST*)((BYTE*)pidl + cb);
		}
	}
	return pIdlFile;
}

int GetPidlSize(ITEMIDLIST* pidl)
{
	int sz = 0;
	USHORT cb;

	if (pidl) 
	{
		sz = 2;
		while (cb = pidl->mkid.cb) 
		{
			sz += cb;
			pidl = (ITEMIDLIST*)((BYTE*)pidl + cb);
		}
	}
	
	return sz;
}


SAFEARRAY* AllocSafeArrayForPidl(HINSTANCE hOleAut32, ITEMIDLIST* pidl, int sz)
{

	SAFEARRAY* (WINAPI* lfpSafeArrayCreateVector)(VARTYPE, LONG, UINT);
	SAFEARRAY* parray;
	lfpSafeArrayCreateVector=(SAFEARRAY* (WINAPI*)(VARTYPE, LONG, UINT))
		GetProcAddress(hOleAut32, "SafeArrayCreateVector");

	if (lfpSafeArrayCreateVector && 
		pidl && sz) 
	{
		parray = lfpSafeArrayCreateVector(VT_UI1, 0, sz);
		if (parray)
			memcpy(parray->pvData, pidl, sz);
		return parray;
	}

	return NULL;
}

BOOL SetVariantArg(HINSTANCE hOleAut32, VARIANTARG* pVariantArg, ITEMIDLIST* pidl)
{
	SAFEARRAY* parray=AllocSafeArrayForPidl(hOleAut32, pidl, GetPidlSize(pidl));
	
	if ( parray !=NULL ) 
	{
		ZeroMemory(pVariantArg, sizeof(VARIANTARG));
		pVariantArg->vt = VT_ARRAY|VT_UI1;
		pVariantArg->parray = parray;
		return TRUE;
	}
	
	return FALSE;
}

HRESULT ClearVariantArg(HINSTANCE hOleAut32, VARIANTARG* pVariantArg)
{
	
	HRESULT ret=E_FAIL;
	HRESULT (WINAPI* lpfVariantClear)(VARIANTARG *);

	if (hOleAut32) 
	{
		lpfVariantClear=(HRESULT (WINAPI*)(VARIANTARG *))
			GetProcAddress(hOleAut32, "VariantClear");

		if (lpfVariantClear)
			ret = lpfVariantClear(pVariantArg);
	}

	return E_FAIL;
}

HRESULT SelectItem(IShellFolderViewDual* pIShellFolderViewDual, ITEMIDLIST* pidl, int dwFlags)
{
	HRESULT hr = E_FAIL;
	HINSTANCE hOleAut32=LoadLibrary(TEXT("OLEAUT32.DLL"));
	VARIANTARG VariantArg;
	
	if(hOleAut32)
	{
		if (SetVariantArg(hOleAut32, &VariantArg, pidl)) {
			hr = pIShellFolderViewDual->SelectItem(&VariantArg, dwFlags);
			ClearVariantArg(hOleAut32, &VariantArg);
		}
		
		FreeLibrary(hOleAut32);
	}
	return hr;
}

BOOL UD_SHOpenFolderAndSelectItems(ITEMIDLIST *pidlTarget)
{
	ITEMIDLIST *pidlFile, *pidlFolder;
	ITEMIDLIST *pIdlTemp;
	USHORT cb;
	IShellFolderViewDual* pIShellFolderViewDual;

	HRESULT (WINAPI *lpfSHOpenFolderAndSelectItems) 
		(LPCITEMIDLIST*, UINT, LPCITEMIDLIST*, DWORD);

	HINSTANCE hShell32;
	hShell32 = LoadLibrary(TEXT("SHELL32.DLL"));
	if (hShell32 == NULL)
		return FALSE;
	
	lpfSHOpenFolderAndSelectItems = 
		(HRESULT (WINAPI*)(LPCITEMIDLIST*, UINT, LPCITEMIDLIST*, DWORD)) 
		GetProcAddress(hShell32, "SHOpenFolderAndSelectItems");
	if (lpfSHOpenFolderAndSelectItems != NULL)
	{
		//on WinXP or later Platform...
		if (SUCCEEDED(lpfSHOpenFolderAndSelectItems((LPCITEMIDLIST*)pidlTarget,0,(LPCITEMIDLIST*)NULL,0))) 
		{
			FreeLibrary(hShell32);
			return TRUE;
		}
		FreeLibrary(hShell32);
		return FALSE;
	}

	FreeLibrary(hShell32);
	///on Win2k or earlier Platform...

	//Get File Item  ITEMIDLIST from Target ITEMIDLIST
	pidlFolder=pidlTarget;
	pIdlTemp=pidlFolder;
	while (cb = pIdlTemp->mkid.cb) 
	{
		pidlFile = pIdlTemp;
		pIdlTemp = (ITEMIDLIST*)((BYTE*)pIdlTemp + cb);
	}
	//save sb...
	cb = pidlFile->mkid.cb;
	pidlFile->mkid.cb = 0; 
	/// Open Folder First...
	if ( SUCCEEDED( GetShellFolderViewDual(pidlFolder, &pIShellFolderViewDual) ) ) 
	{
		pidlFile->mkid.cb = cb;
		//select Target Item(s)...
		if(pIShellFolderViewDual != NULL)
		{
			// 0x00 Deselect the item.  
			// 0x01 Select the item.  
			// 0x03 Put the item in edit mode.  
			// 0x04 Deselect all but the specified item.  
			// 0x08 Ensure the item is displayed in the view.  
			// 0x10 Give the item the focus.
			SelectItem(pIShellFolderViewDual, pidlFile, 0x1D/*0x10|0x08|0x4|0x1*/);
			pIShellFolderViewDual->Release();
		}
		return TRUE;
	}
	
	return FALSE;
}

void DoLocateFileEx(LPTSTR szTarget)
{
	HRESULT hres; 
	IShellLink *psl; 
	ITEMIDLIST *pidl;
	IPersistFile *ppf; 
	
	CoInitialize(NULL);

	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres)) 
	{ 
		psl->SetPath(szTarget);
		psl->GetIDList(&pidl);
		
		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
		if (SUCCEEDED(hres)) 
		{ 
			WCHAR wsz[MAX_PATH]; 
#ifdef UNICODE
			lstrcpyW(wsz, szTarget);
#else
			// Ensure that the string is Unicode. 
			MultiByteToWideChar(CP_ACP, 0, szTarget, -1, wsz, MAX_PATH); 
#endif//UNICODE
			
			// Load the shortcut. 
			hres = ppf->Load(wsz, STGM_READ); 
			
			if (SUCCEEDED(hres)) 
			{ 
				/// Get ITEMIDLIST from shortcut
				psl->GetIDList(&pidl);
			}
			ppf->Release();
		}
		
		UD_SHOpenFolderAndSelectItems(pidl);
		
		psl->Release();
	}

	CoUninitialize();
}


//////////////////////////////////////////////////////////////////////////


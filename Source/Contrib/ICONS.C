/****************************************************************************\
*            
*     FILE:     ICONS.C
*
*     PURPOSE:  IconPro Project Icon handing Code C file
*
*     COMMENTS: This file contains the icon handling code
*
*     FUNCTIONS:
*      EXPORTS: 
*               ReadIconFromICOFile        - Reads Icon from ICO file
*               WriteIconToICOFile         - Writes Icon to ICO file
*               MakeIconFromResource       - Makes HICON from a resource
*               ReadIconFromEXEFile        - Reads Icon from a EXE or DLL file
*               IconImageToClipBoard       - Puts icon image on clipboard
*               IconImageFromClipBoard     - Gets icon image from clipboard
*               CreateBlankNewFormatIcon   - Makes a new, blank icon image
*               DrawXORMask                - Draws XOR mask using DIBs
*               DrawANDMask                - Draws AND mask using DIBs
*               GetXORImageRect            - Calculates XOR image position
*               MakeNewANDMaskBasedOnPoint - Calculates new AND mask
*               ConvertBMPFileToIcon       - Converts BMP to Icon
*               IconImageToBMPFile         - Writes an icon image to BMP file
*      LOCALS:
*               ReadICOHeader              - Reads ICO file header
*               AdjustIconImagePointers    - Adjusts internal pointers
*               GetIconFromInstance        - Extracts Icon from Instance
*               WriteICOHeader             - Writes ICO file header
*               CalculateImageOffset       - Calcs offset in file of image
*               DIBToIconImage             - Converts DIB to icon image
*
*     Copyright 1995 Microsoft Corp.
*
*
* History:
*                July '95 - Created
*
\****************************************************************************/
#include <Windows.h>
#include "Icons.H"
#include "Dib.H"


/****************************************************************************/
// Structs used locally (file scope)
// Resource Position info - size and offset of a resource in a file
typedef struct
{
    DWORD	dwBytes;
    DWORD	dwOffset;
} RESOURCEPOSINFO, *LPRESOURCEPOSINFO;
// EXE/DLL icon information - filename, instance handle and ID
typedef struct
{
    LPCTSTR    	szFileName;
    HINSTANCE	hInstance;
    LPTSTR    	lpID;
} EXEDLLICONINFO, *LPEXEDLLICONINFO;
/****************************************************************************/


/****************************************************************************/
// Prototypes for local functions
UINT ReadICOHeader( HANDLE hFile );
BOOL AdjustIconImagePointers( LPICONIMAGE lpImage );
HICON GetIconFromInstance( HINSTANCE hInstance, LPTSTR nIndex );
BOOL WriteICOHeader( HANDLE hFile, UINT nNumEntries );
DWORD CalculateImageOffset( LPICONRESOURCE lpIR, UINT nIndex );
BOOL DIBToIconImage( LPICONIMAGE lpii, LPBYTE lpDIB, BOOL bStretch );
/****************************************************************************/

/****************************************************************************
*
*     FUNCTION: DoFreeIconResource
*
*     PURPOSE:  Free a ICONRESOURCE memory alloced by readxx functions.
*
*     PARAMS:   LPICONRESOURCE lpIR - pointer to ICONRESOURCE memory
*
*     RETURNS:  this is a routine with no return value...
*
* History:
*                2008.7.29 - Skight Created
*
\****************************************************************************/

void DoFreeIconResource(LPICONRESOURCE lpIR)
{
	if(lpIR)
	{
		UINT i;
		
		// Free all the bits
		for( i=0; i< lpIR->nNumImages; i++ )
		{
			if( lpIR->IconImages[i].lpBits != NULL )
			{
				free( lpIR->IconImages[i].lpBits );
			}
			
		}
		free( lpIR );
	}
}
/****************************************************************************
*
*     FUNCTION: MakeIconFromResource
*
*     PURPOSE:  Makes an HICON from an icon resource
*
*     PARAMS:   LPICONIMAGE	lpIcon - pointer to the icon resource
*
*     RETURNS:  HICON - handle to the new icon, NULL for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
HICON MakeIconFromResource( LPICONIMAGE lpIcon )
{
    HICON        	hIcon = NULL;

    // Sanity Check
    if( lpIcon == NULL )
        return NULL;
    if( lpIcon->lpBits == NULL )
        return NULL;
    // Let the OS do the real work :)
    hIcon = CreateIconFromResourceEx( lpIcon->lpBits, lpIcon->dwNumBytes, TRUE, 0x00030000, 
            (*(LPBITMAPINFOHEADER)(lpIcon->lpBits)).biWidth, (*(LPBITMAPINFOHEADER)(lpIcon->lpBits)).biHeight/2, 0 );
    
    // It failed, odds are good we're on NT so try the non-Ex way
    if( hIcon == NULL )
    {
        // We would break on NT if we try with a 16bpp image
        if(lpIcon->lpbi->bmiHeader.biBitCount != 16)
        {	
            hIcon = CreateIconFromResource( lpIcon->lpBits, lpIcon->dwNumBytes, TRUE, 0x00030000 );
        }
    }
    return hIcon;
}
/* End MakeIconFromResource() **********************************************/





/****************************************************************************
*
*     FUNCTION: ReadIconFromICOFile
*
*     PURPOSE:  Reads an Icon Resource from an ICO file
*
*     PARAMS:   LPCTSTR szFileName - Name of the ICO file
*
*     RETURNS:  LPICONRESOURCE - pointer to the resource, NULL for failure
*
* History:
*                July '95 - Created
*
* Revision:		
				2008.7 - by Skight
\****************************************************************************/
LPICONRESOURCE ReadIconFromICOFile( LPCTSTR szFileName )
{
    LPICONRESOURCE    	lpIR = NULL, lpNew = NULL;
    HANDLE            	hFile = NULL;
    LPRESOURCEPOSINFO	lpRPI = NULL;
    UINT                i;
    DWORD            	dwBytesRead;
    LPICONDIRENTRY    	lpIDE = NULL;


    // Open the file
    if( (hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Opening File for Reading"), szFileName, MB_OK );
#endif
        return NULL;
    }
    // Allocate memory for the resource structure
    if( (lpIR = (LPICONRESOURCE)malloc( sizeof(ICONRESOURCE) )) == NULL )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Allocating Memory"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        return NULL;
    }
    // Read in the header
    if( (lpIR->nNumImages = ReadICOHeader( hFile )) == (UINT)-1 ||
		lpIR->nNumImages==0 )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Reading File Header"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    // Adjust the size of the struct to account for the images
    if( (lpNew = (LPICONRESOURCE)realloc( lpIR, sizeof(ICONRESOURCE) + ((lpIR->nNumImages-1) * sizeof(ICONIMAGE)) )) == NULL )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Allocating Memory"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    lpIR = lpNew;
    // Store the original name
    lstrcpy( lpIR->szOriginalICOFileName, szFileName );
    lstrcpy( lpIR->szOriginalDLLFileName, TEXT("") );
    // Allocate enough memory for the icon directory entries
    if( (lpIDE = (LPICONDIRENTRY)malloc( lpIR->nNumImages * sizeof( ICONDIRENTRY ) ) ) == NULL )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Allocating Memory"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    // Read in the icon directory entries
    if( ! ReadFile( hFile, lpIDE, lpIR->nNumImages * sizeof( ICONDIRENTRY ), &dwBytesRead, NULL ) )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Reading File"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    if( dwBytesRead != lpIR->nNumImages * sizeof( ICONDIRENTRY ) )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Reading File"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        free( lpIR );
        return NULL;
    }
    // Loop through and read in each image
    for( i = 0; i < lpIR->nNumImages; i++ )
    {
        // Allocate memory for the resource
        if( (lpIR->IconImages[i].lpBits = (LPBYTE)malloc(lpIDE[i].dwBytesInRes)) == NULL )
        {
#ifdef _DEBUG
            MessageBox( NULL, TEXT("Error Allocating Memory"), szFileName, MB_OK );
#endif
            CloseHandle( hFile );
            free( lpIR );
            free( lpIDE );
            return NULL;
        }
        lpIR->IconImages[i].dwNumBytes = lpIDE[i].dwBytesInRes;
        // Seek to beginning of this image
        if( SetFilePointer( hFile, lpIDE[i].dwImageOffset, NULL, FILE_BEGIN ) == 0xFFFFFFFF )
        {
#ifdef _DEBUG
            MessageBox( NULL, TEXT("Error Seeking in File"), szFileName, MB_OK );
#endif
            CloseHandle( hFile );
            free( lpIR );
            free( lpIDE );
            return NULL;
        }
        // Read it in
        if( ! ReadFile( hFile, lpIR->IconImages[i].lpBits, lpIDE[i].dwBytesInRes, &dwBytesRead, NULL ) )
        {
#ifdef _DEBUG
            MessageBox( NULL, TEXT("Error Reading File"), szFileName, MB_OK );
#endif
            CloseHandle( hFile );
            free( lpIR );
            free( lpIDE );
            return NULL;
        }
        if( dwBytesRead != lpIDE[i].dwBytesInRes )
        {
#ifdef _DEBUG
            MessageBox( NULL, TEXT("Error Reading File"), szFileName, MB_OK );
#endif
            CloseHandle( hFile );
            free( lpIDE );
            free( lpIR );
            return NULL;
        }
        // Set the internal pointers appropriately
        if( ! AdjustIconImagePointers( &(lpIR->IconImages[i]) ) )
        {
#ifdef _DEBUG
            MessageBox( NULL, TEXT("Error Converting to Internal Format"), szFileName, MB_OK );
#endif
            CloseHandle( hFile );
            free( lpIDE );
            free( lpIR );
            return NULL;
        }
    }
    // Clean up	
    free( lpIDE );
    free( lpRPI );
    CloseHandle( hFile );
    return lpIR;
}
/* End ReadIconFromICOFile() **********************************************/




/****************************************************************************
*
*     FUNCTION: AdjustIconImagePointers
*
*     PURPOSE:  Adjusts internal pointers in icon resource struct
*
*     PARAMS:   LPICONIMAGE lpImage - the resource to handle
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL AdjustIconImagePointers( LPICONIMAGE lpImage )
{
    // Sanity check
    if( lpImage==NULL )
        return FALSE;
    // BITMAPINFO is at beginning of bits
    lpImage->lpbi = (LPBITMAPINFO)lpImage->lpBits;
    // Width - simple enough
    lpImage->Width = lpImage->lpbi->bmiHeader.biWidth;
    // Icons are stored in funky format where height is doubled - account for it
    lpImage->Height = (lpImage->lpbi->bmiHeader.biHeight)/2;
    // How many colors?
    lpImage->Colors = lpImage->lpbi->bmiHeader.biPlanes * lpImage->lpbi->bmiHeader.biBitCount;
    // XOR bits follow the header and color table
    lpImage->lpXOR = (LPBYTE)FindDIBBits((LPSTR)lpImage->lpbi);
    // AND bits follow the XOR bits
    lpImage->lpAND = lpImage->lpXOR + (lpImage->Height*BytesPerLine((LPBITMAPINFOHEADER)(lpImage->lpbi)));
    return TRUE;
}
/* End AdjustIconImagePointers() *******************************************/




/****************************************************************************
*
*     FUNCTION: ReadICOHeader
*
*     PURPOSE:  Reads the header from an ICO file
*
*     PARAMS:   HANDLE hFile - handle to the file
*
*     RETURNS:  UINT - Number of images in file, -1 for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
UINT ReadICOHeader( HANDLE hFile )
{
    WORD    Input;
    DWORD	dwBytesRead;

    // Read the 'reserved' WORD
    if( ! ReadFile( hFile, &Input, sizeof( WORD ), &dwBytesRead, NULL ) )
        return (UINT)-1;
    // Did we get a WORD?
    if( dwBytesRead != sizeof( WORD ) )
        return (UINT)-1;
    // Was it 'reserved' ?   (ie 0)
    if( Input != 0 )
        return (UINT)-1;
    // Read the type WORD
    if( ! ReadFile( hFile, &Input, sizeof( WORD ), &dwBytesRead, NULL ) )
        return (UINT)-1;
    // Did we get a WORD?
    if( dwBytesRead != sizeof( WORD ) )
        return (UINT)-1;
    // Was it type 1?
    if( Input != 1 )
        return (UINT)-1;
    // Get the count of images
    if( ! ReadFile( hFile, &Input, sizeof( WORD ), &dwBytesRead, NULL ) )
        return (UINT)-1;
    // Did we get a WORD?
    if( dwBytesRead != sizeof( WORD ) )
        return (UINT)-1;
    // Return the count
    return Input;
}
/* End ReadICOHeader() ****************************************************/



/****************************************************************************
*
*     FUNCTION: GetIconFromInstance
*
*     PURPOSE:  Callback for enumerating resources in a DLL/EXE
*
*     PARAMS:   HINSTANCE hInstance - Instance handle for this module
*               LPTSTR    nIndex    - Resource index
*
*     RETURNS:  HICON - Handle to the icon, NULL for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
HICON GetIconFromInstance( HINSTANCE hInstance, LPTSTR nIndex )
{
    HICON	hIcon = NULL;
    HRSRC	hRsrc = NULL;
    HGLOBAL	hGlobal = NULL;
    LPVOID	lpRes = NULL;
    int    	nID;

    // Find the group icon
    if( (hRsrc = FindResource( hInstance, nIndex, RT_GROUP_ICON )) == NULL )
        return NULL;
    if( (hGlobal = LoadResource( hInstance, hRsrc )) == NULL )
        return NULL;
    if( (lpRes = LockResource(hGlobal)) == NULL )
        return NULL;

    // Find this particular image
    nID = LookupIconIdFromDirectory( (LPBYTE)lpRes, TRUE );
    if( (hRsrc = FindResource( hInstance, MAKEINTRESOURCE(nID), RT_ICON )) == NULL )
        return NULL;
    if( (hGlobal = LoadResource( hInstance, hRsrc )) == NULL )
        return NULL;
    if( (lpRes = LockResource(hGlobal)) == NULL )
        return NULL;
    // Let the OS make us an icon
    hIcon = CreateIconFromResource((LPBYTE)lpRes, SizeofResource(hInstance,hRsrc), TRUE, 0x00030000 );
    return hIcon;
}
/* End GetIconFromInstance() ***********************************************/


/****************************************************************************
*
*     FUNCTION: ReadIconFromPEFile
*
*     PURPOSE:  Load an Icon Resource from a DLL/EXE file
*
*     PARAMS:   LPCTSTR szFileName - name of DLL/EXE file
*
*     RETURNS:  LPICONRESOURCE - pointer to icon resource
*
* History:
*                July '95 - Created
* Revision:		
				2008.7 - by Skight
\****************************************************************************/
LPICONRESOURCE ReadIconFromPEFile( LPCTSTR szFileName , LPTSTR lpID)
{
    LPICONRESOURCE    	lpIR = NULL, lpNew = NULL;
    HINSTANCE        	hLibrary;
    EXEDLLICONINFO    	EDII;

    // Load the DLL/EXE - NOTE: must be a 32bit EXE/DLL for this to work
    if( (hLibrary = LoadLibraryEx( szFileName, NULL, LOAD_LIBRARY_AS_DATAFILE )) == NULL )
    {
        // Failed to load - abort
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Loading File - Choose a 32bit DLL or EXE"), szFileName, MB_OK );
#endif
        return NULL;
    }
    // Store the info
    EDII.szFileName = szFileName;
    EDII.hInstance = hLibrary;
    // "Which Icon?"
    if(lpID != NULL )
    {
        HRSRC        	hRsrc = NULL;
        HGLOBAL        	hGlobal = NULL;
        LPMEMICONDIR    lpIcon = NULL;
        UINT            i;

        // Find the group icon resource
        if( (hRsrc = FindResource( hLibrary, lpID, RT_GROUP_ICON )) == NULL )
        {
            FreeLibrary( hLibrary );
            return NULL;
        }
        if( (hGlobal = LoadResource( hLibrary, hRsrc )) == NULL )
        {
            FreeLibrary( hLibrary );
            return NULL;
        }
        if( (lpIcon = (LPMEMICONDIR)LockResource(hGlobal)) == NULL )
        {
            FreeLibrary( hLibrary );
            return NULL;
        }
		
		if( lpIcon->idCount<=0 )
		{
            FreeLibrary( hLibrary );
            return NULL;
		}
        // Allocate enough memory for the images
        if( (lpIR = (LPICONRESOURCE)malloc( sizeof(ICONRESOURCE) + ((lpIcon->idCount-1) * sizeof(ICONIMAGE)) )) == NULL )
        {
#ifdef _DEBUG
            MessageBox( NULL, TEXT("Error Allocating Memory"), szFileName, MB_OK );
#endif
            FreeLibrary( hLibrary );
            return NULL;
        }
        // Fill in local struct members
        lpIR->nNumImages = lpIcon->idCount;
        lstrcpy( lpIR->szOriginalDLLFileName, szFileName );
        lstrcpy( lpIR->szOriginalICOFileName, TEXT("") );
        // Loop through the images
        for( i = 0; i < lpIR->nNumImages; i++ )
        {
            // Get the individual image
            if( (hRsrc = FindResource( hLibrary, MAKEINTRESOURCE(lpIcon->idEntries[i].nID), RT_ICON )) == NULL )
            {
                free( lpIR );
                FreeLibrary( hLibrary );
                return NULL;
            }
            if( (hGlobal = LoadResource( hLibrary, hRsrc )) == NULL )
            {
                free( lpIR );
                FreeLibrary( hLibrary );
                return NULL;
            }
            // Store a copy of the resource locally
            lpIR->IconImages[i].dwNumBytes = SizeofResource( hLibrary, hRsrc );
            lpIR->IconImages[i].lpBits = (LPBYTE)malloc( lpIR->IconImages[i].dwNumBytes );
            memcpy( lpIR->IconImages[i].lpBits, LockResource( hGlobal ), lpIR->IconImages[i].dwNumBytes );
            // Adjust internal pointers
            if( ! AdjustIconImagePointers( &(lpIR->IconImages[i]) ) )
            {
#ifdef _DEBUG
                MessageBox( NULL, TEXT("Error Converting to Internal Format"), szFileName, MB_OK );
#endif
                free( lpIR );
                FreeLibrary( hLibrary );
                return NULL;
            }
        }
    }
    FreeLibrary( hLibrary );
    return lpIR;
}
/* End ReadIconFromEXEFile() ************************************************/



/****************************************************************************
*
*     FUNCTION: WriteICOHeader
*
*     PURPOSE:  Writes the header to an ICO file
*
*     PARAMS:   HANDLE hFile       - handle to the file
*               UINT   nNumEntries - Number of images in file
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL WriteICOHeader( HANDLE hFile, UINT nNumEntries )
{
    WORD    Output;
    DWORD	dwBytesWritten;

    // Write 'reserved' WORD
    Output = 0;
    if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
        return FALSE;
    // Did we write a WORD?
    if( dwBytesWritten != sizeof( WORD ) )
        return FALSE;
    // Write 'type' WORD (1)
    Output = 1;
    if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
        return FALSE;
    // Did we write a WORD?
    if( dwBytesWritten != sizeof( WORD ) )
        return FALSE;
    // Write Number of Entries
    Output = (WORD)nNumEntries;
    if( ! WriteFile( hFile, &Output, sizeof( WORD ), &dwBytesWritten, NULL ) )
        return FALSE;
    // Did we write a WORD?
    if( dwBytesWritten != sizeof( WORD ) )
        return FALSE;
    return TRUE;
}
/* End WriteICOHeader() ****************************************************/



/****************************************************************************
*
*     FUNCTION: CalculateImageOffset
*
*     PURPOSE:  Calculates the file offset for an icon image
*
*     PARAMS:   LPICONRESOURCE lpIR   - pointer to icon resource
*               UINT           nIndex - which image?
*
*     RETURNS:  DWORD - the file offset for that image
*
* History:
*                July '95 - Created
*
\****************************************************************************/
DWORD CalculateImageOffset( LPICONRESOURCE lpIR, UINT nIndex )
{
    DWORD	dwSize;
    UINT    i;

    // Calculate the ICO header size
    dwSize = 3 * sizeof(WORD);
    // Add the ICONDIRENTRY's
    dwSize += lpIR->nNumImages * sizeof(ICONDIRENTRY);
    // Add the sizes of the previous images
    for(i=0;i<nIndex;i++)
        dwSize += lpIR->IconImages[i].dwNumBytes;
    // we're there - return the number
    return dwSize;
}
/* End CalculateImageOffset() ***********************************************/




/****************************************************************************
*
*     FUNCTION: WriteIconToICOFile
*
*     PURPOSE:  Writes the icon resource data to an ICO file
*
*     PARAMS:   LPICONRESOURCE lpIR       - pointer to icon resource
*               LPCTSTR        szFileName - name for the ICO file
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL WriteIconToICOFile( LPICONRESOURCE lpIR, LPCTSTR szFileName )
{
    HANDLE    	hFile;
    UINT        i;
    DWORD    	dwBytesWritten;

    // open the file
    if( (hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Opening File for Writing"), szFileName, MB_OK );
#endif
        return FALSE;
    }
    // Write the header
    if( ! WriteICOHeader( hFile, lpIR->nNumImages ) )
    {
#ifdef _DEBUG
        MessageBox( NULL, TEXT("Error Writing ICO File"), szFileName, MB_OK );
#endif
        CloseHandle( hFile );
        return FALSE;
    }
    // Write the ICONDIRENTRY's
	if(lpIR->nNumImages<1)
	{
		CloseHandle( hFile );
		DeleteFile(szFileName);
		return FALSE;
	}
    for( i=0; i<lpIR->nNumImages; i++ )
    {
        ICONDIRENTRY    ide;

        // Convert internal format to ICONDIRENTRY
        ide.bWidth = lpIR->IconImages[i].Width;
        ide.bHeight = lpIR->IconImages[i].Height;
        ide.bReserved = 0;
        ide.wPlanes = lpIR->IconImages[i].lpbi->bmiHeader.biPlanes;
        ide.wBitCount = lpIR->IconImages[i].lpbi->bmiHeader.biBitCount;
        if( (ide.wPlanes * ide.wBitCount) >= 8 )
            ide.bColorCount = 0;
        else
            ide.bColorCount = 1 << (ide.wPlanes * ide.wBitCount);
        ide.dwBytesInRes = lpIR->IconImages[i].dwNumBytes;
        ide.dwImageOffset = CalculateImageOffset( lpIR, i );
        // Write the ICONDIRENTRY out to disk
        if( ! WriteFile( hFile, &ide, sizeof( ICONDIRENTRY ), &dwBytesWritten, NULL ) )
            return FALSE;
        // Did we write a full ICONDIRENTRY ?
        if( dwBytesWritten != sizeof( ICONDIRENTRY ) )
            return FALSE;
    }
    // Write the image bits for each image
    for( i=0; i<lpIR->nNumImages; i++ )
    {
        DWORD dwTemp = lpIR->IconImages[i].lpbi->bmiHeader.biSizeImage;

        // Set the sizeimage member to zero
        lpIR->IconImages[i].lpbi->bmiHeader.biSizeImage = 0;
        // Write the image bits to file
        if( ! WriteFile( hFile, lpIR->IconImages[i].lpBits, lpIR->IconImages[i].dwNumBytes, &dwBytesWritten, NULL ) )
            return FALSE;
        if( dwBytesWritten != lpIR->IconImages[i].dwNumBytes )
            return FALSE;
        // set it back
        lpIR->IconImages[i].lpbi->bmiHeader.biSizeImage = dwTemp;
    }
    CloseHandle( hFile );
    return TRUE;
}
/* End WriteIconToICOFile() **************************************************/


/****************************************************************************
*
*     FUNCTION: IconImageToClipBoard
*
*     PURPOSE:  Copies an icon image to the clipboard in CF_DIB format
*
*     PARAMS:   LPICONIMAGE lpii - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL IconImageToClipBoard( LPICONIMAGE lpii )
{
    HANDLE	hGlobal;
    LPSTR	lpBits;

    // Open the clipboard
    if( OpenClipboard( NULL ) )
    {
        // empty it
        if( EmptyClipboard() )
        {
            // Make a buffer to send to clipboard
            hGlobal = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, lpii->dwNumBytes );
            lpBits = (LPSTR)GlobalLock( hGlobal );
            // Copy the bits to the buffer
            memcpy( lpBits, lpii->lpBits, lpii->dwNumBytes );
            // Adjust for funky height*2 thing
            ((LPBITMAPINFOHEADER)lpBits)->biHeight /= 2;
            GlobalUnlock( hGlobal );
            // Send it to the clipboard
            SetClipboardData( CF_DIB, hGlobal );
            CloseClipboard();
            return TRUE;
        }
    }
    return FALSE;
}
/* End IconImageToClipBoard() ***********************************************/



/****************************************************************************
*
*     FUNCTION: IconImageFromClipBoard
*
*     PURPOSE:  Creates an icon image from the CF_DIB clipboard entry
*
*     PARAMS:   LPICONIMAGE lpii          - pointer to icon image data
*               BOOL        bStretchToFit - TRUE to stretch, FALSE to take
*                                           the upper left corner of the DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL IconImageFromClipBoard( LPICONIMAGE lpii, BOOL bStretchToFit )
{
    LPBITMAPINFO    lpbi;
    HANDLE        	hClipGlobal;
    BOOL            bRet = FALSE;

    // Open the clipboard
    if( OpenClipboard( NULL ) )
    {
        // Get the CF_DIB data from it
        if( (hClipGlobal = GetClipboardData( CF_DIB )) != NULL )
        {
            // Lock it down
            if( (lpbi=(LPBITMAPINFO)GlobalLock(hClipGlobal)) != NULL )
            {
                // Convert it to an icon image
                bRet = DIBToIconImage( lpii, (LPBYTE)lpbi, bStretchToFit );
                GlobalUnlock( hClipGlobal );
            }
        }
        CloseClipboard();
    }
    return bRet;
}
/* End IconImageFromClipBoard() ********************************************/



/****************************************************************************
*
*     FUNCTION: DIBToIconImage
*
*     PURPOSE:  Converts a CF_DIB memory block to an icon image
*
*     PARAMS:   LPICONIMAGE lpii          - pointer to icon image data
*               LPBYTE      lpDIB         - a pointer to the CF_DIB block
*               BOOL        bStretchToFit - TRUE to stretch, FALSE to take
*                                           the upper left corner of the DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL DIBToIconImage( LPICONIMAGE lpii, LPBYTE lpDIB, BOOL bStretch )
{
    LPBYTE    	lpNewDIB;

    // Sanity check
    if( lpDIB == NULL )
        return FALSE;

    // Let the DIB engine convert color depths if need be
    lpNewDIB = ConvertDIBFormat( (LPBITMAPINFO)lpDIB, lpii->Width, lpii->Height, lpii->Colors, bStretch );

    // Now we have a cool new DIB of the proper size/color depth
    // Lets poke it into our data structures and be done with it

    // How big is it?
    lpii->dwNumBytes = sizeof( BITMAPINFOHEADER )                    	// Header
                    + PaletteSize( (LPSTR)lpNewDIB )                    // Palette
                    + lpii->Height * BytesPerLine( (LPBITMAPINFOHEADER)lpNewDIB )	// XOR mask
                    + lpii->Height * WIDTHBYTES( lpii->Width );        	// AND mask

    // If there was already an image here, free it
    if( lpii->lpBits != NULL )
        free( lpii->lpBits );
    // Allocate enough room for the new image
    if( (lpii->lpBits = (LPBYTE)malloc( lpii->dwNumBytes )) == NULL )
    {
        free( lpii );
        return FALSE;
    }
    // Copy the bits
    memcpy( lpii->lpBits, lpNewDIB, sizeof( BITMAPINFOHEADER ) + PaletteSize( (LPSTR)lpNewDIB ) );
    // Adjust internal pointers/variables for new image
    lpii->lpbi = (LPBITMAPINFO)(lpii->lpBits);
    lpii->lpbi->bmiHeader.biHeight *= 2;
    lpii->lpXOR = (LPBYTE)FindDIBBits( (LPSTR)(lpii->lpBits) );
    memcpy( lpii->lpXOR, (LPBYTE)FindDIBBits((LPSTR)lpNewDIB), lpii->Height * BytesPerLine( (LPBITMAPINFOHEADER)lpNewDIB ) );
    lpii->lpAND = lpii->lpXOR + lpii->Height * BytesPerLine( (LPBITMAPINFOHEADER)lpNewDIB );
    memset( lpii->lpAND, 0, lpii->Height * WIDTHBYTES( lpii->Width ) );
    // Free the source
    free( lpNewDIB );
    return TRUE;
}
/* End DIBToIconImage() ***************************************************/




/****************************************************************************
*
*     FUNCTION: CreateBlankNewFormatIcon
*
*     PURPOSE:  Creates a blank icon image for a new format
*
*     PARAMS:   LPICONIMAGE lpii          - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL CreateBlankNewFormatIcon( LPICONIMAGE lpii )
{
    DWORD            	dwFinalSize;
    BITMAPINFOHEADER    bmih;

    // Fill in the bitmap header
    ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );
    bmih.biSize = sizeof( BITMAPINFOHEADER );
    bmih.biBitCount = lpii->Colors;
    bmih.biClrUsed = 0;
    
    // How big will the final thing be?
    // Well, it'll have a header
    dwFinalSize = sizeof( BITMAPINFOHEADER );
    // and a color table (even if it's zero length)
    dwFinalSize += PaletteSize( (LPSTR)&bmih );
    // and XOR bits
    dwFinalSize += lpii->Height * WIDTHBYTES( lpii->Width * lpii->Colors );
    // and AND bits. That's about it :)
    dwFinalSize += lpii->Height * WIDTHBYTES( lpii->Width );

    // Allocate some memory for it
    lpii->lpBits = (LPBYTE)malloc( dwFinalSize );
    ZeroMemory( lpii->lpBits, dwFinalSize );
    lpii->dwNumBytes = dwFinalSize;
    lpii->lpbi = (LPBITMAPINFO)(lpii->lpBits);
    lpii->lpXOR =(LPBYTE) ( 
		(LPSTR)(lpii->lpbi) + sizeof(BITMAPINFOHEADER) + PaletteSize( (LPSTR)&bmih )
		);
    lpii->lpAND = lpii->lpXOR + (lpii->Height * WIDTHBYTES( lpii->Width * lpii->Colors ));

    // The bitmap header is zeros, fill it out
    lpii->lpbi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER ); 
    lpii->lpbi->bmiHeader.biWidth = lpii->Width;
    // Don't forget the funky height*2 icon resource thing
    lpii->lpbi->bmiHeader.biHeight = lpii->Height * 2; 
    lpii->lpbi->bmiHeader.biPlanes = 1; 
    lpii->lpbi->bmiHeader.biBitCount = lpii->Colors; 
    lpii->lpbi->bmiHeader.biCompression = BI_RGB; 
                   
    return TRUE;
}
/* End CreateBlankNewFormatIcon() ******************************************/



/****************************************************************************
*
*     FUNCTION: GetXORImageRect
*
*     PURPOSE:  Given a bounding Rect, calculates the XOR mask display Rect 
*
*     PARAMS:   RECT        Rect   - Bounding rect for drawing area
*               LPICONIMAGE lpIcon - pointer to icon image data
*
*     RETURNS:  RECT - the rect where the XOR image will be drawn
*
* History:
*                July '95 - Created
*
\****************************************************************************/
RECT GetXORImageRect( RECT Rect, LPICONIMAGE lpIcon )
{
    RECT    NewRect;

    // Just center the thing in the bounding display rect
    NewRect.left = Rect.left + ((RectWidth(Rect)-lpIcon->lpbi->bmiHeader.biWidth)/2);
    NewRect.top = Rect.top + ((RectHeight(Rect)-(lpIcon->lpbi->bmiHeader.biHeight/2))/2);
    NewRect.bottom = NewRect.top + (lpIcon->lpbi->bmiHeader.biHeight/2);
    NewRect.right = NewRect.left + lpIcon->lpbi->bmiHeader.biWidth;
    return NewRect;
}
/* End GetXORImageRect() ***************************************************/


/****************************************************************************
*
*     FUNCTION: DrawXORMask
*
*     PURPOSE:  Using DIB functions, draw XOR mask on hDC in Rect
*
*     PARAMS:   HDC         hDC    - The DC on which to draw
*               RECT        Rect   - Bounding rect for drawing area
*               LPICONIMAGE lpIcon - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*     COMMENTS: Does not use any palette information since the
*               OS won't when it draws the icon anyway.
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL DrawXORMask( HDC hDC, RECT Rect, LPICONIMAGE lpIcon )
{
    int            	x, y;

    // Sanity checks
    if( lpIcon == NULL )
        return FALSE;
    if( lpIcon->lpBits == NULL )
        return FALSE;

    // Account for height*2 thing
    lpIcon->lpbi->bmiHeader.biHeight /= 2;

    // Locate it
    x = Rect.left + ((RectWidth(Rect)-lpIcon->lpbi->bmiHeader.biWidth)/2);
    y = Rect.top + ((RectHeight(Rect)-lpIcon->lpbi->bmiHeader.biHeight)/2);

    // Blast it to the screen
    SetDIBitsToDevice( hDC, x, y, lpIcon->lpbi->bmiHeader.biWidth, lpIcon->lpbi->bmiHeader.biHeight, 0, 0, 0, lpIcon->lpbi->bmiHeader.biHeight, lpIcon->lpXOR, lpIcon->lpbi, DIB_RGB_COLORS );

    // UnAccount for height*2 thing
    lpIcon->lpbi->bmiHeader.biHeight *= 2;

    return TRUE;
}
/* End DrawXORMask() *******************************************************/




/****************************************************************************
*
*     FUNCTION: DrawANDMask
*
*     PURPOSE:  Using DIB functions, draw AND mask on hDC in Rect
*
*     PARAMS:   HDC         hDC    - The DC on which to draw
*               RECT        Rect   - Bounding rect for drawing area
*               LPICONIMAGE lpIcon - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL DrawANDMask( HDC hDC, RECT Rect, LPICONIMAGE lpIcon )
{
    LPBITMAPINFO    lpbi;
    int            	x, y;

    // Sanity checks
    if( lpIcon == NULL )
        return FALSE;
    if( lpIcon->lpBits == NULL )
        return FALSE;

    // Need a bitmap header for the mono mask
    lpbi = (LPBITMAPINFO)malloc( sizeof(BITMAPINFO) + (2 * sizeof( RGBQUAD )) );
    lpbi->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
    lpbi->bmiHeader.biWidth = lpIcon->lpbi->bmiHeader.biWidth;
    lpbi->bmiHeader.biHeight = lpIcon->lpbi->bmiHeader.biHeight/2;
    lpbi->bmiHeader.biPlanes = 1;
    lpbi->bmiHeader.biBitCount = 1;
    lpbi->bmiHeader.biCompression = BI_RGB;
    lpbi->bmiHeader.biSizeImage = 0;
    lpbi->bmiHeader.biXPelsPerMeter = 0;
    lpbi->bmiHeader.biYPelsPerMeter = 0;
    lpbi->bmiHeader.biClrUsed = 0;
    lpbi->bmiHeader.biClrImportant = 0;
    lpbi->bmiColors[0].rgbRed = 0;
    lpbi->bmiColors[0].rgbGreen = 0;
    lpbi->bmiColors[0].rgbBlue = 0;
    lpbi->bmiColors[0].rgbReserved = 0;
    lpbi->bmiColors[1].rgbRed = 255;
    lpbi->bmiColors[1].rgbGreen = 255;
    lpbi->bmiColors[1].rgbBlue = 255;
    lpbi->bmiColors[1].rgbReserved = 0;

    // Locate it
    x = Rect.left + ((RectWidth(Rect)-lpbi->bmiHeader.biWidth)/2);
    y = Rect.top + ((RectHeight(Rect)-lpbi->bmiHeader.biHeight)/2);

    // Blast it to the screen
    SetDIBitsToDevice( hDC, x, y, lpbi->bmiHeader.biWidth, lpbi->bmiHeader.biHeight, 0, 0, 0, lpbi->bmiHeader.biHeight, lpIcon->lpAND, lpbi, DIB_RGB_COLORS );

    // clean up
    free( lpbi );

    return TRUE;
}
/* End DrawANDMask() *******************************************************/



/****************************************************************************
*
*     FUNCTION: MakeNewANDMaskBasedOnPoint
*
*     PURPOSE:  Creates a new AND mask for the icon image
*
*     PARAMS:   LPICONIMAGE lpIcon - pointer to icon image data
*               POINT       pt     - coords of transparent pixel
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
*     COMMENTS: Creates the AND mask using the color of the pixel at pt
*               as a transparent color. The XOR mask is changed as well.
*               This is because the OS expects the XOR mask to have the
*               AND mask already applied (ie black in transparent areas)
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL MakeNewANDMaskBasedOnPoint( LPICONIMAGE lpIcon, POINT pt )
{
    HBITMAP        	hXORBitmap, hOldXORBitmap;
    HDC            	hDC, hMemDC1;
    LPBYTE        	pXORBits;
    COLORREF        crTransparentColor;
    LONG            i,j;


    // Account for height*2 thing
    lpIcon->lpbi->bmiHeader.biHeight /= 2;

    // Need a DC
    hDC = GetDC( NULL );

    // Use DIBSection for source
    hXORBitmap = CreateDIBSection( hDC, lpIcon->lpbi, DIB_RGB_COLORS, (PVOID*)&pXORBits, NULL, 0  );
    memcpy( pXORBits, lpIcon->lpXOR, (lpIcon->lpbi->bmiHeader.biHeight) * BytesPerLine((LPBITMAPINFOHEADER)(lpIcon->lpbi)) );
    hMemDC1 = CreateCompatibleDC( hDC );
    hOldXORBitmap = (HBITMAP)SelectObject( hMemDC1, hXORBitmap );

    // Set the color table if need be
    if( lpIcon->lpbi->bmiHeader.biBitCount <= 8 )
        SetDIBColorTable( hMemDC1, 0, DIBNumColors((LPSTR)(lpIcon->lpbi)), lpIcon->lpbi->bmiColors);
    
    // What's the transparent color?
    crTransparentColor = GetPixel( hMemDC1, pt.x, pt.y );

    // Loop through the pixels
    for(i=0;i<lpIcon->lpbi->bmiHeader.biWidth;i++)
    {
        for(j=0;j<lpIcon->lpbi->bmiHeader.biHeight;j++)
        {
            // Is the source transparent at this point?
            if( GetPixel( hMemDC1, i, j ) == crTransparentColor )
            {
                // Yes, so set the pixel in AND mask, and clear it in XOR mask
                SetMonoDIBPixel( lpIcon->lpAND, lpIcon->lpbi->bmiHeader.biWidth, lpIcon->lpbi->bmiHeader.biHeight, i, j, TRUE );     
                if( lpIcon->lpbi->bmiHeader.biBitCount == 1 )
                    SetMonoDIBPixel( pXORBits, lpIcon->lpbi->bmiHeader.biWidth, lpIcon->lpbi->bmiHeader.biHeight, i, j, FALSE );     
                else
                    SetPixelV( hMemDC1, i, j, RGB(0,0,0) );
            }
            else
            {
                // No, so clear pixel in AND mask
                SetMonoDIBPixel( lpIcon->lpAND, lpIcon->lpbi->bmiHeader.biWidth, lpIcon->lpbi->bmiHeader.biHeight, i, j, FALSE );    
            }
        }
    }
    // Flush the SetPixelV() calls
    GdiFlush();

    SelectObject( hMemDC1, hOldXORBitmap );

    // Copy the new XOR bits back to our storage
    memcpy( lpIcon->lpXOR, pXORBits, (lpIcon->lpbi->bmiHeader.biHeight) * BytesPerLine((LPBITMAPINFOHEADER)(lpIcon->lpbi)) );

    // Clean up
    DeleteObject( hXORBitmap );
    DeleteDC( hMemDC1 );
    ReleaseDC( NULL, hDC );


    // UnAccount for height*2 thing
    lpIcon->lpbi->bmiHeader.biHeight *= 2;
    return TRUE;
}
/* End MakeNewANDMaskBasedOnPoint() *****************************************/


/****************************************************************************
*
*     FUNCTION: IconImageFromBMPFile
*
*     PURPOSE:  Creates an icon image from a BMP file
*
*     PARAMS:   LPCTSTR     szFileName    - Filename for BMP file
*               LPICONIMAGE lpii          - pointer to icon image data
*               BOOL        bStretchToFit - TRUE to stretch, FALSE to take
*                                           the upper left corner of the DIB
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL IconImageFromBMPFile( LPCTSTR szFileName, LPICONIMAGE lpii, BOOL bStretchToFit )
{
    LPBYTE        	lpDIB = NULL;
    BOOL            bRet = FALSE;

    if( (lpDIB=ReadBMPFile(szFileName)) == NULL )
        return FALSE;
    // Convert it to an icon image
    bRet = DIBToIconImage( lpii, lpDIB, bStretchToFit );
    free( lpDIB );
    return bRet;
}
/* End IconImageFromBMPFile() ********************************************/




/****************************************************************************
*
*     FUNCTION: IconImageToBMPFile
*
*     PURPOSE:  Creates BMP file from an icon image
*
*     PARAMS:   LPCTSTR     szFileName    - Filename for BMP file
*               LPICONIMAGE lpii          - pointer to icon image data
*
*     RETURNS:  BOOL - TRUE for success, FALSE for failure
*
* History:
*                July '95 - Created
*
\****************************************************************************/
BOOL IconImageToBMPFile( LPCTSTR szFileName, LPICONIMAGE lpii )
{
    return WriteBMPFile( szFileName, (LPBYTE)lpii->lpbi );
}


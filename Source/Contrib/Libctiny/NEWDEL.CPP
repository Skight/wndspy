//==========================================
// LIBCTINY - Matt Pietrek 2001
// MSDN Magazine, January 2001
//==========================================
#include <windows.h>

void * __cdecl operator new(unsigned int s)
{
    return HeapAlloc( GetProcessHeap(), 0, s );
}

void __cdecl operator delete( void * p )
{
    HeapFree( GetProcessHeap(), 0, p );
}

#==================================================
# LIBCTINY - Matt Pietrek 1996
# Microsoft Systems Journal, October 1996
# FILE: LIBCTINY.MAK - Makefile for Microsoft version
#==================================================
PROJ = LIBCTINY

OBJS =  CRT0TCON.OBJ CRT0TWIN.OBJ DLLCRT0.OBJ ARGCARGV.OBJ PRINTF.OBJ\
        SPRINTF.OBJ PUTS.OBJ ALLOC.OBJ ALLOC2.OBJ ALLOCSUP.OBJ STRUPLWR.OBJ \
        ISCTYPE.OBJ ATOL.OBJ STRICMP.OBJ NEWDEL.OBJ INITTERM.OBJ

CC = CL
CC_OPTIONS = /c /W3 /DWIN32_LEAN_AND_MEAN

!ifdef DEBUG
CC_OPTIONS = $(CC_OPTIONS) /Zi
!else
CC_OPTIONS = $(CC_OPTIONS) /O1
!endif

$(PROJ).LIB: $(OBJS)
    LIB /OUT:$(PROJ).LIB $(OBJS)

.CPP.OBJ:
    $(CC) $(CC_OPTIONS) $<
    
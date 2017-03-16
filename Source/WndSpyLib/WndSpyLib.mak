# Microsoft Developer Studio Generated NMAKE File, Based on WndSpyLib.dsp
!IF "$(CFG)" == ""
CFG=WndSpyLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WndSpyLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WndSpyLib - Win32 Release" && "$(CFG)" != "WndSpyLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WndSpyLib.mak" CFG="WndSpyLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WndSpyLib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WndSpyLib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "WndSpyLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\WndSpy.dll" "$(OUTDIR)\WndSpyLib.bsc"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WndSpyLib.obj"
	-@erase "$(INTDIR)\WndSpyLib.res"
	-@erase "$(INTDIR)\WndSpyLib.sbr"
	-@erase "$(INTDIR)\WndSpyLibOpt.obj"
	-@erase "$(INTDIR)\WndSpyLibOpt.sbr"
	-@erase "$(OUTDIR)\WndSpy.dll"
	-@erase "$(OUTDIR)\WndSpy.exp"
	-@erase "$(OUTDIR)\WndSpy.lib"
	-@erase "$(OUTDIR)\WndSpyLib.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "_USRDLL" /D "WNDSPYLIB_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\WndSpyLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\WndSpyLib.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WndSpyLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\WndSpyLib.sbr" \
	"$(INTDIR)\WndSpyLibOpt.sbr"

"$(OUTDIR)\WndSpyLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x67BF0000" /version:1.6 /dll /incremental:no /pdb:"$(OUTDIR)\WndSpy.pdb" /machine:I386 /def:".\WndSpyLib.def" /out:"$(OUTDIR)\WndSpy.dll" /implib:"$(OUTDIR)\WndSpy.lib" 
DEF_FILE= \
	".\WndSpyLib.def"
LINK32_OBJS= \
	"$(INTDIR)\WndSpyLib.obj" \
	"$(INTDIR)\WndSpyLibOpt.obj" \
	"$(INTDIR)\WndSpyLib.res"

"$(OUTDIR)\WndSpy.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WndSpyLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\WndSpy.dll"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WndSpyLib.obj"
	-@erase "$(INTDIR)\WndSpyLib.res"
	-@erase "$(INTDIR)\WndSpyLibOpt.obj"
	-@erase "$(OUTDIR)\WndSpy.dll"
	-@erase "$(OUTDIR)\WndSpy.exp"
	-@erase "$(OUTDIR)\WndSpy.ilk"
	-@erase "$(OUTDIR)\WndSpy.lib"
	-@erase "$(OUTDIR)\WndSpy.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "WNDSPYLIB_EXPORTS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\WndSpyLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\WndSpyLib.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WndSpyLib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\WndSpy.pdb" /debug /machine:I386 /def:".\WndSpyLib.def" /out:"$(OUTDIR)\WndSpy.dll" /implib:"$(OUTDIR)\WndSpy.lib" /pdbtype:sept /NOENTRY 
LINK32_OBJS= \
	"$(INTDIR)\WndSpyLib.obj" \
	"$(INTDIR)\WndSpyLibOpt.obj" \
	"$(INTDIR)\WndSpyLib.res"

"$(OUTDIR)\WndSpy.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("WndSpyLib.dep")
!INCLUDE "WndSpyLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "WndSpyLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WndSpyLib - Win32 Release" || "$(CFG)" == "WndSpyLib - Win32 Debug"
SOURCE=.\WndSpyLib.cpp

!IF  "$(CFG)" == "WndSpyLib - Win32 Release"


"$(INTDIR)\WndSpyLib.obj"	"$(INTDIR)\WndSpyLib.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WndSpyLib - Win32 Debug"


"$(INTDIR)\WndSpyLib.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\WndSpyLibOpt.cpp

!IF  "$(CFG)" == "WndSpyLib - Win32 Release"


"$(INTDIR)\WndSpyLibOpt.obj"	"$(INTDIR)\WndSpyLibOpt.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "WndSpyLib - Win32 Debug"


"$(INTDIR)\WndSpyLibOpt.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\WndSpyLib.rc

"$(INTDIR)\WndSpyLib.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 


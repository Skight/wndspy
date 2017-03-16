# Microsoft Developer Studio Project File - Name="WndSpy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WndSpy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WndSpy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WndSpy.mak" CFG="WndSpy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WndSpy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WndSpy - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WndSpy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /O /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:1.6 /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"msvcrtd.lib"

!ELSEIF  "$(CFG)" == "WndSpy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib ole32.lib oleaut32.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WndSpy - Win32 Release"
# Name "WndSpy - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Source\Contrib.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgAppSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgColorSpy.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgEffectOption.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgIconExtractor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgPswSniffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgSaveIconOption.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgSndMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabClass.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabMani.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabStyles.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgWndEnumer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgWndModifier.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GlobalVar.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HsCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SndMsgFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SpyFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainToolTips.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMsgWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyToolContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\TempWork.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpy.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\WndSpyGui.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Source\AppOpt.h
# End Source File
# Begin Source File

SOURCE=.\Source\Contrib.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgAbout.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgAppSettings.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgColorSpy.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgEffectOption.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgIconExtractor.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgPswSniffer.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgSaveIconOption.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgSndMsg.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabClass.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabFinder.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabGeneral.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabMani.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabProcess.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs\DlgTabStyles.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgWndEnumer.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\DlgWndModifier.h
# End Source File
# Begin Source File

SOURCE=.\Source\GlobalVar.h
# End Source File
# Begin Source File

SOURCE=.\Source\HsCommon.h
# End Source File
# Begin Source File

SOURCE=.\Source\HsCommon\RectPosRgnEx.h
# End Source File
# Begin Source File

SOURCE=.\Source\resource.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SndMsgFunc.h
# End Source File
# Begin Source File

SOURCE=.\Source\SpyFunc.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlg.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainDlgTabs.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMainToolTips.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyMsgWnd.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\SpyToolContainer.h
# End Source File
# Begin Source File

SOURCE=.\Source\StrTableDef.h
# End Source File
# Begin Source File

SOURCE=.\TempWork.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpy.h
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyGui\WndSpyGui.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Source\Res\DragDropper.cur
# End Source File
# Begin Source File

SOURCE=.\Source\Res\DragFind.cur
# End Source File
# Begin Source File

SOURCE=.\Source\Res\DragFinder.cur
# End Source File
# Begin Source File

SOURCE=.\Source\Res\DragIcon_Emp.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\DragIcon_Nom.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\DragIcon_Picker.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\DragMover.cur
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Goto.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Goto_grey.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\MainIcon.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\SmallIcon.ico
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpy.RC
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_Hidden.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_HiddenRoot.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_Hint.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_Normal.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_Root.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_SpecChild.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_SpecHidden.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_SpecHint.ico
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Wtree_Thread.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\Source\Res\Manifest.xml
# End Source File
# Begin Source File

SOURCE=.\Source\Res\Win2Win.wav
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpyLib\Release\WndSpy.dll
# End Source File
# Begin Source File

SOURCE=.\Source\WndSpy.Manifest
# End Source File
# End Target
# End Project

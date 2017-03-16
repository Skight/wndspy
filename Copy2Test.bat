@echo off
color 2E
title Copy To Test Area

::-----------------------

SET RELEASEEXE="%cd%\release\wndspy.exe"
SET TESTEXE="D:\VPC Share\WndSpy.exe"

::----------------------

:CopyRetry
echo Copy: %RELEASEEXE%
echo To: %TESTEXE%
copy %RELEASEEXE% %TESTEXE%
echo Press any key to recopy...
pause>nul
::-----------------------
@goto CopyRetry
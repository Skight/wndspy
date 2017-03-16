@echo off
color 1E
Title Release Appliation

::----------------------
SET APPNAME=WndSpy
::----------------------
SET RELEASEEXE="%cd%\release\%APPNAME%.exe"
SET RELEASECHM="%cd%\Help\%APPNAME%.chm"
SET RELEASEDIR="%cd%\WindowSpy\"

::----------------------
copy %RELEASEEXE% %RELEASEDIR%
copy %RELEASECHM% %RELEASEDIR%
pause
::-----------------------

@echo off
set SET_TITLE=Clean up Workspace
color 1E
goto lab_begin

:lab_process:
color 1E
title Cleaning...

cd %cd%
del /s /q *.plg
del /s /q *.ncb
del /s /q *.opt
del /s /q *.aps
del /s /q "debug\*.*"
del /s /q "Source\WndSpyLib\debug\*.*"

set SET_INFO="Clean up Release as well, continue? (y/n)  "

set /p SET_CONFIRM=%SET_INFO%

if /i %SET_CONFIRM% == y (
del /s /q "release\*.*"
del /s /q "Source\WndSpyLib\release\*.*"
)
if /i %SET_CONFIRM% == n (
exit
)
echo Clean up Finished, press any key to exit...
title Finished
pause>NUL
exit


:lab_begin
cls

title %SET_TITLE%
set SET_CONFIRM=x
set SET_INFO="Clean up workspace, continue? (y/n)  "
set /p SET_CONFIRM=%SET_INFO%

if /i %SET_CONFIRM% == y (
goto lab_process
)

if /i %SET_CONFIRM% == n (
exit
)

color 0%time:~7,1%
goto lab_begin
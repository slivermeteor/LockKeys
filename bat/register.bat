@echo off

if exist "%SystemRoot%\system32\atl100.dll" (
    ::xcopy atl100.dll D:\ /-y
    ::echo "1"
) else (
        xcopy atl100.dll C:\Windows\System32 /-y 
        regsvr32 atl100.dll /n /i
     )

setlocal
set uac=~uac_permission_tmp_%random%
md "%SystemRoot%\system32\%uac%" 2>nul
if %errorlevel%==0 (
    rd "%SystemRoot%\system32\%uac%" >nul 2>nul ) else (
    echo set uac = CreateObject^("Shell.Application"^)>"%temp%\%uac%.vbs"
    echo uac.ShellExecute "%~s0","","","runas",1 >>"%temp%\%uac%.vbs"
    echo WScript.Quit >>"%temp%\%uac%.vbs"
    "%temp%\%uac%.vbs" /f
    del /f /q "%temp%\%uac%.vbs" & exit )
endlocal

:Run
regsvr32 "%~dp0%LockKeys.dll"

exit
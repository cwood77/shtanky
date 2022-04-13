@echo off
if "%1" == "boot" goto %1
if "%1x" == "x" goto noarg

bin\out\debug\appr.exe
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat %*
goto end

:boot
echo (running tests to generate output without checking output)
bin\out\debug\appr.exe -skipchecks
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat run
goto end

:noarg
call appr.bat run
goto end

:broken
echo WTH? appr returned an errorcode

:end

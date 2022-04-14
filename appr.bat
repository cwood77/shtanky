@echo off
setlocal

:parse
if "%1" == "boot" goto %1
if "%1x" == "x" goto noarg
if "%1" == "verbose" goto %1

bin\out\debug\appr.exe %verb%
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

:verbose
set verb=-noboot -verbose
shift
goto parse

:broken
echo WTH? appr returned an errorcode

:end

@echo off

:parse
if "%1" == "boot" goto %1
if "%1" == "verbose" goto %1
if "%1" == "rel" goto %1
if "%1" == "!" goto tweak
if "%1x" == "x" goto noarg

bin\out\debug\appr.exe %*
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat %*
goto end

:boot
echo (running tests to generate output without checking output)
bin\out\debug\appr.exe -skipchecks
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat run
goto end

:verbose
echo (setting verbose output)
bin\out\debug\appr.exe -%*
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat run
goto end

:rel
echo (running tests to against release mode)
bin\out\release\appr.exe rel
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat run
goto end

:tweak
echo (running test subset)
bin\out\debug\appr.exe %*
if NOT %ERRORLEVEL% == 0 goto broken
call bin\.appr.bat run
goto end

:noarg
rem re-call self with default arg 'run'
call appr.bat run
goto end

:broken
echo WTH? appr returned an errorcode

:end

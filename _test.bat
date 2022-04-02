@echo off
goto end

del /S /Q testdata\*.ara.lh >nul 2>&1
del /S /Q testdata\*.ara.ls >nul 2>&1
del /S /Q testdata\*.ara.ls.asm >nul 2>&1

bin\out\debug\araceli.exe >nul 2>&1
if exist testdata\test\test.ara.lh goto next1
echo araceli FAILED to generate lh
goto fail
:next1
if exist testdata\test\test.ara.ls goto next2
echo araceli FAILED to generate ls
goto fail
:next2

bin\out\debug\liam.exe >nul 2>&1
if exist testdata\test\test.ara.ls.asm goto next3
echo liam FAILED to generate asm
goto fail
:next3
fc /b testdata\test\test.ara.ls.asm testdata\test\test-expected.asm >nul 2>&1
if %ERRORLEVEL% == 0 goto next4
echo liam FAILED to generated correct asm
goto fail
:next4
echo pass

goto end
:fail
echo FAIL!
:end

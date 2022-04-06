@echo off
if "%1x" == "x" goto usage
goto %1
:usage
echo usage: bless [asm/list]
goto end

:asm
copy testdata\test\test.ara.ls.asm testdata\test\test-expected.asm
goto end

:list
copy testdata\test\test.ara.ls.asm.list testdata\test\test-expected.list
goto end

:end

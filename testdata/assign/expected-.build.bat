@echo off

echo *****************
echo ***   liam    ***
echo *****************
bin\out\debug\liam ".\testdata\assign\main.ara.ls"
bin\out\debug\liam ".\testdata\assign\.target.ara.ls"
bin\out\debug\liam ".\testdata\sht\cons\program.ara.ls"

echo *******************
echo ***   shtasm    ***
echo *******************
bin\out\debug\shtasm "testdata\sht\oscall.asm"
bin\out\debug\shtasm ".\testdata\assign\main.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\assign\.target.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\cons\program.ara.ls.asm"

echo *******************
echo ***   shlink    ***
echo *******************
bin\out\debug\shlink ".\testdata\test\.app" ".\testdata\assign\main.ara.ls.asm.o" ".\testdata\assign\.target.ara.ls.asm.o" ".\testdata\sht\cons\program.ara.ls.asm.o" "testdata\sht\oscall.asm.o"

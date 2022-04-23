@echo off

echo *****************
echo ***   liam    ***
echo *****************
bin\out\debug\liam ".\testdata\test\test.ara.ls"
bin\out\debug\liam ".\testdata\test\.target.ara.ls"
bin\out\debug\liam ".\testdata\sht\cons\program.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\object.ara.ls"

echo *******************
echo ***   shtasm    ***
echo *******************
bin\out\debug\shtasm "testdata\sht\oscall.asm"
bin\out\debug\shtasm ".\testdata\test\test.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\test\.target.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\cons\program.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\object.ara.ls.asm"

echo *******************
echo ***   shlink    ***
echo *******************
bin\out\debug\shlink ".\testdata\test\.app" ".\testdata\test\test.ara.ls.asm.o" ".\testdata\test\.target.ara.ls.asm.o" ".\testdata\sht\cons\program.ara.ls.asm.o" ".\testdata\sht\core\object.ara.ls.asm.o" "testdata\sht\oscall.asm.o"

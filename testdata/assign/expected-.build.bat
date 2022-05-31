@echo off

echo *****************
echo ***   liam    ***
echo *****************
bin\out\debug\liam ".\testdata\assign\main.ara.ls"
bin\out\debug\liam ".\testdata\assign\.target.ara.ls"
bin\out\debug\liam ".\testdata\sht\cons\program.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\array.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\loopInst.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\object.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\string.ara.ls"

echo *******************
echo ***   shtasm    ***
echo *******************
bin\out\debug\shtasm ".\testdata\assign\main.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\assign\.target.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\cons\program.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\array.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\loopInst.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\object.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\string.ara.ls.asm"
bin\out\debug\shtasm "testdata\sht\oscall.asm"
bin\out\debug\shtasm "testdata\sht\string.asm"

echo *******************
echo ***   shlink    ***
echo *******************
bin\out\debug\shlink ".\testdata\test\.app" ".\testdata\assign\main.ara.ls.asm.o" ".\testdata\assign\.target.ara.ls.asm.o" ".\testdata\sht\cons\program.ara.ls.asm.o" ".\testdata\sht\core\array.ara.ls.asm.o" ".\testdata\sht\core\loopInst.ara.ls.asm.o" ".\testdata\sht\core\object.ara.ls.asm.o" ".\testdata\sht\core\string.ara.ls.asm.o" "testdata\sht\oscall.asm.o" "testdata\sht\string.asm.o"

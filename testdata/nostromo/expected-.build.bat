@echo off

echo *****************
echo ***   liam    ***
echo *****************
bin\out\debug\liam ".\testdata\nostromo\uart16550\driver.ara.ls"
bin\out\debug\liam ".\testdata\nostromo\boot.ara.ls"
bin\out\debug\liam ".\testdata\nostromo\stringout.ara.ls"
bin\out\debug\liam ".\testdata\nostromo\.target.ara.ls"
bin\out\debug\liam ".\testdata\sht\cons\program.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\array.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\loopInst.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\object.ara.ls"
bin\out\debug\liam ".\testdata\sht\core\string.ara.ls"

echo *******************
echo ***   shtasm    ***
echo *******************
bin\out\debug\shtasm ".\testdata\nostromo\uart16550\driver.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\nostromo\boot.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\nostromo\stringout.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\nostromo\.target.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\cons\program.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\array.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\loopInst.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\object.ara.ls.asm"
bin\out\debug\shtasm ".\testdata\sht\core\string.ara.ls.asm"
bin\out\debug\shtasm "testdata\sht\oscall.asm"
bin\out\debug\shtasm "testdata\sht\flags.asm"
bin\out\debug\shtasm "testdata\sht\string.asm"
bin\out\debug\shtasm "testdata\sht\array.asm"

echo *******************
echo ***   shlink    ***
echo *******************
bin\out\debug\shlink ".\testdata\test\.app" ".\testdata\nostromo\uart16550\driver.ara.ls.asm.o" ".\testdata\nostromo\boot.ara.ls.asm.o" ".\testdata\nostromo\stringout.ara.ls.asm.o" ".\testdata\nostromo\.target.ara.ls.asm.o" ".\testdata\sht\cons\program.ara.ls.asm.o" ".\testdata\sht\core\array.ara.ls.asm.o" ".\testdata\sht\core\loopInst.ara.ls.asm.o" ".\testdata\sht\core\object.ara.ls.asm.o" ".\testdata\sht\core\string.ara.ls.asm.o" "testdata\sht\oscall.asm.o" "testdata\sht\flags.asm.o" "testdata\sht\string.asm.o" "testdata\sht\array.asm.o"

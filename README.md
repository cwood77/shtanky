# shtanky

`Shtanky`[^1] is a toy 64-bit OS.  Code for shtanky is written in a homebrew language, `araceli`, which is compiled on Windows but generates `shantky` code.

## parts, and their status

(status updated 30-march-2022)
|part|description|status|
|---|---|---|
|`araceli`|an OO homebrewed language that generates `liam` code|working towards "hello world" app|
|`liam`|a C-like homebrewed language that generates `shtasm` code|working towards "hello world" app|
|`shtasm`|a assembler that generates x86-64 code|_not started_|
|`shlink`|a linker that links shtasm object code into executables|_not started_|
|`shtemu`|an emulator that runs shtanky executables on Windows 64|_not started_|
|...???...|I haven't really elaborated anything past here yet...|
|`shboot`|UEFI bootloader written in C++/MSVC|_not started_|
|`shtanky`|OS written in araceli|_not started_|

## design philosophy

1. first and foremost, this is a hobbist project; it should be **fun**.  When in tension, prefer _fun_ to _practical_.
1. secondly, let's use this project to **learn modern OS and chipset technology**.  E.g., even though I'm far more familiar with x86 and BIOS, those technologies are effectively dead; x86-64 and UEFI are the modern standard.

## technicals

Technical ideas and decisions thus far:
- exclusively x86-64
- boot via UEFI rather than BIOS
- follow the canonical Windows 64 calling convention, with no special optimizations for leaf functions, etc.
- defer floating point as long as I can get away with it
- defer SSE, etc.
- single processor only (for now)
- support protection (i.e. all rings)
- support multiprocess, thread, user
- explore interesting Intel tech for memory protection, shadow stacks, etc.  But avoid vendor lock-in.
- prefer Windows 64-style exception unwinding (i.e. happy path is 0-cost)
- gates to kernel mode done via `.osCall` instrinsic which is typically a `syscall` instruction
- emulate on Windows 64 by generating the `.osCall` intrinsic differently in liam (and below?)

## todo
- [ ] notice that calls trash regs
- [ ] preserve nonvolitile regs in pro/epilogue
- [ ] use shadow space for spilling 4 regs
- [ ] change 'storage' to allow for stack vars
- [ ] immediate ops
- [ ] patchable immediate ops
- [ ] func labels
- [ ] instr overload selection

[^1]: The name "shtanky" was suggested by my son, Ethan (age 11).  I selected the first of his suggestions that didn't include "my dad's butt" somewhere in the title.

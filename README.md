# shtanky

`Shtanky`[^1] is a toy 64-bit OS.  Code for shtanky is written in a homebrew language, `araceli`, which is compiled on Windows but generates `shantky` code.

## parts, and their status

(status updated 7-april-2022)
|part|description|status|
|---|---|---|
|`araceli`|an OO homebrewed language that generates `liam` code|working towards "hello world" app|
|`liam`|a C-like homebrewed language that generates `shtasm` code|working towards "hello world" app|
|`shtasm`|a assembler that generates x86-64 code|working towards "hello world" app|
|`shlink`|a linker that links shtasm object code into executables|working towards "hello world" app|
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

### shtasm / shlink / shload boundaries

#### `app` format and loaders
- `Shlink` produces position-independent code (PIC), generally in the following format
  - header
  - metadata
  - payload section boundaries and attributes
  - import table
  - export table
  - payload
- `Shload` both the emulator _and_ the OS loader can read this formats
- Import/export is used to
  - locate the primary entrypoint
  - house runtime linking.
- Loaders patch import tables to implement runtime linking

#### `o` format and the linker
- `Shtasm` output files contain a stream objects, each earmarked with one of the following sections
  - zero-initialized
  - constant, initialized
  - initialized
  - uniniatilied
  - code
  - patch tables
- `Shtasm` source demarkates the sections
- Runtime linking is done by generating import tables, which are patched by the loader
- `Shlink` is responsible for statically linking a single binary into PIC, which include potentially disassembling/reassembling instructions with patches
  - types of patching shlink must understand
    - call immediate
    - call variable
    - global var refs (`LEA REX.W + 8D /r`)

## todo
- [ ] handle volatile regs (i.e. regs must be preserved around calls if in use)
- [x] handle nonvolitile regs (i.e. must be preserved in pro/epilog if used)
- [ ] use shadow space for spilling 4 regs
- [x] change 'storage' to allow for stack vars
~~- [ ] more codegen tests~~
- [ ] codegen test that exercises stack
- [ ] stack locals
- [x] subcall locals
- [ ] exercise splitter
- [x] immediate ops
- [x] patchable ops
- [x] func labels
- [x] instr overload selection
- [x] call decomposition
- [ ] impl frame ptrs

--- multi file compile ---
- [x] batch build file generation
- [x] compiler args

--- other file compile req's ---
- [x] hoist strings
- [x] too many call arg
- [x] segment directives
- [x] call nodes (not just invoke)
- [ ] int literals

- [x] araceli bootstrap codegen

- [ ] code shape transform
- [ ] stemu callbacks
- [ ] stemu
- [ ]   get stemu passing (e.g. study virtual calls)
- [ ] are linker TOCs working?

[^1]: The name "shtanky" was suggested by my son, Ethan (age 11).  I selected the first of his suggestions that didn't include "my dad's butt" somewhere in the title.

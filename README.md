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

### Generic programming

The most natural application of the existing boundary philosophy would be to create a seperate template language/app that compiles into araceli, in the same way araceli compiles down to liam, etc.  This language would generate specific code from generic code allowing lower levels to be generic-ignorant.  Even more flexible code generation could exist in a layer above this generic implementor.

A big problem with this approach is that, generally, all the necessary specific instances of a generic are not obviously enumeratable.  I could:
1. pregenerate all possible cases (NOPE! - see below),
1. require constraints that allow explicit enumeration, or
1. implement some sort of iterative communication between apps that allows a template to be instantiated as needed.

What if I made type compile into a cookie that's passed into generic methods?
What if I made generic classes generate instances based simply on the presence of instances?
Would those two together be enought?

Combining the two strategies would lead to prolems; i.e. if a generic method instantiated a generic class.

More generally, I'm not sure pre-enumeration of all possible instances is possible, as this set seems unbounded.  E.g for types A<T>, isn't an instance A<A<A<bool>>>, etc.?

Let's consider contraints.  Even just supporting the base class constraint would make the set bounded... true or false?  False.  Consider transientServiceProxy<transientServiceProxy<something>>.  What if I just didn't support that?  Would that be a problem?  Potentially.  I could envision scenarios were this is deserable.

There are
1. generic classes
1. generic methods (in non-generic classes)
1. generic functions

Probably you'd generate classes first, then the other two.  This would mean classes couldn't derive from themselves (reasonable), but also couldn't reference themselves (maybe unreasonable).

Coming back to the boundedness.  Is that really a potentially desirable scenario?  Let's push on that.  What about transientServiceProxy<cancellingService<driverService<iText>>> as a decorator chain?

What if rather than all this stuff I did some basic enumeration guesses but then required users to list instance combinations, a la early C++ template implementations 'export' requirements?  Only for classes.  That sould allow step 1 above prettily readily.  Using baseclass contraints would them enable steps 2 and 3.

Remainging problems:
1. how to implement `type`
2. how to locate all the files participating in enumeration

`type` is purely dependent, and thus could be generated as needed.  The other problem is real though.  Hm.  Perhaps it's just an input.

AST nodes: generic(and args), type, instantiate, that's it?
Also need to worry about lexical analysis of < and >.

## todo
By milestone

### Stemu can callback itself and survive
- [x] stemu callbacks - shlink side
- [x] are linker TOCs working?
- [x] stemu impl
- [x] stemu callbacks - stemu side
- [x] get stemu passing (e.g. study virtual calls)
- [x] <enterfunc> is double-allocating stack space
- [x] setup an ATS

### Hello World MVP
- [x] code shape transform
- [x] handle volatile regs (i.e. regs must be preserved around calls if in use)
- [ ] impl frame ptrs

### Must, but Later
- [x] int literals
- [x] invoke is sometimes a nonvirtual call
- [ ] shlink should inflate segments to page boundaries

### Could, Someday
- [ ] use shadow space for spilling 4 regs
- [ ] more codegen tests
- [ ] codegen test that exercises stack
- [ ] exercise splitter
- [ ] stemu could implement page protection modes on different segmente (e.g. const)
- [ ] generate windows PE from shlink

### Big, Hairy
- [ ] templ
- [ ] gempl
- [x] cop
- [x] array class?
- [ ] passing args into stemu
- [ ] heap class allocation (actually needs a heap in stemu/stanky)
- [ ] DLLs
- [ ] maybe a 'grep' tool that understand the languages--i.e. can skip comments, etc.

### Nostromo
Start building the OS and fill out the language as you go
- [ ] while loop, with names, and break
- [ ] peek/poke
- [ ] register codegen
- [ ] if
- [ ] not
- [ ] strings?  length, index
- [ ] no size for int types means lots of funcs with 8

What is a string?
- it's a type that compiles down to an array and and a set of shtasm intrinsics
- I think I can still get away with widening chars to 64-bits as long as the actual
  array is 8-bit...?  What would an index be?  a move8.  then with an xor?  maybe it's easier to just support 8-bit

### Test strategy

Tests are not free.  Already, test run time is significant.  I want to be thoughtful about avoiding redundant tests.

Tests:
- shtemu / sandbox
  - this isn't really a test but a sandbox for developing new features.  The idea is that it allows "bottom up" addition of features by having a shtasm -> shtemu sandbox, along with a liam -> shtemu sandbox, along with a araceli -> shtemu sandbox.  Changes here are likely transitory.
- assign
   - this is really a front-end test that checks AST parse trees for things like associativity
- test
   - this is really a MVP hello world test
- nostromo
   - this is really a sandbox for future work

So, do all these really qualify as tests?  Do all of these require testing at all levels?  Do levels really need to exercise all artifacts?

Levels include
- AST verification (.ast x 4)
- Araceli verification (.build.bat, .lh, .ls)
- Liam verification (.asm, .lir, .lir-post)
- Shtasm verification (.o, .mc-list, .list)
- Shlink verification (.list, .app)
- Shtemu verification (.log)

Intermediate artifacts are useful for debugging problems, but are they helpful for detecting problems?  I don't think so.

Propsed changes
1. [ ] Reoder tests in order of complexity
1. [x] Add execution to helloWorld.
1. [ ] Limit araceli to the ast level.
1. [\] Disable comparison of all intermediate output for tests that actually run (i.e. capturing only .log).
1. [ ] Rename tests as follows:
    - assign -> **ast**
    - shtemu -> **sandbox**
    - test -> **helloWorld**
    - nostromo -> _no change_
1. [x] Exercise branches by passing command-line input through shtemu
1. [ ] Strive to get execution of nostromo.

[^1]: The name "shtanky" was suggested by my son, Ethan (age 11).  I selected the first of his suggestions that didn't include "my dad's butt" somewhere in the title.

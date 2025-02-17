# Register allocation in liam works in 9 passes

All these passes run per LIR object (e.g. function):

1. Variable Creation (`lirVarGen`)
2. Indicate Instruction Storage Requirements (`instrPrefs`)
3. Move Variables into Storage (`varSplitter`)
4. Resvole Conflicts over Storage (`varCombiner`)
5. Decide Undecided Stack Storage (`stackAllocation`)
6. Decide Undecided Register Storage (`varAllocation`)
7. Elide Artificial Variables (`spuriousVarStripper`)
8. Decide Temporaries for Storage Requirement Implementation (`splitResolver`)
9. Final Generation (`asmCodeGen`)

TODO: maybe add 0 which injects burn and trash arguments in astCodeGen?

## 1. Variable Creation

`lirVarGen` creates variables for each `lirArg`.

In this pass:
- variables are created per LIR object, per argument name (i.e. `name()`)
- `lirArgConst` arguments are set to require immediate storage

Hence the _name_ of an argument denotes what is and is not the same variable.  Variables are the same conceptual value throughout a function.  Variables can also be labels or literals.

TODO: should instances of the same constant be the same variable?

## 2. Indicate Instruction Storage Requirements

In this pass, instructions that have specific requirements on the storage of arguments indicate those requirements.  This is limited to only a handful of instructions:

- `kEnterFunc`, `kCall`, `kSyscall`, and `kRet` require arguments be stored per calling convention

- `kReserve` assumes that all locals are stack-bound (i.e. `kStorageUndecidedStack`)

Calls also have spurious _burn_ and _trash_ arguments, which are assigned storage here:
- if a call has less than 4 arguments, burn arguments were added earlier to make up the difference.  These arguments are always bound to registers per calling convention
- two trash arguments are always present, which are bound to the volatile registers
- burn and trash arguments are removed later by the `spuriousVarStripper`

TODO: why are all locals stack-bound?  Why not support register-based locals?
TODO: what about registers that are input to calls, but still trashed by those calls when the instruction completes?

## 3. Move Variables into Storage

Next, the splitter checks each variable in the LIR object.  For any variable with multiple storage requirements, it
- implements the first demands, if necessary, and
- implements each secondary demand

In all cases, the splitter injects a split or mov instruction to move the variable from it's original location to other locations.

"First demands" refers to the first instruction that has two or more demands on the same variable.  This is restricted to moves from immediate data.  These moves are implemented directly as `kMov` instructions rather than split instructions.

"Secondary demands" refers to each subsequent insruction with any demands at all.  These are implemented by injecting moves to the new storage.

For any split instruction added by the splitter, variables area assigned to the arguments, and destination storage is always set, but source storage never is.  That storage is resolved later, and allows the split resolver to pick the storage of the variable that is most optimal.  This, in fact, is the whole purpose of the split instruction.  `kMov` instructions added by the splitter, do have source storage set.

The splitter never changes the storage demands of existing instructions.

TODO: is it ok to only handle immediate data as first demands?
TODO: shouldn't `requireStorage` be called on `kMov` (not split) for the source?

## 4. Resvole Conflicts over Storage

The varCombiner traverses the instructions in the LIR object, keeping track of all variables which have outstanding references (i.e. are "alive").  If, at any instruction, if there are multiple living variables that in the same storage, the combiner kicks in to resolve the conflict.

(Note that, when the combiner is examining a variable's storage, it is using APIs like `var::getStorageAt` which returns where the variable would be based on its requirements.  That is, just because a variable is in a certain storage an instruction _i_, that doesn't mean that _i_ requires that storage, but rather than _i-1_ did, for example.)

Conflicts are resolved by categorizing the claimants into three buckets: winners, runners-up, and losers:
- winners are variables who whose storage requirement was set by this instruction.
- runners-up are non-winners who require this storage in later instructions, and
- losers are non-winners who don't.

Having anything other than exactly one winner is an insanity.

Both runners-up and losers are ejected from the storage, but runners-up are restored to the storage just before the subsequent instruction that requires it.

Like the splitter, the combiner injects move instructions to implement the changes in storage.  However, the combiner _does_ modify the storage requirements of the original instruction.  This is because the combiner iterates until it find no conflicts, and it changes this storage to cue itself not to run again.

TODO: why must the combiner loop?

## 5. Decide Undecided Stack Storage

The `stackAllocator` simply looks for any variable with a storage set to `kStorageUndecidedStack` and picks free location, growing the local stack as necessary.

## 6. Decide Undecided Register Storage

The `varAllocator` looks for variables that _never_ have _any_ storage indicated, and choose storage for these variables.

Storage is chosen for variables in priority order, where more frequently referenced variables are prioritized first.

Individual storage is chosen using the `varFinder`, with it first being seeded to avoid the storage in use during the lifetime of the variable in question.

Storage chosen by `varFinder::chooseFreeStorage` may be registers, if one is available, or stack.

TODO: why is it safe to add a stack variable at this time?
TODO: understand how/where the varFinder is used better

## 7. Elide Artificial Variables

In this phase, variables that don't fit into the generated assembly are removed.

For example,
- call instructions keep only the call ptr--all inputs are removed
- the return instruction removes all arguments

## 8. Decide Temporaries for Storage Requirement Implementation

The `splitResolver` converts any `kSplit` instruction into a `kMov` instruction, and sets the missing source storage to match the storage the variable has at the previous instruction.  That variable must have exactly one storage or an insanity is triggered.

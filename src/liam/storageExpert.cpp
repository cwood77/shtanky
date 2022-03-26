
// all regs
// used regs - recalculate each time



// for each instr
//  what vars are alive
//    any need space?
//       assign free storage (what storage is free? = living var's combined assignments)
//       or alloc stack
//    any have duplicate space?
//       evict somebody to stack
//    any have unimplementable overloads? - I don't even know if this is possible
//       generate a mov

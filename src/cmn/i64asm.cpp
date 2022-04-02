#include "i64asm.hpp"

namespace cmn {
namespace tgt {
namespace i64 {

static const genInfo kGen[] = {

   // some examples from MSVC disassembly
   // 48 83 ec 28    == sub rsp,28h
   // e8 10 00 00 00 == call 07FF7F5891020h
   // 48 83 c4 28    == add rsp,28h
   // c3             == ret

   //                                                      *see kRmYes
   //         guid            rex        opcode       co  M*  SIB  di imm
   { "SUB{REX.W + 83 /5 ib}", 0x48, {0x83,   0,   0},  0, 5, false, 0 ,1 }, // r/m64, imm8
   { "CALL{E8 cd}",              0, {0xE8,   0,   0},  4, 0, false, 0 ,0 },
   { "ADD{REX.W + 83 /0 ib}", 0x48, {0x83,   0,   0},  0, 8, false, 0, 1 }, // r/m64, imm8
   { "RET{near}",                0, {0xC3,   0,   0},  0, 0, false, 0, 0 },

   { "",                         0, {   0,   0,   0},  0, 0, false, 0, 0 },

};

const genInfo *getGenInfo() { return kGen; }

} // namespace i64
} // namespace tgt
} // namespace cmn

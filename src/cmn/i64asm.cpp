#include "i64asm.hpp"
#include <stdexcept>

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

   { NULL,                       0, {   0,   0,   0},  0, 0, false, 0, 0 },

};

const genInfo *getGenInfo() { return kGen; }

static const genInfo2 kGen2[] = {
   /*
   { "SUB{REX.W + 83 /5 ib}", (unsigned char[]){
      genInfo2::kFixedByte, 0x48,
      genInfo2::kFixedByte, 0x83,
      genInfo2::kModRMArg1FixedByte, 0x5,
      genInfo2::kArg2Imm8,
      genInfo2::kEndOfInstr
   } },
   { "CALL{E8 cd}", (unsigned char[]){
      genInfo2::kFixedByte, 0xe8,
      genInfo2::kArg1RipRelAddr32,
      genInfo2::kEndOfInstr
   } },
   { "ADD{REX.W + 83 /0 ib}", (unsigned char[]){
      genInfo2::kFixedByte, 0x48,
      genInfo2::kFixedByte, 0x83,
      genInfo2::kModRMArg1FixedByte, 0x0,
      genInfo2::kArg2Imm8,
      genInfo2::kEndOfInstr
   } },
   { "RET{near}", (unsigned char[]){
      genInfo2::kFixedByte, 0xC3,
      genInfo2::kEndOfInstr
   } },
   */

   { "PUSH{FF /6}", (unsigned char[]){
      genInfo2::kFixedByte, 0xFF,
      genInfo2::kModRMArg1FixedByte, 0x6
   },
   { genInfo2::kModRmR, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },
};

argTypes asmArgInfo::computeArgType()
{
   if(flags & kReg64)
      return kR64;
   else
      throw std::runtime_error("can't compute arg type in " __FILE__);
}

} // namespace i64
} // namespace tgt
} // namespace cmn

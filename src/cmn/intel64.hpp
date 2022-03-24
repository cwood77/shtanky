#pragma once
#include "target.hpp"

namespace cmn {
namespace tgt {
namespace i64 {

   // these are flags to support trash, reqs

// all regs support 64, 32, 16, 8 bits
enum {
   kNone;

   // supports TWO 8 bit (high byte unsupported by shtanky currently)
   kRegA;
   kRegB;
   kRegC;
   kRegD;

   // supports ONE 8 bit
   kRegBP;
   kRegSP;

   kRegSI;
   kRegDI;

   kReg8;
   kReg9;
   kReg10;
   kReg11;
   kReg12;
   kReg13;
   kReg14;

   kFirstStackVar;
};

class targetInfo : public iTargetInfo {
};

} // namespace i64
} // namespace tgt
} // namespace cmn

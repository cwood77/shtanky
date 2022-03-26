#pragma once
#include "target.hpp"

namespace cmn {
namespace tgt {
namespace i64 {

   // these are flags to support trash, reqs

// all regs support 64, 32, 16, 8 bits
enum {
   kNone,

   // supports TWO 8 bit (high byte unsupported by shtanky currently)
   kRegA,
   kRegB,
   kRegC,
   kRegD,

   // supports ONE 8 bit
   kRegBP,
   kRegSP,

   kRegSI,
   kRegDI,

   kReg8,
   kReg9,
   kReg10,
   kReg11,
   kReg12,
   kReg13,
   kReg14,
   kReg15,
};

                  //RSP+8 = RCX

} // namespace i64

class shtankyCallingConvention : public tgt::iCallingConvention {
};

class w64CallingConvention : public tgt::iCallingConvention {
public:
   virtual bool stackArgsPushRToL() const;
   virtual size_t getShadowSpace() const;
   virtual void getRValAndArgBank(std::vector<size_t>& v) const;
   virtual void getTrashBank(std::vector<size_t>& v) const;
};

class w64EmuSyscall : public tgt::iSyscallConvention {
};

class w64EnumTargetInfo : public tgt::iTargetInfo {
public:
   virtual const iProcessorInfo& getProc() const;
   virtual const iCallingConvention& getCallConvention() const { return m_cc; }
   virtual const iSyscallConvention& getSyscallConvention() const;

private:
   w64CallingConvention m_cc;
};

} // namespace tgt
} // namespace cmn

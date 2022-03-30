#pragma once
#include "target.hpp"

namespace cmn {
namespace tgt {
namespace i64 {

   // these are flags to support trash, reqs

// all regs support 64, 32, 16, 8 bits
enum {
   // supports TWO 8 bit (high byte unsupported by shtanky currently)
   kRegA = _kStorageFirstReg, // 23
   kRegB,                     // 24
   kRegC,                     // 25
   kRegD,

   // supports ONE 8 bit
   kRegBP,
   kRegSP,

   kRegSI,
   kRegDI,

   kReg8,  // 31
   kReg9,  // 32
   kReg10, // 33
   kReg11,
   kReg12,
   kReg13,
   kReg14,
   kReg15,
};

                  //RSP+8 = RCX

} // namespace i64

class x8664Processor : public tgt::iProcessorInfo {
public:
   virtual void createRegisterBank(std::vector<size_t>& v) const;
   virtual void createRegisterMap(std::map<size_t,size_t>& m) const;
   virtual const char *getRegName(size_t r) const;
   virtual const instrInfo *getInstr(instrIds i) const;
};

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
   virtual size_t getSize(size_t s) const { return s==0 ? 8 : s; } // 64-bit target
   virtual const iProcessorInfo& getProc() const { return m_proc; }
   virtual const iCallingConvention& getCallConvention() const { return m_cc; }
   virtual const iSyscallConvention& getSyscallConvention() const;

private:
   x8664Processor m_proc;
   w64CallingConvention m_cc;
};

} // namespace tgt
} // namespace cmn

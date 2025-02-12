#pragma once
#include "target.hpp"

namespace cmn {
namespace tgt {
namespace i64 {

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

} // namespace i64

class x8664Processor : public tgt::iProcessorInfo {
public:
   virtual void createRegisterMap(std::map<size_t,size_t>& m) const;
   virtual const char *getRegName(size_t r) const;
   virtual const instrInfo *getInstr(instrIds i) const;
};

class w64CallingConvention : public tgt::iCallingConvention {
public:
   virtual bool stackArgsPushRToL() const { return true; }
   virtual size_t getShadowSpace() const { return 32; }
   virtual size_t getArgumentStackSpace(std::vector<size_t>& v) const;
   virtual void getRValAndArgBank(std::vector<size_t>& v) const;
   virtual bool requiresPrologEpilogSave(size_t r) const;
   virtual bool requiresSubCallSave(size_t r) const;
   virtual void createRegisterBankInPreferredOrder(std::vector<size_t>& v) const;
   virtual void createScratchRegisterBank(std::vector<size_t>& v) const;
   virtual void alignStackForSubcall(const size_t& currentAlignment, size_t& adjustment) const;
};

class w64EmuTargetInfo : public tgt::iTargetInfo {
public:
   virtual size_t getRealSize(size_t s) const { return s==0 ? 8 : s; } // 64-bit target
   virtual const iProcessorInfo& getProc() const { return m_proc; }
   virtual const iCallingConvention& getCallConvention() const { return m_cc; }
   virtual const iSyscallConvention& getSyscallConvention() const;

private:
   x8664Processor m_proc;
   w64CallingConvention m_cc;
};

} // namespace tgt
} // namespace cmn

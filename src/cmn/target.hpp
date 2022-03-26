#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace cmn {
namespace tgt {

enum argTypes {
   kArgTypeNone,

   kR8,
   kR16,
   kR32,
   kR64,

   kI8,
   kI16,
   kI32,
   kI64,

   kM8,
   kM16,
   kM32,
   kM64,
};

enum instrIds {
   kDeclParam,

   kPush,
   kPop,

   kMov,

   kCall,
   kRet,

   kSyscall,
};

class instrFmt {
public:
   argTypes a1;
   argTypes a2;
   argTypes a3;
   argTypes a4;

   size_t reqs1;
   size_t reqs2;
   size_t reqs3;
   size_t reqs4;

   size_t trash;
};

enum {
   kStackStorage = 1
};

class instrInfo {
public:
   const char *name;
   const instrFmt *fmts;
};

// which args are stack and which are reg
// which reg are scratch
// shadow stack space
class iCallingConvention {
public:
   virtual bool stackArgsPushRToL() const = 0;
   virtual size_t getShadowSpace() const = 0;
   //virtual size_t getRegisterHome(size_t r) const = 0;
   virtual void getRValAndArgBank(std::vector<size_t>& v) const = 0;
   virtual void getTrashBank(std::vector<size_t>& v) const = 0;
};

class iSyscallConvention : public iCallingConvention {
public:
   virtual instrIds getOpCode(std::string& thunk) const = 0;
};

class iProcessorInfo {
public:
   virtual void createRegisterBank(std::vector<size_t>& v) const = 0;
   virtual const char *getRegName(size_t r) const = 0;
   virtual const instrInfo *getInstr(instrIds i) const = 0;
};

class iTargetInfo {
public:
   virtual const iProcessorInfo& getProc() const = 0;
   virtual const iCallingConvention& getCallConvention() const = 0;
   virtual const iSyscallConvention& getSyscallConvention() const = 0;
};

} // namespace tgt

} // namespace cmn

#pragma once
#include <cstddef>
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

class instrInfo {
public:
   const char *name;
   const instrFmt *fmts;
};

class iTargetInfo {
public:
   virtual void createRegisterBank(std::vector<size_t>& v) const = 0;
   virtual const char *getRegName(size_t r) const = 0;
   virtual const instrInfo *getInstr(instrIds i) const = 0;
};

} // namespace tgt

} // namespace cmn

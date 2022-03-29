#include "intel64.hpp"

namespace cmn {
namespace tgt {

void x8664Processor::createRegisterBank(std::vector<size_t>& v) const
{
   v.push_back(i64::kRegB);
   v.push_back(i64::kReg10);
   v.push_back(i64::kReg11);
   v.push_back(i64::kReg12);
   v.push_back(i64::kReg13);
   v.push_back(i64::kReg14);
   v.push_back(i64::kReg15);
   v.push_back(i64::kRegSI);
   v.push_back(i64::kRegDI);

   // used in cc
   v.push_back(i64::kRegC);
   v.push_back(i64::kRegD);
   v.push_back(i64::kReg8);
   v.push_back(i64::kReg9);
   v.push_back(i64::kRegA);
}

void x8664Processor::createRegisterMap(std::map<size_t,size_t>& m) const
{
   m[i64::kRegA];
   m[i64::kRegB];
   m[i64::kRegC];
   m[i64::kRegD];

   m[i64::kRegBP];
   m[i64::kRegSP];

   m[i64::kRegSI];
   m[i64::kRegDI];

   m[i64::kReg8];
   m[i64::kReg9];
   m[i64::kReg10];
   m[i64::kReg11];
   m[i64::kReg12];
   m[i64::kReg13];
   m[i64::kReg14];
   m[i64::kReg15];
}

const char *x8664Processor::getRegName(size_t r) const
{
   throw 3.14;
}

const instrInfo *x8664Processor::getInstr(instrIds i) const
{
   throw 3.14;
}

bool w64CallingConvention::stackArgsPushRToL() const
{
   return true;
}

size_t w64CallingConvention::getShadowSpace() const
{
   return 32;
}

void w64CallingConvention::getRValAndArgBank(std::vector<size_t>& v) const
{
   v.reserve(5);
   v.push_back(i64::kRegA);
   v.push_back(i64::kRegC);
   v.push_back(i64::kRegD);
   v.push_back(i64::kReg8);
   v.push_back(i64::kReg9);
}

void w64CallingConvention::getTrashBank(std::vector<size_t>& v) const
{
   v.reserve(7);
   v.push_back(i64::kRegA);
   v.push_back(i64::kRegC);
   v.push_back(i64::kRegD);
   v.push_back(i64::kReg8);
   v.push_back(i64::kReg9);
   v.push_back(i64::kReg10);
   v.push_back(i64::kReg11);
}

const iSyscallConvention& w64EnumTargetInfo::getSyscallConvention() const
{
   throw 3.14;
}

} // namespace tgt
} // namespace cmn

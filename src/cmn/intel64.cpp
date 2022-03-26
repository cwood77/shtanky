#include "intel64.hpp"

namespace cmn {
namespace tgt {

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

const iProcessorInfo& w64EnumTargetInfo::getProc() const
{
   throw 3.14;
}

const iSyscallConvention& w64EnumTargetInfo::getSyscallConvention() const
{
   throw 3.14;
}

} // namespace tgt
} // namespace cmn

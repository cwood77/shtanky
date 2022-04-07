#include "target.hpp"
#include <stdexcept>

namespace cmn {
namespace tgt {

const instrFmt& instrInfo::demandFmt(const std::vector<argTypes> a) const
{
   argTypes a1 = a.size() > 0 ? a[0] : kArgTypeNone;
   argTypes a2 = a.size() > 1 ? a[1] : kArgTypeNone;
   argTypes a3 = a.size() > 2 ? a[2] : kArgTypeNone;
   argTypes a4 = a.size() > 3 ? a[3] : kArgTypeNone;
   if(a.size() > 4)
      throw std::runtime_error("4+ args not implemented");

   const instrFmt *pFmt = fmts;
   while(true)
   {
      if(!pFmt || pFmt->guid == NULL)
         throw std::runtime_error("instr format not found");

      if((pFmt->a1 & a1) &&
         (pFmt->a2 & a2) &&
         (pFmt->a3 & a3) &&
         (pFmt->a4 & a4))
         return *pFmt;

      pFmt++;
   }
}

argTypes asmArgInfo::computeArgType()
{
   if(flags & kMem8)
      return kM8;
   else if(flags & kReg64)
      return kR64;
   else if(flags & kImm8)
      return kI8;
   else
      throw std::runtime_error("can't compute arg type in " __FILE__);
}

} // namespace tgt
} // namespace cmn

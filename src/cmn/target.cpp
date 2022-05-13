#include "target.hpp"
#include "throw.hpp"
#include <stdexcept>

namespace cmn {
namespace tgt {

instrFmt::instrFmt(const char *g, size_t a1, size_t a2, size_t a3, size_t a4, const char *io, bool ss)
: guid(g), argIo(io), stackSensitive(ss), a1(a1), a2(a2), a3(a3), a4(a4)
{
}

size_t getMatchPattern(argTypes a)
{
   size_t rVal = 0;
   switch(a)
   {
      // widen R8 --> ... --> R64
      case argTypes::kR8:
         rVal |= argTypes::kR8;
      case argTypes::kR16:
         rVal |= argTypes::kR16;
      case argTypes::kR32:
         rVal |= argTypes::kR32;
      case argTypes::kR64:
         rVal |= argTypes::kR64;
         break;

      // widen I8 --> ... --> I64
      case argTypes::kI8:
         rVal |= argTypes::kI8;
      case argTypes::kI16:
         rVal |= argTypes::kI16;
      case argTypes::kI32:
         rVal |= argTypes::kI32;
      case argTypes::kI64:
         rVal |= argTypes::kI64;
         break;

      // no widening
      case argTypes::kArgTypeNone:
      case argTypes::kM8:
      case argTypes::kM16:
      case argTypes::kM32:
      case argTypes::kM64:
         rVal = a;
         break;

      default:
         cdwTHROW("ISE");
   }
   return rVal;
}

const instrFmt& instrInfo::demandFmt(const std::vector<argTypes> a) const
{
   const instrFmt *f = findFmt(a);
   if(!f)
      cdwTHROW("instr format not found");
   return *f;
}

const instrFmt *instrInfo::findFmt(const std::vector<argTypes> a) const
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
         return NULL;

      if((pFmt->a1 & getMatchPattern(a1)) &&
         (pFmt->a2 & getMatchPattern(a2)) &&
         (pFmt->a3 & getMatchPattern(a3)) &&
         (pFmt->a4 & getMatchPattern(a4)))
         return pFmt;

      pFmt++;
   }
}

asmArgInfo::asmArgInfo()
: flags(0), disp(0)
{
   data.qwords.v[0] = 0;
}

argTypes asmArgInfo::computeArgType()
{
   if(flags == (kImm32 | kLabel))
      return kI32;
   else if(flags == (kMem64 | kLabel))
      return kM64;
   else if(flags & kLabel)
      cdwTHROW("unimpl'd label format");

   else if(flags & kImm8)
      return kI8;
   else if(flags & kImm16)
      return kI16;
   else if(flags & kImm32)
      return kI32;
   else if(flags & kImm64)
      return kI64;

   else if(flags & kMem64)
      return kM64;

   else if(flags & kReg64) // consider REGS last, as they can be ORd with memory
      return kR64;

   else
      cdwTHROW("can't compute arg type");
}

const char *iProcessorInfo::getRegName(size_t r) const
{
   switch(r)
   {
      case kStorageUnassigned:
         return "<unassigned>";
         break;
      case kStorageImmediate:
         return "<immediate>";
         break;
      case kStorageUndecidedStack:
         return "<undecided-stack>";
         break;
      default:
         cdwTHROW("unknown reg");
   }
}

} // namespace tgt
} // namespace cmn

#include "../cmn/target.hpp"
#include "varFinder.hpp"

namespace liam {

void varFinder::resetUsedStorage()
{
   m_inUse.clear();
   m_t.getProc().createRegisterMap(m_inUse);
}

void varFinder::recordStorageUsed(size_t s)
{
   m_inUse[s]++;
   m_regsUsed.insert(s);
}

size_t varFinder::chooseFreeStorage(size_t pseudoSize)
{
   size_t reg = 0;
   if(tryFindFreeRegister(reg))
      return reg;

   // use a stack slot
   return decideStackStorage(pseudoSize);
}

size_t varFinder::decideStackStorage(size_t pseudoSize)
{
   size_t actualSize = m_t.getRealSize(pseudoSize);
   size_t s =  cmn::tgt::makeStackStorage(m_stackLocalSpace-actualSize);
   m_stackLocalSpace += actualSize;
   recordStorageUsed(cmn::tgt::kStorageStackFramePtr);
   return s;
}

size_t varFinder::pickScratchRegister(bool& needsSpill)
{
   std::vector<size_t> bank;
   m_t.getCallConvention().createScratchRegisterBank(bank);
   for(size_t i=0;i<bank.size();i++)
   {
      if(m_inUse[bank[i]]==0)
      {
         needsSpill = false;
         return bank[i];
      }
   }

   // if all else fails, require a spill
   needsSpill = true;
   return bank[0];
}

bool varFinder::tryFindFreeRegister(size_t& reg)
{
   std::vector<size_t> regs;
   m_t.getCallConvention().createRegisterBankInPreferredOrder(regs);

   for(size_t i=0;i<regs.size();i++)
   {
      if(m_inUse[regs[i]]==0)
      {
         recordStorageUsed(regs[i]);
         reg = regs[i];
         return true;
      }
   }

   return false;
}

} // namespace liam

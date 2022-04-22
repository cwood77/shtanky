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
   std::vector<size_t> regs;
   m_t.getCallConvention().createRegisterBankInPreferredOrder(regs);

   for(size_t i=0;i<regs.size();i++)
   {
      if(m_inUse[regs[i]]==0)
      {
         recordStorageUsed(regs[i]);
         return regs[i];
      }
   }

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

} // namespace liam

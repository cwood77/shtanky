#include "../cmn/target.hpp"
#include "varFinder.hpp"
#include <stdexcept>

namespace liam {

void varFinder::reset()
{
   m_inUse.clear();
   m_t.getProc().createRegisterMap(m_inUse);
}

size_t varFinder::chooseFreeStorage()
{
   std::vector<size_t> regs;
   m_t.getProc().createRegisterBank(regs);

   for(size_t i=0;i<regs.size();i++)
   {
      if(m_inUse[regs[i]]==0)
      {
         recordStorageUsed(regs[i]);
         return regs[i];
      }
   }

   throw std::runtime_error("no free register in combine");
}

} // namespace liam

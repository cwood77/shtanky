#include "../cmn/target.hpp"
#include "varFinder.hpp"
#include <stdexcept>

namespace liam {

size_t varFinder::chooseFreeStorage(std::map<size_t,size_t>& inUse)
{
   std::vector<size_t> regs;
   m_t.getProc().createRegisterBank(regs);

   for(size_t i=0;i<regs.size();i++)
      if(inUse[i]==0)
         return i;

   throw std::runtime_error("no free register in combine");
}

} // namespace liam

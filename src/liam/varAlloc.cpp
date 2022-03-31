#include "lir.hpp"
#include "varAlloc.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"
#include <stdio.h>

namespace liam {

void varAllocator::onInstrWithAvailVar(lirInstr& i)
{
   availVarPass::onInstrWithAvailVar(i);

   for(auto it=m_v.all().begin();it!=m_v.all().end();++it)
   {
      var& vr = *it->second;
      if(vr.isAlive(i.orderNum))
      {
         auto storage = vr.getStorageAt(i.orderNum);
         if(storage.size() == 0)
         {
            // alive variable with no assigned storage... assign some now
            size_t stor = m_f.chooseFreeStorage(vr.getSize());
            vr.requireStorage(i,stor);
            ::printf("[varAlloc] picking %lld for var %s\n",stor,vr.name.c_str());
         }
      }
   }
}

} // namespace liam

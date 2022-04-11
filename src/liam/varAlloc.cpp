#include "../cmn/trace.hpp"
#include "lir.hpp"
#include "varAlloc.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"

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

            size_t stor = cmn::tgt::kStorageUnassigned;
            if(dynamic_cast<const lirArgConst*>(&vr.lastArg()))
            {
               // this is a constant... use immediate
               stor = cmn::tgt::kStorageImmediate;
               cdwVERBOSE("[varAlloc] picking IMMEDIATE for var %s\n",vr.name.c_str());
            }
            else
            {
               stor = m_f.chooseFreeStorage(vr.getSize());
               cdwVERBOSE("[varAlloc] picking %lld for var %s\n",stor,vr.name.c_str());
            }

            vr.requireStorage(i,stor);
         }
      }
   }
}

} // namespace liam

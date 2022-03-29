#include "../cmn/target.hpp"
#include "availVarPass.hpp"
#include "lir.hpp"
#include "varGen.hpp"
#include <exception>

namespace liam {

class restartSignal : public std::exception {};

void availVarPass::run()
{
   lirInstr *pInstr = &m_s.pTail->head();
   while(true)
   {
      try
      {
         onInstr(*pInstr);
      }
      catch(restartSignal&)
      {
         pInstr = &m_s.pTail->head();
         continue;
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

void availVarPass::onInstr(lirInstr& i)
{
   m_inUse.clear();
   m_t.getProc().createRegisterMap(m_inUse);

   for(auto it=m_v.all().begin();it!=m_v.all().end();++it)
   {
      // for each var

      var& vr = *it->second;
      if(vr.isAlive(i.orderNum))
      {
         // for each var storage

         auto storage = vr.getStorageAt(i.orderNum);
         for(auto jit=storage.begin();jit!=storage.end();++jit)
            onInstrStorage(i,*jit);
      }
   }

   onInstrWithAvailVar(i);
}

void availVarPass::onInstrStorage(lirInstr& i, size_t storage)
{
   m_inUse[storage]++;
}

void availVarPass::restart()
{
   throw restartSignal();
}

} // namespace liam

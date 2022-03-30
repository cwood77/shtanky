#include "../cmn/target.hpp"
#include "availVarPass.hpp"
#include "lir.hpp"
#include "varFinder.hpp"
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
   m_f.reset();

   for(auto it=m_v.all().begin();it!=m_v.all().end();++it)
   {
      var& vr = *it->second;
      if(vr.isAlive(i.orderNum))
         onLivingVar(i,vr);
   }

   onInstrWithAvailVar(i);
}

void availVarPass::onLivingVar(lirInstr& i, var& v)
{
   auto storage = v.getStorageAt(i.orderNum);
   for(auto jit=storage.begin();jit!=storage.end();++jit)
      onInstrStorage(i,v,*jit);
}

void availVarPass::onInstrStorage(lirInstr& i, var& v, size_t storage)
{
   m_f.recordStorageUsed(storage);
}

void availVarPass::restart()
{
   throw restartSignal();
}

} // namespace liam

#include "lir.hpp"
#include "varGen.hpp"
#include "varSplitter.hpp"
#include <stdio.h>

namespace liam {

void varSplitter::split(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
{
   varSplitter self(s,v,t);

   for(auto it=v.all().begin();it!=v.all().end();++it)
      self.checkVar(*it->second);
}

void varSplitter::checkVar(var& v)
{
   if(v.storageToInstrMap.size() > 1)
   {
      // this var as at least two different storage demands

      auto it=v.instrToStorageMap.begin();
      //size_t prevI = it->first;
      auto pPrevSs = &it->second;
      for(++it;it!=v.instrToStorageMap.end();++it)
      {
         size_t currI = it->first;
         auto pCurrSs = &it->second;

         for(auto currS=pCurrSs->begin();currS!=pCurrSs->end();++currS)
         {
            if(pPrevSs->find(*currS)!=pPrevSs->end())
               // this instruction wants the storage the var already has
               ;
            else
            {
               size_t prevS = *pPrevSs->begin();

               // move from prevS -> currS
               emitMoveBefore(v,currI,prevS,*currS);
            }
         }

         //prevI = currI;
         pPrevSs = pCurrSs;
      }
   }

   // update the table _after_ iterating on it
   for(auto it=m_newInstrs.begin();it!=m_newInstrs.end();++it)
      v.requireStorage(*it->first,it->second);
}

void varSplitter::emitMoveBefore(var& v, size_t orderNum, size_t srcStor, size_t destStor)
{
   ::printf("emitting move for split!\n");

   auto& mov = m_s.pTail->head().search(orderNum).injectBefore(cmn::tgt::kMov);
   auto& dest = mov.addArg<lirArgVar>("spltD",v.getSize());
   auto& src = mov.addArg<lirArgVar>("spltS",v.getSize());

   v.refs[mov.orderNum].push_back(&dest);
   v.refs[mov.orderNum].push_back(&src);

   m_newInstrs.push_back(std::make_pair<lirInstr*,size_t>(&mov,(size_t)srcStor));
   m_newInstrs.push_back(std::make_pair<lirInstr*,size_t>(&mov,(size_t)destStor));

   v.storageDisambiguators[&src] = srcStor;
   v.storageDisambiguators[&dest] = destStor;
}

} // namespace liam

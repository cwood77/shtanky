#include "../cmn/target.hpp"
#include "lir.hpp"
#include "varCombiner.hpp"
#include "varGen.hpp"
#include <stdexcept>
#include <stdio.h>

namespace liam {

size_t chooseFreeStorage(cmn::tgt::iTargetInfo& t, std::map<size_t,size_t>& inUse)
{
   std::vector<size_t> regs;
   t.getProc().createRegisterBank(regs);

   for(size_t i=0;i<regs.size();i++)
      if(inUse[i]==0)
         return i;

   throw std::runtime_error("no free register in combine");
}

void varCombiner::combine(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
{
   varCombiner self(s,v,t);

   lirInstr *pInstr = &s.pTail->head();
   while(true)
   {
      // for each instr

      std::map<size_t,size_t> inUse;
      t.getProc().createRegisterMap(inUse);
      std::map<size_t,std::set<var*> > clients;

      for(auto it=v.all().begin();it!=v.all().end();++it)
      {
         // for each var

         var& vr = *it->second;
         if(vr.isAlive(pInstr->orderNum))
         {
            // for each var storage

            auto storage = vr.getStorageAt(pInstr->orderNum);
            for(auto jit=storage.begin();jit!=storage.end();++jit)
            {
               // record usage
               inUse[*jit]++;
               clients[*jit].insert(it->second);
            }
         }
      }

      for(auto it=clients.begin();it!=clients.end();++it)
      {
         // for each storage

         if(it->second.size() > 1)
         {
            // two or more variables claimed this spot!

            // pick one to be the winner
            //
            // a winner is the _only_ variable that has a storage requirement _after_ this
            // instr
            std::set<var*> losers = it->second;
            var *pWinner = NULL;
            for(auto jit=losers.begin();jit!=losers.end();++jit)
            {
               if((*jit)->requiresStorageLater(pInstr->orderNum,it->first))
               {
                  if(pWinner)
                     throw std::runtime_error("varCombined failed: multiple winners");
                  pWinner = *jit;
               }
            }
            losers.erase(pWinner);

            // the winner gets to keep the storage

            for(auto jit=losers.begin();jit!=losers.end();++jit)
            {
               // for each loser

               ::printf("[varCombiner] handling loser!\n");

               // stash loser to a free storage loc and fixup subsequent refs
               size_t altStorage = chooseFreeStorage(t,inUse);
               (*jit)->updateStorageHereAndAfter(*pInstr,it->first,altStorage);

               // emit a move to implement this
               {
                  auto& mov = pInstr->injectBefore(cmn::tgt::kMov);
                  auto& dest = pInstr->addArg<lirArgVar>(":combDest",0);
                  auto& src = pInstr->addArg<lirArgVar>(":combSrc",0);

                  (*jit)->refs[mov.orderNum].push_back(&dest);
                  (*jit)->refs[mov.orderNum].push_back(&src);

                  (*jit)->requireStorage(mov,it->first);
                  (*jit)->requireStorage(mov,altStorage);

                  (*jit)->storageDisambiguators[&dest] = altStorage;
                  (*jit)->storageDisambiguators[&src] = it->first;
               }
            }

            // restart algorithm
            // ... I think
            pInstr = &s.pTail->head();
            continue;
         }
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

} // namespace liam

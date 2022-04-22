#include "../cmn/fmt.hpp"
#include "../cmn/target.hpp"
#include "../cmn/trace.hpp"
#include "lir.hpp"
#include "varCombiner.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"
#include <stdexcept>
#include <stdio.h>

namespace liam {

bool varCombiner::isInfiniteStorage(size_t s)
{
   return

      // immediate args can't collide with one another
      s == cmn::tgt::kStorageImmediate ||

      // the stack is virtually infinite, especially when slots
      // haven't even been picked
      s == cmn::tgt::kStorageUndecidedStack;
}

void varCombiner::onInstr(lirInstr& i)
{
   m_clients.clear();
   availVarPass::onInstr(i);
}

void varCombiner::onInstrStorage(lirInstr& i, var& v, size_t storage)
{
   m_clients[storage].insert(&v);
   availVarPass::onInstrStorage(i,v,storage);
}

void varCombiner::onInstrWithAvailVar(lirInstr& i)
{
   for(auto it=m_clients.begin();it!=m_clients.end();++it)
   {
      // for each storage

      if(it->second.size() > 1 && !isInfiniteStorage(it->first))
      {
         // two or more variables claimed this spot!
         cdwDEBUG("%d variables claimed storage %lld!\n",it->second.size(),it->first);
         cdwDEBUG("   when handling instr %llu\n",i.orderNum);

         // pick one to be the winner
         //
         // a winner is the _only_ variable that has a storage requirement _after_ this
         // instr
         std::set<var*> losers = it->second;
         var *pWinner = NULL;
         for(auto jit=losers.begin();jit!=losers.end();++jit)
         {
            if((*jit)->requiresStorageLater(i.orderNum,it->first))
            {
               if(pWinner)
                  throw std::runtime_error("varCombined failed: multiple winners");
               pWinner = *jit;
            }
         }
         losers.erase(pWinner);

         // the winner gets to keep the storage
         cdwDEBUG("   winner is %s\n",pWinner->name.c_str());

         for(auto jit=losers.begin();jit!=losers.end();++jit)
         {
            // for each loser

            cdwDEBUG("   handling loser %s\n",(*jit)->name.c_str());

            // stash loser to a free storage loc and fixup subsequent refs
            size_t altStorage = m_f.chooseFreeStorage((*jit)->getSize());
            (*jit)->updateStorageHereAndAfter(i,it->first,altStorage);
            cdwDEBUG("      evicting to %lld\n",altStorage);

            // emit a move to implement this
            {
               auto& mov = i.injectBefore(*new lirInstr(cmn::tgt::kMov));
               mov.comment =  cmn::fmt("      (preserve) [combiner]");
               auto& dest = mov.addArg<lirArgVar>(":combDest",0);
               auto& src = mov.addArg<lirArgVar>(":combSrc",0);

               (*jit)->refs[mov.orderNum].push_back(&dest);
               (*jit)->refs[mov.orderNum].push_back(&src);

               (*jit)->requireStorage(mov.orderNum,it->first);
               (*jit)->requireStorage(mov.orderNum,altStorage);

               (*jit)->storageDisambiguators[&dest] = altStorage;
               (*jit)->storageDisambiguators[&src] = it->first;
            }
         }

         // restart algorithm
         restart();
      }
   }
}

} // namespace liam

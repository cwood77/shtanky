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

      if(it->second.size() <= 1 || isInfiniteStorage(it->first))
         continue;

      // two or more variables claimed this spot!
      cdwDEBUG("%d>1 variables claimed storage %lld!\n",it->second.size(),it->first);
      cdwDEBUG("   when handling instr %llu\n",i.orderNum);

      // categorize all clients as winners or losers; winners require
      // this storage later whereas losers do not
      std::set<var*> winners;
      std::map<size_t,var*> winnerMap;
      std::set<var*> losers;
      for(auto jit=it->second.begin();jit!=it->second.end();++jit)
      {
         size_t priority = (*jit)->requiresStorageNext(i.orderNum,it->first);

         cdwDEBUG("   %s requires storage %lld after instr #%lld at #%lld\n",
            (*jit)->name.c_str(),
            it->first,
            i.orderNum,
            priority);

         if(priority)
         {
            winnerMap[priority] = *jit;
            winners.insert(*jit);
         }
         else
            losers.insert(*jit);
      }
      if(winnerMap.size() != winners.size())
         cdwTHROW("insanity!  multiple winners with same priority?");

      // the winner gets to keep the storage
      // pick the winner that needs the storage soonest
      if(!winnerMap.size())
         cdwTHROW("insanity!");
      var *pWinner = winnerMap.begin()->second;
      cdwDEBUG("   winner is %s\n",pWinner->name.c_str());

      // move the runners-up to losers
      for(auto jit=winners.begin();jit!=winners.end();++jit)
         if(*jit != pWinner)
            losers.insert(*jit);

      // evict all the losers (and runners-up)
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

      // restore runners-up
      if(winners.size() > 1)
         cdwTHROW("runners up isn't implemented yet");

      // restart algorithm
      restart();
   }
}

} // namespace liam

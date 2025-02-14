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
         continue; // no collision!

      // collision: two or more variables claimed this spot at the same time!
      cdwDEBUG("%d>1 variables claimed storage %lld!\n",it->second.size(),it->first);
      cdwDEBUG("   when handling instr %llu\n",i.orderNum);

      resolveCollision(i,it->first,it->second);
   }
}

void varCombiner::resolveCollision(lirInstr& i, size_t storage, std::set<var*>& vars)
{
   // categorize all clients as winners, losers, or runners-up: winners keep the
   // storage; losers are evicted elsewhere, runners up are evicted but restored to
   // the storage later
   std::map<size_t,var*> runnerUpMap;
   std::set<var*> losers;
   std::set<var*> winners;
   for(auto pVar=vars.begin();pVar!=vars.end();++pVar)
   {
      // i.e. if this variable's most previous storage is different (or unspecified)
      //      i.e. this instruction is the _first_ to introduce this requirement
      //      TODO > what if the most recent instr has no reqs?
      if(!(*pVar)->alreadyWantedStorage(i.orderNum,storage))
         winners.insert(*pVar);
      else
      {
         // i.e. where is this storage required next after i.orderNum (0==never)
         size_t priority = (*pVar)->requiresStorageNext(i.orderNum,storage);

         cdwDEBUG("   %s requires storage %lld after instr #%lld at #%lld\n",
            (*pVar)->name.c_str(),
            storage,
            i.orderNum,
            priority);

         if(priority)
            runnerUpMap[priority] = *pVar;
         losers.insert(*pVar);
      }
   }

   // pick a winner
   // the winner _must_ be the only var that is introducing a requirement now (i.e.
   // not before this instruction).  This is because if such a variable lost,
   // the preserve instruction inject before this instruction would trigger another
   // conflict, leading to an infinite chain of conflicts.
   var *pWinner = NULL;
   if(winners.size() == 1)
      pWinner = *winners.begin();
   else
      cdwTHROW("insanity!  winners size is %lld",winners.size());
   cdwDEBUG("   winner is %s\n",pWinner->name.c_str());
   //pWinner->requireStorage(i.orderNum,storage);

   // evict all the losers (and runners-up)
   std::map<var*,size_t> altStorageMap;
   for(auto jit=losers.begin();jit!=losers.end();++jit)
   {
      // for each loser

      cdwDEBUG("   handling loser %s\n",(*jit)->name.c_str());

      // stash loser to a free storage loc and fixup subsequent refs
      size_t altStorage = m_f.chooseFreeStorage((*jit)->getSize());
//            updateStorageHereAndAfter is weirdly named
//              - if instr i doesn't have a storage req on old, add one for nu
      // TODO why can't I remove this?
      // TODO ... or, at least, just do a change storage?
      (*jit)->requireNewStorageIfNotOld(i,storage,altStorage);
      //(*jit)->changeStorage(i.orderNum,storage,altStorage);
      cdwDEBUG("      evicting to %lld\n",altStorage);

      // emit a move to implement this
      emitMoveBefore(i,**jit,"(preserve) [combiner]",storage,altStorage);
      altStorageMap[*jit] = altStorage;
   }

   // placate runners-up
   // for runners up, I just hosed them by evicting them elsewhere
   // un-evict them before their storage requirement is needed again
   for(auto jit=runnerUpMap.begin();jit!=runnerUpMap.end();++jit)
   {
      if(jit->second == pWinner)
         continue;

      // emit a move back
      lirInstr& antecedent = i.tail().searchUp([&](auto& i)
         { return i.orderNum == jit->first; });

      emitMoveBefore(
         antecedent,*jit->second,
         "(restore [combiner])",
         altStorageMap[jit->second],storage);
   }

   // restart algorithm
   restart();
}

void varCombiner::emitMoveBefore(lirInstr& antecedent, var& v, const std::string& comment, size_t srcStorage, size_t destStorage)
{
   auto& mov = antecedent.injectBefore(*new lirInstr(cmn::tgt::kMov));
   mov.comment =  comment;
   auto& dest = mov.addArg<lirArgVar>(":combDest",0);
   auto& src = mov.addArg<lirArgVar>(":combSrc",0);

   v.refs[mov.orderNum].push_back(&src);
   v.refs[mov.orderNum].push_back(&dest);

   v.requireStorage(mov.orderNum,srcStorage);
   v.requireStorage(mov.orderNum,destStorage);

   v.storageDisambiguators[&src] = srcStorage;
   v.storageDisambiguators[&dest] = destStorage;
}

} // namespace liam

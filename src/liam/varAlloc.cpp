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
               // TODO HACK - where is this now? In varGen
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
         else if(storage.size() == 1
               && *storage.begin() == cmn::tgt::kStorageUndecidedStack)
         {
            // TODO HACK - where is this now? NOWHERE!
            // this guy asked for a stack slot, but didn't care where
            auto stor = m_f.decideStackStorage(vr.getSize());
            vr.changeStorage(i,cmn::tgt::kStorageUndecidedStack,stor);
         }
      }
   }
}

void stackAllocator::run(varTable& v, varFinder& f)
{
   for(auto it=v.all().begin();it!=v.all().end();++it)
   {
      auto jit = it->second->storageToInstrMap.find(cmn::tgt::kStorageUndecidedStack);
      if(jit != it->second->storageToInstrMap.end())
      {
         // at least one instruction wants a stack allocation
         auto nu = f.decideStackStorage(it->second->getSize());
         auto instrs = jit->second;
         for(auto kit=instrs.begin();kit!=instrs.end();++kit)
         {
            cdwDEBUG("deciding undecided stack storage as %lld for instr %lld\n",nu,*kit);
            it->second->changeStorage(*kit,cmn::tgt::kStorageUndecidedStack,nu);
         }
      }
   }
}

// vars that are accessed more frequently make better use of a register
class varPrioritySorter {
public:
   bool operator()(const var *pLhs, const var *pRhs) const
   {
      if(pLhs->refs.size() != pRhs->refs.size())
         return (pLhs->refs.size() > pRhs->refs.size());
      else
         return pLhs < pRhs;
   }
};

void varAllocator2::run(varTable& v, varFinder& f)
{
   std::set<var*,varPrioritySorter> priOrder;

   // find all vars that need storage, sorted in priority order
   for(auto it=v.all().begin();it!=v.all().end();++it)
      if(it->second->storageToInstrMap.size()==0)
         priOrder.insert(it->second);

   // what I do next is expensive, so don't if you can avoid it
   if(!priOrder.size())
      return;

   std::vector<size_t> ideal;
   m_t.getCallConvention().createRegisterBankInPreferredOrder(ideal);
   for(auto vit=priOrder.begin();vit!=priOrder.end();++vit)
   {
      size_t firstAlive = (*vit)->refs.begin()->first;
      size_t lastAlive = (--((*vit)->refs.end()))->first;

      f.onNewInstr();
      // record all the storage used by variables living during my lifetime
      for(auto it=v.all().begin();it!=v.all().end();++it)
         if(it->second->isAlive(firstAlive,lastAlive))
            for(auto jit=it->second->instrToStorageMap.begin();
               jit!=it->second->instrToStorageMap.end();++jit)
               if(firstAlive <= jit->first && jit->first <= lastAlive)
                  //used.insert(jit->second.begin(),jit->second.end());
                  for(auto kit=jit->second.begin();kit!=jit->second.end();++kit)
                     f.recordStorageUsed(*kit);

      size_t ans = f.chooseFreeStorage((*vit)->lastArg().getSize());
      cdwDEBUG("assigning r%ld for var %s\n",ans,(*vit)->name.c_str());
      (*vit)->requireStorage(firstAlive,ans);
   }
}

} // namespace liam

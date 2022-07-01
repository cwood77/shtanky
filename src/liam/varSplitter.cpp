#include "../cmn/fmt.hpp"
#include "../cmn/trace.hpp"
#include "lir.hpp"
#include "varGen.hpp"
#include "varSplitter.hpp"
#include <stdio.h>

namespace liam {

void varSplitter::split(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
{
   varSplitter self(s,v,t);

   lirInstr *pInstr = &s.pTail->head();
   while(true)
   {
      for(auto it=pInstr->getArgs().begin();it!=pInstr->getArgs().end();++it)
      {
         var *pVar = v.fetch(**it);
         if(pVar)
            self.checkVar(*pVar);
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

void varSplitter::checkVar(var& v)
{
   if(m_done.find(&v)!=m_done.end())
      return;
   m_done.insert(&v);

   if(v.storageToInstrMap.size() > 1)
   {
      // this var has at least two different storage demands

      auto it=v.instrToStorageMap.begin();
      auto pPrevSs = &it->second;

      // implement first storage requirements
      implementFirstStorageRequirements(v, it->first,it->second);

      // for each subsequent requirement, implement it if not already the case
      for(++it;it!=v.instrToStorageMap.end();++it)
      {
         size_t currI = it->first;
         auto *pCurrSs = &it->second;

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

         pPrevSs = pCurrSs;
      }
   }

   // update the table _after_ iterating on it
   for(auto it=m_newInstrs.begin();it!=m_newInstrs.end();++it)
      v.requireStorage(it->first->orderNum,it->second);
   m_newInstrs.clear();
}

// even initial requirements may require implementation if there's multiple of them
// for example, consider "function(1)"; in this case @1 will have two initial requirements:
// one for immediate, and one for rcx.  The splitter needs to inject "mov, rcx, 1".
void varSplitter::implementFirstStorageRequirements(var& v, size_t orderNum, std::set<size_t>& reqs)
{
   if(reqs.size() == 1)
      return;

   // search for at least one primordial source (e.g. immediate storage)
   auto primordialStorage=reqs.begin();
   for(;primordialStorage!=reqs.end();++primordialStorage)
      if(*primordialStorage == cmn::tgt::kStorageImmediate)
         break;
   if(primordialStorage == reqs.end())
      cdwTHROW("can't implement first storage requirements");

   for(auto it=reqs.begin();it!=reqs.end();++it)
   {
      if(it == primordialStorage)
         continue;
      emitMoveBefore(v,orderNum,*primordialStorage,*it);
   }
}

void varSplitter::emitMoveBefore(var& v, size_t orderNum, size_t srcStor, size_t destStor)
{
   cdwDEBUG("emitting move for split before %lld (%lld -> %lld)\n",orderNum,srcStor,destStor);

   const bool hasImm = (srcStor == cmn::tgt::kStorageImmediate);

   auto& mov = m_s.pTail
      ->searchUp([=](auto& i){ return i.orderNum == orderNum; })
         .injectBefore(*new lirInstr(hasImm ? cmn::tgt::kMov : cmn::tgt::kSplit));
   mov.comment = cmn::fmt("      (%s req for %s) [splitter]",
      v.name.c_str(),
      m_t.getProc().getRegName(destStor));

   auto& dest = mov.addArg<lirArgVar>("spltD",v.getSize());

   lirArg *pSrc = NULL;
   if(hasImm)
      pSrc = &mov.addArg<lirArgConst>(v.getImmediateData(),v.getSize());
   else
   {
      pSrc = &mov.addArg<lirArgVar>("spltS",v.getSize());
      tryPreserveDisp(v,orderNum,*pSrc);
   }

   v.refs[mov.orderNum].push_back(&dest);
   v.refs[mov.orderNum].push_back(pSrc);

   // n.b. the source is _not_ a requirement.  I care where it's going, but not
   //      really where it's coming from.  This gives more flexibility to the combiner.
   m_newInstrs.push_back(std::make_pair<lirInstr*,size_t>(&mov,(size_t)destStor));

   v.storageDisambiguators[&dest] = destStor;

   if(hasImm)
      // kMov won't have a 2nd splitter pass, so do some extra stuff now
      v.storageDisambiguators[pSrc] = cmn::tgt::kStorageImmediate;
}

void varSplitter::tryPreserveDisp(var& v, size_t orderNum, lirArg& splitSrcArg)
{
   // consider returning a field (e.g. "ret [rcx+8]").  In this case, I'll split rcx -> rax,
   // when I really want to split [rcx+8] -> rax.  So, preserve any displacement
   // while splitting

   auto& args = v.refs[orderNum];
   if(args.size() < 1)
      cdwTHROW("insanity");

   // it's possible to have multiple refs on the same instr (e.g. consider x->foo(), which
   // will translate to call [foo], foo if foo is the first vtbl entry
   // in the case of multiple, choose the disp if all args are identical; otherwise use no
   // disp
   int disp = 0;
   bool addrOf = false;
   bool conflict = false;
   {
      auto& origArg = **args.begin();
      disp = origArg.disp;
      addrOf = origArg.addrOf;
   }
   for(auto it=++(args.begin());!conflict && it!=args.end();++it)
   {
      if(disp != (*it)->disp)
         conflict = true;
      if(addrOf != (*it)->addrOf)
         conflict = true;
   }

   if(conflict)
      return;

   splitSrcArg.disp = disp;
   splitSrcArg.addrOf = addrOf;
}

void splitResolver::run()
{
   lirInstr *pInstr = &m_s.pTail->head();
   while(true)
   {
      if(pInstr->instrId == cmn::tgt::kSplit)
      {
         var& src = m_v.demand(*pInstr->getArgs()[1]);

         auto prev = src.getStorageAt(pInstr->orderNum-1);
         if(prev.size() != 1)
            cdwTHROW("insane!  too many previous storages!");

         src.requireStorage(pInstr->orderNum,*prev.begin()); // TODO - necessary?
         src.storageDisambiguators[pInstr->getArgs()[1]] = *prev.begin();

         pInstr->instrId = cmn::tgt::kMov;
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

} // namespace liam

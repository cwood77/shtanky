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

void varSplitter::emitMoveBefore(var& v, size_t orderNum, size_t srcStor, size_t destStor)
{
   cdwDEBUG("emitting move for split before %lld (%lld -> %lld)\n",orderNum,srcStor,destStor);

   auto& mov = m_s.pTail
      ->searchUp([=](auto& i){ return i.orderNum == orderNum; })
         .injectBefore(*new lirInstr(cmn::tgt::kSplit));
   mov.comment = cmn::fmt("      (%s req for %s) [splitter]",
      v.name.c_str(),
      m_t.getProc().getRegName(destStor));

   auto& dest = mov.addArg<lirArgVar>("spltD",v.getSize());

   lirArg *pSrc = NULL;
   if(srcStor == cmn::tgt::kStorageImmediate)
      pSrc = &mov.addArg<lirArgConst>(v.getImmediateData(),v.getSize());
   else
   {
      pSrc = &mov.addArg<lirArgVar>("spltS",v.getSize());
      preserveDisp(v,orderNum,*pSrc);
   }

   v.refs[mov.orderNum].push_back(&dest);
   v.refs[mov.orderNum].push_back(pSrc);

   // n.b. the source is _not_ a requirement.  I care where it's going, but not
   //      really where it's coming from.  This gives more flexibility to the combiner.
   m_newInstrs.push_back(std::make_pair<lirInstr*,size_t>(&mov,(size_t)destStor));

   v.storageDisambiguators[&dest] = destStor;
}

void varSplitter::preserveDisp(var& v, size_t orderNum, lirArg& splitSrcArg)
{
   // consider returning a field (e.g. "ret [rcx+8]").  In this case, I'll split rcx -> rax,
   // when I really want to split [rcx+8] -> rax.  So, preserve any displacement
   // while splitting

   auto& args = v.refs[orderNum];
   if(args.size() != 1)
      cdwTHROW("insanity");
   auto& origArg = **args.begin();

   splitSrcArg.disp = origArg.disp;
   splitSrcArg.addrOf = origArg.addrOf;
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

         src.requireStorage(pInstr->orderNum,*prev.begin());
         src.storageDisambiguators[pInstr->getArgs()[1]] = *prev.begin();

         pInstr->instrId = cmn::tgt::kMov;
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

} // namespace liam

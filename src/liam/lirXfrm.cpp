#include "../cmn/unique.hpp"
#include "lir.hpp"
#include "lirXfrm.hpp"
#include "varGen.hpp"

namespace liam {

lirTransform::~lirTransform()
{
   for(auto it=m_changes.begin();it!=m_changes.end();++it)
      delete *it;
}

void lirTransform::runStreams(lirStreams& lir)
{
   for(auto it=lir.page.begin();it!=lir.page.end();++it)
   {
      m_pCurrStream = &it->second;
      runStream(it->second);
   }

   m_pCurrStream = NULL;
   applyChanges();
}

void lirTransform::runStream(lirStream& s)
{
   auto *pInstr = &s.pTail->head();
   while(true)
   {
      runInstr(*pInstr);

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

void lirTransform::runInstr(lirInstr& i)
{
   for(auto it=i.getArgs().begin();it!=i.getArgs().end();++it)
      runArg(i,**it);
}

void lirTransform::scheduleInjectBefore(lirInstr& noob, lirInstr& before)
{
   m_changes.push_back(new injectChange(noob,before,true));
}

void lirTransform::scheduleInjectAfter(lirInstr& noob, lirInstr& after)
{
   m_changes.push_back(new injectChange(noob,after,false));
}

void lirTransform::scheduleAppend(lirInstr& noob)
{
   m_changes.push_back(new appendChange(noob,getCurrentStream()));
}

void lirTransform::injectChange::apply()
{
   if(m_beforeAfter)
   {
      m_antecedent.injectBefore(*m_pNoob);
      m_pNoob = NULL;
   }
   else
   {
      m_antecedent.injectAfter(*m_pNoob);
      m_pNoob = NULL;
   }
}

void lirTransform::appendChange::apply()
{
   m_s.pTail->append(*m_pNoob);
   m_pNoob = NULL;
}

void lirTransform::applyChanges()
{
   for(auto it=m_changes.begin();it!=m_changes.end();++it)
      (*it)->apply();
}

void lirNameCollector::runArg(lirInstr& i, lirArg& a)
{
   m_u.seed(a.getName());
}

void lirCallVirtualStackCalculation::runInstr(lirInstr& i)
{
   if(i.instrId == cmn::tgt::kPreCallStackAlloc)
      m_pPreCall = &i;

   lirTransform::runInstr(i);

   if(i.instrId == cmn::tgt::kCall)
   {
      // ignore the calladdr/instptr for calls/invokes
      m_argRealSizes.erase(m_argRealSizes.begin());

      size_t subcallStackSize = m_t.getCallConvention().getShadowSpace();
      subcallStackSize += m_t.getCallConvention().getArgumentStackSpace(m_argRealSizes);
      m_pPreCall->addArg<lirArgConst>("totalStackSize",subcallStackSize);

      m_pPreCall = NULL;
      m_argRealSizes.clear();
   }
}

void lirCallVirtualStackCalculation::runArg(lirInstr& i, lirArg& a)
{
   if(i.instrId == cmn::tgt::kCall)
      m_argRealSizes.push_back(m_t.getRealSize(a.getSize()));

   lirTransform::runArg(i,a);
}

void lirPairedInstrDecomposition::runStream(lirStream& s)
{
   // inject kSelectSegment at the start of each stream
   auto i = new lirInstr(cmn::tgt::kSelectSegment);
   i->addArg<lirArgConst>(s.segment,0);
   scheduleInjectBefore(*i,s.pTail->head());

   lirTransform::runStream(s);
}

void lirPairedInstrDecomposition::runInstr(lirInstr& i)
{
   // inject kExitFunc
   if(i.instrId == cmn::tgt::kEnterFunc)
   {
      auto noob = new lirInstr(cmn::tgt::kExitFunc);
      scheduleAppend(*noob);
   }

   // inject kUnreserveLocal
   if(i.instrId == cmn::tgt::kReserveLocal)
   {
      auto noob = new lirInstr(cmn::tgt::kUnreserveLocal);
      noob->addArg(i.getArgs()[0]->clone());
      // I know there's an kExitFunc instr; I want just ahead of that
      scheduleInjectAfter(*noob,i.tail());
   }

   // inject kPostCallStackAlloc
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& other = i.searchUp(cmn::tgt::kPreCallStackAlloc);
      auto noob = new lirInstr(cmn::tgt::kPostCallStackAlloc);
      noob->addArg(other.getArgs()[0]->clone());
      scheduleInjectAfter(*noob,i);
   }

   lirTransform::runInstr(i);
}

void lirNumberingTransform::runStream(lirStream& s)
{
   m_next = 10;
   lirTransform::runStream(s);
}

void lirNumberingTransform::runInstr(lirInstr& i)
{
   i.orderNum = m_next;
   m_next += 10;
   lirTransform::runInstr(i);
}

void lirCodeShapeDecomposition::runInstr(lirInstr& i)
{
   // eventually, genericize this to notice when any instr fmt is unimplementable
   // and tweak until it's ok
   //
   // for now, only handle special cases

   // call can't handle immediate passed args
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& args = i.getArgs();
      for(size_t j=2;j<args.size();j++)
      {
         lirArg *pArg = args[j];
         lirArgConst *pImm = dynamic_cast<lirArgConst*>(pArg);
         if(!pImm)
            continue;

         cmn::uniquifier u;
         lirNameCollector(u).runStream(getCurrentStream());

         auto pTmp = new lirArgTemp(u.makeUnique(""),pImm->getSize());

         auto pMov = new lirInstr(cmn::tgt::kMov);
         pMov->addArg(*pTmp);
         pMov->addArg(*pImm);
         pMov->comment = "shape:hoist imm from call";

         scheduleInjectBefore(*pMov,i);

         // go ahead and swap out the arg directly; this is safe b/c I haven't
         // traversed it yet
         args[j] = &pTmp->clone();
      }
   }

   // call can't handle any addr modifications (i.e. displacemnets or derefs)
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& args = i.getArgs();
      for(size_t j=2;j<args.size();j++)
      {
         lirArg *pArg = args[j];
         if(!pArg->disp && !pArg->addrOf)
            continue;

         cmn::uniquifier u;
         lirNameCollector(u).runStream(getCurrentStream());

         auto pTmp = new lirArgTemp(u.makeUnique(""),pArg->getSize());

         auto pMov = new lirInstr(cmn::tgt::kMov);
         pMov->addArg(*pTmp);
         pMov->addArg(*pArg);
         pMov->comment = "shape:hoist addrOf from call";

         scheduleInjectBefore(*pMov,i);

         // go ahead and swap out the arg directly; this is safe b/c I haven't
         // traversed it yet
         args[j] = &pTmp->clone();
      }
   }

   lirTransform::runInstr(i);
}

void runLirTransforms(lirStreams& lir, cmn::tgt::iTargetInfo& t)
{
   { lirCallVirtualStackCalculation xfrm(t); xfrm.runStreams(lir); }
   { lirPairedInstrDecomposition xfrm; xfrm.runStreams(lir); }
   { lirCodeShapeDecomposition xfrm; xfrm.runStreams(lir); }
   { lirNumberingTransform xfrm; xfrm.runStreams(lir); }
}

void lirVarGen::runArg(lirInstr& i, lirArg& a)
{
   var& v = m_v.create(a.getName());
   v.refs[i.orderNum].push_back(&a);

   if(dynamic_cast<lirArgConst*>(&a))
      v.requireStorage(i.orderNum,cmn::tgt::kStorageImmediate);
}

} // namespace liam

#include "lir.hpp"
#include "lirXfrm.hpp"

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

void runLirTransforms(lirStreams& lir, cmn::tgt::iTargetInfo& t)
{
   { lirCallVirtualStackCalculation xfrm(t); xfrm.runStreams(lir); }
   { lirPairedInstrDecomposition xfrm; xfrm.runStreams(lir); }
}

} // namespace liam

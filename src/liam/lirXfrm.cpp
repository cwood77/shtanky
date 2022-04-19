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
}

void lirTransform::runArg(lirArg& a)
{
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
      // TODO HACK walk arguments to lirInstr to calculate stack size
      // first - seperate transform!
         if(0)
      noob->addArg(other.getArgs()[0]->clone());
      scheduleInjectAfter(*noob,i);
   }
}

void runLirTransforms(lirStreams& lir)
{
   { lirPairedInstrDecomposition xfrm; xfrm.runStreams(lir); }
}

} // namespace liam

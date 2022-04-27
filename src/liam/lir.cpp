#include "../cmn/obj-fmt.hpp"
#include "../cmn/out.hpp"
#include "../cmn/textTable.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "lir.hpp"
#include <algorithm>
#include <sstream>

namespace liam {

lirArg& lirArg::copyFieldsInto(lirArg& noob) const
{
   noob.disp = disp;
   noob.addrOf = addrOf;
   return noob;
}

lirInstr::lirInstr(const cmn::tgt::instrIds id)
: orderNum(0)
, instrId(id)
, m_pPrev(NULL)
, m_pNext(NULL)
{
}

lirInstr::~lirInstr()
{
   for(auto it=m_args.begin();it!=m_args.end();++it)
      delete *it;
   delete m_pNext;
}

lirInstr& lirInstr::injectBefore(lirInstr& noob)
{
   noob.m_pPrev = m_pPrev;
   if(m_pPrev)
      m_pPrev->m_pNext = &noob;

   noob.m_pNext = this;
   m_pPrev = &noob;

   noob.pickOrderNumForNewLocation();

   return noob;
}

lirInstr& lirInstr::injectAfter(lirInstr& noob)
{
   if(m_pNext)
      return m_pNext->injectBefore(noob);
   else
      return append(noob);
}

lirInstr& lirInstr::append(lirInstr& noob)
{
   lirInstr& _tail = tail();
   _tail.m_pNext = &noob;
   noob.m_pPrev = &_tail;
   noob.pickOrderNumForNewLocation();
   return noob;
}

lirInstr& lirInstr::head()
{
   lirInstr *pPtr = this;
   while(pPtr->m_pPrev)
      pPtr = pPtr->m_pPrev;
   return *pPtr;
}

lirInstr& lirInstr::tail()
{
   lirInstr *pPtr = this;
   while(pPtr->m_pNext)
      pPtr = pPtr->m_pNext;
   return *pPtr;
}

lirInstr& lirInstr::searchUp(std::function<bool(const lirInstr&)> pred)
{
   lirInstr *pPtr = this;
   while(pPtr)
   {
      if(pred(*pPtr))
         return *pPtr;
      pPtr = pPtr->m_pPrev;
   }
   cdwTHROW("searchUp failed to find match");
}

void lirInstr::pickOrderNumForNewLocation()
{
   size_t before = m_pPrev ? m_pPrev->orderNum : 0;
   size_t after = m_pNext ? m_pNext->orderNum : 0;

   if(before == after && before == 0)
   {
      // unordered chain; no worries
      orderNum = 0;
      return;
   }

   if(before >= after)
      cdwTHROW("insane LIR chain with numder %lld, then %lld",before,after);

   auto distance = after-before;
   size_t candidate = before + (distance / 2.0);

   // integer division truncation means 1/2 = 0;
   if(candidate == before)
      candidate++;

   if(candidate == before || candidate == after)
      cdwTHROW("insanity picking new order num; b=%lld,c=%lld,a=%lld",
         before,candidate,after);

   cdwVERBOSE("changing order of instr w/ instrId=%lld: %lld -> %lld\n",
      instrId,orderNum,candidate);
   orderNum = candidate;
}

lirStream::~lirStream()
{
   delete pTail;
}

lirStream& lirStreams::addNewObject(const std::string& name, const std::string& segment)
{
   objects.push_back(lirStream());
   lirStream& last = objects.back();
   last.name = name;
   last.segment = segment;
   return last;
}

void lirFormatter::format(lirStreams& s)
{
   m_s.stream() << "=== LIR bundle has " << s.objects.size() << " objects(s) ===   (hint: $=var, ~=temp, @=immediate)" << std::endl;
   for(auto it=s.objects.begin();it!=s.objects.end();++it)
   {
      m_s.stream() << std::endl;
      m_s.stream() << "----- start stream " << it->name << std::endl;
      format(*it);
   }
   m_s.stream() << std::endl;
   m_s.stream() << "=== end of LIR bundle dump ===" << std::endl;
   appendTargetHints();
}

void lirFormatter::format(lirStream& s)
{
   cmn::textTable t;
   cmn::textTableLineWriter w(t);

   lirInstr *pInstr = &s.pTail->head();
   while(true)
   {
      format(*pInstr,w);
      w.advanceLine();
      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }

   t.compileAndWrite(m_s.stream());
   m_s.stream() << std::endl;
}

void lirFormatter::format(lirInstr& i, cmn::textTableLineWriter& t)
{
   t[0] << i.orderNum;
   t[1] << m_t.getProc().getInstr(i.instrId)->name;

   for(auto it=i.getArgs().begin();it!=i.getArgs().end();++it)
   {
      if(it != i.getArgs().begin())
         t[2] << ", ";
      format(**it,t);
   }

   if(!i.comment.empty())
   t[3] << ";;; " << i.comment;
}

void lirFormatter::format(lirArg& a, cmn::textTableLineWriter& t)
{
   const char *pType = "$";
   if(dynamic_cast<lirArgConst*>(&a))
      pType = "@";
   else if(dynamic_cast<lirArgTemp*>(&a))
      pType = "~";

   if(a.addrOf)
      t[2] << "[";

   t[2] << pType << a.getName();

   if(a.disp && 0 < a.disp)
      t[2] << "+" << a.disp;
   if(a.disp && 0 > a.disp)
      t[2] << a.disp;

   if(a.addrOf)
      t[2] << "]";

   t[2] << "/" << a.getSize();
}

void lirFormatter::appendTargetHints()
{
   m_s.stream() << std::endl;

   m_s.stream() << "~~~ some debugging hints for this target ~~~" << std::endl << std::endl;

   {
      std::vector<size_t> regsORder,regsPassing;
      m_t.getCallConvention().createRegisterBankInPreferredOrder(regsORder);
      m_t.getCallConvention().getRValAndArgBank(regsPassing);
      cmn::textTable regTable;
      regTable(0,0) << "pref order";
      regTable(1,0) << "passing order";
      regTable(2,0) << "saved in Prolog or Call";
      regTable(3,0) << "as int";
      size_t i=1;
      for(auto it=regsORder.begin();it!=regsORder.end();++it,i++)
      {
         regTable(0,i) << m_t.getProc().getRegName(*it);

         auto ans = std::find(regsPassing.begin(),regsPassing.end(),*it);
         if(ans != regsPassing.end())
            regTable(1,i) << std::distance(regsPassing.begin(),ans);
         else
            regTable(1,i) << "-";

         if(m_t.getCallConvention().requiresPrologEpilogSave(*it))
            regTable(2,i) << "P";
         if(m_t.getCallConvention().requiresSubCallSave(*it))
            regTable(2,i) << "C";

         regTable(3,i) << *it;
      }
      regTable.compileAndWrite(m_s.stream());
      m_s.stream() << std::endl << std::endl;
   }

   {
      std::map<size_t,size_t> regs;
      m_t.getProc().createRegisterMap(regs);
      cmn::textTable regTable;
      regTable(0,0) << "reg";
      size_t i=1;
      for(auto it=regs.begin();it!=regs.end();++it,i++)
      {
         regTable(0,i) << m_t.getProc().getRegName(it->first);
         regTable(1,i) << it->first;
      }
      regTable.compileAndWrite(m_s.stream());
      m_s.stream() << std::endl << std::endl;
   }

   m_s.stream() << "cc shadow space = " << m_t.getCallConvention().getShadowSpace()
      << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//
// NEW LIR API
//
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

lirGenerator::lirGenerator(lirStreams& lir, cmn::tgt::iTargetInfo& t)
: m_lir(lir), m_t(t), m_pCurrStream(NULL)
{
}

void lirGenerator::createNewStream(const std::string& segment, const std::string& comment)
{
   m_pCurrStream = &m_lir.addNewObject(comment,segment);
}

instrBuilder lirGenerator::append(cmn::node& n, cmn::tgt::instrIds id)
{
   auto *pInstr = new lirInstr(id);
   if(!m_pCurrStream->pTail)
      m_pCurrStream->pTail = pInstr;
   else
      m_pCurrStream->pTail->append(*pInstr);
   return instrBuilder(*this,m_t,*pInstr,n);
}

instructionless lirGenerator::noInstr(cmn::node& n)
{
   return instructionless(*this,n);
}

void lirGenerator::bindArg(cmn::node& n, lirArg& a)
{
   // I'm borrowing from an instr, so don't delete these
   m_nodeTable[&n] = &a;
}

void lirGenerator::addArgFromNode(cmn::node& n, lirInstr& i)
{
   // because instrs own their args, make a copy
   i.addArg(m_nodeTable[&n]->clone());
}

const lirArg& instructionless::borrowArgFromChild(cmn::node& n)
{
   return *m_g.m_nodeTable[&n]; // TODO HACK- method?
}

instructionless& instructionless::returnToParent(lirArg& a)
{
   m_g.bindArg(m_n,a);
   m_g.m_adoptedOrphans.insert(&a); // TODO HACK- method?
   return *this;
}





// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************


lirBuilder::~lirBuilder()
{
   for(auto it=m_orphans.begin();it!=m_orphans.end();++it)
      delete *it;
}

void lirBuilder::createNewStream(const std::string& name, const std::string& segment)
{
   m_pCurrStream = &m_lir.addNewObject(name,segment);
}

const lirArg& lirBuilder::borrowArgFromChild(cmn::node& n)
{
   return *m_cache[&n];
}

lirBuilder::instrBuilder lirBuilder::nodeScope::append(cmn::tgt::instrIds i)
{
   auto *pI = new lirInstr(i);
   if(m_b.m_pCurrStream->pTail == NULL)
      m_b.m_pCurrStream->pTail = pI;
   else
      m_b.m_pCurrStream->pTail->append(*pI);

   return lirBuilder::instrBuilder(m_b,m_n,*pI);
}

lirBuilder::nodeScope& lirBuilder::nodeScope::returnToParent(lirArg& a)
{
   m_b.adoptArg(m_n,a);
   return *this;
}

void lirBuilder::publishArg(cmn::node& n, lirArg& a)
{
   m_cache[&n] = &a;
}

void lirBuilder::adoptArg(cmn::node& n, lirArg& a)
{
   publishArg(n,a);
   m_orphans.insert(&a);
}

void lirBuilder::addArgFromNode(cmn::node& n, lirInstr& i)
{
   i.addArg(m_cache[&n]->clone());
}

} // namespace liam

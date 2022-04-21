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

lirInstr::~lirInstr()
{
   for(auto it=m_args.begin();it!=m_args.end();++it)
      delete *it;
   delete m_pNext;
}

lirInstr& lirInstr::append(lirInstr*& pPrev, const cmn::tgt::instrIds id, const std::string& comment)
{
   auto *pNoob = new lirInstr(id);

   if(pPrev)
   {
      pPrev->m_pNext = pNoob;
      pNoob->m_pPrev = pPrev;
      pNoob->orderNum = pPrev->orderNum + 10;
   }

   pPrev = pNoob;

   pNoob->comment = comment;

   return *pNoob;
}

lirInstr& lirInstr::injectBefore(const cmn::tgt::instrIds id, const std::string& comment)
{
   auto *pNoob = new lirInstr(id);
   pNoob->orderNum = orderNum - 1;
   pNoob->comment = comment;
   return injectBefore(*pNoob);
}

lirInstr& lirInstr::injectBefore(lirInstr& noob)
{
   noob.m_pPrev = m_pPrev;
   if(m_pPrev)
      m_pPrev->m_pNext = &noob;

   noob.m_pNext = this;
   m_pPrev = &noob;

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
   return noob;
}

lirInstr& lirInstr::search(size_t orderNum)
{
   lirInstr *pThumb = this;
   while(true)
   {
      if(pThumb->orderNum == orderNum)
         return *pThumb;
      pThumb = &pThumb->next();
   }

   throw std::runtime_error("instr not found!");
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

lirInstr& lirInstr::searchUp(cmn::tgt::instrIds id)
{
   lirInstr *pPtr = this;
   while(pPtr)
   {
      if(pPtr->instrId == id)
         return *pPtr;
      pPtr = pPtr->m_pPrev;
   }
   cdwTHROW("searchUp failed for instr %d",id);
}

lirInstr::lirInstr(const cmn::tgt::instrIds id)
: orderNum(10)
, instrId(id)
, pInstrFmt(NULL)
, m_pPrev(NULL)
, m_pNext(NULL)
{
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
   lirInstr& i = lirInstr::append(m_pCurrStream->pTail,id,"");
   return instrBuilder(*this,m_t,i,n);
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

} // namespace liam

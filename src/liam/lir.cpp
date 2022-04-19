#include "../cmn/obj-fmt.hpp"
#include "../cmn/out.hpp"
#include "../cmn/textTable.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "lir.hpp"
#include <sstream>

namespace liam {

// when does a temporary become a variable?
//
// when it needs storage across instrs
// varRefs and literals are inlined into their instrs and thus don't qualify
//
// a = b + 1
//
//   =
// a    +
//     b  1 
//
// in this example, b, 1, and a are inlined.  But the temporary between + and = needs
// storage. (this example is a little contrived b/c + is actually an accumulation into b)
//
// can you know from any instruction whether it's a temporary or not?

lirArg& lirArg::copyFieldsInto(lirArg& noob) const
{
   noob.disp = disp;
   noob.addrOf = addrOf;
   return noob;
}

void lirArgVar::dump() const
{
   cdwDEBUG("v/%s/%lld+%d%s",name.c_str(),m_size,disp,addrOf ? "& " : " ");
}

void lirArgConst::dump() const
{
   cdwDEBUG("c/%s/%lld+%d%s",name.c_str(),m_size,disp,addrOf ? "& " : " ");
}

void lirArgTemp::dump() const
{
   cdwDEBUG("t/%s/%lld+%d%s",name.c_str(),m_size,disp,addrOf ? "& " : " ");
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

void lirInstr::dump()
{
   cdwDEBUG("%lld INSTR %d   ",orderNum,instrId);
   for(auto it=m_args.begin();it!=m_args.end();++it)
      (*it)->dump();
   cdwDEBUG("\n");

   if(m_pNext)
      m_pNext->dump();
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

void lirStream::dump()
{
   pTail->head().dump();
}

void lirStreams::dump()
{
   for(auto it=page.begin();it!=page.end();++it)
   {
      cdwDEBUG("----- %s\n",it->first.c_str());
      it->second.dump();
   }
}

void lirStreams::onNewPageStarted(const std::string& key)
{
   lirStream& noob = page[key];
   noob.pTop = this;

   if(page.size() == 1) return;

   size_t last = 0;
   for(auto it=page.begin();it!=page.end();++it)
   {
      if(key == it->first)
         continue;

      if(last < it->second.pTail->orderNum)
         last = it->second.pTail->orderNum;
   }

   last += 20;
   cdwDEBUG("reassigning new page to order %lld\n",last);
   noob.pTail->orderNum = last;
}

void lirFormatter::format(lirStreams& s)
{
   m_s.stream() << "=== LIR bundle has " << s.page.size() << " stream(s) ===   (hint: $=var, ~=temp, @=immediate)" << std::endl;
   for(auto it=s.page.begin();it!=s.page.end();++it)
   {
      m_s.stream() << "----- start stream " << it->first << std::endl;
      format(it->second);
   }
   m_s.stream() << "=== end of LIR bundle dump ===" << std::endl;
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
   // build this into the stream itself
   m_pCurrStream = &m_lir.page[comment]; // uniquify
   m_pCurrStream->segment = segment;
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

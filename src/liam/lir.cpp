#include "../cmn/obj-fmt.hpp"
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

   pNoob->m_pPrev = m_pPrev;
   if(m_pPrev)
      m_pPrev->m_pNext = pNoob;

   pNoob->m_pNext = this;
   m_pPrev = pNoob;

   pNoob->comment = comment;

   return *pNoob;
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

void lirGenerator::createNewStream(size_t flags, const std::string& comment)
{
   // build this into the stream itself
   m_pCurrStream = &m_lir.page[comment + "_NOOB!"];
   /*
   append(cmn::tgt::kSelectSegment)
      .withArg<lirArgConst>(cmn::objfmt::obj::kLexConst,0);
      */
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

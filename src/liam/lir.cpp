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

#if 0
lirInstr& lirStreams::search(size_t orderNum)
{
   for(auto it=page.begin();it!=page.end();++it)
   {
      if(orderNum <= it->second.pTail->orderNum)
      {
         auto& h = it->second.pTail->head();
         if(orderNum >= h.orderNum)
         {
            return h.search(orderNum);
         }
      }
   }

   cdwTHROW("can't find orderNum on any page..?");
}
#endif

} // namespace liam

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
   ::printf("v/%s/%lld ",name.c_str(),m_size);
}

void lirArgConst::dump() const
{
   ::printf("c/%s/%lld ",name.c_str(),m_size);
}

lirInstr::~lirInstr()
{
   for(auto it=m_args.begin();it!=m_args.end();++it)
      delete *it;
   delete m_pNext;
}

lirInstr& lirInstr::append(lirInstr*& pPrev, const cmn::tgt::instrIds id)
{
   auto *pNoob = new lirInstr(id);

   if(pPrev)
   {
      pPrev->m_pNext = pNoob;
      pNoob->m_pPrev = pPrev;
      pNoob->orderNum = pPrev->orderNum + 1;
   }

   pPrev = pNoob;
   return *pNoob;
}

void lirInstr::dump()
{
   ::printf("%lld INSTR %d   ",orderNum,instrId);
   for(auto it=m_args.begin();it!=m_args.end();++it)
      (*it)->dump();
   ::printf("\n");

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
: orderNum(0)
, instrId(id)
, pInstrFmt(NULL)
, m_pPrev(NULL)
, m_pNext(NULL)
{
}

lirVar::lirVar()
: firstAccess(0)
, lastAccess(0)
, numAccesses(0)
, size(0)
, global(0)
, stackSlot(0)
, targetStorage(0)
{
}

lirStream::~lirStream()
{
   for(auto it=m_temps.begin();it!=m_temps.end();++it)
      delete it->second;
   for(auto it=m_nodeOutputs.begin();it!=m_nodeOutputs.end();++it)
      delete it->second;
}

void lirStream::dump()
{
   pTail->head().dump();
}

lirArg& lirStream::createNamedArg(lirInstr& i, const std::string& name, size_t size)
{
   auto *pArg = new lirArgVar(name,size);
   i.addArg(*pArg);

   auto& var = m_varTable[name];
   // recoard as var
   var.firstAccess = i.orderNum;
   var.lastAccess = i.orderNum;
   var.size = size;
   var.global = 1; // TODO

   return *pArg;
}

lirArg& lirStream::claimArg(cmn::node& n, lirInstr& i)
{
   lirArg *pArg = m_nodeOutputs[&n];
   m_nodeOutputs.erase(&n);

   auto pAsVar = dynamic_cast<lirArgVar*>(pArg);
   if(pAsVar)
   {
      if(m_temps.find(pArg)!=m_temps.end())
      {
         // convert temporary into variable if actually consumed
         lirInstr *pOrigInstr = m_temps[pArg];
         m_temps.erase(pArg);

         std::stringstream tmpName;
         tmpName << "_tmp" << m_nTemp++;
         pAsVar->name = tmpName.str();

         auto& var = m_varTable[tmpName.str()];
         // recoard as var
         var.firstAccess = pOrigInstr->orderNum;
         var.lastAccess = pOrigInstr->orderNum;
         var.size = 0; // TODO
      }

      // record usage
      auto it = m_varTable.find(pAsVar->name);
      if(it!=m_varTable.end())
      {
         it->second.lastAccess = i.orderNum;
         it->second.numAccesses++;
      }
   }

   i.addArg(*pArg);
   return *pArg;
}

std::vector<lirVar*> lirStream::getVariablesInScope(size_t instrOrderNum)
{
   throw 3.14;
}

void lirStream::_donate(cmn::node& n, lirArg& a)
{
   m_nodeOutputs[&n] = &a;
}

void lirStream::_createTemporary(cmn::node& n, lirInstr& i, lirArg& a)
{
   m_nodeOutputs[&n] = &a;
   m_temps[&a] = &i;
}

void lirStreams::dump()
{
   for(auto it=page.begin();it!=page.end();++it)
   {
      ::printf("----- %s\n",it->first.c_str());
      it->second.dump();
   }
}

} // namespace liam

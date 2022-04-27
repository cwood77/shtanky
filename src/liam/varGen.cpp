#include "../cmn/throw.cpp"
#include "../cmn/trace.cpp"
#include "lir.hpp"
#include "varGen.hpp"
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace liam {

const lirArg& var::lastArg()
{
   return **(--((--(refs.end()))->second.end()));
}

size_t var::getSize()
{
   return (*refs.begin()->second.begin())->getSize();
}

bool var::isAlive(size_t orderNum)
{
   return refs.begin()->first <= orderNum && orderNum <= (--(refs.end()))->first;
}

bool var::isAlive(size_t start, size_t end)
{
   return isAlive(start) || isAlive(end) ||
      (start < refs.begin()->first && (--(refs.end()))->first < end);
}

// ans    req
//  b  0
//  b  1  BX
//  b  2
//  d  3  DX   splitter will inject at 2.5 to mov BX <- BX
//  d  4
//
std::set<size_t> var::getStorageAt(size_t orderNum)
{
   if(instrToStorageMap.size() == 0)
      return std::set<size_t>();

   auto found = instrToStorageMap.begin();
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first <= orderNum)
         found = it;
      else
         break;

   return found->second;
}

size_t var::getStorageFor(size_t orderNum, lirArg& a)
{
   auto it = storageDisambiguators.find(&a);
   if(it != storageDisambiguators.end())
      return it->second;

   auto stors = getStorageAt(orderNum);
   if(stors.size() != 1)
      cdwTHROW("insane?  somehow arg %s on instr %lld has no storage?",
         a.getName().c_str(),orderNum);
   return *(stors.begin());
}

size_t var::requiresStorageNext(size_t orderNum, size_t storage)
{
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first > orderNum)
         if(it->second.find(storage) != it->second.end())
            return it->first;

   return 0;
}

void var::requireStorage(size_t orderNum, size_t s)
{
   instrToStorageMap[orderNum].insert(s);
   storageToInstrMap[s].insert(orderNum);
}

void var::changeStorage(size_t orderNum, size_t old, size_t nu)
{
   instrToStorageMap[orderNum].erase(old);
   instrToStorageMap[orderNum].insert(nu);
   storageToInstrMap[old].erase(orderNum);
   if(storageToInstrMap[old].size() == 0)
      storageToInstrMap.erase(old);
   storageToInstrMap[nu].insert(orderNum);

   for(auto it=storageDisambiguators.begin();it!=storageDisambiguators.end();++it)
      if(it->second == old)
         it->second = nu;
}

void var::updateStorageHereAndAfter(lirInstr& i, size_t old, size_t nu)
{
   cdwDEBUG("--updateStorageHereAndAfter[%lld,%lld->%lld]\n",i.orderNum,old,nu);

   lirInstr *pInstr = &i;
   while(true)
   {
      auto it = instrToStorageMap.find(pInstr->orderNum);
      if(it!=instrToStorageMap.end() && it->second.find(old)!=it->second.end())
      {
         cdwDEBUG("   hit\n");
         it->second.erase(old);
         it->second.insert(nu);

         storageToInstrMap[old].erase(pInstr->orderNum);
         if(storageToInstrMap[old].size() == 0)
            storageToInstrMap.erase(old);
         storageToInstrMap[nu].insert(pInstr->orderNum);

         auto& args = i.getArgs();
         for(auto jit=args.begin();jit!=args.end();++jit)
         {
            auto kit = storageDisambiguators.find(*jit);
            if(kit != storageDisambiguators.end() && kit->second == old)
               kit->second = nu;
         }
      }
      else
      {
         cdwDEBUG("   ?no hit\n");

         if(pInstr->orderNum == i.orderNum)
         {
            cdwDEBUG("      since this is instr %llu, inject a requirement\n",i.orderNum);
            requireStorage(i.orderNum,nu);
         }
      }

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

size_t virtStackTable::reserveVirtStorage(size_t real)
{
   size_t v = cmn::tgt::makeVStack(m_next++);
   m_map[v] = real;
   return v;
}

size_t virtStackTable::mapToReal(size_t virt)
{
   auto it = m_map.find(virt);
   if(it == m_map.end())
      cdwTHROW("unknown virtual stack storage %lld",virt);
   return it->second;
}

varTable::~varTable()
{
   for(auto it=m_vars.begin();it!=m_vars.end();++it)
      delete it->second;
}

var& varTable::create(const std::string& name)
{
   var*& pVar = m_vars[name];
   if(!pVar)
   {
      pVar = new var;
      pVar->name = name;
   }
   return *pVar;
}

var& varTable::demand(const std::string& name)
{
   var *pVar = m_vars[name];
   if(!pVar)
      throw std::runtime_error("missing var");
   return *pVar;
}

var& varTable::demand(lirArg& a)
{
   var *pAns = fetch(a);

   if(!pAns)
      cdwTHROW("variable not found!");

   return *pAns;
}

var *varTable::fetch(lirArg& a)
{
   for(auto it=m_vars.begin();it!=m_vars.end();++it)
      for(auto jit=it->second->refs.begin();jit!=it->second->refs.end();++jit)
         for(auto kit=jit->second.begin();kit!=jit->second.end();++kit)
            if(*kit == &a)
               return it->second;

   return NULL;
}

} // namespace liam

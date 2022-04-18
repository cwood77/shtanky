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

void var::requireStorage(lirInstr& i, size_t s)
{
   instrToStorageMap[i.orderNum].insert(s);
   storageToInstrMap[s].insert(i.orderNum);
}

void var::requireStorage(size_t orderNum, size_t s)
{
   instrToStorageMap[orderNum].insert(s);
   storageToInstrMap[s].insert(orderNum);
}

void var::changeStorage(lirInstr& i, size_t old, size_t nu)
{
   instrToStorageMap[i.orderNum].erase(old);
   instrToStorageMap[i.orderNum].erase(nu);
   storageToInstrMap[old].erase(i.orderNum);
   if(storageToInstrMap[old].size())
      storageToInstrMap.erase(old);
   storageToInstrMap[nu].insert(i.orderNum);
}

bool var::isAlive(size_t orderNum)
{
   return refs.begin()->first <= orderNum && orderNum <= (--(refs.end()))->first;
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

bool var::requiresStorageLater(size_t orderNum, size_t storage)
{
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first > orderNum)
         if(it->second.find(storage) != it->second.end())
            return true;

   return false;
}

std::string var::getImmediateData()
{
   return dynamic_cast<const lirArgConst&>(lastArg()).name;
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
            requireStorage(i,nu);
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
      throw std::runtime_error("unknown virtual stack storage");
   return it->second;
}

varTable::~varTable()
{
   for(auto it=m_vars.begin();it!=m_vars.end();++it)
      delete it->second;
}

std::string varTable::pickUniqueName(const std::string& nameHint)
{
   std::string nameBase = ":";
   nameBase += nameHint;
   for(size_t i=0;;i++)
   {
      std::stringstream name;
      name << nameBase << i;
      if(m_vars.find(name.str())==m_vars.end())
         return name.str();
   }
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

size_t varTable::getStorageFor(size_t orderNum, lirArg& a)
{
   var& v = demand(a);

   auto it = v.storageDisambiguators.find(&a);
   if(it != v.storageDisambiguators.end())
      return it->second;

   auto stors = v.getStorageAt(orderNum);
   if(stors.size() != 1)
      throw std::runtime_error("insanity!");
   return *(stors.begin());
}

varGenerator::~varGenerator()
{
   for(auto it=m_donations.begin();it!=m_donations.end();++it)
      delete it->second;
}

varWriter varGenerator::createPrivateVar(size_t instrNum, lirArg& a, const std::string& nameHint, ...)
{
   va_list ap;
   va_start(ap,nameHint);
   char buffer[1000];
   ::vsnprintf(buffer,1000,nameHint.c_str(),ap);
   va_end(ap);
   if(::strlen(buffer)==0)
      throw std::runtime_error("nameHint must be provied");

   return varWriter(this,buffer,instrNum,&a);
}

varWriter varGenerator::createNamedVar(size_t instrNum, lirArgVar& a)
{
   return varWriter(this,"",instrNum,&a);
}

lirArg& varGenerator::claimAndAddArgByName(lirInstr& i, const std::string& n)
{
   var& v = m_vTable.demand(n);

   auto& a = duplicateArg(v.lastArg());
   v.refs[i.orderNum].push_back(&a);
   i.addArg(a);

   return a;
}

lirArg& varGenerator::claimAndAddArgOffWire(lirInstr& i, cmn::node& n)
{
   lirArg *pArg = m_donations[&n];
   m_donations.erase(&n);
   if(pArg)
   {
      // use donation
      // LAME TODO HACK - share code with createVar()
      std::string m_nameHint = m_donatedNames[&n];
      m_donatedNames.erase(&n);
      var *pVar = NULL;
      if(m_nameHint.empty())
         pVar = &m_vTable.create(dynamic_cast<lirArgVar&>(*pArg).name);
      else
      {
         auto finalName = m_vTable.pickUniqueName(m_nameHint);
         pVar = &m_vTable.create(finalName);
      }
      pVar->refs[i.orderNum].push_back(pArg);

      i.addArg(*pArg);

      // donated constants are literals
      // TODO HACK is this true/safe/good?
      if(dynamic_cast<lirArgConst*>(pArg))
         pVar->requireStorage(i.orderNum-2, cmn::tgt::kStorageImmediate);

      return *pArg;
   }

   // otherwise:
   //
   // use wire

   if(m_onWire.find(&n)==m_onWire.end())
      throw std::runtime_error("claiming wire when nothing was published");

   var& v = *m_onWire[&n];
   m_onWire.erase(&n);

   auto& a = duplicateArg(v.lastArg());
   v.refs[i.orderNum].push_back(&a);
   i.addArg(a);

   return a;
}

void varGenerator::publishOnWire(cmn::node& n, var& v)
{
   if(m_onWire.find(&n)!=m_onWire.end())
      throw std::runtime_error("double publish!");
   m_onWire[&n] = &v;
}

void varGenerator::donateToWire(cmn::node& n, lirArg& _a, const std::string& nameHint)
{
   if(m_onWire.find(&n)!=m_onWire.end())
      throw std::runtime_error("can't donate when already published");
   lirArg*& a = m_donations[&n];
   if(a)
      throw std::runtime_error("can't donate when already donated");
   a = &_a;
   m_donatedNames[&n] = nameHint;
}

lirArg& varGenerator::duplicateArg(const lirArg& a)
{
   const lirArgVar *pVar = dynamic_cast<const lirArgVar*>(&a);
   const lirArgConst *pConst = dynamic_cast<const lirArgConst*>(&a);
   if(pVar)
      return *new lirArgVar(pVar->name,pVar->getSize());
   else
      return *new lirArgConst(pConst->name,pConst->getSize());
}

var& varWriter::createVar()
{
   var *pVar = NULL;
   if(m_nameHint.empty())
      pVar = &m_pVGen->m_vTable.create(dynamic_cast<lirArgVar&>(*m_pArg).name);
   else
   {
      auto finalName = m_pVGen->m_vTable.pickUniqueName(m_nameHint);
      pVar = &m_pVGen->m_vTable.create(finalName);
   }
   pVar->refs[m_num].push_back(m_pArg);
   return *pVar;
}

void varWriter::finalize()
{
   if(!m_donated && !m_pub)
      createVar();
}

} // namespace liam

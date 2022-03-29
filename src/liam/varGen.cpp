#include "lir.hpp"
#include "varGen.hpp"
#include <sstream>
#include <stdarg.h>
#include <stdexcept>
#include <stdio.h>

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

bool var::isAlive(size_t orderNum)
{
   return refs.begin()->first <= orderNum && orderNum <= (--(refs.end()))->first;
}

std::set<size_t> var::getStorageAt(size_t orderNum)
{
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first >= orderNum)
         return it->second;

   throw std::runtime_error("can't find storage at that time");
}

bool var::requiresStorageLater(size_t orderNum, size_t storage)
{
   for(auto it=instrToStorageMap.begin();it!=instrToStorageMap.end();++it)
      if(it->first > orderNum)
         if(it->second.find(storage) != it->second.end())
            return true;

   return false;
}

void var::updateStorageHereAndAfter(lirInstr& i, size_t old, size_t nu)
{
   lirInstr *pInstr = &i;
   while(true)
   {
      auto it = instrToStorageMap.find(pInstr->orderNum);
      if(it!=instrToStorageMap.end() && it->second.find(old)!=it->second.end())
      {
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

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
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
   for(auto it=m_vars.begin();it!=m_vars.end();++it)
      for(auto jit=it->second->refs.begin();jit!=it->second->refs.end();++jit)
         for(auto kit=jit->second.begin();kit!=jit->second.end();++kit)
            if(*kit == &a)
               return *it->second;

   throw std::runtime_error("variable not found!");
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
   auto finalName = m_vTable.pickUniqueName(buffer);

   var& v = m_vTable.create(finalName);
   //v.refs[instrNum].push_back(&a);

   return varWriter(this,&v,instrNum,&a);
}

varWriter varGenerator::createNamedVar(size_t instrNum, lirArgVar& a)
{
   var& v = m_vTable.create(a.name);
   //v.refs[instrNum].push_back(&a);

   return varWriter(this,&v,instrNum,&a);
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
      auto finalName = m_vTable.pickUniqueName("donation");
      var& v = m_vTable.create(finalName);
      v.refs[i.orderNum].push_back(pArg);
      i.addArg(*pArg);
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

void varGenerator::donateToWire(cmn::node& n, lirArg& _a)
{
   if(m_onWire.find(&n)!=m_onWire.end())
      throw std::runtime_error("can't donate when already published");
   lirArg*& a = m_donations[&n];
   if(a)
      throw std::runtime_error("can't donate when already donated");
   a = &_a;
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

void varWriter::finalize()
{
   if(!m_donated)
      m_pVar->refs[m_num].push_back(m_pArg);
}

} // namespace liam

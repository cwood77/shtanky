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
      pVar = new var;
   return *pVar;
}

var& varTable::demand(const std::string& name)
{
   var *pVar = m_vars[name];
   if(!pVar)
      throw std::runtime_error("missing var");
   return *pVar;
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

#include "type-i.hpp"
#include "type.hpp"

namespace cmn {
namespace type {

table::~table()
{
   for(auto it=m_allTypes.begin();it!=m_allTypes.end();++it)
      delete it->second;
}

iType& table::fetch(const std::string& name)
{
   iType*& pAns = m_allTypes[name];
   if(!pAns)
      pAns = new stubTypeWrapper(name);
   return *pAns;
}

iType& table::publish(iType *pType)
{
   iType*& pAns = m_allTypes[pType->getName()];
   if(!pAns)
      pAns = pType;
   else
   {
      auto pStub = dynamic_cast<stubTypeWrapper*>(pAns);
      if(pStub)
         pStub->m_pReal = pType;
      else
         delete pType;
   }
   return *pAns;
}

typeBuilder *typeBuilder::createVoid()
{
   return new typeBuilder(new voidType());
}

typeBuilder *typeBuilder::createClass(const std::string& name)
{
   return new typeBuilder(new userClassType(name));
}

typeBuilder::~typeBuilder()
{
   delete m_pHead;
}

typeBuilder& typeBuilder::array()
{
   // wrapp type, by finishing and restarting
   return *this;
}

typeBuilder& typeBuilder::addMember(const std::string& name, iType& ty)
{
   auto& cl = dynamic_cast<userClassType&>(*m_pHead);
   cl.m_members[name] = &ty;
   return *this;
}

iType& typeBuilder::finish()
{
   auto copy = m_pHead;
   m_pHead = NULL; // I no longer own this
   return gTable->publish(copy);
}

} // namespace type
} // namespace cmn

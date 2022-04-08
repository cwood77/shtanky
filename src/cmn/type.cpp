#include "type-i.hpp"
#include "type.hpp"

namespace cmn {
namespace type {

bool typeBase::_is(const std::string& name) const
{
   if(name == typeid(iType).name())
      return true;
   else
      return false;
}

void *typeBase::_as(const std::string& name)
{
   if(name == typeid(iType).name())
      return static_cast<iType*>(this);
   else
      throw std::runtime_error(cmn::fmt("can't cast to type %s",name.c_str()));
}

iType& stubTypeWrapper::demandReal()
{
   if(pReal)
      return *pReal;
   else
      throw std::runtime_error(cmn::fmt("type %s not resolved when needed",m_name.c_str()));
}

const iType& stubTypeWrapper::demandReal() const
{
   if(pReal)
      return *pReal;
   else
      throw std::runtime_error(cmn::fmt("type %s not resolved when needed",m_name.c_str()));
}

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
      if(pStub && pStub->pReal == NULL)
         pStub->pReal = pType;
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
   delete m_pType;
}

typeBuilder& typeBuilder::array()
{
   // wrapp type, by finishing and restarting
   return *this;
}

typeBuilder& typeBuilder::addMember(const std::string& name, iType& ty)
{
   auto& cl = dynamic_cast<userClassType&>(*m_pType);
   cl.m_members[name] = &ty;
   return *this;
}

iType& typeBuilder::finish()
{
   auto copy = m_pType;
   m_pType = NULL; // I no longer own this
   return gTable->publish(copy);
}

} // namespace type
} // namespace cmn

#include "ast.hpp"
#include "trace.hpp"
#include "type-i.hpp"
#include "type.hpp"

namespace cmn {
namespace type {

timedGlobal<table> gTable;
timedGlobal<nodeCache> gNodeCache;

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
      // table empty
      pAns = pType;
   else
   {
      // table already has entry!

      auto pStub = dynamic_cast<stubTypeWrapper*>(pAns);
      if(pStub && pStub->pReal == NULL && !dynamic_cast<stubTypeWrapper*>(pType))
         // existing stub needs a real value
         pStub->pReal = pType;

      else if(pType == pAns)
         ; // just no nothing if addin myself

      else
         // otherwise, if not assuming ownership of pType, kill it
         delete pType;
   }
   return *pAns;
}

void table::dump()
{
   cdwDEBUG("::global type table has %lld entries\n",m_allTypes.size());
   for(auto it=m_allTypes.begin();it!=m_allTypes.end();++it)
      cdwDEBUG("   %s\n",it->first.c_str());
}

typeBuilder *typeBuilder::createString()
{
   return new typeBuilder(new stringType());
}

typeBuilder *typeBuilder::createVoid()
{
   return new typeBuilder(new voidType());
}

typeBuilder *typeBuilder::createClass(const std::string& name)
{
   return new typeBuilder(new userClassType(name));
}

typeBuilder *typeBuilder::createPtr()
{
   return new typeBuilder(new ptrType());
}

typeBuilder *typeBuilder::open(iType& t)
{
   return new typeBuilder(&t,false);
}

typeBuilder::~typeBuilder()
{
   if(m_own)
      delete m_pType;
}

typeBuilder& typeBuilder::array()
{
   m_pType = new arrayOfType(finish());
   m_own = true;
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
   if(!m_pType)
      throw std::runtime_error("wtf?");

   auto copy = m_pType;
   m_pType = NULL; // I no longer own this
   return gTable->publish(copy);
}

bool nodeCache::hasType(const node& n) const
{
   return m_cache.find(&n)!=m_cache.end();
}

iType& nodeCache::demand(const node& n)
{
   iType *pTy = m_cache[&n];
   if(!pTy)
      throw std::runtime_error("type not in node cache");
   return *pTy;
}

void nodeCache::publish(const node& n, iType& t)
{
   if(hasType(n))
      throw std::runtime_error("double publish!");

   m_cache[&n] = &t;
}

void nodeCache::dump()
{
   cdwDEBUG("::node cache has %lld entries\n",m_cache.size());
   for(auto it=m_cache.begin();it!=m_cache.end();++it)
   {
      cdwDEBUG("   %s\n",typeid(*it->first).name());
      cdwDEBUG("       %s\n",it->second->getName().c_str());
   }
}

} // namespace type
} // namespace cmn

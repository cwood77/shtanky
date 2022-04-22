#include "ast.hpp"
#include "target.hpp"
#include "throw.hpp"
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
      cdwTHROW("can't cast to type %s",name.c_str());
}

const size_t staticallySizedType::getRealAllocSize(const tgt::iTargetInfo& t) const
{
   return t.getRealSize(m_size);
}

bool userClassType::_is(const std::string& name) const
{
   if(name == typeid(iStructType).name())
      return true;
   else
      return typeBase::_is(name);
}

void *userClassType::_as(const std::string& name)
{
   if(name == typeid(iStructType).name())
      return static_cast<iStructType*>(this);
   else
      return typeBase::_as(name);
}

const size_t userClassType::getRealAllocSize(const tgt::iTargetInfo& t) const
{
   size_t totalSize = 0;
   for(auto it=m_order.begin();it!=m_order.end();++it)
      totalSize += t.getRealSize(m_members.find(*it)->second->getPseudoRefSize());
   return totalSize;
}

iType& userClassType::getField(const std::string& name)
{
   auto it = m_members.find(name);
   if(it == m_members.end())
      cdwTHROW("type '%s' does note have field '%s'\n",getName().c_str(),name.c_str());
   return *it->second;
}

size_t userClassType::getOffsetOfField(const std::string& name, const tgt::iTargetInfo& t) const
{
   size_t offset = 0;
   for(auto it=m_order.begin();it!=m_order.end();++it)
   {
      if(*it != name)
         offset += t.getRealSize(m_members.find(*it)->second->getPseudoRefSize());
      else
         return offset;
   }

   cdwTHROW("type '%s' doesn't have field '%s'",getName().c_str(),name.c_str());
}

void userClassType::addField(const std::string& name, iType& f)
{
   m_order.push_back(name);
   m_members[name] = &f;
}

bool functionType::_is(const std::string& name) const
{
   if(name == typeid(iFunctionType).name())
      return true;
   else
      return typeBase::_is(name);
}

void *functionType::_as(const std::string& name)
{
   if(name == typeid(iFunctionType).name())
      return static_cast<iFunctionType*>(this);
   else
      return typeBase::_as(name);
}

void functionType::appendArgType(const std::string& name, iType& ty)
{
   m_argNames.push_back(name);
   m_argTypes.push_back(&ty);
}

iType& stubTypeWrapper::demandReal()
{
   if(pReal)
      return *pReal;
   else
      cdwTHROW("type %s not resolved when needed",m_name.c_str());
}

const iType& stubTypeWrapper::demandReal() const
{
   if(pReal)
      return *pReal;
   else
      cdwTHROW("type %s not resolved when needed",m_name.c_str());
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
      // table is empty; add it
      pAns = pType;
   else
   {
      // table already has entry!
      auto pStub = dynamic_cast<stubTypeWrapper*>(pAns);
      if(pStub && pStub->pReal == NULL && !dynamic_cast<stubTypeWrapper*>(pType))
         // existing stub needs a real value
         pStub->pReal = pType;

      else if(pType == pAns)
         ; // just do nothing if addin myself

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
      cdwDEBUG("   %s - %s\n",it->first.c_str(),typeid(*it->second).name());
}

typeBuilder *typeBuilder::createString()
{
   return new typeBuilder(new stringType());
}

typeBuilder *typeBuilder::createInt()
{
   return new typeBuilder(new intType());
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
   return new typeBuilder(&t,/*own*/false);
}

typeBuilder *typeBuilder::createFunction(const std::string& fqn)
{
   return new typeBuilder(new functionType(fqn));
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
   cl.addField(name,ty);
   return *this;
}

typeBuilder& typeBuilder::setClassType(iType& ty)
{
   auto& ft = dynamic_cast<functionType&>(*m_pType);
   ft.setClassType(ty);
   return *this;
}

typeBuilder& typeBuilder::setStatic(bool v)
{
   auto& ft = dynamic_cast<functionType&>(*m_pType);
   ft.setStatic(v);
   return *this;
}

typeBuilder& typeBuilder::setReturnType(iType& ty)
{
   auto& ft = dynamic_cast<functionType&>(*m_pType);
   ft.setReturnType(ty);
   return *this;
}

typeBuilder& typeBuilder::appendArgType(const std::string& name, iType& ty)
{
   auto& ft = dynamic_cast<functionType&>(*m_pType);
   ft.appendArgType(name,ty);
   return *this;
}

iType& typeBuilder::finish()
{
   if(!m_pType)
      cdwTHROW("wtf?");

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
   if(!hasType(n))
   {
      gTable->dump();
      dump();
      cdwTHROW("type not in node cache");
   }

   return *m_cache[&n];
}

void nodeCache::publish(const node& n, iType& t)
{
   if(hasType(n))
   {
      dump();
      cdwTHROW("double publish!");
   }

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

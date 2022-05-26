#include "../cmn/trace.hpp"
#include "genericUnlinker.hpp"
#include <cstring>

namespace syzygy {

size_t genericUnlinker::ignoreRefsToGenerics(cmn::symbolTable& sTable)
{
   size_t nChanges = 0;

   for(auto it=sTable.unresolved.begin();it!=sTable.unresolved.end();)
   {
      if(::strchr((*it)->ref.c_str(),'<'))
      {
         sTable.unresolved.erase(*it);
         it = sTable.unresolved.begin();
         nChanges++;
      }
      else
         ++it;
   }

   return nChanges;
}

void genericUnlinker::visit(cmn::classNode& n)
{
   if(!m_insideGeneric) return;

   for(auto& link : n.baseClasses)
      ignore(link);

   hNodeVisitor::visit(n);
}

void genericUnlinker::visit(cmn::methodNode& n)
{
   if(!m_insideGeneric) return;

   ignore(n.baseImpl);

   hNodeVisitor::visit(n);
}

void genericUnlinker::visit(cmn::userTypeNode& n)
{
   if(!m_insideGeneric) return;

   ignore(n.pDef);

   hNodeVisitor::visit(n);
}

void genericUnlinker::visit(cmn::callNode& n)
{
   if(!m_insideGeneric) return;

   ignore(n.pTarget);

   hNodeVisitor::visit(n);
}

void genericUnlinker::visit(cmn::varRefNode& n)
{
   if(!m_insideGeneric) return;

   ignore(n.pSrc);

   hNodeVisitor::visit(n);
}

void genericUnlinker::visit(cmn::genericNode& n)
{
   m_insideGeneric = true;
   hNodeVisitor::visit(n);
   m_insideGeneric = false;
}

void genericUnlinker::ignore(cmn::linkBase& l)
{
   auto it = m_sTable.unresolved.find(&l);
   if(it != m_sTable.unresolved.end())
   {
      m_sTable.unresolved.erase(&l);
      nChanges++;
   }
}

bool unlinkGenerics(cmn::node& root, cmn::symbolTable& sTable)
{
   // unlink the generics
   // this is required because otherwise the templates themselves may keep the graph
   // from linking (e.g. 'T' won't be found)
   genericUnlinker v(sTable);
   root.acceptVisitor(v);
   cdwVERBOSE("unlinked %lld generics\n",v.nChanges);

   size_t nChanges = genericUnlinker::ignoreRefsToGenerics(sTable);
   cdwVERBOSE("unlinked %lld refs to generics\n",nChanges);

   return v.nChanges + nChanges;
}

} // namespace syzygy

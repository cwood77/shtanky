#include "genericUnlinker.hpp"

namespace syzygy {

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

} // namespace syzygy

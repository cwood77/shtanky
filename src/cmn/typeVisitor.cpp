#include "type.hpp"
#include "typeVisitor.hpp"

namespace cmn {

void coarseTypeVisitor::visit(classNode& n)
{
   /*
   if(type::gNodeCache->hasType(n))
      return; //hNodeVisitor::visit(n);

   std::unique_ptr<type::typeBuilder> pBuilder(type::typeBuilder::createClass(n.name));
   */
}

void coarseTypeVisitor::visit(fieldNode& n)
{
   /*typeBuilderVisitor child;
   n.demandSoleChild<typeNode>().acceptVisitor(child);

   m_pBuilder->addMember(n.name,child.getType());*/
}

void coarseTypeVisitor::visit(funcNode& n)
{
   // ???
}

} // namespace cmn

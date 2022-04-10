#include "type.hpp"
#include "typeVisitor.hpp"

namespace cmn {

void typeBuilderVisitor::visit(strTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createString());
   hNodeVisitor::visit(n);
}

void typeBuilderVisitor::visit(arrayTypeNode& n)
{
   m_pBuilder->array();
   hNodeVisitor::visit(n);
}

void typeBuilderVisitor::visit(voidTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createVoid());
   hNodeVisitor::visit(n);
}

void typeBuilderVisitor::visit(userTypeNode& n)
{
   auto c = n.pDef.getRefee();
   if(!c)
      throw std::runtime_error("unlinked userTypeNode in typeVisitor");

   type::iType *pTy = &type::gTable->fetch(c->name);

   m_pBuilder.reset(type::typeBuilder::open(*pTy));
   hNodeVisitor::visit(n);
}

void typeBuilderVisitor::visit(ptrTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createPtr());
   hNodeVisitor::visit(n);
}

void coarseTypeVisitor::visit(classNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createClass(n.name));
   hNodeVisitor::visit(n);
   m_pBuilder->finish();
   m_pBuilder.reset();
}

void coarseTypeVisitor::visit(fieldNode& n)
{
   typeBuilderVisitor child;
   n.demandSoleChild<typeNode>().acceptVisitor(child);
   m_pBuilder->addMember(n.name,child.getType());
}

void fineTypeVisitor::visit(fieldAccessNode& n)
{
   hNodeVisitor::visit(n);

   type::gNodeCache->publish(n,
      type::gNodeCache->demand(n.demandSoleChild<cmn::node>())
         .as<type::iStructType>()
         .getField(n.name));
}

void fineTypeVisitor::visit(varRefNode& n)
{
   hNodeVisitor::visit(n);

   cmn::typeNode *pTy = n.pDef.getRefee();
   typeBuilderVisitor child;
   pTy->acceptVisitor(child);
   type::gNodeCache->publish(n,child.getType());
}

void fineTypeVisitor::visit(stringLiteralNode& n)
{
   hNodeVisitor::visit(n);

   std::unique_ptr<type::typeBuilder> b(type::typeBuilder::createString());
   type::gNodeCache->publish(n,b->finish());
}

void fineTypeVisitor::visit(boolLiteralNode& n)
{
   hNodeVisitor::visit(n);

   // TODO bool type
   std::unique_ptr<type::typeBuilder> b(type::typeBuilder::createString());
   type::gNodeCache->publish(n,b->finish());
}

void fineTypeVisitor::visit(intLiteralNode& n)
{
   hNodeVisitor::visit(n);

   // TODO int type
   std::unique_ptr<type::typeBuilder> b(type::typeBuilder::createString());
   type::gNodeCache->publish(n,b->finish());
}

} // namespace cmn

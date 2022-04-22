#include "type.hpp"
#include "typeVisitor.hpp"

namespace cmn {

void builtInTypeVisitor::visit(strTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createString());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(arrayTypeNode& n)
{
   m_pBuilder->array();
   hNodeVisitor::visit(n);
}

void builtInTypeVisitor::visit(voidTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createVoid());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

// a word on user types:
// here, you are guaranteed to get a stub (userTypeVisitor hasn't run yet)
// but that's ok (and required) b/c otherwise nobody creates types for
// userTypeNodes
void builtInTypeVisitor::visit(userTypeNode& n)
{
   m_pBuilder.reset(
      type::typeBuilder::open(
         type::gTable->fetch(
            fullyQualifiedName::build(*n.pDef.getRefee()))));
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(ptrTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createPtr());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(stringLiteralNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createString());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(boolLiteralNode& n)
{
   cdwTHROW("unimpled");
}

void builtInTypeVisitor::visit(intLiteralNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createInt());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void userTypeVisitor::visit(classNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createClass(n.name));
   hNodeVisitor::visit(n);
   auto& t = m_pBuilder->finish();
   type::gNodeCache->publish(n,t);
   m_pBuilder.reset();
}

void userTypeVisitor::visit(fieldNode& n)
{
   auto& fieldType = type::gNodeCache->demand(n.demandSoleChild<typeNode>());
   m_pBuilder->addMember(n.name,fieldType);
}

void functionVisitor::visit(classNode& n)
{
   m_pClass = &n;
   hNodeVisitor::visit(n);
   m_pClass = NULL;
}

void functionVisitor::visit(methodNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createFunction(fullyQualifiedName::build(n,n.name)));
   m_pBuilder->setClassType(type::gNodeCache->demand(*m_pClass));
   m_pBuilder->setStatic(n.flags & nodeFlags::kStatic);

   m_pBuilder->setReturnType(type::gNodeCache->demand(n.demandSoleChild<typeNode>()));
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void functionVisitor::visit(funcNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createFunction(fullyQualifiedName::build(n,n.name)));
   m_pBuilder->setStatic(true);
   m_pBuilder->setReturnType(type::gNodeCache->demand(n.demandSoleChild<typeNode>()));

   m_pBuilder->setReturnType(type::gNodeCache->demand(n.demandSoleChild<typeNode>()));
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void functionVisitor::visit(argNode& n)
{
   m_pBuilder->appendArgType(
      n.name,
      type::gNodeCache->demand(n.demandSoleChild<typeNode>()));
}

void typePropagator::visit(invokeNode& n)
{
   // TODO need function types for this... do I need this?   YES! at least, eventually
   // defer this until method linking is done in the symbol table
   hNodeVisitor::visit(n);
}

void typePropagator::visit(argNode& n)
{
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,type::gNodeCache->demand(*n.getChildren()[0]));
}

void typePropagator::visit(invokeFuncPtrNode& n)
{
   hNodeVisitor::visit(n);

   // TODO HACK unimplementable until the AST has a way to declare custom func ptr types
   //           i.e. stops using 'ptr'
   if(0) {
   auto& instType = type::gNodeCache->demand(*n.getChildren()[0]);
   auto& rVal = instType.as<type::iFunctionType>().getReturnType();
   type::gNodeCache->publish(n,rVal);
   }
}

void typePropagator::visit(fieldAccessNode& n)
{
   hNodeVisitor::visit(n);

   type::gNodeCache->publish(n,
      type::gNodeCache->demand(n.demandSoleChild<cmn::node>())
         .as<type::iStructType>()
         .getField(n.name));
}

void typePropagator::visit(callNode& n)
{
   hNodeVisitor::visit(n);

   auto fqn = fullyQualifiedName::build(n,n.name);
   auto& rVal = type::gTable->fetch(fqn).as<type::iFunctionType>().getReturnType();
   type::gNodeCache->publish(n,rVal);
}

void typePropagator::visit(varRefNode& n)
{
   hNodeVisitor::visit(n);

   typeNode *pTy = n.pDef.getRefee();
   type::gNodeCache->publish(n,type::gNodeCache->demand(*pTy));
}

void typePropagator::visit(bopNode& n)
{
   cdwTHROW("unimpled");
}

void propagateTypes(cmn::node& root)
{
   { builtInTypeVisitor v; root.acceptVisitor(v); }
   { userTypeVisitor v; root.acceptVisitor(v); }
   { functionVisitor v; root.acceptVisitor(v); }
   { typePropagator v; root.acceptVisitor(v); }
}

} // namespace cmn

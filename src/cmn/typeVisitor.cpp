#include "type.hpp"
#include "typeVisitor.hpp"

namespace cmn {

// here, you are guaranteed to get a stub (userTypeVisitor hasn't run yet)
// that allows transforms later to demand a type
void builtInTypeVisitor::visit(classNode& n)
{
   m_pBuilder.reset(
      type::typeBuilder::open(
         type::gTable->fetch(
            fullyQualifiedName::build(n))));
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);

   hNodeVisitor::visit(n);
}

void builtInTypeVisitor::visit(strTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createString());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(boolTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createBool());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(intTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createInt());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

void builtInTypeVisitor::visit(arrayTypeNode& n)
{
   m_pBuilder->wrapArray();
   hNodeVisitor::visit(n);
}

void builtInTypeVisitor::visit(voidTypeNode& n)
{
   m_pBuilder.reset(type::typeBuilder::createVoid());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
}

// here, you are guaranteed to get a stub (userTypeVisitor hasn't run yet)
// that allows transforms later to demand a type
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
   m_pBuilder.reset(type::typeBuilder::createBool());
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,m_pBuilder->finish());
   m_pBuilder.reset(NULL);
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
   m_pBuilder.reset(type::typeBuilder::createClass(fullyQualifiedName::build(n)));
   for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
      m_pBuilder->addBase(type::gNodeCache->demand(*it->getRefee()));
   hNodeVisitor::visit(n);
   m_pBuilder->finish();
   // this type was published earlier in the gNodeCache (in builtInTypeVisitor)
   m_pBuilder.reset();
}

void userTypeVisitor::visit(methodNode& n)
{
   m_pBuilder->addMethod(n.name);
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

void globalTypePropagator::visit(fieldNode& n)
{
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,type::gNodeCache->demand(n.demandSoleChild<typeNode>()));
}

void globalTypePropagator::visit(constNode& n)
{
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,type::gNodeCache->demand(n.demandSoleChild<typeNode>()));
}

void typePropagator::visit(argNode& n)
{
   hNodeVisitor::visit(n);
   type::gNodeCache->publish(n,type::gNodeCache->demand(*n.getChildren()[0]));
}

void typePropagator::visit(invokeNode& n)
{
   // TODO need function types for this... do I need this?   YES! at least, eventually
   // defer this until method linking is done in the symbol table
   hNodeVisitor::visit(n);
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

   auto fqn = fullyQualifiedName::build(n.pTarget.getRefee()->getNode(),n.pTarget.ref);
   auto& rVal = type::gTable->fetch(fqn).as<type::iFunctionType>().getReturnType();
   type::gNodeCache->publish(n,rVal);
}

void typePropagator::visit(localDeclNode& n)
{
   hNodeVisitor::visit(n);

   type::gNodeCache->publish(n,type::gNodeCache->demand(n.demandSoleChild<typeNode>()));
}

void typePropagator::visit(varRefNode& n)
{
   hNodeVisitor::visit(n);

   type::gNodeCache->publish(n,type::gNodeCache->demand(*n.pSrc._getRefee()));
}

void typePropagator::visit(bopNode& n)
{
   hNodeVisitor::visit(n);

   type::gNodeCache->publish(n,type::gNodeCache->demand(*n.getChildren()[0]));
}

void typePropagator::visit(indexNode& n)
{
   hNodeVisitor::visit(n);

   auto& lhs = type::gNodeCache->demand(*n.getChildren()[0]);

   // maybe operator overloading will kick in?
   if(lhs.is<type::iStructType>())
   {
      auto& asStruct = lhs.as<type::iStructType>();
      if(asStruct.hasMethod("indexOpGet"))
      {
         std::string funcFqn = lhs.getName() + ".indexOpGet";
         auto& func = type::gTable->fetch(funcFqn).as<type::iFunctionType>();
         type::gNodeCache->publish(n,func.getReturnType());
         return;
      }
   }

   // otherwise, it's just an erray
   std::unique_ptr<type::typeBuilder> pBuilder(type::typeBuilder::open(lhs));
   pBuilder->unwrapArray();

   type::gNodeCache->publish(n,pBuilder->finish());
}

void propagateTypes(cmn::node& root)
{
   { builtInTypeVisitor v; root.acceptVisitor(v); }
   { userTypeVisitor v; root.acceptVisitor(v); }
   { functionVisitor v; root.acceptVisitor(v); }
   { globalTypePropagator v; root.acceptVisitor(v); }
   { typePropagator v; root.acceptVisitor(v); }
}

} // namespace cmn

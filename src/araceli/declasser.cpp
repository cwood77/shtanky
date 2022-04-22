#include "declasser.hpp"

namespace araceli {

void declasser::visit(cmn::invokeNode& n)
{
   std::unique_ptr<cmn::invokeFuncPtrNode> pInvoke(new cmn::invokeFuncPtrNode());
   std::unique_ptr<cmn::fieldAccessNode> pVTable(new cmn::fieldAccessNode());
   std::unique_ptr<cmn::fieldAccessNode> pFuncPtr(new cmn::fieldAccessNode());
   std::unique_ptr<cmn::varRefNode> pSelf(new cmn::varRefNode());
   std::unique_ptr<cmn::argNode> pSelfArg(new cmn::argNode());

   // stash the inst name (and assume it's a varRefnode... d'oh!)
   cmn::varRefNode& inst = dynamic_cast<cmn::varRefNode&>(*n.getChildren()[0]);
   std::string instName = inst.pDef.ref;

   // configure the nodes
   pVTable->name = "_vtbl";
   pFuncPtr->name = n.proto.ref;
   pSelf->pDef.ref = instName;
   pSelf->pDef.bind(*inst.pDef.getRefee());

   // chain the nodes
   pVTable->appendChild(inst);
   pFuncPtr->appendChild(*pVTable.release());
   pSelfArg->appendChild(*pSelf.release());
   pInvoke->appendChild(*pFuncPtr.release());
   pInvoke->appendChild(*pSelfArg.release());
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      if(*it == &inst)
         continue;
      pInvoke->appendChild(**it);
   }
   n.getChildren().clear();

   // replace invoke with invokeFuncPtr
   auto nInvoke = pInvoke.get();
   delete n.getParent()->replaceChild(n,*pInvoke.release());

   hNodeVisitor::visit(*nInvoke);
}

void declasser::visit(cmn::varRefNode& n)
{
   hNodeVisitor::visit(n);

   auto pType = n.pDef.getRefee();
   if(!pType)
      throw std::runtime_error("ISE: unbound verRef in transform");

   const bool isField = (dynamic_cast<cmn::fieldNode*>(pType->getParent()));

   if(isField)
   {
      // change reference into explicit field access on 'self'
      // otherwise, it's a local or arg; leave it alone

      std::unique_ptr<cmn::varRefNode> pSelf(new cmn::varRefNode());
      std::unique_ptr<cmn::fieldAccessNode> pField(new cmn::fieldAccessNode());

      pSelf->pDef.ref = "self";
      pField->name = n.pDef.ref;

      pField->appendChild(*pSelf.release());

      delete n.getParent()->replaceChild(n,*pField.release());
   }
}

} // namespace araceli

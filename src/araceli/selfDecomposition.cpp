#include "selfDecomposition.hpp"

namespace araceli {

void selfDecomposition::visit(cmn::methodNode& n)
{
   if(!(n.flags & cmn::nodeFlags::kStatic))
   {
      // inject a 'self' argument first

      cmn::node root;
      cmn::treeWriter(root)
      .append<cmn::argNode>([](auto&a){a.name="self";})
         .append<cmn::userTypeNode>([&](auto&t)
         {
            t.pDef.ref = cmn::fullyQualifiedName::build(n.getAncestor<cmn::classNode>());
            t.pDef.bind(n.getAncestor<cmn::classNode>());
         })
      ;
      n.insertChild(0,*root.lastChild());
      root.getChildren().clear();
   }

   hNodeVisitor::visit(n);
}

void selfDecomposition::visit(cmn::invokeNode& n)
{
   // change invoke nodes to invokeFuncPtr nodes using a vtbl

   // grab instance (LHS) of invoke
   // this is some variable expression.  It's a class, but could be any class.
   cmn::node *pInstance = n.getChildren()[0];
   n.removeChild(*pInstance);

   // write a new tree of invokeFuncPtr
   std::unique_ptr<cmn::invokeFuncPtrNode> pFuncPtr(new cmn::invokeFuncPtrNode());
   cmn::treeWriter w(*pFuncPtr.get());
   w
   .append<cmn::fieldAccessNode>([&](auto&f){f.name=n.proto.ref;})
      .append<cmn::fieldAccessNode>([](auto&f){f.name="_vtbl";})
         .get().appendChild(*pInstance);
   ;

   // the first parameter to any method is the self
   pFuncPtr->appendChild(cmn::cloneTree(*pInstance));

   // transfer any remaining children to the new node
   while(n.getChildren().size())
   {
      pFuncPtr->appendChild(*n.getChildren()[0]);
      n.removeChild(*n.getChildren()[0]);
   }

   // replace the invoke
   auto *pNaked = pFuncPtr.get();
   delete n.getParent()->replaceChild(n,*pFuncPtr.release());

   hNodeVisitor::visit(*pNaked);
}

void selfDecomposition::visit(cmn::varRefNode& n)
{
   hNodeVisitor::visit(n);

   auto pSrc = n.pSrc.getRefee();
   const bool isField = (dynamic_cast<cmn::fieldNode*>(pSrc));

   // change reference into explicit field access on 'self'
   // otherwise, it's a local or arg; leave it alone
   if(!isField)
      return;

   std::unique_ptr<cmn::varRefNode> pSelf(new cmn::varRefNode());
   std::unique_ptr<cmn::fieldAccessNode> pField(new cmn::fieldAccessNode());

   pSelf->pSrc.ref = "self";
   pField->name = n.pSrc.ref;

   pField->appendChild(*pSelf.release());
   delete n.getParent()->replaceChild(n,*pField.release());
}

} // namespace araceli

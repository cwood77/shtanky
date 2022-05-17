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
         })
      ;
      n.insertChild(0,*root.lastChild());
      root.getChildren().clear();
   }

   hNodeVisitor::visit(n);
}

void selfDecomposition::visit(cmn::invokeNode& n)
{
   // first, determine whether this is a virtual call or not
   const bool isVirtual = n.proto.getRefee()->isDynDispatch();

   // change invoke nodes to invokeFuncPtr nodes using a vtbl, or call nodes

   // grab instance (LHS) of invoke
   // this is some variable expression.  It's a class, but could be any class.
   cmn::node *pInstance = n.getChildren()[0];
   n.removeChild(*pInstance);

   // write a new tree of invokeFuncPtr
   std::unique_ptr<cmn::node> pLowered;
   if(isVirtual)
   {
      pLowered.reset(new cmn::invokeFuncPtrNode());
      cmn::treeWriter w(*pLowered.get());
      w
      .append<cmn::fieldAccessNode>([&](auto&f){f.name=n.proto.ref;})
         .append<cmn::fieldAccessNode>([](auto&f){f.name="_vtbl";})
            .get().appendChild(cmn::cloneTree(*pInstance));
      ;
   }
   else
   {
      auto *pC = new cmn::callNode();
      pC->pTarget.ref = cmn::fullyQualifiedName::build(
         n.proto.getRefee()->getNode(),
         n.proto.getRefee()->getShortName());
      pLowered.reset(pC);
   }

   // the first parameter to any method is the self
   pLowered->appendChild(*pInstance);

   // transfer any remaining children to the new node
   while(n.getChildren().size())
   {
      pLowered->appendChild(*n.getChildren()[0]);
      n.removeChild(*n.getChildren()[0]);
   }

   // replace the invoke
   auto *pNaked = pLowered.get();
   delete n.getParent()->replaceChild(n,*pLowered.release());

   visitChildren(*pNaked);
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

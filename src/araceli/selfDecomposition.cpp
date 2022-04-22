#include "selfDecomposition.hpp"

namespace araceli {

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
}

} // namespace araceli

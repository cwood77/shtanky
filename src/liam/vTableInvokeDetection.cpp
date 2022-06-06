#include "../cmn/type.hpp"
#include "vTableInvokeDetection.hpp"

namespace liam {

// typical vtbl invoke:
//
//             invokeFuncPtrNode                            invokeVTableNode
//  fieldAccess(name)       self  arg1  arg2   ->   fieldAccess(_vtbl)  self arg1 arg2
//  fieldAccess(_vtbl)                                    self
//       self
//
// so find invokeFuncPtrNodes with two fieldAccess nodes, where leafmost FA node is
// accessing the name 'vtbl' and it's return type has the attribute '[vtbl]'
//
void vTableInvokeDetector::visit(cmn::invokeFuncPtrNode& n)
{
   hNodeVisitor::visit(n);

   cmn::node& inst = *n.getChildren()[0];
   auto *pFa = dynamic_cast<cmn::fieldAccessNode*>(&inst);
   if(!pFa) return;
   if(pFa->getChildren().size() != 1) return;

   auto *pVTable = dynamic_cast<cmn::fieldAccessNode*>(pFa->getChildren()[0]);
   if(!pVTable) return;
   if(pVTable->name != "_vtbl") return;

   auto& ty = cmn::type::gNodeCache->demand(*pVTable);
   if(!ty.is<cmn::type::iStructType>()) return;
   auto& sty = ty.as<cmn::type::iStructType>();

   // checks all pass, replace the nodes

   auto *pNoob = new cmn::invokeVTableNode();
   pNoob->index = sty.getOffsetOfField(pFa->name,m_t);
   pNoob->appendChild(*pVTable);
   pFa->getChildren().clear();
   for(size_t i=1;i<n.getChildren().size();i++)
      pNoob->appendChild(*n.getChildren()[i]);
   n.getChildren().clear();
   delete n.getParent()->replaceChild(n,*pNoob);
}

} // namespace liam

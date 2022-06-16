#include "../cmn/stlutil.hpp"
#include "loopTransforms.hpp"

namespace liam {

void forLoopToWhileLoopConverter::visit(cmn::forLoopNode& n)
{
   hNodeVisitor::visit(n);

   // replace this for loop with a while loop

   // i->setBounds(min,max)
   cmn::fileNode root;
   cmn::treeWriter(root)
      .append<cmn::callNode>([](auto& c){c.pTarget.ref=".sht.core.forLoopInst.setBounds";})
         .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
            .backTo<cmn::callNode>()
         .append<cmn::intLiteralNode>([](auto& i){ i.lexeme="0"; })
            .backTo<cmn::callNode>()
         .append<cmn::intLiteralNode>([](auto& i){ i.lexeme="1"; })
   ;

   auto *pCall = root.getChildren()[0];
   std::unique_ptr<cmn::node> pOld(n.getParent()->replaceChild(n,*pCall));
   root.getChildren().clear();

   // i->setDir(true)
   cmn::treeWriter(*pCall)
      .after<cmn::callNode>([](auto& c){c.pTarget.ref=".sht.core.loopInstBase.setDir";})
         .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
            .backTo<cmn::callNode>()
         .append<cmn::boolLiteralNode>([](auto& i){ i.value=true; })
            .backTo<cmn::callNode>()
   ;

   // while(i->inBounds())
   auto& w = cmn::treeWriter(*pCall)
      .after<cmn::whileLoopNode>([&](auto& l)
      {
         // loops need even lineNumbers intact for GUID calculation, so copy everything
         cmn::fieldCopyingNodeVisitor v(l,false);
         v.visit(static_cast<cmn::loopBaseNode&>(n));
      })
         .after<cmn::callNode>([](auto& c){c.pTarget.ref=".sht.core.forLoopInst.inBounds";})
            .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
               .backTo<cmn::whileLoopNode>().get()
   ;

   // transfer over loop body
   for(auto *pChild : pOld->getChildren())
      w.appendChild(*pChild);
   pOld->getChildren().clear();
}

void loopDecomposer::visit(cmn::whileLoopNode& n)
{
   auto& seq = n.demandSoleChild<cmn::sequenceNode>();
   auto guid = n.computeLoopGuid();

   auto& ifNode = cmn::treeWriter(seq)
      // <loopstart>
      .prepend<cmn::loopStartNode>([&](auto& b){ b.name = guid; })

      // if(<condition>)
      //    ;
      // else
      //    <break>
      .after<cmn::ifNode>()
         .append<cmn::sequenceNode>()
            .backTo<cmn::ifNode>()
         .append<cmn::sequenceNode>()
            .append<cmn::loopBreakNode>([&](auto& b){ b.name = guid; })
               .backTo<cmn::ifNode>().get()
   ;

   cmn::treeWriter(seq)
      // i->bump();
      .append<cmn::invokeFuncPtrNode>()
         .append<cmn::fieldAccessNode>([](auto& f){ f.name = "bump"; })
            .append<cmn::fieldAccessNode>([](auto& f){ f.name = "_vtbl"; })
               .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
                  .backTo<cmn::invokeFuncPtrNode>()
         .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
            .backTo<cmn::sequenceNode>()

      // <loopend>
      .append<cmn::loopEndNode>([&](auto& b){ b.name = guid; })
   ;

   // fill in the condition of the if
   ifNode.insertChild(0,*n.getChildren()[0]);

   // delete myself and orphan my kids (they're adopted now)
   n.getChildren().clear();
   delete n.getParent()->replaceChild(n,seq);
}

} // namespace liam

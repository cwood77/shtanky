#include "../cmn/stlutil.hpp"
#include "loopTransforms.hpp"

namespace liam {

void forLoopToWhileLoopConverter::visit(cmn::forLoopNode& n)
{
   hNodeVisitor::visit(n);

   if(n.getChildren().size() != 3)
      cdwTHROW("forLoopNode children is %d != 3",n.getChildren().size());

   // replace this for loop with a while loop

   // i->setBounds(min,max)
   cmn::fileNode root;
   auto& c = cmn::treeWriter(root)
      .append<cmn::callNode>([](auto& c){c.pTarget.ref=".sht.core.forLoopInst.setBounds";})
         .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
            .backTo<cmn::callNode>().get()
   ;
   c.appendChild(*n.getChildren()[0]); n.getChildren().erase(n.getChildren().begin());
   c.appendChild(*n.getChildren()[0]); n.getChildren().erase(n.getChildren().begin());

   // replace old loop with set bounds, but keep old copy for later
   auto *pSetBnds = root.getChildren()[0];
   std::unique_ptr<cmn::node> pOldLoop(n.getParent()->replaceChild(n,*pSetBnds));
   root.getChildren().clear();

   auto& w = cmn::treeWriter(*pSetBnds)
      // i->setDir(true)
      .after<cmn::callNode>([](auto& c){c.pTarget.ref=".sht.core.loopInstBase.setDir";})
         .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
            .backTo<cmn::callNode>()
         .append<cmn::boolLiteralNode>([](auto& i){ i.value=true; })
            .backTo<cmn::callNode>()

      // while(i->inBounds())
      .after<cmn::whileLoopNode>([&](auto& l)
      {
         // loops need even lineNumbers intact for GUID calculation, so copy everything
         cmn::fieldCopyingNodeVisitor v(l,false);
         v.visit(static_cast<cmn::loopBaseNode&>(n));
      })
         .append<cmn::callNode>([](auto& c){c.pTarget.ref=".sht.core.forLoopInst.inBounds";})
            .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
               .backTo<cmn::whileLoopNode>().get()
   ;

   // transfer over loop body
   w.appendChild(*pOldLoop->getChildren()[0]); pOldLoop->getChildren().clear();
}

void loopDecomposer::visit(cmn::whileLoopNode& n)
{
   if(n.getChildren().size() != 2)
      cdwTHROW("whileLoop children is %d != 2",n.getChildren().size());

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

   // delete myself and disown my kids (they're adopted now)
   n.getChildren().clear();
   delete n.getParent()->replaceChild(n,seq);
}

} // namespace liam

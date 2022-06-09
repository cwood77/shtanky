#include "../cmn/stlutil.hpp"
#include "loopTransforms.hpp"

namespace liam {

void loopDecomposer::visit(cmn::forLoopNode& n)
{
   auto& seq = n.demandSoleChild<cmn::sequenceNode>();
   auto guid = n.computeLoopGuid();

   // some of these statements are prepended, so they're backwards

   // if(i:_condition)
   //    ;
   // else
   //    <break>
   cmn::treeWriter(seq)
      .prepend<cmn::ifNode>()
         .append<cmn::fieldAccessNode>([](auto& f){ f.name = "_condition"; })
            .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
               .backTo<cmn::ifNode>()
         .append<cmn::sequenceNode>()
               .backTo<cmn::ifNode>()
         .append<cmn::sequenceNode>()
            .append<cmn::loopBreakNode>([&](auto& b){ b.name = guid; })
   ;

   // i:_condition = i:count < 1;
   cmn::treeWriter(seq)
      .prepend<cmn::assignmentNode>()
         .append<cmn::fieldAccessNode>([](auto& f){ f.name = "_condition"; })
            .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
               .backTo<cmn::assignmentNode>()
         .get().appendChild(*n.getChildren()[0]);
   ;

   // <loopstart>
   cmn::treeWriter(seq)
      .prepend<cmn::loopStartNode>([&](auto& b){ b.name = guid; })
         .backTo<cmn::sequenceNode>()
      .append<cmn::callNode>([](auto& c){ c.pTarget.ref = ".sht.core.loopInst.bump"; })
         .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = n.name; })
            .backTo<cmn::sequenceNode>()
      .append<cmn::loopEndNode>([&](auto& b){ b.name = guid; })
   ;

   n.getChildren().clear();
   delete n.getParent()->replaceChild(n,seq);
}

} // namespace liam

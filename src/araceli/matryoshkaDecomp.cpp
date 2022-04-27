#include "../cmn/ast.hpp"
#include "classInfo.hpp"
#include "matryoshkaDecomp.hpp"

namespace araceli {

void matryoshkaDecomposition::run()
{
   for(auto it=m_cc.classes.begin();it!=m_cc.classes.end();++it)
      writeSuperCtor(it->second);
}

void matryoshkaDecomposition::writeSuperCtor(classInfo& ci)
{
   cmn::node root;
   cmn::treeWriter w(root);
   w
   // function decl
   .append<cmn::funcNode>([&](auto& f){ f.name = ci.name + "_sctor"; })
      .append<cmn::argNode>([](auto& a){ a.name = "self"; })
         .append<cmn::userTypeNode>([&](auto& t) { t.pDef.ref = ci.fqn; })
            .backTo<cmn::funcNode>()
      .append<cmn::voidTypeNode>()
         .backTo<cmn::funcNode>()
      .append<cmn::sequenceNode>()
   ;

   // cache self arg
   cmn::argNode& selfArg
      = dynamic_cast<cmn::argNode&>(
         *w.get().getAncestor<cmn::funcNode>().getChildren()[0]);

   if(ci.bases.size())
   {
      w
      // call base sctor
      .append<cmn::callNode>([&](auto& c)
      {
         c.pTarget.ref = ci.bases.back() + "_sctor";
      })
         .append<cmn::varRefNode>([&](auto& v) { v.pSrc.ref = "self"; })
            .backTo<cmn::sequenceNode>()
      ;
   }

   w
   // set vtbl
   .append<cmn::assignmentNode>()
      .append<cmn::fieldAccessNode>([](auto& f){ f.name = "_vtbl"; })
         .append<cmn::varRefNode>([&](auto& v) { v.pSrc.ref = "self"; })
            .backTo<cmn::assignmentNode>()
      .append<cmn::varRefNode>([&](auto& v)
      {
         v.pSrc.ref = ci.fqn + "_vtbl_inst";
         // TODO left unbound!
      })
         .backTo<cmn::sequenceNode>()

   // call cctor
   .append<cmn::callNode>([&](auto& c)
   {
      c.pTarget.ref = ci.fqn + ".cctor";
   })
      .append<cmn::varRefNode>([&](auto& v) { v.pSrc.ref = "self"; })
         .backTo<cmn::sequenceNode>()
   ;

   // add nodes
   ci.pNode->getAncestor<cmn::fileNode>().appendChild(*root.getChildren()[0]);
   root.getChildren().clear();
}

void matryoshkaDecomposition::linkSuperCtors()
{
}

} // namespace araceli

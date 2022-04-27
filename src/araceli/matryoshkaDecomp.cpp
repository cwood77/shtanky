#include "../cmn/ast.hpp"
#include "classInfo.hpp"
#include "matryoshkaDecomp.hpp"

namespace araceli {

void matryoshkaDecomposition::run()
{
   for(auto it=m_cc.classes.begin();it!=m_cc.classes.end();++it)
   {
      writeSuperFunc(it->second,"ctor",true);
      writeSuperFunc(it->second,"dtor",false);
   }
}

void matryoshkaDecomposition::writeBaseCall(classInfo& ci, const std::string& ssuffix, cmn::treeWriter& w)
{
   w
   .append<cmn::callNode>([&](auto& c)
   {
      c.pTarget.ref = ci.bases.back() + ssuffix;
   })
      .append<cmn::varRefNode>([&](auto& v) { v.pSrc.ref = "self"; })
         .backTo<cmn::sequenceNode>()
   ;
}

void matryoshkaDecomposition::writeSuperFunc(classInfo& ci, const std::string& suffix, bool baseFirst)
{
   const std::string ssuffix = "_s" + suffix;
   const std::string csuffix = ".c" + suffix;

   cmn::node root;
   cmn::treeWriter w(root);
   w
   // function decl
   .append<cmn::funcNode>([&](auto& f){ f.name = ci.name + ssuffix; })
      .append<cmn::argNode>([](auto& a){ a.name = "self"; })
         .append<cmn::userTypeNode>([&](auto& t) { t.pDef.ref = ci.fqn; })
            .backTo<cmn::funcNode>()
      .append<cmn::voidTypeNode>()
         .backTo<cmn::funcNode>()
      .append<cmn::sequenceNode>()
   ;

   // call base sctor/sdtor
   if(ci.bases.size() && baseFirst)
      writeBaseCall(ci,ssuffix,w);

   w
   // set vtbl
   .append<cmn::assignmentNode>()
      .append<cmn::fieldAccessNode>([](auto& f){ f.name = "_vtbl"; })
         .append<cmn::varRefNode>([&](auto& v) { v.pSrc.ref = "self"; })
            .backTo<cmn::assignmentNode>()
      .append<cmn::varRefNode>([&](auto& v)
      {
         v.pSrc.ref = ci.fqn + "_vtbl_inst";
      })
         .backTo<cmn::sequenceNode>()

   // call cctor
   .append<cmn::callNode>([&](auto& c)
   {
      c.pTarget.ref = ci.fqn + csuffix;
   })
      .append<cmn::varRefNode>([&](auto& v) { v.pSrc.ref = "self"; })
         .backTo<cmn::sequenceNode>()
   ;

   // call base sctor/sdtor
   if(ci.bases.size() && !baseFirst)
      writeBaseCall(ci,ssuffix,w);

   // add nodes
   ci.pNode->getAncestor<cmn::fileNode>().appendChild(*root.getChildren()[0]);
   root.getChildren().clear();
}

} // namespace araceli

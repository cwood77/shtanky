#include "ctorDtorGenerator.hpp"

namespace araceli {

void ctorDtorGenerator::visit(cmn::classNode& n)
{
   cmn::treeWriter w(n);

   w
      .append<cmn::methodNode>([](auto&m){m.name="cctor";})
         .append<cmn::sequenceNode>()
      .backTo<cmn::classNode>()
      .append<cmn::methodNode>([](auto&m)
      {
         m.name = "cdtor";
         m.flags |= cmn::nodeFlags::kOverride;
      })
         .append<cmn::sequenceNode>()
   ;

#if 0
   // supert-constructor
   cmn::userTypeNode *pSelf = NULL;
   w
   .backTo<cmn::fileNode>()
      .append<cmn::funcNode>([&](auto& f)
      { f.name = cmn::fullyQualifiedName::build(n,".sctor"); })
         .append<cmn::argNode>([](auto& a){ a.name = "self"; })
            .append<cmn::userTypeNode>([&](auto& t)
            { t.pDef.ref = cmn::fullyQualifiedName::build(n); t.pDef.bind(n); pSelf=&t; })
      .backTo<cmn::funcNode>()
         .append<cmn::sequenceNode>()
   ;
#endif

#if 0
   w
      .append<cmn::assignmentNode>()
         .append<cmn::fieldAccessNode>([](auto& f){ f.name = "vtbl"; })
            .append<cmn::varRefNode>([&](auto& v){ v.pDef.ref="self"; v.pDef.bind(*pSelf); })
         .parent()
         .append<cmn::varRefNode>()
   ;
#endif
}

} // namespace araceli

#include "ctorDtorGenerator.hpp"

namespace araceli {

void ctorDtorGenerator::visit(cmn::classNode& n)
{
   auto classFqn = cmn::fullyQualifiedName::build(n);

   cmn::treeWriter(n)
   .append<cmn::methodNode>([](auto&m){m.name="cctor";})
      .append<cmn::sequenceNode>()
         .backTo<cmn::methodNode>()
      .append<cmn::userTypeNode>([&](auto& t)
      {
         t.pDef.ref = classFqn;
         t.pDef.bind(n);
      })

      .backTo<cmn::classNode>()
   .append<cmn::methodNode>([](auto&m)
   {
      m.name = "cdtor";
      m.flags |= cmn::nodeFlags::kVirtual;
   })
      .append<cmn::sequenceNode>()
         .backTo<cmn::methodNode>()
      .append<cmn::userTypeNode>([&](auto& t)
      {
         t.pDef.ref = classFqn;
         t.pDef.bind(n);
      })
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
            .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref="self"; v.pSrc.bind(*pSelf); })
         .parent()
         .append<cmn::varRefNode>()
   ;
#endif
}

} // namespace araceli

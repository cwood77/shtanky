#include "ctorDtorGenerator.hpp"

namespace araceli {

void ctorDtorGenerator::visit(cmn::classNode& n)
{
   auto classFqn = cmn::fullyQualifiedName::build(n);

   cmn::treeWriter(n)
   .append<cmn::methodNode>([](auto&m){m.name="cctor";})
      .append<cmn::sequenceNode>()
         .backTo<cmn::methodNode>()
      .append<cmn::userTypeNode>([&](auto& t) { t.pDef.ref = classFqn; })

      .backTo<cmn::classNode>()
   .append<cmn::methodNode>([](auto&m)
   {
      m.name = "cdtor";
      m.flags |= cmn::nodeFlags::kVirtual;
   })
      .append<cmn::sequenceNode>()
         .backTo<cmn::methodNode>()
      .append<cmn::userTypeNode>([&](auto& t) { t.pDef.ref = classFqn; })
   ;
}

} // namespace araceli

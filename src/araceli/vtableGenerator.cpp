#include "../cmn/ast.hpp"
#include "classInfo.hpp"
#include "vtableGenerator.hpp"

namespace araceli {

void vtableGenerator::generate(classCatalog& cc)
{
   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      std::unique_ptr<cmn::classNode> pClass(new cmn::classNode());
      pClass->name = cit->first + "_vtbl2";

      std::unique_ptr<cmn::constNode> pConst(new cmn::constNode());
      pConst->name = cit->first + "_vtbl2_inst";

      auto& sNode = cmn::treeWriter(*pConst.get())
         .append<cmn::userTypeNode>([&](auto& t)
         {
            t.pDef.ref = pClass->name;
            t.pDef.bind(*pClass.get());
         })
            .backTo<cmn::constNode>()
         .append<cmn::structLiteralNode>()
            .get();

      for(auto mit=cit->second.inheritedAndDirectMethods.begin();mit!=cit->second.inheritedAndDirectMethods.end();++mit)
      {
         cmn::treeWriter(*pClass.get())
            .append<cmn::fieldNode>([&](auto& f){ f.name = mit->name; })
               .append<cmn::ptrTypeNode>();

         cmn::treeWriter(sNode)
            .append<cmn::varRefNode>([=](auto& v)
            {
               v.pSrc.ref = mit->fqn;
//               v.pSrc.bind(*mit->pMethod);
            });
      }

      cmn::fileNode& file = cit->second.pNode->getAncestor<cmn::fileNode>();
      file.appendChild(*pClass.release());
      file.appendChild(*pConst.release());
   }
}

} // namespace araceli

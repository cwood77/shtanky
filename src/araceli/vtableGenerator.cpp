#include "../cmn/ast.hpp"
#include "../cmn/nameUtil.hpp"
#include "classInfo.hpp"
#include "vtableGenerator.hpp"

namespace araceli {

void vtableGenerator::generate(classCatalog& cc)
{
   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      std::unique_ptr<cmn::classNode> pClass(new cmn::classNode());
      pClass->name = cit->second.name + "_vtbl";

      std::unique_ptr<cmn::constNode> pConst(new cmn::constNode());
      pConst->name = cit->second.name + "_vtbl_inst";

      auto& sNode = cmn::treeWriter(*pConst.get())
         .append<cmn::userTypeNode>([&](auto& t)
         {
            t.pDef.ref = pClass->name;
            t.pDef.bind(*pClass.get());
         })
            .backTo<cmn::constNode>()
         .append<cmn::structLiteralNode>()
            .get();

      for(auto mit=cit->second.inheritedAndDirectMethods.begin();
         mit!=cit->second.inheritedAndDirectMethods.end();++mit)
      {
         cmn::treeWriter(*pClass.get())
            .append<cmn::fieldNode>([&](auto& f){ f.name = mit->name; })
               .append<cmn::ptrTypeNode>();

         cmn::treeWriter(sNode)
            .append<cmn::varRefNode>([=](auto& v)
            {
               v.pSrc.ref = cmn::fullyQualifiedName::build(*cit->second.pNode,mit->name);
               // these methods have been moved, so bind to the function version
               v.pSrc.bind(*mit->pAsFunc);
            });
      }

      cit->second.pVTableClass = pClass.get();

      cmn::fileNode& file = cit->second.pNode->getAncestor<cmn::fileNode>();
      file.appendChild(*pClass.release());
      file.appendChild(*pConst.release());
   }
}

} // namespace araceli

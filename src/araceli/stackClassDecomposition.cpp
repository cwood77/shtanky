#include "stackClassDecomposition.hpp"

namespace araceli {

void stackClassDecomposition::visit(cmn::localDeclNode& n)
{
   auto& ty = n.demandSoleChild<cmn::typeNode>();
   auto *pAsClass = dynamic_cast<cmn::userTypeNode*>(&ty);
   if(pAsClass)
   {
      if(pAsClass->pDef.getRefee() == NULL)
         cdwTHROW("unlinked user type for local");

      m_stackClasses.push_back(std::make_pair<cmn::localDeclNode*,std::string>(
         &n,
         cmn::fullyQualifiedName::build(*pAsClass->pDef.getRefee())));
   }
}

void stackClassDecomposition::inject()
{
   for(auto it=m_stackClasses.rbegin();it!=m_stackClasses.rend();++it)
      inject(*it->first,it->second);
}

void stackClassDecomposition::inject(cmn::localDeclNode& local, const std::string& className)
{
   auto& seq = local.getAncestor<cmn::sequenceNode>();

   cmn::sequenceNode root;
   cmn::treeWriter(root)

   // sctor call
   .append<cmn::callNode>([&](auto& c){ c.pTarget.ref = className + "_sctor"; })
      .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = local.name; })
         .backTo<cmn::sequenceNode>()

   // sdtor call
   .append<cmn::callNode>([&](auto& c){ c.pTarget.ref = className + "_sdtor"; })
      .append<cmn::varRefNode>([&](auto& v){ v.pSrc.ref = local.name; })

   ;

   // place them at start and end of sequence
   //seq.insertChild(0,*root.getChildren()[0]);
   local.getParent()->insertChildAfter(*root.getChildren()[0],local);
   seq.appendChild(*root.getChildren()[1]);
   root.getChildren().clear();
}

} // namespace araceli

#include "../cmn/trace.hpp"
#include "abstractGenerator.hpp"

namespace araceli {

void abstractGenerator::visit(cmn::classNode& n)
{
   populateVTable(n);

   auto fqn = cmn::fullyQualifiedName::build(n);
   classInfoOLD& info = m_infos.catalog[fqn];

   // generate abstract bodies
   for(auto it=info.lastMethod.begin();it!=info.lastMethod.end();++it)
   {
      if(it->second->flags & cmn::nodeFlags::kAbstract)
      {
         cdwDEBUG("would generate abstract impl of method '%s' for class '%s'\n",
            it->first.c_str(),
            fqn.c_str());

         cmn::methodNode *pNoob = NULL;
         if(&it->second->getAncestor<cmn::classNode>() == &n)
         {
            // I am the abstract base class... just convert the node
            pNoob = it->second;
         }
         else
         {
            // I inherited this interface without implementing it
            // Implement it now
            pNoob = &dynamic_cast<cmn::methodNode&>(cmn::cloneTree(*it->second));
            n.appendChild(*pNoob);
         }

         //pNoob->flags &= ~cmn::nodeFlags::kAbstract;
         //pNoob->flags |= cmn::nodeFlags::kOverride;
         pNoob->appendChild(*new cmn::sequenceNode());
      }
   }
}

void abstractGenerator::populateVTable(cmn::classNode& n)
{
   auto fqn = cmn::fullyQualifiedName::build(n);
   classInfoOLD& info = m_infos.catalog[fqn];

   // calculate all base classes
   auto l = n.computeLineage();
   for(auto it=l.begin();it!=l.end();++it)
   {
      auto methods = (*it)->getChildrenOf<cmn::methodNode>();
      for(auto mit=methods.begin();mit!=methods.end();++mit)
      {
         if((*mit)->flags & (
            cmn::nodeFlags::kOverride |
            cmn::nodeFlags::kAbstract))
         {
            info.vTable[(*mit)->name] = cmn::fullyQualifiedName::build(**mit,(*mit)->name);
            info.lastMethod[(*mit)->name] = *mit;
         }
         if((*mit)->flags & cmn::nodeFlags::kAbstract)
            info.abstracts.insert(cmn::fullyQualifiedName::build(**mit,(*mit)->name));
      }
   }

   // see how many abstracts remain in the table
   std::set<std::string> methodFqns;
   for(auto it=info.vTable.begin();it!=info.vTable.end();++it)
      methodFqns.insert(it->second);
   std::set<std::string> stillAbstract;
   for(auto it=info.abstracts.begin();it!=info.abstracts.end();++it)
      if(methodFqns.find(*it)!=methodFqns.end())
         stillAbstract.insert(*it);
   info.abstracts = stillAbstract;
}

} // namespace araceli

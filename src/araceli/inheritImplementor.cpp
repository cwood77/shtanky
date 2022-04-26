#include "../cmn/ast.hpp"
#include "../cmn/nameUtil.hpp"
#include "../cmn/trace.hpp"
#include "classInfo.hpp"
#include "inheritImplementor.hpp"

namespace araceli {

void inheritImplementor::generate(classCatalog& cc)
{
   combineFieldsAndRemoveBases(cc);
   addVPtrs(cc);
}

// duplicate fields from all base classes directly in each descendant class,
// and remove base class links from class node
void inheritImplementor::combineFieldsAndRemoveBases(classCatalog& cc)
{
   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      classInfo& ci = cit->second;

      // gather fields
      std::vector<cmn::fieldNode*> totalFields;
      for(auto lit=ci.bases.begin();lit!=ci.bases.end();++lit)
      {
         cdwDEBUG("checking ancestor for fields: %s\n",lit->c_str());
         cc.classes[*lit].pNode->getChildrenOf(totalFields);
      }
      cdwDEBUG("found %lld field(s)\n",totalFields.size());

      // inject inherited fields _above_ any fields I actually own
      // (do this by iterating backwards and injecting at head)
      for(auto fit=totalFields.rbegin();fit!=totalFields.rend();++fit)
         ci.pNode->insertChild(0,cmn::cloneTree(**fit));

      // now, remove the base classes
      ci.pNode->baseClasses.clear();
   }
}

// this is performed as a separate pass so we don't "inherit" vptrs fields from bases 
void inheritImplementor::addVPtrs(classCatalog& cc)
{
   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      classInfo& ci = cit->second;

      // inject a vtbl field last (i.e. first)
      {
         cmn::node root;
         cmn::treeWriter(root)
         .append<cmn::fieldNode>([](auto& f){ f.name = "_vtbl"; })
            .append<cmn::userTypeNode>([&](auto& t)
            {
               t.pDef.ref = ci.name + "_vtbl";
               t.pDef.bind(*ci.pVTableClass);
            })
         ;
         ci.pNode->insertChild(0,*root.lastChild());
         root.getChildren().clear();
      }
   }
}

} // namespace araceli

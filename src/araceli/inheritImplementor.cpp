#include "../cmn/ast.hpp"
#include "../cmn/nameUtil.hpp"
#include "../cmn/trace.hpp"
#include "classInfo.hpp"
#include "inheritImplementor.hpp"

namespace araceli {

void fieldInitializer::visit(cmn::boolTypeNode& n)
{
   cmn::treeWriter(m_seq)
   .append<cmn::assignmentNode>()
      .append<cmn::fieldAccessNode>([&](auto& f){ f.name = m_fname; })
         .append<cmn::varRefNode>([](auto& vr){ vr.pSrc.ref = "self"; })
            .backTo<cmn::assignmentNode>()
      .append<cmn::boolLiteralNode>();
}

void fieldInitializer::visit(cmn::userTypeNode& n)
{
   // usertypes are ptrs here, so.... skip sctor calls
   /*
   auto fqn = cmn::fullyQualifiedName::build(*n.pDef.getRefee()) + "_sctor";

   cmn::treeWriter(m_seq)
   .append<cmn::callNode>([&](auto& c){ c.pTarget.ref = fqn; })
      .append<cmn::fieldAccessNode>([&](auto& f){ f.name = m_fname; })
         .append<cmn::varRefNode>([](auto& vr){ vr.pSrc.ref = "self"; });
   */
}

void fieldInitializer::setToZero()
{
   cmn::treeWriter(m_seq)
   .append<cmn::assignmentNode>()
      .append<cmn::fieldAccessNode>([&](auto& f){ f.name = m_fname; })
         .append<cmn::varRefNode>([](auto& vr){ vr.pSrc.ref = "self"; })
            .backTo<cmn::assignmentNode>()
      .append<cmn::intLiteralNode>([](auto& l){ l.lexeme = "0"; });
}

void inheritImplementor::generate(classCatalog& cc)
{
   combineFieldsAndRemoveBases(cc);
   initializeFields(cc);
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

void inheritImplementor::initializeFields(classCatalog& cc)
{
   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      classInfo& ci = cit->second;

      // fill in the compiler-generated ctor
      auto& cctor = ci.pNode->getAncestor<cmn::fileNode>()
         .filterSoleChild<cmn::funcNode>(
            [&](auto& f){ return f.name == ci.name + ".cctor"; })
         .demandSoleChild<cmn::sequenceNode>();

      std::vector<cmn::fieldNode*> totalFields = ci.pNode->getChildrenOf<cmn::fieldNode>();
      for(auto *pF : totalFields)
      {
         bool hasInitializer = (pF->getChildren().size() > 1);
         if(hasInitializer)
         {
            cmn::treeWriter(cctor)
            .append<cmn::assignmentNode>()
               .append<cmn::fieldAccessNode>([&](auto& f){ f.name = pF->name; })
                  .append<cmn::varRefNode>([](auto& vr){ vr.pSrc.ref = "self"; })
                     .backTo<cmn::assignmentNode>().get()
            .appendChild(*pF->getChildren()[1]);
            pF->getChildren().resize(1);
         }
         else
         {
            auto& ty = pF->demandSoleChild<cmn::typeNode>();
            fieldInitializer v(cctor,pF->name);
            ty.acceptVisitor(v);
         }
      }
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
            .append<cmn::userTypeNode>([&](auto& t) { t.pDef.ref = ci.name + "_vtbl"; })
         ;
         ci.pNode->insertChild(0,*root.lastChild());
         root.getChildren().clear();
      }
   }
}

} // namespace araceli

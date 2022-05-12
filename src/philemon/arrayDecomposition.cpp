#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "arrayDecomposition.hpp"

namespace philemon {

void arrayTypeNameBuilder::visit(cmn::boolTypeNode& n)
{
   name << "bool";
   hNodeVisitor::visit(n);
}

void arrayTypeNameBuilder::visit(cmn::intTypeNode& n)
{
   name << "int";
   hNodeVisitor::visit(n);
}

void arrayTypeNameBuilder::visit(cmn::arrayTypeNode& n)
{
   name << ".sht.core.array<";
   n.getParent()->acceptVisitor(*this);
   name << ">";
}

void arrayTypeNameBuilder::visit(cmn::voidTypeNode& n)
{
   name << "void";
   hNodeVisitor::visit(n);
}

void arrayTypeNameBuilder::visit(cmn::userTypeNode& n)
{
   name << n.pDef.ref;
   hNodeVisitor::visit(n);
}

void arrayTypeNameBuilder::visit(cmn::ptrTypeNode& n)
{
   name << "ptr";
   hNodeVisitor::visit(n);
}

void arrayDecomposition::visit(cmn::arrayTypeNode& n)
{
   // only add the leaf-most array, in case they're nested
   if(n.getChildren().size() == 0)
      m_nodes.insert(&n);

   hNodeVisitor::visit(n);
}

void arrayDecomposition::replaceNodes()
{
   for(auto it=m_nodes.begin();it!=m_nodes.end();++it)
   {
      cdwVERBOSE("decomposing array found in %s, line %lu\n",
         (*it)->getAncestor<cmn::fileNode>().fullPath.c_str(),
         (*it)->lineNumber);

      arrayTypeNameBuilder instName;
      (*it)->acceptVisitor(instName);

      auto *pNoob = new cmn::userTypeNode();
      pNoob->pDef.ref = instName.name.str();
      m_instancesNeeded.insert(instName.name.str());

      cmn::typeNode& dead = dynamic_cast<cmn::typeNode&>(*instName.pLastNodeSeen);
      delete dead.getParent()->replaceChild(dead,*pNoob);
   }
}

void arrayDecomposition::injectInstances(cmn::node& r)
{
   // locate the array class
   auto& f = r.filterSoleChild<cmn::scopeNode>([](auto& n)
      { return n.scopeName == "sht"; }
      ).filterSoleChild<cmn::scopeNode>([](auto& n)
         { return n.scopeName == "core"; }
         ).filterSoleChild<cmn::fileNode>([](auto& n)
            { return cmn::pathUtil::getLastPart(n.fullPath) == "array.ara"; });

   // remove instances already there
   auto inodes = f.getChildrenOf<cmn::instantiateNode>();
   for(auto it=inodes.begin();it!=inodes.end();++it)
      m_instancesNeeded.erase((*it)->text);

   // add any new instances
   for(auto it=m_instancesNeeded.begin();it!=m_instancesNeeded.end();++it)
      cmn::treeWriter(f)
         .append<cmn::instantiateNode>([&](auto& i){ i.text = *it; });
}

} // namespace philemon

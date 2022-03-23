#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "codegen.hpp"

namespace araceli {

void codeGen::visit(fileNode& n)
{
   fileNode *pPrev = m_pActiveFile;
   m_pActiveFile = &n;
   hNodeVisitor::visit(n);
   m_pActiveFile = pPrev;
}

void codeGen::visit(classNode& n)
{
   auto& header = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader).stream();
   //auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   // gather fields from self _and_ all parents (no inheritance in liam)
   std::vector<fieldNode*> totalFields;
   if(n.flags & (nodeFlags::kAbstract | nodeFlags::kInterface))
      ; // do not generate data for uninstantiable classes; their descendents will
   else
   {
      auto lineage = n.computeLineage();
      for(auto it=lineage.begin();it!=lineage.end();++it)
         (*it)->getChildrenOf(totalFields);
   }

   if(totalFields.size())
   {
      header
         << "class " << fullyQualifiedName::build(n) << " {" << std::endl
      ;

      for(auto it=totalFields.begin();it!=totalFields.end();++it)
      {
         header << "tbd - " << (*it)->name << std::endl;
      }

      header
         << "}" << std::endl
      ;
   }

   hNodeVisitor::visit(n);
}

} // namespace araceli

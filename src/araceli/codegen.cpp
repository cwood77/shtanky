#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "codegen.hpp"

namespace araceli {

void liamTypeWriter::visit(typeNode& n)
{
   auto typeQuals = n.getChildrenOf<typeNode>();
   for(auto it=typeQuals.begin();it!=typeQuals.end();++it)
      (*it)->acceptVisitor(*this);
}

void liamTypeWriter::visit(strTypeNode& n)
{
   m_o << "str";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(arrayTypeNode& n)
{
   m_o << "[]";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(voidTypeNode& n)
{
   m_o << "void";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(userTypeNode& n)
{
   m_o << n.pDef.ref;
   hNodeVisitor::visit(n);
}

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
   // however, parents _also_ are generated.  This is in case they have methods
   // which would need a self parameter
   std::vector<fieldNode*> totalFields;
   auto lineage = n.computeLineage();
   for(auto it=lineage.begin();it!=lineage.end();++it)
      (*it)->getChildrenOf(totalFields);

   header
      << "class " << fullyQualifiedName::build(n) << " {" << std::endl
   ;

   for(auto it=totalFields.begin();it!=totalFields.end();++it)
   {
      header << "   " << (*it)->name << " : ";

      liamTypeWriter tyW(header);
      (*it)->demandSoleChild<typeNode>().acceptVisitor(tyW);

      header << ";" << std::endl;
   }

   header
      << "}" << std::endl
   ;

   auto methods = n.getChildrenOf<methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
   {
      if(!((*it)->flags & nodeFlags::kAbstract))
      {
         header
            << std::endl
            << fullyQualifiedName::build(**it,(*it)->baseImpl.ref) << "("
         ;

         auto args = (*it)->getChildrenOf<argNode>();
         for(auto jit=args.begin();jit!=args.end();++jit)
         {
            if(jit!=args.begin())
               header << "," << std::endl << "   ";
            header
               << (*jit)->name << " : ";
            ;
            liamTypeWriter tyW(header);
            (*jit)->demandSoleChild<typeNode>().acceptVisitor(tyW);
         }

         header
            << ");" << std::endl
         ;
      }
   }

   hNodeVisitor::visit(n);
}

} // namespace araceli

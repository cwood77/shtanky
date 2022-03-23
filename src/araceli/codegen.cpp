#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "codegen.hpp"

namespace araceli {

void liamTypeWriter::visit(cmn::typeNode& n)
{
   auto typeQuals = n.getChildrenOf<cmn::typeNode>();
   for(auto it=typeQuals.begin();it!=typeQuals.end();++it)
      (*it)->acceptVisitor(*this);
}

void liamTypeWriter::visit(cmn::strTypeNode& n)
{
   m_o << "str";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(cmn::arrayTypeNode& n)
{
   m_o << "[]";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(cmn::voidTypeNode& n)
{
   m_o << "void";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(cmn::userTypeNode& n)
{
   m_o << n.pDef.ref;
   hNodeVisitor::visit(n);
}

void codeGen::visit(cmn::fileNode& n)
{
   cmn::fileNode *pPrev = m_pActiveFile;
   m_pActiveFile = &n;
   hNodeVisitor::visit(n);
   m_pActiveFile = pPrev;
}

void codeGen::visit(cmn::classNode& n)
{
   auto& header = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader).stream();
   //auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   std::string vname;
   generateClassVTable(n,header,vname);
   generateClassType(n,header,vname);
   generateClassPrototypes(n,header);

   hNodeVisitor::visit(n);
}

void codeGen::generateClassVTable(cmn::classNode& n, std::ostream& header, std::string& vname)
{
   std::vector<cmn::methodNode*> totalMethods;
   auto lineage = n.computeLineage();
   for(auto it=lineage.begin();it!=lineage.end();++it)
      (*it)->getChildrenOf(totalMethods);

   std::vector<std::string> names;
   for(auto it=totalMethods.begin();it!=totalMethods.end();++it)
      if((*it)->flags & cmn::nodeFlags::kAbstract)
         names.push_back((*it)->baseImpl.ref);

   if(names.size() == 0)
      return;

   vname = cmn::fullyQualifiedName::build(n,"vtbl");
   header
      << "class " << vname << " {" << std::endl
   ;

   for(auto it=names.begin();it!=names.end();++it)
      header << "   " << *it << " : ptr;" << std::endl;

   header
      << "}" << std::endl
      << std::endl
   ;
}

void codeGen::generateClassType(cmn::classNode& n, std::ostream& header, const std::string& vname)
{
   // gather fields from self _and_ all parents (no inheritance in liam)
   // however, parents _also_ are generated.  This is in case they have methods
   // which would need a self parameter
   std::vector<cmn::fieldNode*> totalFields;
   auto lineage = n.computeLineage();
   for(auto it=lineage.begin();it!=lineage.end();++it)
      (*it)->getChildrenOf(totalFields);

   header
      << "class " << cmn::fullyQualifiedName::build(n) << " {" << std::endl
   ;

   if(!vname.empty())
      header << "   vtbl : " << vname << ";" << std::endl;

   for(auto it=totalFields.begin();it!=totalFields.end();++it)
   {
      header << "   " << (*it)->name << " : ";

      liamTypeWriter tyW(header);
      (*it)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

      header << ";" << std::endl;
   }

   header
      << "}" << std::endl
   ;
}

void codeGen::generateClassPrototypes(cmn::classNode& n, std::ostream& header)
{
   header
      << cmn::fullyQualifiedName::build(n,"ctor") << "() : "
         << cmn::fullyQualifiedName::build(n) << ";" << std::endl;
   ;

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
   {
      if(!((*it)->flags & cmn::nodeFlags::kAbstract))
      {
         header
            << std::endl
            << cmn::fullyQualifiedName::build(**it,(*it)->baseImpl.ref) << "("
         ;

         bool firstParam = true;
         if(!((*it)->flags & cmn::nodeFlags::kStatic))
         {
            header << "self : " << cmn::fullyQualifiedName::build(n);
            firstParam = false;
         }

         auto args = (*it)->getChildrenOf<cmn::argNode>();
         for(auto jit=args.begin();jit!=args.end();++jit)
         {
            if(!firstParam)
               header << "," << std::endl << "   ";

            header
               << (*jit)->name << " : ";
            ;
            liamTypeWriter tyW(header);
            (*jit)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

            firstParam = false;
         }

         header
            << ");" << std::endl
         ;
      }
   }
}

} // namespace araceli

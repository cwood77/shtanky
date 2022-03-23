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

   std::string vname;
   generateClassVTable(n,header,vname);
   generateClassType(n,header,vname);
   generateClassPrototypes(n,header);

   hNodeVisitor::visit(n);
}

void codeGen::generateClassVTable(classNode& n, std::ostream& header, std::string& vname)
{
   std::vector<methodNode*> totalMethods;
   auto lineage = n.computeLineage();
   for(auto it=lineage.begin();it!=lineage.end();++it)
      (*it)->getChildrenOf(totalMethods);

   std::vector<std::string> names;
   for(auto it=totalMethods.begin();it!=totalMethods.end();++it)
      if((*it)->flags & nodeFlags::kAbstract)
         names.push_back((*it)->baseImpl.ref);

   if(names.size() == 0)
      return;

   vname = fullyQualifiedName::build(n,"vtbl");
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

void codeGen::generateClassType(classNode& n, std::ostream& header, const std::string& vname)
{
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

   if(!vname.empty())
      header << "   vtbl : " << vname << ";" << std::endl;

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
}

void codeGen::generateClassPrototypes(classNode& n, std::ostream& header)
{
   header
      << fullyQualifiedName::build(n,"ctor") << "() : "
         << fullyQualifiedName::build(n) << ";" << std::endl;
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

         bool firstParam = true;
         if(!((*it)->flags & nodeFlags::kStatic))
         {
            header << "self : " << fullyQualifiedName::build(n);
            firstParam = false;
         }

         auto args = (*it)->getChildrenOf<argNode>();
         for(auto jit=args.begin();jit!=args.end();++jit)
         {
            if(!firstParam)
               header << "," << std::endl << "   ";

            header
               << (*jit)->name << " : ";
            ;
            liamTypeWriter tyW(header);
            (*jit)->demandSoleChild<typeNode>().acceptVisitor(tyW);

            firstParam = false;
         }

         header
            << ");" << std::endl
         ;
      }
   }
}

} // namespace araceli

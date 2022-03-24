#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "codegen.hpp"
#include <stdexcept>

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
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   std::string vname;
   generateClassVTable(n,header,vname);
   generateClassType(n,header,vname);
   generateClassPrototypes(n,header);

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
      generateClassMethod(n,**it,source);
}

void codeGen::visit(cmn::sequenceNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   source << "{" << std::endl;

   visitChildren(n);

   source << "}" << std::endl;
}

void codeGen::visit(cmn::invokeNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   if(n.getChildren().size() < 1)
      throw std::runtime_error("eh?  bad invokeNode");

   n.getChildren()[0]->acceptVisitor(*this);
   source << "->" << n.proto.ref << "(";
   bool first = true;
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      if(it==n.getChildren().begin())
         continue;
      if(!first)
         source << ",";
      (*it)->acceptVisitor(*this);
      first = false;
   }
   source << ");" << std::endl;
}

// TODO LAME - share some code here
void codeGen::visit(cmn::callNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   source << n.name << "(";
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      if(it==n.getChildren().begin())
         source << ",";
      (*it)->acceptVisitor(*this);
   }
   source << ");" << std::endl;
}

void codeGen::visit(cmn::varRefNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << n.pDef.ref;
   visitChildren(n);
}

void codeGen::visit(cmn::stringLiteralNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << "\"" << n.value << "\"";
   visitChildren(n);
}

void codeGen::visit(cmn::boolLiteralNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << n.value;
   visitChildren(n);
}

void codeGen::visit(cmn::intLiteralNode& n)
{
   auto& source = m_out.get(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << n.value;
   visitChildren(n);
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
      << std::endl
      << "func " << cmn::fullyQualifiedName::build(n,"ctor") << "() : "
         << cmn::fullyQualifiedName::build(n) << ";" << std::endl;
   ;

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
   {
      if(!((*it)->flags & cmn::nodeFlags::kAbstract))
      {
         generateMethodSignature(**it,header);
         header << ";" << std::endl;
      }
   }
}

void codeGen::generateClassMethod(cmn::classNode& n, cmn::methodNode& m, std::ostream& source)
{
   generateMethodSignature(m,source);
   source << std::endl;
   visitChildren(m);
}

void codeGen::generateMethodSignature(cmn::methodNode& m, std::ostream& s)
{
   s
      << std::endl
      << "func " << cmn::fullyQualifiedName::build(m,m.baseImpl.ref) << "("
   ;

   bool firstParam = true;
   if(!(m.flags & cmn::nodeFlags::kStatic))
   {
      s << "self : " << cmn::fullyQualifiedName::build(m);
      firstParam = false;
   }

   auto args = m.getChildrenOf<cmn::argNode>();
   for(auto jit=args.begin();jit!=args.end();++jit)
   {
      if(!firstParam)
         s << "," << std::endl << "   ";

      s
         << (*jit)->name << " : ";
      ;
      liamTypeWriter tyW(s);
      (*jit)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

      firstParam = false;
   }

   s
      << ") : void" // TODO - actually print the right return value
   ;
}

} // namespace araceli

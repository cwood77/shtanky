#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "codegen.hpp"
#include <stdexcept>

namespace araceli {

void fileRefs::flush(const std::string& path, std::ostream& stream)
{
   //return;
   for(auto it=m_paths.begin();it!=m_paths.end();++it)
   {
      // compute rel path from 'path' to 'it'

      if(*it == path)
         continue; // don't depend on yourself... duh.

      auto resolvedPath = cmn::pathUtil::computeRefPath(path,*it);

      stream << std::endl;
      stream << "ref \"" << resolvedPath << "\";" << std::endl;
   }

   m_paths.clear();
}

void fileRefCollector::onLink(cmn::linkBase& l)
{
   if(m_pRefs == NULL)
      return;

   //return;
   cmn::node *p = l._getRefee();
   // self isn't linked in some codegen
   //if(!p)
   //   throw std::runtime_error("unlinked ref during collect");
   if(!p)
      return;

   auto& f = p->getAncestor<cmn::fileNode>();
   if(f.fullPath.empty())
      throw std::runtime_error("missing file path during collect");

   auto adjF = cmn::pathUtil::addExtension(f.fullPath,cmn::pathUtil::kExtLiamHeader);
   m_pRefs->addRef(adjF);

   cdwDEBUG("adding ref to '%s' b/c of link w/ ref '%s'\n",adjF.c_str(),l.ref.c_str());
}

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
   m_o << cmn::fullyQualifiedName::build(*n.pDef.getRefee());
   m_refs.onLink(n.pDef);
   hNodeVisitor::visit(n);
}

void codeGen::visit(cmn::fileNode& n)
{
   cmn::fileNode *pPrev = m_pActiveFile;
   m_pActiveFile = &n;

   hNodeVisitor::visit(n);

   {
      auto& header = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader).stream();
      m_hRefs.flush(
         cmn::pathUtil::addExtension(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader),
         header);
   }
   {
      auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
      m_sRefs.addRef(
         cmn::pathUtil::addExtension(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader));
      m_sRefs.flush(
         cmn::pathUtil::addExtension(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource),
         source);
   }
   m_refColl.bind(NULL);
   m_pActiveFile = pPrev;
}

void codeGen::visit(cmn::classNode& n)
{
   auto& header = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader).stream();
   m_refColl.bind(&m_hRefs);

   std::string vname;
   generateClassVTable(n,header,vname);
   generateClassType(n,header,vname);
   generateClassPrototypes(n,header);

   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   m_refColl.bind(&m_sRefs);

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
      if(((*it)->flags & cmn::nodeFlags::kAbstract) == 0)
         generateClassMethod(n,**it,source);
}

void codeGen::visit(cmn::sequenceNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   source << "{" << std::endl;

   visitChildren(n);

   source << "}" << std::endl;
}

void codeGen::visit(cmn::invokeNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   if(n.getChildren().size() < 1)
      throw std::runtime_error("eh?  bad invokeNode");

   n.getChildren()[0]->acceptVisitor(*this);
   source << "->" << n.proto.ref;

   generateCallFromOpenParen(n,true);
}

void codeGen::visit(cmn::invokeFuncPtrNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   if(n.getChildren().size() < 1)
      throw std::runtime_error("eh?  bad invokeNode");

   n.getChildren()[0]->acceptVisitor(*this);
   source << "->";

   generateCallFromOpenParen(n,true);
}

void codeGen::visit(cmn::fieldAccessNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   visitChildren(n);
   source << ":" << n.name;
}

void codeGen::visit(cmn::callNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   source << n.name;

   generateCallFromOpenParen(n,false);
}

void codeGen::visit(cmn::varRefNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << n.pDef.ref;
   m_refColl.onLink(n.pDef);
   visitChildren(n);
}

void codeGen::visit(cmn::stringLiteralNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << "\"" << n.value << "\"";
   visitChildren(n);
}

void codeGen::visit(cmn::boolLiteralNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << n.value;
   visitChildren(n);
}

void codeGen::visit(cmn::intLiteralNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
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
      header << "   _vtbl : " << vname << ";" << std::endl;

   for(auto it=totalFields.begin();it!=totalFields.end();++it)
   {
      header << "   " << (*it)->name << " : ";

      liamTypeWriter tyW(header,m_refColl);
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
      liamTypeWriter tyW(s,m_refColl);
      (*jit)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

      firstParam = false;
   }

   s
      << ") : void" // TODO - actually print the right return value
   ;
}

void codeGen::generateCallFromOpenParen(cmn::node& n, bool skipFirst)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << "(";
   bool first = true;
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      if(skipFirst && it==n.getChildren().begin())
         continue;
      if(!first)
         source << ",";
      (*it)->acceptVisitor(*this);
      first = false;
   }
   source << ");" << std::endl;
}

} // namespace araceli

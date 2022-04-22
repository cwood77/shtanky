#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "codegen.hpp"
#include <stdexcept>

namespace araceli {

void fileRefs::flush(std::ostream& stream)
{
   for(auto it=m_paths.begin();it!=m_paths.end();++it)
   {
      // compute rel path from 'destPath' to 'it'

      if(*it == destPath)
         continue; // don't depend on yourself... duh.

      auto resolvedPath = cmn::pathUtil::computeRefPath(destPath,*it);

      stream << "ref \"" << resolvedPath << "\";" << std::endl;
   }

   m_paths.clear();
}

void fileRefCollector::onLink(cmn::linkBase& l)
{
   if(m_pRefs == NULL)
      return;

   cmn::node *p = l._getRefee();
   // self isn't linked in some codegen
   //if(!p)
   //   throw std::runtime_error("unlinked ref during collect");
   if(!p)
      return;

   auto& f = p->getAncestor<cmn::fileNode>();
   if(f.fullPath.empty())
      throw std::runtime_error("missing file path during collect");

   auto adjF = cmn::pathUtil::addExt(f.fullPath,cmn::pathUtil::kExtLiamHeader);
   m_pRefs->addRef(adjF);

   cdwDEBUG("adding ref from %s to '%s' b/c of link w/ ref '%s' to node '%s'\n",
      m_pRefs->destPath.c_str(),
      adjF.c_str(),
      l.ref.c_str(),
      typeid(*l._getRefee()).name());
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
   m_hRefs.destPath
      = cmn::pathUtil::addExt(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader);
   m_sRefs.destPath
      = cmn::pathUtil::addExt(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource);

   hNodeVisitor::visit(n);

   {
      auto& header = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader).stream();
      m_hRefs.flush(header);
   }
   {
      auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

      // TODO should this be stuffed into the target?
      // emit a prototype for .osCall into every ls file
      source << "func ._osCall(code : str, payload : str) : void;" << std::endl;
      source << std::endl;

      m_sRefs.addRef(m_hRefs.destPath); // always include your own header :)
      m_sRefs.flush(source);
   }

   m_refColl.bind(NULL);
   m_pActiveFile = pPrev;
}

void codeGen::visit(cmn::classNode& n)
{
   auto& header = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamHeader);
   m_refColl.bind(&m_hRefs);

   std::string vname;
   generateClassVTable(n,header,vname);
   generateClassType(n,header,vname);
   generateClassPrototypes(n,header);

   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource);
   m_refColl.bind(&m_sRefs);

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
      if(((*it)->flags & cmn::nodeFlags::kAbstract) == 0)
         generateClassMethod(n,**it,source);
}

void codeGen::visit(cmn::constNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   source << "const " << n.name << " : ";
   liamTypeWriter tyW(source,m_refColl);
   n.getChildren()[0]->acceptVisitor(tyW);
   source << " = ";
   n.getChildren()[1]->acceptVisitor(*this);
   source << ";" << std::endl;
}

void codeGen::visit(cmn::sequenceNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource);

   source.stream() << cmn::indent(source) << "{" << std::endl;

   {
      cmn::autoIndent _i(source);

      for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      {
         source.stream() << cmn::indent(source);
         (*it)->acceptVisitor(*this);
         source.stream() << ";" << std::endl;
      }
   }

   source.stream() << cmn::indent(source) << "}" << std::endl;
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

void codeGen::visit(cmn::localDeclNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource);

   source.stream() << "var " << n.name << " : ";
   liamTypeWriter tyW(source.stream(),m_refColl);
   n.demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

   if(n.getChildren().size() > 1)
   {
      source.stream() << " = ";
      n.getChildren()[1]->acceptVisitor(*this);
   }
}

void codeGen::visit(cmn::varRefNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();
   source << n.pDef.ref;
   m_refColl.onLink(n.pDef);
   visitChildren(n);
}

void codeGen::visit(cmn::assignmentNode& n)
{
   auto& source = m_out.get<cmn::outStream>(m_pActiveFile->fullPath,cmn::pathUtil::kExtLiamSource).stream();

   n.getChildren()[0]->acceptVisitor(*this);

   source << " = ";

   n.getChildren()[1]->acceptVisitor(*this);
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
   source << n.lexeme;
   visitChildren(n);
}

void codeGen::generateClassVTable(cmn::classNode& n, cmn::outStream& header, std::string& vname)
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
   header.stream()
      << "class " << vname << " {" << std::endl
   ;

   {
      cmn::autoIndent _i(header);
      for(auto it=names.begin();it!=names.end();++it)
         header.stream() << cmn::indent(header)  << *it << " : ptr;" << std::endl;
   }

   header.stream()
      << "}" << std::endl
      << std::endl
   ;
}

void codeGen::generateClassType(cmn::classNode& n, cmn::outStream& header, const std::string& vname)
{
   // gather fields from self _and_ all parents (no inheritance in liam)
   // however, parents _also_ are generated.  This is in case they have methods
   // which would need a self parameter
   std::vector<cmn::fieldNode*> totalFields;
   auto lineage = n.computeLineage();
   for(auto it=lineage.begin();it!=lineage.end();++it)
      (*it)->getChildrenOf(totalFields);

   header.stream()
      << "class " << cmn::fullyQualifiedName::build(n) << " {" << std::endl
   ;

   {
      cmn::autoIndent _i(header);

      if(!vname.empty())
         header.stream() << cmn::indent(header) << "_vtbl : " << vname << ";" << std::endl;

      for(auto it=totalFields.begin();it!=totalFields.end();++it)
      {
         header.stream() << cmn::indent(header) << (*it)->name << " : ";

         liamTypeWriter tyW(header.stream(),m_refColl);
         (*it)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

         header.stream() << ";" << std::endl;
      }
   }

   header.stream()
      << "}" << std::endl
   ;
}

void codeGen::generateClassPrototypes(cmn::classNode& n, cmn::outStream& header)
{
   header.stream()
      << std::endl
      << cmn::indent(header) << "func " << cmn::fullyQualifiedName::build(n,"ctor") << "() : "
         << cmn::fullyQualifiedName::build(n) << ";" << std::endl
      << std::endl
   ;

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
   {
      if(!((*it)->flags & cmn::nodeFlags::kAbstract))
      {
         generateMethodSignature(**it,header);
         header.stream() << ";" << std::endl << std::endl;
      }
   }
}

void codeGen::generateClassMethod(cmn::classNode& n, cmn::methodNode& m, cmn::outStream& source)
{
   generateMethodSignature(m,source);
   source.stream() << std::endl;
   visitChildren(m);
   source.stream() << std::endl;
}

void codeGen::generateMethodSignature(cmn::methodNode& m, cmn::outStream& s)
{
   if(m.attributes.size())
      for(auto it=m.attributes.begin();it!=m.attributes.end();++it)
         s.stream() << cmn::indent(s) << "[" << *it << "]" << std::endl;

   s.stream()
      << cmn::indent(s) << "func " << cmn::fullyQualifiedName::build(m,m.baseImpl.ref) << "("
      << std::endl
   ;

   cmn::autoIndent _i(s);

   bool firstParam = true;
   if(!(m.flags & cmn::nodeFlags::kStatic))
   {
      s.stream() << cmn::indent(s) << "self : " << cmn::fullyQualifiedName::build(m);
      firstParam = false;
   }

   auto args = m.getChildrenOf<cmn::argNode>();
   for(auto jit=args.begin();jit!=args.end();++jit)
   {
      if(!firstParam)
         s.stream() << "," << std::endl;

      s.stream()
         << cmn::indent(s) << (*jit)->name << " : ";
      ;
      liamTypeWriter tyW(s.stream(),m_refColl);
      (*jit)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

      firstParam = false;
   }

   s.stream()
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
   source << ")";
}

} // namespace araceli

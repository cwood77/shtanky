#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "codegen2.hpp"
#include "iTarget.hpp"
#include <stdexcept>

namespace araceli2 {

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
   if(!p)
   {
      cdwDEBUG(
         "ignoring unlinked link with ref '%s' of type '%s' for fileRef computation\n",
         l.ref.c_str(),
         typeid(l).name());
      return;
   }

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

void liamTypeWriter::visit(cmn::ptrTypeNode& n)
{
   m_o << "ptr";
   hNodeVisitor::visit(n);
}

void codeGenBase::visit(cmn::fileNode& n)
{
   hNodeVisitor::visit(n);
   appendFileSuffix();
   m_refs.flush(m_pOut->stream());
}

codeGenBase::codeGenBase(cmn::outBundle& out, const std::string& path) : m_pOut(NULL)
{
   m_pOut = &out.get<cmn::outStream>(path);
   m_refs.destPath = path;
   m_refColl.bind(&m_refs);
}

void codeGenBase::generatePrototype(cmn::funcNode& m)
{
   m_pOut->stream() << cmn::indent(*m_pOut);

   if(m.attributes.size())
      for(auto it=m.attributes.begin();it!=m.attributes.end();++it)
         m_pOut->stream() << cmn::indent(*m_pOut) << "[" << *it << "]" << std::endl;

   cmn::autoIndent _i(*m_pOut);
   m_pOut->stream()
      << "func " << m.name << "("
      << std::endl << cmn::indent(*m_pOut)
   ;

   bool firstParam = true;
   auto args = m.getChildrenOf<cmn::argNode>();
   for(auto jit=args.begin();jit!=args.end();++jit)
   {
      if(!firstParam)
         m_pOut->stream() << "," << std::endl << cmn::indent(*m_pOut);

      m_pOut->stream()
         << (*jit)->name << " : ";
      ;
      liamTypeWriter tyW(m_pOut->stream(),m_refColl);
      (*jit)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

      firstParam = false;
   }

   m_pOut->stream() << ") : ";
   liamTypeWriter tyW(m_pOut->stream(),m_refColl);
   m.demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);
}

void headerCodeGen::visit(cmn::classNode& n)
{
   // gather fields from self _and_ all parents (no inheritance in liam)
   // however, parents _also_ are generated.  This is in case they have methods
   // which would need a self parameter
   std::vector<cmn::fieldNode*> totalFields;
#if 0
   auto lineage = n.computeLineage();
   for(auto it=lineage.begin();it!=lineage.end();++it)
      (*it)->getChildrenOf(totalFields);
#endif
   n.getChildrenOf(totalFields);

   m_pOut->stream()
      << cmn::indent(*m_pOut) << "class " << cmn::fullyQualifiedName::build(n)
      << " {" << std::endl
   ;

   {
      cmn::autoIndent _i(*m_pOut);

      // hmm....
      // TODO if I move this into a transform, then I must really move field collapsing above too.
//      m_pOut->stream() << cmn::indent(*m_pOut) << "_vtbl : " << (cmn::fullyQualifiedName::build(n) + "_vtbl") << ";" << std::endl;

      for(auto it=totalFields.begin();it!=totalFields.end();++it)
      {
         m_pOut->stream() << cmn::indent(*m_pOut) << (*it)->name << " : ";

         liamTypeWriter tyW(m_pOut->stream(),m_refColl);
         (*it)->demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

         m_pOut->stream() << ";" << std::endl;
      }
   }

   m_pOut->stream()
      << cmn::indent(*m_pOut) << "}" << std::endl
      << std::endl
   ;
}

void headerCodeGen::visit(cmn::funcNode& n)
{
   generatePrototype(n);
   m_pOut->stream() << ";" << std::endl;
   m_pOut->stream() << std::endl;
}

void sourceCodeGen::visit(cmn::constNode& n)
{
   m_pOut->stream() << cmn::indent(*m_pOut) << "const " << n.name << " : ";
   liamTypeWriter tyW(m_pOut->stream(),m_refColl);
   n.getChildren()[0]->acceptVisitor(tyW);
   m_pOut->stream() << " = ";
   n.getChildren()[1]->acceptVisitor(*this);
   m_pOut->stream() << ";" << std::endl;
   m_pOut->stream() << std::endl;
}

void sourceCodeGen::visit(cmn::funcNode& n)
{
   generatePrototype(n);
   m_pOut->stream() << std::endl;
   hNodeVisitor::visit(n);
}

void sourceCodeGen::visit(cmn::sequenceNode& n)
{
   m_pOut->stream() << cmn::indent(*m_pOut) << "{" << std::endl;
   {
      cmn::autoIndent _i(*m_pOut);
      for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      {
         m_pOut->stream() << cmn::indent(*m_pOut);
         (*it)->acceptVisitor(*this);
         m_pOut->stream() << ";" << std::endl;
      }
   }
   m_pOut->stream() << cmn::indent(*m_pOut) << "}" << std::endl;
   m_pOut->stream() << std::endl;
}

void sourceCodeGen::visit(cmn::invokeFuncPtrNode& n)
{
   n.getChildren()[0]->acceptVisitor(*this);
   m_pOut->stream() << "->";

   generateCallFromOpenParen(n,true);
}

void sourceCodeGen::visit(cmn::fieldAccessNode& n)
{
   hNodeVisitor::visit(n);
   m_pOut->stream() << ":" << n.name;
}

void sourceCodeGen::visit(cmn::callNode& n)
{
   m_pOut->stream() << n.name;

   generateCallFromOpenParen(n,false);
}

void sourceCodeGen::visit(cmn::localDeclNode& n)
{
   m_pOut->stream() << "var " << n.name << " : ";
   liamTypeWriter tyW(m_pOut->stream(),m_refColl);
   n.demandSoleChild<cmn::typeNode>().acceptVisitor(tyW);

   if(n.getChildren().size() > 1)
   {
      m_pOut->stream() << " = ";
      n.getChildren()[1]->acceptVisitor(*this);
   }
}

void sourceCodeGen::visit(cmn::varRefNode& n)
{
   m_pOut->stream() << n.pSrc.ref;
   m_refColl.onLink(n.pSrc);
   hNodeVisitor::visit(n);
}

void sourceCodeGen::visit(cmn::assignmentNode& n)
{
   n.getChildren()[0]->acceptVisitor(*this);
   m_pOut->stream() << " = ";
   n.getChildren()[1]->acceptVisitor(*this);
}

void sourceCodeGen::visit(cmn::stringLiteralNode& n)
{
   m_pOut->stream() << "\"" << n.value << "\"";
   hNodeVisitor::visit(n);
}

void sourceCodeGen::visit(cmn::boolLiteralNode& n)
{
   m_pOut->stream() << n.value;
   hNodeVisitor::visit(n);
}

void sourceCodeGen::visit(cmn::intLiteralNode& n)
{
   m_pOut->stream() << n.lexeme;
   hNodeVisitor::visit(n);
}

void sourceCodeGen::visit(cmn::structLiteralNode& n)
{
   m_pOut->stream() << "{ ";
   generateCommaDelimitedChildren(n);
   m_pOut->stream() << " }";
}

void sourceCodeGen::appendFileSuffix()
{
   m_tgt.liamCodegen(*m_pOut);
   m_pOut->stream() << std::endl;
   m_refs.addRef(m_headerPath);
}

void sourceCodeGen::generateCallFromOpenParen(cmn::node& n, bool skipFirst)
{
   m_pOut->stream() << "(";
   generateCommaDelimitedChildren(n,skipFirst);
   m_pOut->stream() << ")";
}

void sourceCodeGen::generateCommaDelimitedChildren(cmn::node& n, bool skipFirst)
{
   bool first = true;
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      if(skipFirst && it==n.getChildren().begin())
         continue;
      if(!first)
         m_pOut->stream() << ",";
      (*it)->acceptVisitor(*this);
      first = false;
   }
}

void codeGen::visit(cmn::fileNode& n)
{
   auto header = cmn::pathUtil::addExt(n.fullPath,cmn::pathUtil::kExtLiamHeader);
   auto source = cmn::pathUtil::addExt(n.fullPath,cmn::pathUtil::kExtLiamSource);

   { headerCodeGen v(m_out,header); n.acceptVisitor(v); }
   { sourceCodeGen v(m_out,header,source,m_tgt); n.acceptVisitor(v); }
}

} // namespace araceli

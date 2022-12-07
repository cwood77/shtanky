#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "codegen.hpp"
#include "iTarget.hpp"
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
      if(resolvedPath.c_str()[0] != '.')
         resolvedPath = std::string(".\\") + resolvedPath;

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
      cdwTHROW("dangling links illegal in codegen");
   }
   if(dynamic_cast<cmn::intrinsicNode*>(p))
   {
      cdwDEBUG(
         "ignoring intrinsic '%s' for fileRef computation\n",
         l.ref.c_str());
      return;
   }

   auto& f = p->getAncestor<cmn::fileNode>();
   if(f.fullPath.empty())
      throw std::runtime_error("missing file path during collect");

   auto adjF = cmn::pathUtil::replaceOrAddExt(f.fullPath,"ph",cmn::pathUtil::kExtLiamHeader);
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

void liamTypeWriter::visit(cmn::boolTypeNode& n)
{
   m_o << "bool";
   hNodeVisitor::visit(n);
}

void liamTypeWriter::visit(cmn::intTypeNode& n)
{
   m_o << "int";
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
   writeAttributes(m);

   m_pOut->stream() << cmn::indent(*m_pOut);

   cmn::autoIndent _i(*m_pOut);
   m_pOut->stream()
      << "func " << cmn::fullyQualifiedName::build(m,m.name) << "("
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

void codeGenBase::writeAttributes(cmn::node& n)
{
   if(n.attributes.size())
      for(auto it=n.attributes.begin();it!=n.attributes.end();++it)
         m_pOut->stream() << cmn::indent(*m_pOut) << "[" << *it << "]" << std::endl;
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
   writeAttributes(n);
   m_pOut->stream() << cmn::indent(*m_pOut) << "const " << cmn::fullyQualifiedName::build(n,n.name) << " : ";
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
         bool isSeq = (dynamic_cast<cmn::sequenceNode*>(*it));
         if(!isSeq)
            m_pOut->stream() << cmn::indent(*m_pOut);
         (*it)->acceptVisitor(*this);
         cmn::statementNeedsSemiColon sc;
         (*it)->acceptVisitor(sc);
         if(sc.needs())
            m_pOut->stream() << ";" << std::endl;
      }
   }
   m_pOut->stream() << cmn::indent(*m_pOut) << "}" << std::endl;
   m_pOut->stream() << std::endl;
}

void sourceCodeGen::visit(cmn::returnNode& n)
{
   m_pOut->stream() << "return ";
   hNodeVisitor::visit(n);
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
   m_pOut->stream() << cmn::fullyQualifiedName::build(
      n.pTarget.getRefee()->getNode(),
      n.pTarget.getRefee()->getShortName());
   m_refColl.onLink(n.pTarget);
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

void sourceCodeGen::visit(cmn::bopNode& n)
{
   n.getChildren()[0]->acceptVisitor(*this);
   m_pOut->stream() << " " << n.op << " ";
   n.getChildren()[1]->acceptVisitor(*this);
}

void sourceCodeGen::visit(cmn::ifNode& n)
{
   m_pOut->stream() << "if(";
   n.getChildren()[0]->acceptVisitor(*this);
   m_pOut->stream() << ")" << std::endl;

   n.getChildren()[1]->acceptVisitor(*this);

   if(n.getChildren().size() > 2)
   {
      m_pOut->stream() << std::endl << cmn::indent(*m_pOut) << "else";
      const bool isNested = dynamic_cast<cmn::ifNode*>(n.getChildren()[2]);
      if(isNested)
         m_pOut->stream() << " ";
      else
         m_pOut->stream() << std::endl;
      n.getChildren()[2]->acceptVisitor(*this);
   }
}

void sourceCodeGen::visit(cmn::forLoopNode& n)
{
   if(n.getChildren().size() != 3)
      cdwTHROW("forLoopNode children is %d != 3",n.getChildren().size());

   m_pOut->stream() << "for[" << n.name << "](";
   n.getChildren()[0]->acceptVisitor(*this);
   m_pOut->stream() << ",";
   n.getChildren()[1]->acceptVisitor(*this);
   m_pOut->stream() << ")" << std::endl;
   n.getChildren()[2]->acceptVisitor(*this);
}

void sourceCodeGen::visit(cmn::whileLoopNode& n)
{
   if(n.getChildren().size() != 2)
      cdwTHROW("whileLoopNode children is %d != 2",n.getChildren().size());

   m_pOut->stream() << "while[" << n.name << "](";
   n.getChildren()[0]->acceptVisitor(*this);
   m_pOut->stream() << ")" << std::endl;
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
   auto header = cmn::pathUtil::replaceOrAddExt(n.fullPath,"ph",cmn::pathUtil::kExtLiamHeader);
   auto source = cmn::pathUtil::replaceOrAddExt(n.fullPath,"ph",cmn::pathUtil::kExtLiamSource);

   { headerCodeGen v(m_out,header); n.acceptVisitor(v); }
   { sourceCodeGen v(m_out,header,source,m_tgt); n.acceptVisitor(v); }
}

} // namespace araceli

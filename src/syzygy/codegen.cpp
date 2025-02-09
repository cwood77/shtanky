#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "codegen.hpp"

namespace syzygy {

class writeAttributes {
public:
   explicit writeAttributes(cmn::node& n, cmn::outStream& s) : m_n(n), m_s(s) {}

   void write(std::ostream& s) const
   {
      if(m_n.attributes.size() == 0) return;
      for(auto it=m_n.attributes.begin();it!=m_n.attributes.end();++it)
         s << "[" << *it << "] " << std::endl << cmn::indent(m_s);
   }

private:
   cmn::node& m_n;
   cmn::outStream& m_s;
};

std::ostream& operator<<(std::ostream& s, const writeAttributes& w)
{ w.write(s); return s; }

class writeFlags {
public:
   explicit writeFlags(cmn::node& n) : m_n(n) {}

   void write(std::ostream& s) const
   {
      if(m_n.flags == 0) return;

      if(m_n.flags & cmn::nodeFlags::kPublic)
         s << "public ";
      if(m_n.flags & cmn::nodeFlags::kProtected)
         s << "protected ";
      if(m_n.flags & cmn::nodeFlags::kPrivate)
         s << "private ";

      if(m_n.flags & cmn::nodeFlags::kVirtual)
         s << "virtual ";
      if(m_n.flags & cmn::nodeFlags::kOverride)
         s << "override ";
      if(m_n.flags & cmn::nodeFlags::kAbstract)
         s << "abstract ";

      if(m_n.flags & cmn::nodeFlags::kStatic)
         s << "static ";
   }

private:
   cmn::node& m_n;
};

std::ostream& operator<<(std::ostream& s, const writeFlags& w)
{ w.write(s); return s; }

void codegen::visit(cmn::fileNode& n)
{
   m_pCurrFile = &n;
   m_pCurrStream = NULL;
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::classNode& n)
{
   auto& s = getOutStream();

   s.stream() << writeAttributes(n,s) << getNameForClassFlags(n) << " " << n.name;
   for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
   {
      if(it == n.baseClasses.begin())
         s.stream() << " : ";
      else
         s.stream() << ", ";
      s.stream() << it->ref;
   }
   s.stream() << " {" << std::endl;
   s.stream() << std::endl;

   {
      cmn::autoIndent _i(s);
      hNodeVisitor::visit(n);
   }

   s.stream() << std::endl;
   s.stream() << "}" << std::endl;
   s.stream() << std::endl;
}

void codegen::visit(cmn::methodNode& n)
{
   visitFunctionLikeThing(n,n.name);
}

void codegen::visit(cmn::fieldNode& n)
{
   auto& s = getOutStream();

   s.stream() << cmn::indent(s) << writeAttributes(n,s);
   visitNameTypePair(n,n.name);

   if(n.getChildren().size() > 1)
   {
      s.stream() << " = ";
      n.getChildren()[1]->acceptVisitor(*this);
   }

   s.stream() << ";" << std::endl;
}

void codegen::visit(cmn::constNode& n)
{
   auto& s = getOutStream();

   s.stream() << "const " << n.name << " : ";
   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << " = ";
   n.getChildren()[1]->acceptVisitor(*this);
   s.stream() << ";";
}

void codegen::visit(cmn::funcNode& n)
{
   visitFunctionLikeThing(n,n.name);
}

void codegen::visit(cmn::argNode& n)
{
   visitNameTypePair(n,n.name);
}

void codegen::visit(cmn::strTypeNode& n)
{
   getOutStream().stream() << "str";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::boolTypeNode& n)
{
   getOutStream().stream() << "bool";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::intTypeNode& n)
{
   getOutStream().stream() << "int";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::arrayTypeNode& n)
{
   getOutStream().stream() << "[]";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::voidTypeNode& n)
{
   getOutStream().stream() << "void";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::userTypeNode& n)
{
   getOutStream().stream() << n.pDef.ref;
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::ptrTypeNode& n)
{
   getOutStream().stream() << "ptr";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::sequenceNode& n)
{
   auto& s = getOutStream();

   s.stream() << cmn::indent(s) << "{" << std::endl;
   {
      cmn::autoIndent _i(s);
      for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      {
         bool isSeq = (dynamic_cast<cmn::sequenceNode*>(*it));
         if(!isSeq)
            s.stream() << cmn::indent(s);
         (*it)->acceptVisitor(*this);
         cmn::statementNeedsSemiColon sc;
         (*it)->acceptVisitor(sc);
         if(sc.needs())
            s.stream() << ";" << std::endl;
      }
   }
   s.stream() << cmn::indent(s) << "}" << std::endl;
}

void codegen::visit(cmn::returnNode& n)
{
   auto& s = getOutStream();

   s.stream() << cmn::indent(s) << "return ";
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::invokeNode& n)
{
   auto& s = getOutStream();

   if(n.getChildren().size() < 1)
      cdwTHROW("insanity");

   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << "->" << n.proto.ref;
   visitCallLikeThingAtParens(n,1);
}

void codegen::visit(cmn::invokeFuncPtrNode& n)
{
   auto& s = getOutStream();

   if(n.getChildren().size() < 1)
      cdwTHROW("insanity");

   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << "->";
   visitCallLikeThingAtParens(n,1);
}

void codegen::visit(cmn::fieldAccessNode& n)
{
   auto& s = getOutStream();

   hNodeVisitor::visit(n);
   s.stream() << ":" << n.name;
}

void codegen::visit(cmn::callNode& n)
{
   auto& s = getOutStream();

   s.stream() << n.pTarget.ref;
   visitCallLikeThingAtParens(n,0);
}

void codegen::visit(cmn::localDeclNode& n)
{
   auto& s = getOutStream();

   s.stream() << "var " << n.name << " : ";
   n.getChildren()[0]->acceptVisitor(*this);
   if(n.getChildren().size() > 1)
   {
      if(n.getChildren().size() != 2)
         cdwTHROW("insanity");

      s.stream() << " = ";
      n.getChildren()[1]->acceptVisitor(*this);
   }
}

void codegen::visit(cmn::varRefNode& n)
{
   auto& s = getOutStream();
   s.stream() << n.pSrc.ref;
}

void codegen::visit(cmn::assignmentNode& n)
{
   auto& s = getOutStream();

   if(n.getChildren().size() != 2)
      cdwTHROW("insanity");

   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << " = ";
   n.getChildren()[1]->acceptVisitor(*this);
}

void codegen::visit(cmn::bopNode& n)
{
   auto& s = getOutStream();

   if(n.getChildren().size() != 2)
      cdwTHROW("insanity");

   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << " " << n.op << " ";
   n.getChildren()[1]->acceptVisitor(*this);
}

void codegen::visit(cmn::indexNode& n)
{
   auto& s = getOutStream();

   if(n.getChildren().size() != 2)
      cdwTHROW("insanity");

   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << "[";
   n.getChildren()[1]->acceptVisitor(*this);
   s.stream() << "]";
}

void codegen::visit(cmn::ifNode& n)
{
   auto& s = getOutStream();

   s.stream() << "if(";
   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << ")" << std::endl;

   n.getChildren()[1]->acceptVisitor(*this);

   if(n.getChildren().size() > 2)
   {
      s.stream() << cmn::indent(s) << "else";
      const bool isNested = dynamic_cast<cmn::ifNode*>(n.getChildren()[2]);
      if(isNested)
         s.stream() << " ";
      else
         s.stream() << std::endl;
      n.getChildren()[2]->acceptVisitor(*this);
   }
}


#if 0
void codegen::visit(cmn::loopIntrinsicNode& n)
{
   auto& s = getOutStream();
   s.stream() << "loop(" << n.name << ")";
}
#endif

void codegen::visit(cmn::forLoopNode& n)
{
   if(n.getChildren().size() != 3)
      cdwTHROW("forLoopNode children is %d != 3",n.getChildren().size());

   auto& s = getOutStream();
   s.stream() << "for[" << n.name << "](";
   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << ",";
   n.getChildren()[1]->acceptVisitor(*this);
   s.stream() << ")" << std::endl;
   n.getChildren()[2]->acceptVisitor(*this);
}

void codegen::visit(cmn::whileLoopNode& n)
{
   if(n.getChildren().size() != 2)
      cdwTHROW("whileLoopNode children is %d != 2",n.getChildren().size());

   auto& s = getOutStream();
   s.stream() << "while[" << n.name << "](";
   n.getChildren()[0]->acceptVisitor(*this);
   s.stream() << ")" << std::endl;
   n.getChildren()[1]->acceptVisitor(*this);
}

void codegen::visit(cmn::stringLiteralNode& n)
{
   auto& s = getOutStream();
   s.stream() << "\"" << n.value << "\"";
}

void codegen::visit(cmn::boolLiteralNode& n)
{
   auto& s = getOutStream();
   s.stream() << (n.value ? "true" : "false" );
}

void codegen::visit(cmn::intLiteralNode& n)
{
   auto& s = getOutStream();
   s.stream() << n.lexeme;
}

void codegen::visit(cmn::structLiteralNode& n)
{
   auto& s = getOutStream();
   s.stream() << "{ ";
   hNodeVisitor::visit(n);
   s.stream() << " }";
}

void codegen::visit(cmn::genericNode& n)
{
   auto& s = getOutStream();
   s.stream() << "generic<";

   auto cons = n.getChildrenOf<cmn::constraintNode>();
   for(auto it = cons.begin();it!=cons.end();++it)
   {
      if(it!=cons.begin())
         s.stream() << ",";
      s.stream() << (*it)->name;
   }

   s.stream() << ">" << std::endl;
   hNodeVisitor::visit(n);
}

void codegen::visit(cmn::instantiateNode& n)
{
   auto& s = getOutStream();
   s.stream() << "instantiate " << n.text << ";" << std::endl;
   s.stream() << std::endl;
   hNodeVisitor::visit(n);
}

cmn::outStream& codegen::getOutStream()
{
   if(m_pCurrStream == NULL)
   {
      std::string outPath;
      if(m_addExt)
         outPath = cmn::pathUtil::addExt(m_pCurrFile->fullPath,m_infix);
      else
         outPath = cmn::pathUtil::replaceOrAddExt(m_pCurrFile->fullPath,
            cmn::pathUtil::getExt(m_pCurrFile->fullPath),m_infix);
      cdwDEBUG("infixed path w/ '%s' yields [%s] -> [%s]\n",
         m_infix.c_str(),
         m_pCurrFile->fullPath.c_str(),
         outPath.c_str());
      m_pCurrStream = &m_b.get<cmn::outStream>(outPath);
   }

   return *m_pCurrStream;
}

void codegen::visitFunctionLikeThing(cmn::node& n, const std::string& name)
{
   auto& s = getOutStream();

   s.stream() << cmn::indent(s) << writeAttributes(n,s) << writeFlags(n)
      << name << "(";

   auto args = n.getChildrenOf<cmn::argNode>();
   for(auto it=args.begin();it!=args.end();++it)
   {
      if(it!=args.begin())
         s.stream() << ", ";
      (*it)->acceptVisitor(*this);
   }

   s.stream() << ") : ";
   n.demandSoleChild<cmn::typeNode>().acceptVisitor(*this);

   auto body = n.getChildrenOf<cmn::sequenceNode>();
   if(body.size() == 0)
      s.stream() << ";" << std::endl;
   else
   {
      s.stream() << std::endl;
      (*body.begin())->acceptVisitor(*this);
   }
}

void codegen::visitNameTypePair(cmn::node& n, const std::string& name)
{
   auto& s = getOutStream();

   s.stream() << writeFlags(n) << name << " : ";
   n.getChildren()[0]->acceptVisitor(*this);
}

void codegen::visitCallLikeThingAtParens(cmn::node& n, size_t startIdx)
{
   auto& s = getOutStream();

   s.stream() << "(";
   for(size_t i=startIdx;i<n.getChildren().size();i++)
   {
      if(i != startIdx)
         s.stream() << ",";
      n.getChildren()[i]->acceptVisitor(*this);
   }
   s.stream() << ")";
}

std::string codegen::getNameForClassFlags(cmn::classNode& n)
{
   if(n.flags & cmn::nodeFlags::kAbstract)
      return "abstract";
   if(n.flags & cmn::nodeFlags::kInterface)
      return "interface";
   return "class";
}

} // namespace syzygy

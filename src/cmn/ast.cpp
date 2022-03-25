#include "ast.hpp"
#include <stdio.h>

namespace cmn {

void iNodeVisitor::visitChildren(node& n)
{
   auto v = n.getChildren();
   for(auto it=v.begin();it!=v.end();++it)
      (*it)->acceptVisitor(*this);
}

node::~node()
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
      delete *it;
}

void node::injectAbove(node& n)
{
   m_pParent->appendChild(n);
   n.appendChild(*this);

   auto& oldSiblings = n.m_pParent->m_children;
   for(auto it=oldSiblings.begin();it!=oldSiblings.end();++it)
   {
      if(*it == this)
      {
         oldSiblings.erase(it);
         break;
      }
   }
}

void node::appendChild(node& n)
{
   m_children.push_back(&n);
   n.m_pParent = this;
}

node *node::lastChild()
{
   if(m_children.size() == 0) return NULL;
   return *(--(m_children.end()));
}

std::list<classNode*> classNode::computeLineage()
{
   std::list<classNode*> l;
   computeLineage(l);
   return l;
}

void classNode::computeLineage(std::list<classNode*>& l)
{
   for(auto it=baseClasses.begin();it!=baseClasses.end();++it)
      it->getRefee()->computeLineage(l);
   l.push_back(this);
}

void diagVisitor::visit(araceliProjectNode& n)
{
   ::printf("%sproject; target=%s\n",
      getIndent().c_str(),
      n.targetType.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(liamProjectNode& n)
{
   ::printf("%sproject; path=%s; #search=%lld; #included=%lld\n",
      getIndent().c_str(),
      n.sourceFullPath.c_str(),
      n.searchPaths.size(),
      n.loadedPaths.size());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(scopeNode& n)
{
   ::printf("%sscope; path=%s; name=%s; inProject=%d; loaded=%d\n",
      getIndent().c_str(),
      n.path.c_str(),
      n.scopeName.c_str(),
      n.inProject ? 1 : 0,
      n.loaded ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fileNode& n)
{
   ::printf("%sfile path:%s\n",
      getIndent().c_str(),
      n.fullPath.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fileRefNode& n)
{
   ::printf("%sfileRef path:%s\n",
      getIndent().c_str(),
      n.ref.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(classNode& n)
{
   ::printf("%sclass; name=%s; #bases=%lld\n",
      getIndent().c_str(),
      n.name.c_str(),
      n.baseClasses.size());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(memberNode& n)
{
   ::printf("%smember; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(methodNode& n)
{
   ::printf("%smethod; baseLinked?=%d\n",
      getIndent().c_str(),
      n.baseImpl.getRefee() ? 1 : 0);

   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fieldNode& n)
{
   ::printf("%sfield\n",
      getIndent().c_str());

   hNodeVisitor::visit(n);
}

void diagVisitor::visit(funcNode& n)
{
   ::printf("%sfunc; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(argNode& n)
{
   ::printf("%sarg; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(strTypeNode& n)
{
   ::printf("%sstrType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(arrayTypeNode& n)
{
   ::printf("%sarrayType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(voidTypeNode& n)
{
   ::printf("%svoidType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(userTypeNode& n)
{
   ::printf("%suserType name=%s linked?=%d\n",
      getIndent().c_str(),
      n.pDef.ref.c_str(),
      n.pDef.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(ptrTypeNode& n)
{
}

void diagVisitor::visit(sequenceNode& n)
{
   ::printf("%ssequence\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(invokeNode& n)
{
   ::printf("%sinvoke; name=%s; protoLinked?=%d\n",
      getIndent().c_str(),
      n.proto.ref.c_str(),
      n.proto.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(callNode& n)
{
   ::printf("%scall; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(varRefNode& n)
{
   ::printf("%svarRef; name=%s; linked?=%d\n",
      getIndent().c_str(),
      n.pDef.ref.c_str(),
      n.pDef.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(assignmentNode& n)
{
   ::printf("%sassignment\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(bopNode& n)
{
   ::printf("%sbop %s\n",
      getIndent().c_str(),
      n.op.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(stringLiteralNode& n)
{
   ::printf("%sstrLit; value=%s\n",
      getIndent().c_str(),
      n.value.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(boolLiteralNode& n)
{
   ::printf("%sboolLit; value=%d\n",
      getIndent().c_str(),
      n.value ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(intLiteralNode& n)
{
   ::printf("%sintLit; value=%s\n",
      getIndent().c_str(),
      n.value.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

diagVisitor::autoIndent::autoIndent(diagVisitor& v)
: m_v(v)
{
   m_v.m_nIndents++;
}

diagVisitor::autoIndent::~autoIndent()
{
   m_v.m_nIndents--;
}

std::string diagVisitor::getIndent() const
{
   return std::string(m_nIndents*3,' ');
}

std::string fullyQualifiedName::build(cmn::node& n, const std::string& start)
{
   fullyQualifiedName self;
   self.m_fqn = start;
   n.acceptVisitor(self);
   return self.m_fqn;
}

void fullyQualifiedName::visit(cmn::node& n)
{
   cmn::node *pParent = n.getParent();
   if(pParent)
      pParent->acceptVisitor(*this);
   else
      makeAbsolute();
}

void fullyQualifiedName::visit(scopeNode& n)
{
   prepend(n.scopeName);
   hNodeVisitor::visit(n);
}

void fullyQualifiedName::visit(classNode& n)
{
   prepend(n.name);
   hNodeVisitor::visit(n);
}

void fullyQualifiedName::makeAbsolute()
{
   if(m_fqn.empty()) return;

   if(m_fqn.c_str()[0] != '.')
      m_fqn = std::string(".") + m_fqn;
}

void fullyQualifiedName::prepend(const std::string& n)
{
   if(n.empty()) return;

   makeAbsolute();

   m_fqn = n + m_fqn;
}

} // namespace cmn

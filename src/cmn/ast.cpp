#include "ast.hpp"
#include "trace.hpp"

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

   n.m_pParent->removeChild(*this);
}

void node::appendChild(node& n)
{
   m_children.push_back(&n);
   n.m_pParent = this;
}

void node::insertChild(size_t i, node& n)
{
   m_children.insert(m_children.begin()+i,&n);
   n.m_pParent = this;
}

node *node::replaceChild(node& old, node& nu)
{
   int i=0;
   for(auto it=m_children.begin();it!=m_children.end();++it,i++)
   {
      if(*it == &old)
      {
         m_children[i] = &nu;
         return &old;
      }
   }

   throw std::runtime_error("can't find child to replace");
}

void node::removeChild(node& n)
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &n)
      {
         m_children.erase(it);
         break;
      }
   }
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
   cdwDEBUG("%sproject; target=%s\n",
      getIndent().c_str(),
      n.targetType.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(liamProjectNode& n)
{
   cdwDEBUG("%sproject; path=%s; #included=%lld\n",
      getIndent().c_str(),
      n.sourceFullPath.c_str(),
      n.loadedPaths.size());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(scopeNode& n)
{
   cdwDEBUG("%sscope; path=%s; name=%s; inProject=%d; loaded=%d\n",
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
   cdwDEBUG("%sfile path:%s\n",
      getIndent().c_str(),
      n.fullPath.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fileRefNode& n)
{
   cdwDEBUG("%sfileRef path:%s\n",
      getIndent().c_str(),
      n.ref.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(classNode& n)
{
   cdwDEBUG("%sclass; name=%s; #bases=%lld\n",
      getIndent().c_str(),
      n.name.c_str(),
      n.baseClasses.size());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(memberNode& n)
{
   cdwDEBUG("%smember; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(methodNode& n)
{
   cdwDEBUG("%smethod; baseLinked?=%d\n",
      getIndent().c_str(),
      n.baseImpl.getRefee() ? 1 : 0);

   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fieldNode& n)
{
   cdwDEBUG("%sfield\n",
      getIndent().c_str());

   hNodeVisitor::visit(n);
}

void diagVisitor::visit(constNode& n)
{
   cdwDEBUG("%sconst; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(funcNode& n)
{
   cdwDEBUG("%sfunc; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(argNode& n)
{
   cdwDEBUG("%sarg; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(strTypeNode& n)
{
   cdwDEBUG("%sstrType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(arrayTypeNode& n)
{
   cdwDEBUG("%sarrayType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(voidTypeNode& n)
{
   cdwDEBUG("%svoidType\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(userTypeNode& n)
{
   cdwDEBUG("%suserType name=%s linked?=%d\n",
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
   cdwDEBUG("%ssequence\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(invokeNode& n)
{
   cdwDEBUG("%sinvoke; name=%s; protoLinked?=%d\n",
      getIndent().c_str(),
      n.proto.ref.c_str(),
      n.proto.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(invokeFuncPtrNode& n)
{
   cdwDEBUG("%sinvoke func ptr\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(fieldAccessNode& n)
{
   cdwDEBUG("%sfield access; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(callNode& n)
{
   cdwDEBUG("%scall; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(localDeclNode& n)
{
   cdwDEBUG("%slocalDeclNode; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(varRefNode& n)
{
   cdwDEBUG("%svarRef; name=%s; linked?=%d\n",
      getIndent().c_str(),
      n.pSrc.ref.c_str(),
      n.pSrc.getRefee() ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(assignmentNode& n)
{
   cdwDEBUG("%sassignment\n",
      getIndent().c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(bopNode& n)
{
   cdwDEBUG("%sbop %s\n",
      getIndent().c_str(),
      n.op.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(stringLiteralNode& n)
{
   cdwDEBUG("%sstrLit; value=%s\n",
      getIndent().c_str(),
      n.value.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(boolLiteralNode& n)
{
   cdwDEBUG("%sboolLit; value=%d\n",
      getIndent().c_str(),
      n.value ? 1 : 0);

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(intLiteralNode& n)
{
   cdwDEBUG("%sintLit; value=%s\n",
      getIndent().c_str(),
      n.lexeme.c_str());

   autoIndent _a(*this);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(structLiteralNode& n)
{
   cdwDEBUG("%sstructLit\n",
      getIndent().c_str());

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

void cloningNodeVisitor::visit(node& n)
{
   as<node>().lineNumber = n.lineNumber;
   as<node>().attributes = n.attributes;
   as<node>().flags = n.flags;
}

void cloningNodeVisitor::visit(memberNode& n)
{
   as<memberNode>().name = n.name;
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(methodNode& n)
{
   as<methodNode>().baseImpl.ref = n.baseImpl.ref;
   if(n.baseImpl.getRefee())
      as<methodNode>().baseImpl.bind(*n.baseImpl.getRefee());
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(fieldNode& n)
{
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(argNode& n)
{
   as<argNode>().name = n.name;
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(typeNode& n)
{
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(strTypeNode& n)
{
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(arrayTypeNode& n)
{
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(voidTypeNode& n)
{
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(userTypeNode& n)
{
   as<userTypeNode>().pDef.ref = n.pDef.ref;
   as<userTypeNode>().pDef.bind(*n.pDef.getRefee());
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(ptrTypeNode& n)
{
   hNodeVisitor::visit(n);
}

void cloningNodeVisitor::visit(varRefNode& n)
{
   hNodeVisitor::visit(n);
   as<varRefNode>().pSrc.ref = n.pSrc.ref;
   as<varRefNode>().pSrc.bind(*n.pSrc._getRefee());
}

node& cloneTree(node& n)
{
   creatingNodeVisitor creator;
   n.acceptVisitor(creator);
   { cloningNodeVisitor v(*creator.inst.get()); n.acceptVisitor(v); }

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      creator.inst->appendChild(cloneTree(**it));

   return *creator.inst.release();
}

} // namespace cmn

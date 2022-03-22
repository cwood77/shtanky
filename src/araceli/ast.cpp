#include "ast.hpp"
#include <stdio.h>

namespace araceli {

void diagVisitor::visit(projectNode& n)
{
   ::printf("%sproject; target=%s\n",getIndent().c_str(),n.targetType.c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(scopeNode& n)
{
   ::printf("%sscope; path=%s; name=%s; inProject=%d; loaded=%d\n",
      getIndent().c_str(),
      n.path.c_str(),
      n.scopeName.c_str(),
      n.inProject ? 1 : 0,
      n.loaded ? 1 : 0);
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(fileNode& n)
{
   ::printf("%sfile\n",
      getIndent().c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(classNode& n)
{
   ::printf("%sclass; name=%s; #bases=%lld\n",
      getIndent().c_str(),
      n.name.c_str(),
      n.baseClasses.size());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(memberNode& n)
{
   const char *kAccessStrings[] = { "public", "protected", "private" };

   ::printf("%smember; name=%s; access=%s\n",
      getIndent().c_str(),
      n.name.c_str(),
      kAccessStrings[n.access]);
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(methodNode& n)
{
   ::printf("%smethod; isOverride=%d; baseLinked?=%d\n",
      getIndent().c_str(),
      n.isOverride ? 1 : 0,
      n.baseImpl.pRefee ? 1 : 0);
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(argNode& n)
{
   ::printf("%sarg; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(strTypeNode& n)
{
   ::printf("%sstrType\n",
      getIndent().c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(arrayTypeNode& n)
{
   ::printf("%sarrayType\n",
      getIndent().c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(voidTypeNode& n)
{
   ::printf("%svoidType\n",
      getIndent().c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(fieldNode& n)
{
   ::printf("%sfield\n",
      getIndent().c_str());
   hNodeVisitor::visit(n);
}

void diagVisitor::visit(sequenceNode& n)
{
   ::printf("%ssequence\n",
      getIndent().c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(invokeNode& n)
{
   ::printf("%sinvoke; name=%s; protoLinked?=%d\n",
      getIndent().c_str(),
      n.name.c_str(),
      n.proto.pRefee ? 1 : 0);
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(varRefNode& n)
{
   ::printf("%svarRef; name=%s\n",
      getIndent().c_str(),
      n.name.c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(stringLiteralNode& n)
{
   ::printf("%sstrLit; value=%s\n",
      getIndent().c_str(),
      n.value.c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

void diagVisitor::visit(boolLiteralNode& n)
{
   ::printf("%sboolLit; value=%d\n",
      getIndent().c_str(),
      n.value ? 1 : 0);
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

std::string diagVisitor::getIndent() const
{
   return std::string(m_nIndents,' ');
}

} // namespace araceli

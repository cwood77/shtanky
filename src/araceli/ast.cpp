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

void diagVisitor::visit(fileNode& n)
{
   ::printf("%sfile\n",
      getIndent().c_str());
   m_nIndents+=3;
   hNodeVisitor::visit(n);
   m_nIndents-=3;
}

std::string diagVisitor::getIndent() const
{
   return std::string(m_nIndents,' ');
}

} // namespace araceli

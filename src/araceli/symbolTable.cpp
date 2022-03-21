#include "symbolTable.hpp"
#include <stdio.h>

namespace araceli {

void symbolTable::publish(const std::string& fqn, cmn::node& n)
{
   ::printf("publishing symbol %s\n",fqn.c_str());
}

void symbolTable::tryResolve(const std::string& refingScope, linkBase& l)
{
   ::printf("resolving ref %s with context %s\n",l.ref.c_str(),refingScope.c_str());
}

void fullScopeNameBuilder::visit(scopeNode& n)
{
   fqn = std::string(".") + n.scopeName + fqn;
   hNodeVisitor::visit(n);
}

void fullScopeNameBuilder::visit(classNode& n)
{
   fqn = std::string(".") + n.name + fqn;
   hNodeVisitor::visit(n);
}

} // namespace araceli

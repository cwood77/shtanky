#include "nameUtil.hpp"
#include "symbolTable.hpp"
#include <algorithm>
#include <stdio.h>
#include <string.h>

namespace araceli {

void symbolTable::publish(const std::string& fqn, cmn::node& n)
{
   ::printf("publishing symbol %s for %lld\n",fqn.c_str(),(size_t)&n);
   resolved[fqn] = &n;
}

void symbolTable::tryResolve(const std::string& refingScope, linkBase& l)
{
   ::printf("resolving ref %s with context %s\n",l.ref.c_str(),refingScope.c_str());
   unresolved.insert(&l);

   if(nameUtil::isAbsolute(l.ref))
      tryBind(l.ref,l);
   else
   {
      auto prefix = refingScope;
      while(true)
      {
         auto attempt = nameUtil::append(prefix,l.ref);
         if(tryBind(attempt,l))
            break;
         if(prefix == ".")
            break;
         prefix = nameUtil::stripLast(prefix);
      }
   }
}

bool symbolTable::tryBind(const std::string& fqn, linkBase& l)
{
   ::printf("  checking %s...",fqn.c_str());

   auto it = resolved.find(fqn);
   if(it == resolved.end())
      ::printf("nope\n");
   else
   {
      ::printf("ok!\n");
      l.pRefee = it->second;
      unresolved.erase(&l);
      return true;
   }

   return false;
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

unloadedScopeFinder::unloadedScopeFinder(const std::string& missingRef)
: m_missingRef(missingRef)
{
}

bool unloadedScopeFinder::any()
{
   return m_candidates.size();
}

scopeNode& unloadedScopeFinder::mostLikely()
{
   return *((--(m_candidates.end()))->second);
}

void unloadedScopeFinder::visit(scopeNode& n)
{
   if(!n.loaded)
   {
      fullScopeNameBuilder v;
      n.acceptVisitor(v);

      if(
         ((v.fqn.length()-1) <= m_missingRef.length()) &&
         ::strncmp(v.fqn.c_str()+1,m_missingRef.c_str(),v.fqn.length()-1)==0
        )
      {
         m_candidates[v.fqn] = &n;
      }
      else
      {
         ::printf("   scope %s not a candidate\n",n.path.c_str());
         ::printf("      %s vs %s\n",v.fqn.c_str(),m_missingRef.c_str());
      }
   }
   else
      ::printf("   scope %s already loaded\n",n.path.c_str());

   hNodeVisitor::visit(n);
}

} // namespace araceli

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

void symbolTable::tryResolveExact(const std::string& refingScope, linkBase& l)
{
   ::printf("resolving ref %s with context %s [exact]\n",l.ref.c_str(),refingScope.c_str());
   unresolved.insert(&l);

   if(nameUtil::isAbsolute(l.ref))
      tryBind(l.ref,l);
   else
   {
      auto prefix = refingScope;
      auto attempt = nameUtil::append(prefix,l.ref);
      tryBind(attempt,l);
   }
}

void symbolTable::tryResolveWithParents(const std::string& refingScope, linkBase& l)
{
   ::printf("resolving ref %s with context %s [w/ parents]\n",l.ref.c_str(),refingScope.c_str());
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
      l.bind(*it->second);
      unresolved.erase(&l);
      return true;
   }

   return false;
}

void fullScopeNameBuilder::visit(cmn::node& n)
{
   cmn::node *pParent = n.getParent();
   if(pParent)
      pParent->acceptVisitor(*this);
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

void linkResolver::visit(cmn::node& n)
{
   if(m_l._getRefee())
      return;

   cmn::node *pParent = n.getParent();
   if(pParent)
      pParent->acceptVisitor(*this);
}

void linkResolver::visit(scopeNode& n)
{
   if(m_l._getRefee())
      return;

   if(m_mode & kContainingScopes)
   {
      fullScopeNameBuilder v; // redundant
      n.acceptVisitor(v);
      tryResolve(v.fqn);
      if(m_l._getRefee())
         return;
   }

   hNodeVisitor::visit(n);
}

void linkResolver::visit(classNode& n)
{
   if(m_l._getRefee())
      return;

   fieldGatherer fields;

   if(m_mode & kOwnClass)
   {
      m_mode &= ~kOwnClass; // don't do this again

      if(m_mode & kLocalsAndFields)
         n.acceptVisitor(fields);
      else
      {
         fullScopeNameBuilder v;
         n.acceptVisitor(v);
         tryResolve(v.fqn);
         if(m_l._getRefee())
            return;
      }
   }

   if(m_mode & kBaseClasses)
   {
      for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
      {
         if(it->getRefee())
         {
            if(m_mode & kLocalsAndFields)
            {
               it->getRefee()->acceptVisitor(fields);
               it->getRefee()->acceptVisitor(*this);
            }
            else
            {
               fullScopeNameBuilder v;
               it->getRefee()->acceptVisitor(v);
               tryResolve(v.fqn);
               if(m_l._getRefee())
                  return;
            }
         }
      }
   }

   if(m_mode & kLocalsAndFields)
   {
      n.acceptVisitor(fields);
      for(auto it=fields.fields.begin();it!=fields.fields.end();++it)
      {
         if((*it)->name == m_l.ref)
         {
            typeFinder tyF;
            (*it)->acceptVisitor(tyF);
            m_l.bind(*tyF.pType);
         }
      }
   }

   hNodeVisitor::visit(n);
}

void linkResolver::tryResolve(const std::string& refingScope)
{
   if(m_mode & kContainingScopes)
      m_sTable.tryResolveWithParents(refingScope,m_l);
   else
      m_sTable.tryResolveExact(refingScope,m_l);
}

void nodePublisher::visit(classNode& n)
{
   fullScopeNameBuilder v;
   n.acceptVisitor(v);
   m_sTable.publish(v.fqn,n);

   hNodeVisitor::visit(n);
}

void nodePublisher::visit(memberNode& n)
{
   bool isField = (dynamic_cast<fieldNode*>(&n)!=NULL);
   if(isField || (n.flags & (nodeFlags::kOverride | nodeFlags::kAbstract)))
   {
      fullScopeNameBuilder v;
      v.fqn = std::string(".") + n.name;
      n.acceptVisitor(v);
      m_sTable.publish(v.fqn,n);
   }

   hNodeVisitor::visit(n);
}

void nodeResolver::visit(classNode& n)
{
   for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
   {
      linkResolver v(m_sTable,*it,linkResolver::kContainingScopes);
      n.acceptVisitor(v);
   }

   hNodeVisitor::visit(n);
}

void nodeResolver::visit(methodNode& n)
{
   if(n.flags & nodeFlags::kOverride)
   {
      linkResolver v(m_sTable,n.baseImpl,linkResolver::kBaseClasses);
      n.acceptVisitor(v);
   }

   hNodeVisitor::visit(n);
}

void nodeResolver::visit(userTypeNode& n)
{
   linkResolver v(m_sTable,n.pDef,linkResolver::kContainingScopes);
   n.acceptVisitor(v);

   hNodeVisitor::visit(n);
}

// invoke node linking is a lot more involved.... it depends on the type found in the
// instance
#if 0
void nodeResolver::visit(invokeNode& n)
{
   linkResolver v(m_sTable,n.proto,
      linkResolver::kContainingScopes | linkResolver::kOwnClass);
   n.acceptVisitor(v);

   hNodeVisitor::visit(n);
}
#endif

void nodeResolver::visit(varRefNode& n)
{
   linkResolver v(m_sTable,n.pDef,
      linkResolver::kBaseClasses | linkResolver::kLocalsAndFields);
   n.acceptVisitor(v);

   hNodeVisitor::visit(n);
}

unloadedScopeFinder::unloadedScopeFinder(const std::string& missingRef)
: m_missingRef(missingRef)
{
   // if the ref is FQ, shave off the leading .
   // so I can treat FQ and rel names identically
   if(m_missingRef.c_str()[0] == '.')
      m_missingRef = m_missingRef.c_str()+1;
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

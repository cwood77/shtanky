#include "loader.hpp"
#include "nameUtil.hpp"
#include "symbolTable.hpp"
#include <algorithm>
#include <stdio.h>
#include <string.h>

namespace araceli {

void symbolTable::publish(const std::string& fqn, cmn::node& n)
{
   ::printf("publishing symbol %s for %lld\n",fqn.c_str(),(size_t)&n);
   published[fqn] = &n;
}

void symbolTable::tryResolveVarType(const std::string& objName, cmn::node& obj, cmn::linkBase& l)
{
   if(objName == l.ref)
   {
      l.bind(obj.demandSoleChild<cmn::typeNode>());
      unresolved.erase(&l);
   }
}

void symbolTable::tryResolveExact(const std::string& refingScope, cmn::linkBase& l)
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

void symbolTable::tryResolveWithParents(const std::string& refingScope, cmn::linkBase& l)
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

bool symbolTable::tryBind(const std::string& fqn, cmn::linkBase& l)
{
   ::printf("  checking %s...",fqn.c_str());

   auto it = published.find(fqn);
   if(it == published.end())
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

void linkResolver::visit(cmn::node& n)
{
   if(m_l._getRefee())
      return;

   cmn::node *pParent = n.getParent();
   if(pParent)
      pParent->acceptVisitor(*this);
}

void linkResolver::visit(cmn::scopeNode& n)
{
   if(m_l._getRefee())
      return;

   if(m_mode & kContainingScopes)
   {
      tryResolve(cmn::fullyQualifiedName::build(n));
      if(m_l._getRefee())
         return;
   }

   cmn::hNodeVisitor::visit(n);
}

void linkResolver::visit(cmn::classNode& n)
{
   if(m_l._getRefee())
      return;

   std::vector<cmn::fieldNode*> fields;

   if(m_mode & kOwnClass)
   {
      m_mode &= ~kOwnClass; // don't do this again

      if(m_mode & kLocalsAndFields)
         n.getChildrenOf<cmn::fieldNode>(fields);
      else
      {
         tryResolve(cmn::fullyQualifiedName::build(n));
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
               it->getRefee()->getChildrenOf<cmn::fieldNode>(fields);
               it->getRefee()->acceptVisitor(*this);
            }
            else
            {
               tryResolve(cmn::fullyQualifiedName::build(*it->getRefee()));
               if(m_l._getRefee())
                  return;
            }
         }
      }
   }

   if(m_mode & kLocalsAndFields)
   {
      n.getChildrenOf<cmn::fieldNode>(fields);
      for(auto it=fields.begin();it!=fields.end();++it)
         if(m_l._getRefee() == NULL)
            m_sTable.tryResolveVarType((*it)->name,**it,m_l);
   }

   cmn::hNodeVisitor::visit(n);
}

void linkResolver::visit(cmn::methodNode& n)
{
   if(m_mode & kLocalsAndFields)
   {
      // check args
      auto args = n.getChildrenOf<cmn::argNode>();
      for(auto it=args.begin();it!=args.end();++it)
         if(m_l._getRefee() == NULL)
            m_sTable.tryResolveVarType((*it)->name,**it,m_l);
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

void nodePublisher::visit(cmn::classNode& n)
{
   m_sTable.publish(cmn::fullyQualifiedName::build(n),n);

   cmn::hNodeVisitor::visit(n);
}

void nodePublisher::visit(cmn::memberNode& n)
{
   bool isField = (dynamic_cast<cmn::fieldNode*>(&n)!=NULL);
   if(isField || (n.flags & (cmn::nodeFlags::kOverride | cmn::nodeFlags::kAbstract)))
   {
      m_sTable.publish(cmn::fullyQualifiedName::build(n,n.name),n);
   }

   cmn::hNodeVisitor::visit(n);
}

void nodeResolver::visit(cmn::classNode& n)
{
   for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
   {
      m_sTable.markRequired(*it);
      linkResolver v(m_sTable,*it,linkResolver::kContainingScopes);
      n.acceptVisitor(v);
   }

   cmn::hNodeVisitor::visit(n);
}

void nodeResolver::visit(cmn::methodNode& n)
{
   if(n.flags & cmn::nodeFlags::kOverride)
   {
      m_sTable.markRequired(n.baseImpl);
      linkResolver v(m_sTable,n.baseImpl,linkResolver::kBaseClasses);
      n.acceptVisitor(v);
   }

   cmn::hNodeVisitor::visit(n);
}

void nodeResolver::visit(cmn::userTypeNode& n)
{
   m_sTable.markRequired(n.pDef);
   linkResolver v(m_sTable,n.pDef,
      linkResolver::kOwnClass | linkResolver::kContainingScopes);
   n.acceptVisitor(v);

   cmn::hNodeVisitor::visit(n);
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

void nodeResolver::visit(cmn::varRefNode& n)
{
   m_sTable.markRequired(n.pDef);
   linkResolver v(m_sTable,n.pDef,
      linkResolver::kBaseClasses | linkResolver::kLocalsAndFields);
   n.acceptVisitor(v);

   cmn::hNodeVisitor::visit(n);
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

cmn::scopeNode& unloadedScopeFinder::mostLikely()
{
   return *((--(m_candidates.end()))->second);
}

void unloadedScopeFinder::visit(cmn::scopeNode& n)
{
   if(!n.loaded)
   {
      std::string fqn = cmn::fullyQualifiedName::build(n);

      if(
         ((fqn.length()-1) <= m_missingRef.length()) &&
         ::strncmp(fqn.c_str()+1,m_missingRef.c_str(),fqn.length()-1)==0
        )
      {
         m_candidates[fqn] = &n;
      }
      else
      {
         ::printf("   scope %s not a candidate\n",n.path.c_str());
         ::printf("      %s vs %s\n",fqn.c_str(),m_missingRef.c_str());
      }
   }
   else
      ::printf("   scope %s already loaded\n",n.path.c_str());

   cmn::hNodeVisitor::visit(n);
}

void linkGraph(cmn::node& root)
{
   ::printf("entering link/load loop ----\n");
   symbolTable sTable;
   size_t missingLastTime = 0;
   while(true)
   {
      { nodePublisher p(sTable); cmn::treeVisitor t(p); root.acceptVisitor(t); }
      { nodeResolver r(sTable); cmn::treeVisitor t(r); root.acceptVisitor(t); }
      ::printf("%lld published; %lld unresolved\n",
         sTable.published.size(),
         sTable.unresolved.size());

      size_t nMissing = sTable.unresolved.size();
      if(!nMissing)
         break;

      cmn::scopeNode *pToLoad = NULL;
      for(auto it=sTable.unresolved.begin();it!=sTable.unresolved.end();++it)
      {
         auto refToFind = (*it)->ref;
         unloadedScopeFinder f(refToFind);
         root.acceptVisitor(f);
         if(f.any())
         {
            ::printf("trying to find symbol %s\n",refToFind.c_str());
            pToLoad = &f.mostLikely();
            break;
         }
      }

      if(pToLoad)
      {
         ::printf("loading %s and trying again\n",pToLoad->path.c_str());
         loader::loadFolder(*pToLoad);
         { cmn::diagVisitor v; root.acceptVisitor(v); }
      }
      else
      {
         ::printf("no guesses on what to load to find missing symbols; try settling\n");
         if(nMissing != missingLastTime)
            missingLastTime = nMissing; // retry
         else
            throw std::runtime_error("gave up trying to resolve symbols");
      }
   }
}

} // namespace araceli

#include "nameUtil.hpp"
#include "symbolTable.hpp"
#include <stdio.h>

namespace cmn {

void symbolTable::publish(const std::string& fqn, node& n)
{
   ::printf("publishing symbol %s for %lld\n",fqn.c_str(),(size_t)&n);
   published[fqn] = &n;
}

void symbolTable::tryResolveVarType(const std::string& objName, node& obj, linkBase& l)
{
   if(objName == l.ref)
   {
      l.bind(obj.demandSoleChild<typeNode>());
      unresolved.erase(&l);
   }
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

void linkResolver::visit(node& n)
{
   if(m_l._getRefee())
      return;

   node *pParent = n.getParent();
   if(pParent)
      pParent->acceptVisitor(*this);
}

void linkResolver::visit(scopeNode& n)
{
   if(m_l._getRefee())
      return;

   if(m_mode & kContainingScopes)
   {
      tryResolve(fullyQualifiedName::build(n));
      if(m_l._getRefee())
         return;
   }

   hNodeVisitor::visit(n);
}

void linkResolver::visit(classNode& n)
{
   if(m_l._getRefee())
      return;

   std::vector<fieldNode*> fields;

   if(m_mode & kOwnClass)
   {
      m_mode &= ~kOwnClass; // don't do this again

      if(m_mode & kLocalsAndFields)
         n.getChildrenOf<fieldNode>(fields);
      else
      {
         tryResolve(fullyQualifiedName::build(n));
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
               it->getRefee()->getChildrenOf<fieldNode>(fields);
               it->getRefee()->acceptVisitor(*this);
            }
            else
            {
               tryResolve(fullyQualifiedName::build(*it->getRefee()));
               if(m_l._getRefee())
                  return;
            }
         }
      }
   }

   if(m_mode & kLocalsAndFields)
   {
      n.getChildrenOf<fieldNode>(fields);
      for(auto it=fields.begin();it!=fields.end();++it)
         if(m_l._getRefee() == NULL)
            m_sTable.tryResolveVarType((*it)->name,**it,m_l);
   }

   hNodeVisitor::visit(n);
}

void linkResolver::visit(methodNode& n)
{
   if(m_mode & kLocalsAndFields)
   {
      // check args
      auto args = n.getChildrenOf<argNode>();
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

void nodePublisher::visit(classNode& n)
{
   m_sTable.publish(fullyQualifiedName::build(n),n);

   hNodeVisitor::visit(n);
}

void nodePublisher::visit(memberNode& n)
{
   bool isField = (dynamic_cast<fieldNode*>(&n)!=NULL);
   if(isField || (n.flags & (nodeFlags::kOverride | nodeFlags::kAbstract)))
   {
      m_sTable.publish(fullyQualifiedName::build(n,n.name),n);
   }

   hNodeVisitor::visit(n);
}

void nodeResolver::visit(classNode& n)
{
   for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
   {
      m_sTable.markRequired(*it);
      linkResolver v(m_sTable,*it,linkResolver::kContainingScopes);
      n.acceptVisitor(v);
   }

   hNodeVisitor::visit(n);
}

void nodeResolver::visit(methodNode& n)
{
   if(n.flags & nodeFlags::kOverride)
   {
      m_sTable.markRequired(n.baseImpl);
      linkResolver v(m_sTable,n.baseImpl,linkResolver::kBaseClasses);
      n.acceptVisitor(v);
   }

   hNodeVisitor::visit(n);
}

void nodeResolver::visit(userTypeNode& n)
{
   m_sTable.markRequired(n.pDef);
   linkResolver v(m_sTable,n.pDef,
      linkResolver::kOwnClass | linkResolver::kContainingScopes);
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
   m_sTable.markRequired(n.pDef);
   linkResolver v(m_sTable,n.pDef,
      linkResolver::kBaseClasses | linkResolver::kLocalsAndFields);
   n.acceptVisitor(v);

   hNodeVisitor::visit(n);
}

void nodeLinker::linkGraph(node& root)
{
   ::printf("entering link/load loop ----\n");
   symbolTable sTable;
   size_t missingLastTime = 0;
   while(true)
   {
      { nodePublisher p(sTable); treeVisitor t(p); root.acceptVisitor(t); }
      { nodeResolver r(sTable); treeVisitor t(r); root.acceptVisitor(t); }
      ::printf("%lld published; %lld unresolved\n",
         sTable.published.size(),
         sTable.unresolved.size());

      size_t nMissing = sTable.unresolved.size();
      if(!nMissing)
         break;

      if(!loadAnotherSymbol(root,sTable))
      {
         ::printf("no guesses on what to load to find missing symbols; try settling\n");
         if(nMissing != missingLastTime)
            missingLastTime = nMissing; // retry
         else
            throw std::runtime_error("gave up trying to resolve symbols");
      }
   }
}

} // namespace cmn

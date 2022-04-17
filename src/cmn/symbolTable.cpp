#include "nameUtil.hpp"
#include "symbolTable.hpp"
#include "trace.hpp"

namespace cmn {

void symbolTable::markRequired(linkBase& l)
{
   if(!l._getRefee())
      unresolved.insert(&l);
}

void symbolTable::publish(const std::string& fqn, node& n)
{
   cdwVERBOSE("publishing symbol %s for %lld\n",fqn.c_str(),(size_t)&n);
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
   cdwVERBOSE("resolving ref %s from context %s [exact]\n",l.ref.c_str(),refingScope.c_str());
   if(l._getRefee())
      return;

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
   cdwVERBOSE("resolving ref %s from context %s [w/ context walk]\n",l.ref.c_str(),refingScope.c_str());
   if(l._getRefee())
      return;

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
         if(prefix == "." || prefix == "")
            break;
         prefix = nameUtil::stripLast(prefix);
      }
   }
}

void symbolTable::debugDump()
{
   cdwDEBUG("-- beginning complete dump of symbol table --\n");

   for(auto it=published.begin();it!=published.end();++it)
      cdwDEBUG("  published '%s' of '%s'\n",it->first.c_str(),typeid(*it->second).name());

   for(auto it=unresolved.begin();it!=unresolved.end();++it)
      cdwDEBUG("  unresolved '%s' of '%s'\n",(*it)->ref.c_str(),typeid(**it).name());

   cdwDEBUG("-- end of symbol table --\n");
}

bool symbolTable::tryBind(const std::string& fqn, linkBase& l)
{
   cdwVERBOSE("  checking %s...",fqn.c_str());

   auto it = published.find(fqn);
   if(it == published.end())
      cdwVERBOSE("nope\n");
   else
   {
      cdwVERBOSE("ok!\n");
      l.bind(*it->second);
      unresolved.erase(&l);
      return true;
   }

   return false;
}

void localFinder::visit(node& n)
{
   if(n.getParent())
      n.getParent()->acceptVisitor(*this);
}

void localFinder::visit(methodNode& n)
{
   // no-op; slight optimization to stop chaining early
}

void localFinder::visit(sequenceNode& n)
{
   auto locals = n.getChildrenOf<localDeclNode>();
   for(auto it=locals.begin();it!=locals.end();++it)
   {
      node*& pPtr = m_table[(*it)->name];
      if(pPtr)
         continue; // things in a closer scope win

      pPtr = *it;
   }
}

linkResolver::linkResolver(symbolTable& st, linkBase& l, size_t mode)
: m_sTable(st), m_l(l), m_mode(mode)
{
   if(m_l._getRefee())
      return;

   tryResolve(""); // always make at least one attempt
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

      if(m_mode & kLocalsAndFields) // TODO do I need a special mode for fields?  They're published?
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
   if(m_l._getRefee())
      return;

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

void linkResolver::visit(funcNode& n)
{
   if(m_l._getRefee())
      return;

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

void nodePublisher::visit(constNode& n)
{
   m_sTable.publish(fullyQualifiedName::build(n,n.name),n.demandSoleChild<typeNode>());

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

   if(!n.pDef._getRefee())
   {
      // check locals first
      std::map<std::string,node*> locals;
      { localFinder v(locals); n.acceptVisitor(v); }
      for(auto it=locals.begin();it!=locals.end();++it)
         m_sTable.tryResolveVarType(it->first,*it->second,n.pDef);
   }

   linkResolver v(m_sTable,n.pDef,
      linkResolver::kBaseClasses | linkResolver::kLocalsAndFields);
   n.acceptVisitor(v);

   hNodeVisitor::visit(n);
}

void nodeLinker::linkGraph(node& root)
{
   cdwVERBOSE("entering link/load loop ----\n");
   symbolTable sTable;
   size_t missingLastTime = 0;
   while(true)
   {
      { nodePublisher p(sTable); treeVisitor t(p); root.acceptVisitor(t); }
      { nodeResolver r(sTable); treeVisitor t(r); root.acceptVisitor(t); }
      cdwVERBOSE("%lld published; %lld unresolved\n",
         sTable.published.size(),
         sTable.unresolved.size());

      size_t nMissing = sTable.unresolved.size();
      if(!nMissing)
         break;

      if(!loadAnotherSymbol(root,sTable))
      {
         cdwVERBOSE("no guesses on what to load to find missing symbols; try settling\n");
         if(nMissing != missingLastTime)
            missingLastTime = nMissing; // retry
         else
         {
            sTable.debugDump();
            throw std::runtime_error("gave up trying to resolve symbols");
         }
      }
   }
}

} // namespace cmn

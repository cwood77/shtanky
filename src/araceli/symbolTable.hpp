#pragma once
#include "ast.hpp"
#include <map>
#include <set>

namespace araceli {

// ns1::ns2::classC + a::field
// possible answers
// baseClass's blassClass
// ::ns1::ns2::a::field
// ::ns1::a::field
// ::a::field
// class's have multiple scopes: their canonical scope *and* their inheritance chain

class symbolTable {
public:
   std::map<std::string,cmn::node*> resolved;
   std::set<linkBase*> unresolved;

   void publish(const std::string& fqn, cmn::node& n);
   void tryResolve(const std::string& refingScope, linkBase& l);
};

// knows about scope-providing nodes
class fullScopeNameBuilder : public hNodeVisitor {
public:
   virtual void visit(scopeNode& n);
   virtual void visit(classNode& n);

   std::string fqn;
};

// knows all the scopes of a given node
class linkResolver : public hNodeVisitor {
public:
   linkResolver(symbolTable& st, linkBase& l) : m_sTable(st), m_l(l) {}

   // TODO I think this is all wrong
   virtual void visit(classNode& n)
   {
      for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
      {
         if(it->pRefee)
         {
            fullScopeNameBuilder v;
            it->pRefee->acceptVisitor(v);
            m_sTable.tryResolve(v.fqn,*it);
         }
      }

      hNodeVisitor::visit(n);
   }

private:
   symbolTable& m_sTable;
   linkBase& m_l;
};

class nodePublisher : public hNodeVisitor {
public:
   explicit nodePublisher(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n)
   {
      fullScopeNameBuilder v;
      n.acceptVisitor(v);
      m_sTable.publish(v.fqn,n);

      hNodeVisitor::visit(n);
   }

private:
   symbolTable& m_sTable;
};

// knows all the links in a given node
class nodeResolver : public hNodeVisitor {
public:
   explicit nodeResolver(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n)
   {
      for(auto it=n.baseClasses.begin();it!=n.baseClasses.end();++it)
      {
         linkResolver v(m_sTable,*it);
         n.acceptVisitor(v);
      }

      hNodeVisitor::visit(n);
   }

private:
   symbolTable& m_sTable;
};

template<class T>
class treeVisitor : public hNodeVisitor {
public:
   explicit treeVisitor(symbolTable& st) : m_sTable(st) {}

   virtual void visit(cmn::node& n)
   {
      T v(m_sTable);
      n.acceptVisitor(v);
      visitChildren(n);
   }

private:
   symbolTable& m_sTable;
};

} // namespace araceli

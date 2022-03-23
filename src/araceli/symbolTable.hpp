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
   void tryResolveExact(const std::string& refingScope, linkBase& l);
   void tryResolveWithParents(const std::string& refingScope, linkBase& l);

private:
   bool tryBind(const std::string& fqn, linkBase& l);
};

// knows about scope-providing nodes
class fullScopeNameBuilder : public hNodeVisitor {
public:
   virtual void visit(cmn::node& n);
   virtual void visit(scopeNode& n);
   virtual void visit(classNode& n);

   std::string fqn;
};

// knows all the scopes of a given node
class linkResolver : public hNodeVisitor {
public:
   enum {
      kContainingScopes = 0x1,
      kOwnClass         = 0x2,
      kBaseClasses      = 0x4,
      kLocalsAndFields  = 0x8,
   };

   linkResolver(symbolTable& st, linkBase& l, size_t mode)
   : m_sTable(st), m_l(l), m_mode(mode) {}

   virtual void visit(cmn::node& n);
   virtual void visit(scopeNode& n);
   virtual void visit(classNode& n);

private:
   void tryResolve(const std::string& refingScope);

   symbolTable& m_sTable;
   linkBase& m_l;
   size_t m_mode;
};

// knows all the links in a given node
class nodePublisher : public hNodeVisitor {
public:
   explicit nodePublisher(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n);
   virtual void visit(methodNode& n);

private:
   symbolTable& m_sTable;
};

// knows all the links in a given node
class nodeResolver : public hNodeVisitor {
public:
   explicit nodeResolver(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n);
   virtual void visit(methodNode& n); // really all members
   virtual void visit(userTypeNode& n);
   //virtual void visit(invokeNode& n);

private:
   symbolTable& m_sTable;
};

template<class T>
class treeSymbolVisitor : public hNodeVisitor {
public:
   explicit treeSymbolVisitor(symbolTable& st) : m_sTable(st) {}

   virtual void visit(cmn::node& n)
   {
      T v(m_sTable);
      n.acceptVisitor(v);
      visitChildren(n);
   }

private:
   symbolTable& m_sTable;
};

class unloadedScopeFinder : public hNodeVisitor {
public:
   explicit unloadedScopeFinder(const std::string& missingRef);

   bool any();
   scopeNode& mostLikely();

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(scopeNode& n);

private:
   std::string m_missingRef;
   std::map<std::string,scopeNode*> m_candidates;
};

} // namespace araceli

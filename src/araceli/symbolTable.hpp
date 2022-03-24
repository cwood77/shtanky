#pragma once
#include "../cmn/ast.hpp"
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
   std::set<cmn::linkBase*> unresolved;

   void publish(const std::string& fqn, cmn::node& n);
   void tryResolveExact(const std::string& refingScope, cmn::linkBase& l);
   void tryResolveWithParents(const std::string& refingScope, cmn::linkBase& l);

private:
   bool tryBind(const std::string& fqn, cmn::linkBase& l);
};

// knows about scope-providing nodes
// deprecated
class fullScopeNameBuilder : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n);
   virtual void visit(cmn::scopeNode& n);
   virtual void visit(cmn::classNode& n);

   std::string fqn;
};

class typeFinder : public cmn::araceliVisitor<> {
public:
   typeFinder() : pType(NULL) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::typeNode& n) { pType = &n; }

   cmn::typeNode *pType;
};

class fieldGatherer : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fieldNode& n) { fields.insert(&n); hNodeVisitor::visit(n); }

   std::set<cmn::fieldNode*> fields;
};

// knows all the scopes of a given node
class linkResolver : public cmn::araceliVisitor<> {
public:
   enum {
      kContainingScopes = 0x1,
      kOwnClass         = 0x2,
      kBaseClasses      = 0x4,
      kLocalsAndFields  = 0x8,
   };

   linkResolver(symbolTable& st, cmn::linkBase& l, size_t mode)
   : m_sTable(st), m_l(l), m_mode(mode) {}

   virtual void visit(cmn::node& n);
   virtual void visit(cmn::scopeNode& n);
   virtual void visit(cmn::classNode& n);

private:
   void tryResolve(const std::string& refingScope);

   symbolTable& m_sTable;
   cmn::linkBase& m_l;
   size_t m_mode;
};

// knows all the links in a given node
class nodePublisher : public cmn::araceliVisitor<> {
public:
   explicit nodePublisher(symbolTable& st) : m_sTable(st) {}

   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::memberNode& n);

private:
   symbolTable& m_sTable;
};

// knows all the links in a given node
class nodeResolver : public cmn::araceliVisitor<> {
public:
   explicit nodeResolver(symbolTable& st) : m_sTable(st) {}

   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::methodNode& n);
   virtual void visit(cmn::userTypeNode& n);
   //virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::varRefNode& n);

private:
   symbolTable& m_sTable;
};

// deprecated
template<class T>
class treeSymbolVisitor : public cmn::araceliVisitor<>{
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

class unloadedScopeFinder : public cmn::araceliVisitor<> {
public:
   explicit unloadedScopeFinder(const std::string& missingRef);

   bool any();
   cmn::scopeNode& mostLikely();

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::scopeNode& n);

private:
   std::string m_missingRef;
   std::map<std::string,cmn::scopeNode*> m_candidates;
};

} // namespace araceli

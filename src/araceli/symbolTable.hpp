#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include <set>

namespace araceli {

// the exhaustive list of linkages between AST nodes
//
// type name resolution                                                           enforced
// -    baseClass -> class    via:: type name              for:: enabler           *
// - userTypeNode -> class    via:: type name                                      *
//
// base class member
// -       method -> baseImpl via:: method name                                    *
// -   varRefNode -> type     via:: ?? fields and globals  for:: ara codegen       *
//
// type prop
// -   invokeNode -> method   via:: ?? type prop + name                       NO, and unimpled
//
// anything else?
// fieldAccessNode

class symbolTable {
public:
   std::map<std::string,cmn::node*> published;
   std::set<cmn::linkBase*> unresolved;

   void markRequired(cmn::linkBase& l) { unresolved.insert(&l); }
   void publish(const std::string& fqn, cmn::node& n);
   void tryResolveVarType(const std::string& objName, cmn::node& obj, cmn::linkBase& l);
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
   virtual void visit(cmn::methodNode& n);

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

void linkGraph(cmn::node& root);

} // namespace araceli

#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include <set>

namespace cmn {

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
   std::map<std::string,node*> published;
   std::set<linkBase*> unresolved;

   void markRequired(linkBase& l) { unresolved.insert(&l); }
   void publish(const std::string& fqn, node& n);
   void tryResolveVarType(const std::string& objName, node& obj, linkBase& l);
   void tryResolveExact(const std::string& refingScope, linkBase& l);
   void tryResolveWithParents(const std::string& refingScope, linkBase& l);

private:
   bool tryBind(const std::string& fqn, linkBase& l);
};

// knows all the scopes of a given node
class linkResolver : public araceliVisitor<> {
public:
   enum {
      kContainingScopes = 0x1,
      kOwnClass         = 0x2,
      kBaseClasses      = 0x4,
      kLocalsAndFields  = 0x8,
   };

   linkResolver(symbolTable& st, linkBase& l, size_t mode)
   : m_sTable(st), m_l(l), m_mode(mode) {}

   virtual void visit(node& n);
   virtual void visit(scopeNode& n);
   virtual void visit(classNode& n);
   virtual void visit(methodNode& n);

private:
   void tryResolve(const std::string& refingScope);

   symbolTable& m_sTable;
   linkBase& m_l;
   size_t m_mode;
};

// knows all the links in a given node
class nodePublisher : public araceliVisitor<> {
public:
   explicit nodePublisher(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n);
   virtual void visit(memberNode& n);

private:
   symbolTable& m_sTable;
};

// knows all the links in a given node
class nodeResolver : public araceliVisitor<> {
public:
   explicit nodeResolver(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(userTypeNode& n);
   //virtual void visit(invokeNode& n);
   virtual void visit(varRefNode& n);

private:
   symbolTable& m_sTable;
};

class nodeLinker {
public:
   void linkGraph(node& root);

protected:
   virtual bool loadAnotherSymbol(node& root, symbolTable& sTable) { return false; }
};

void linkGraph(node& root);

} // namespace cmn

namespace araceli {

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

class nodeLinker : public cmn::nodeLinker {
protected:
   virtual bool loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable);
};

} // namespace araceli

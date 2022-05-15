#pragma once
#include "ast.hpp"
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
// -   invokeNode -> method   via:: ?? type prop + name                     NO, and unimpled
//
// anything else?
// fieldAccessNode

// it is ok to dump everything into a single string table?  That would mean names can't
// conflict.  like, a variable can never have the same name as a type (e.g. vtbl in gencode).

class symbolTable {
public:
   std::map<std::string,node*> published;
   std::set<linkBase*> unresolved;

   void markRequired(linkBase& l);
   void publish(const std::string& fqn, node& n);
   void tryResolveVarType(const std::string& objName, node& obj, linkBase& l);
   void tryResolveExact(const std::string& refingScope, linkBase& l);
   void tryResolveWithParents(const std::string& refingScope, linkBase& l);

   void debugDump();

private:
   bool tryBind(const std::string& fqn, linkBase& l);
};

class localFinder : public hNodeVisitor {
public:
   explicit localFinder(std::map<std::string,node*>& table) : m_table(table) {}

   virtual void visit(node& n);
   virtual void visit(methodNode& n);
   virtual void visit(sequenceNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::map<std::string,node*>& m_table;
};

// knows how to bind different kinds of links
class linkResolver : public hNodeVisitor {
public:
   enum {
      kContainingScopes = 0x1,
      kOwnClass         = 0x2,
      kBaseClasses      = 0x4,
      kArgs             = 0x8,
   };

   linkResolver(symbolTable& st, linkBase& l, size_t mode);

   virtual void visit(node& n);
   virtual void visit(scopeNode& n);
   virtual void visit(classNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(funcNode& n);

   virtual void _implementLanguage() {} // all

private:
   void tryResolve(const std::string& refingScope);

   symbolTable& m_sTable;
   linkBase& m_l;
   size_t m_mode;
   bool m_checkedScope;
};

class nodePublisher : public hNodeVisitor {
public:
   explicit nodePublisher(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n);
   virtual void visit(memberNode& n);
   virtual void visit(constNode& n);
   virtual void visit(funcNode& n);

   virtual void _implementLanguage() {} // all

private:
   symbolTable& m_sTable;
};

// knows all the links in a given node
class nodeResolver : public hNodeVisitor {
public:
   explicit nodeResolver(symbolTable& st) : m_sTable(st) {}

   virtual void visit(classNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(userTypeNode& n);
   //virtual void visit(invokeNode& n);
   virtual void visit(callNode& n);
   virtual void visit(varRefNode& n);

   virtual void _implementLanguage() {} // all

private:
   symbolTable& m_sTable;
};

class nodeLinker {
public:
   void linkGraph(node& root);

protected:
   virtual bool tryFixUnresolved(node& root, symbolTable& sTable) { return false; }
   virtual bool loadAnotherSymbol(node& root, symbolTable& sTable) { return false; }
};

} // namespace cmn

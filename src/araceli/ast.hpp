#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class projectNode;
class scopeNode;
class classNode;
class fileNode;

class iNodeVisitor : public cmn::iNodeVisitor {
public:
   virtual void visit(projectNode& n) = 0;
   virtual void visit(scopeNode& n) = 0;
   virtual void visit(classNode& n) = 0;
   virtual void visit(fileNode& n) = 0;
};

class linkBase {
public:
   linkBase() : pRefee(NULL) {}

   std::string ref;
   cmn::node *pRefee;
};

template<class T>
class link : public linkBase {
public:
   bool tryBind(cmn::node& dest);

   T *getRefee();
};

class projectNode : public cmn::node {
public:
   std::string targetType;
};

class scopeNode : public cmn::node {
public:
   scopeNode() : inProject(false), loaded(false) {}

   std::string path;
   std::string scopeName;
   bool inProject;
   bool loaded;
};

class classNode : public cmn::node {
public:
   std::string name;

   std::vector<link<classNode> > baseClasses;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class fileNode : public cmn::node {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class metadata {
};

class metadataBuilder : public iNodeVisitor {
};

class hNodeVisitor : public iNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(projectNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(scopeNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(classNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(fileNode& n) { visit(static_cast<cmn::node&>(n)); }
};

} // namespace araceli

#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class classNode;
class fileNode;

class iNodeVisitor : public cmn::iNodeVisitor {
public:
   virtual void visit(classNode& n) = 0;
   virtual void visit(fileNode& n) = 0;
};

class linkBase {
public:
   std::string ref;
};

template<class T>
class link : public linkBase {
public:
   link() : pRefee(NULL) {}

   bool tryBind(cmn::node& dest);

   T *pRefee;
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

} // namespace araceli

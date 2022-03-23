#pragma once
#include "../cmn/ast.hpp"
#include <stdexcept>
#include <typeinfo>

namespace araceli {

class projectNode;
class scopeNode;
class fileNode;
class classNode;
class memberNode;
class methodNode;
class argNode;
class typeNode;
class strTypeNode;
class arrayTypeNode;
class voidTypeNode;
class userTypeNode;
class fieldNode;
class sequenceNode;
class invokeNode;
class callNode;
class varRefNode;
class stringLiteralNode;
class boolLiteralNode;
class intLiteralNode;

class iNodeVisitor : public cmn::iNodeVisitor {
public:
   virtual void visit(projectNode& n) = 0;
   virtual void visit(scopeNode& n) = 0;
   virtual void visit(fileNode& n) = 0;
   virtual void visit(classNode& n) = 0;
   virtual void visit(memberNode& n) = 0;
   virtual void visit(methodNode& n) = 0;
   virtual void visit(argNode& n) = 0;
   virtual void visit(typeNode& n) = 0;
   virtual void visit(strTypeNode& n) = 0;
   virtual void visit(arrayTypeNode& n) = 0;
   virtual void visit(voidTypeNode& n) = 0;
   virtual void visit(userTypeNode& n) = 0;
   virtual void visit(fieldNode& n) = 0;
   virtual void visit(sequenceNode& n) = 0;
   virtual void visit(invokeNode& n) = 0;
   virtual void visit(callNode& n) = 0;
   virtual void visit(varRefNode& n) = 0;
   virtual void visit(stringLiteralNode& n) = 0;
   virtual void visit(boolLiteralNode& n) = 0;
   virtual void visit(intLiteralNode& n) = 0;
};

namespace nodeFlags {
   enum {
      kOverride  = 1 << 0,
      kAbstract  = 1 << 1,
      kStatic    = 1 << 2,
      kInterface = 1 << 3,

      kPublic    = 1 << 4,
      kProtected = 1 << 5,
      kPrivate   = 1 << 6,
   };
};

class linkBase {
public:
   linkBase() : m_pRefee(NULL) {}

   virtual void bind(cmn::node& dest) = 0;

   std::string ref;

   cmn::node *_getRefee() { return m_pRefee; }

protected:
   cmn::node *m_pRefee;
};

template<class T>
class link : public linkBase {
public:
   virtual void bind(cmn::node& dest)
   {
      m_pRefee = dynamic_cast<T*>(&dest);
      if(!m_pRefee)
         throw std::runtime_error(
            std::string("link expected node type ") + typeid(T).name());
   }

   T *getRefee() { return dynamic_cast<T*>(m_pRefee); }
};

class projectNode : public cmn::node {
public:
   std::string targetType;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class scopeNode : public cmn::node {
public:
   scopeNode() : inProject(false), loaded(false) {}

   std::string path;
   std::string scopeName;
   bool inProject;
   bool loaded;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class fileNode : public cmn::node {
public:
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class classNode : public cmn::node {
public:
   classNode() : flags(0) {}

   size_t flags;
   std::string name;

   std::vector<link<classNode> > baseClasses;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class memberNode : public cmn::node {
public:
   memberNode() : flags(0) {}

   size_t flags;
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class methodNode : public memberNode {
public:
   link<methodNode> baseImpl;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class argNode : public cmn::node {
public:
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class typeNode : public cmn::node {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class strTypeNode : public typeNode {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class arrayTypeNode : public typeNode {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class voidTypeNode : public typeNode {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class userTypeNode : public typeNode {
public:
   link<classNode> pDef;
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class fieldNode : public memberNode {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class sequenceNode : public cmn::node {
public:
   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class invokeNode : public cmn::node {
public:
   link<methodNode> proto; // unlinked
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class callNode : public cmn::node {
public:
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class varRefNode : public cmn::node {
public:
   link<typeNode> pDef; // in work
   std::string name;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class stringLiteralNode : public cmn::node {
public:
   std::string value;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class boolLiteralNode : public cmn::node {
public:
   boolLiteralNode() : value(false) {}

   bool value;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class intLiteralNode : public cmn::node {
public:
   std::string value;

   virtual void acceptVisitor(cmn::iNodeVisitor& v)
   { dynamic_cast<iNodeVisitor&>(v).visit(*this); }
};

class hNodeVisitor : public iNodeVisitor {
public:
   virtual void visit(cmn::node& n) { }
   virtual void visit(projectNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(scopeNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(fileNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(classNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(memberNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(methodNode& n) { visit(static_cast<memberNode&>(n)); }
   virtual void visit(argNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(typeNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(strTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(arrayTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(voidTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(userTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(fieldNode& n) { visit(static_cast<memberNode&>(n)); }
   virtual void visit(sequenceNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(invokeNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(callNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(varRefNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(stringLiteralNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(boolLiteralNode& n) { visit(static_cast<cmn::node&>(n)); }
   virtual void visit(intLiteralNode& n) { visit(static_cast<cmn::node&>(n)); }
};

class diagVisitor : public hNodeVisitor {
public:
   diagVisitor() : m_nIndents(0) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(projectNode& n);
   virtual void visit(scopeNode& n);
   virtual void visit(fileNode& n);
   virtual void visit(classNode& n);
   virtual void visit(memberNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(argNode& n);
   virtual void visit(strTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(fieldNode& n);
   virtual void visit(sequenceNode& n);
   virtual void visit(invokeNode& n);
   virtual void visit(callNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);

private:
   std::string getIndent() const;

   size_t m_nIndents;
};

class treeVisitor : public hNodeVisitor {
public:
   explicit treeVisitor(cmn::iNodeVisitor& inner) : m_inner(inner) {}

   virtual void visit(cmn::node& n)
   {
      n.acceptVisitor(m_inner);
      visitChildren(n);
   }

private:
   cmn::iNodeVisitor& m_inner;
};

} // namespace araceli

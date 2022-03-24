#pragma once

#include <list>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

namespace cmn {

class node;
class araceliProjectNode;
class liamProjectNode;
class scopeNode;
class fileNode;
class fileRefNode;
class classNode;
class memberNode;
class methodNode;
class fieldNode;
class funcNode;
class argNode;
class typeNode;
class strTypeNode;
class arrayTypeNode;
class voidTypeNode;
class userTypeNode;
class ptrTypeNode;
class sequenceNode;
class invokeNode;
class callNode;
class varRefNode;
class stringLiteralNode;
class boolLiteralNode;
class intLiteralNode;

class iNodeVisitor {
public:
   virtual ~iNodeVisitor() {}

   virtual void visit(node& n) = 0;
   virtual void visit(araceliProjectNode& n) = 0;
   virtual void visit(liamProjectNode& n) = 0;
   virtual void visit(scopeNode& n) = 0;
   virtual void visit(fileNode& n) = 0;
   virtual void visit(fileRefNode& n) = 0;
   virtual void visit(classNode& n) = 0;
   virtual void visit(memberNode& n) = 0;
   virtual void visit(methodNode& n) = 0;
   virtual void visit(fieldNode& n) = 0;
   virtual void visit(funcNode& n) = 0;
   virtual void visit(argNode& n) = 0;
   virtual void visit(typeNode& n) = 0;
   virtual void visit(strTypeNode& n) = 0;
   virtual void visit(arrayTypeNode& n) = 0;
   virtual void visit(voidTypeNode& n) = 0;
   virtual void visit(userTypeNode& n) = 0;
   virtual void visit(ptrTypeNode& n) = 0;
   virtual void visit(sequenceNode& n) = 0;
   virtual void visit(invokeNode& n) = 0;
   virtual void visit(callNode& n) = 0;
   virtual void visit(varRefNode& n) = 0;
   virtual void visit(stringLiteralNode& n) = 0;
   virtual void visit(boolLiteralNode& n) = 0;
   virtual void visit(intLiteralNode& n) = 0;

protected:
   virtual void _implementLanguage() = 0;

   void visitChildren(node& n);

   template<class T>
   void unexpected(T& n)
   {
      std::stringstream stream;
      stream << "node type " << typeid(T).name() << " was unexpected";
      throw std::runtime_error(stream.str());
   }
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

class node {
public:
   node() : lineNumber(0), flags(0), m_pParent(NULL) {}
   virtual ~node();

   unsigned long lineNumber;
   std::set<std::string> attributes;
   size_t flags;

   node *getParent() { return m_pParent; }
   void appendChild(node& n);
   std::vector<node*>& getChildren() { return m_children; }

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   // ------ template helpers

   template<class T>
   void getChildrenOf(std::vector<T*>& v)
   {
      for(auto it=m_children.begin();it!=m_children.end();++it)
      {
         auto candidate = dynamic_cast<T*>(*it);
         if(candidate)
            v.push_back(candidate);
      }
   }

   template<class T>
   std::vector<T*> getChildrenOf()
   {
      std::vector<T*> rval;
      getChildrenOf<T>(rval);
      return rval;
   }

   template<class T>
   T& demandSoleChild()
   {
      std::vector<T*> candidates = getChildrenOf<T>();
      if(candidates.size() != 1)
         throw std::runtime_error("expected single child");
      return **candidates.begin();
   }

private:
   node *m_pParent;
   std::vector<node*> m_children;

   node(const node&);
   node& operator=(const node&);
};

class linkBase {
public:
   linkBase() : m_pRefee(NULL) {}

   virtual void bind(node& dest) = 0;

   std::string ref;

   node *_getRefee() { return m_pRefee; }

protected:
   node *m_pRefee;
};

template<class T>
class link : public linkBase {
public:
   virtual void bind(node& dest)
   {
      m_pRefee = dynamic_cast<T*>(&dest);
      if(!m_pRefee)
         throw std::runtime_error(
            std::string("link expected node type ") + typeid(T).name());
   }

   T *getRefee() { return dynamic_cast<T*>(m_pRefee); }
};

// ----------------------- project -----------------------

class araceliProjectNode : public node {
public:
   std::string targetType;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class liamProjectNode : public node {
public:
   std::string sourceFullPath;
   std::list<std::string> searchPaths;
   std::set<std::string> loadedPaths;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class scopeNode : public node {
public:
   scopeNode() : inProject(false), loaded(false) {}

   std::string path; // whatever came from commandline
   std::string scopeName; // last part of path name
   bool inProject;
   bool loaded;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class fileNode : public node {
public:
   std::string fullPath;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class fileRefNode : public node {
public:
   std::string ref;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- coarse -----------------------

class classNode : public node {
public:
   std::string name;

   std::vector<link<classNode> > baseClasses;

   std::list<classNode*> computeLineage();

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

private:
   void computeLineage(std::list<classNode*>& l);
};

class memberNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class methodNode : public memberNode {
public:
   link<methodNode> baseImpl;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class fieldNode : public memberNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class funcNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class argNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- types -----------------------

class typeNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class strTypeNode : public typeNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class arrayTypeNode : public typeNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class voidTypeNode : public typeNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class userTypeNode : public typeNode {
public:
   link<classNode> pDef;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class ptrTypeNode : public typeNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- fine -----------------------

class sequenceNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class invokeNode : public node {
public:
   link<methodNode> proto; // unimpled

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class callNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class varRefNode : public node {
public:
   link<typeNode> pDef;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- literals -----------------------

class stringLiteralNode : public node {
public:
   std::string value;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class boolLiteralNode : public node {
public:
   boolLiteralNode() : value(false) {}

   bool value;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class intLiteralNode : public node {
public:
   std::string value;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- language visitors -----------------------

class hNodeVisitor : public iNodeVisitor {
public:
   virtual void visit(node& n) { }
   virtual void visit(araceliProjectNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(liamProjectNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(scopeNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(fileNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(fileRefNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(classNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(memberNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(methodNode& n) { visit(static_cast<memberNode&>(n)); }
   virtual void visit(fieldNode& n) { visit(static_cast<memberNode&>(n)); }
   virtual void visit(funcNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(argNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(typeNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(strTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(arrayTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(voidTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(userTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(ptrTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(sequenceNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(invokeNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(callNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(varRefNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(stringLiteralNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(boolLiteralNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(intLiteralNode& n) { visit(static_cast<node&>(n)); }
};

class diagVisitor : public hNodeVisitor {
public:
   diagVisitor() : m_nIndents(0) {}

   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(araceliProjectNode& n);
   virtual void visit(liamProjectNode& n);
   virtual void visit(scopeNode& n);
   virtual void visit(fileNode& n);
   virtual void visit(fileRefNode& n);
   virtual void visit(classNode& n);
   virtual void visit(memberNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(fieldNode& n);
   virtual void visit(funcNode& n);
   virtual void visit(argNode& n);
   // type
   virtual void visit(strTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(ptrTypeNode& n);
   virtual void visit(sequenceNode& n);
   virtual void visit(invokeNode& n);
   virtual void visit(callNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);

   virtual void _implementLanguage() {} // all

private:
   class autoIndent {
   public:
      explicit autoIndent(diagVisitor& v);
      ~autoIndent();

   private:
      diagVisitor& m_v;
   };

   std::string getIndent() const;

   size_t m_nIndents;
};

template<class T = hNodeVisitor>
class araceliVisitor : public T {
public:
   virtual void visit(liamProjectNode& n) { T::unexpected(n); }
   virtual void visit(fileRefNode& n) { T::unexpected(n); }
   virtual void visit(funcNode& n) { T::unexpected(n); }
   virtual void visit(ptrTypeNode& n) { T::unexpected(n); }

   virtual void _implementLanguage() {} // araceli
};

template<class T = hNodeVisitor>
class liamVisitor : public T {
public:
   virtual void visit(araceliProjectNode& n) { T::unexpected(n); }
   virtual void visit(scopeNode& n) { T::unexpected(n); }
   virtual void visit(fileNode& n) { T::unexpected(n); }
   virtual void visit(methodNode& n) { T::unexpected(n); }

   virtual void _implementLanguage() {} // liam
};

// ----------------------- helper visitors -----------------------

class fullyQualifiedName : private hNodeVisitor {
public:
   static std::string build(node& n, const std::string& start = "");

   virtual void _implementLanguage() {} // all

private:
   virtual void visit(node& n);
   virtual void visit(scopeNode& n);
   virtual void visit(classNode& n);

   void makeAbsolute();
   void prepend(const std::string& n);

   std::string m_fqn;
};

class treeVisitor : public hNodeVisitor {
public:
   explicit treeVisitor(iNodeVisitor& inner) : m_inner(inner) {}

   virtual void visit(node& n)
   {
      n.acceptVisitor(m_inner);
      visitChildren(n);
   }

   virtual void _implementLanguage() {} // all

private:
   iNodeVisitor& m_inner;
};

} // namespace cmn

#pragma once

#include "throw.hpp"
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

namespace cmn {

class outStream;

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
class constNode;
class funcNode;
class intrinsicNode;
class argNode;
class typeNode;
class strTypeNode;
class boolTypeNode;
class intTypeNode;
class arrayTypeNode;
class voidTypeNode;
class userTypeNode;
class ptrTypeNode;
class sequenceNode;
class invokeNode;
class invokeFuncPtrNode;
class fieldAccessNode;
class callNode;
class localDeclNode;
class varRefNode;
class assignmentNode;
class bopNode;
class indexNode;
class ifNode;
class loopIntrinsicNode;
class forLoopNode;
class loopStartNode;
class loopBreakNode;
class loopEndNode;
class stringLiteralNode;
class boolLiteralNode;
class intLiteralNode;
class structLiteralNode;
class genericNode;
class constraintNode;
class instantiateNode;

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
   virtual void visit(constNode& n) = 0;
   virtual void visit(funcNode& n) = 0;
   virtual void visit(intrinsicNode& n) = 0;
   virtual void visit(argNode& n) = 0;
   virtual void visit(typeNode& n) = 0;
   virtual void visit(strTypeNode& n) = 0;
   virtual void visit(boolTypeNode& n) = 0;
   virtual void visit(intTypeNode& n) = 0;
   virtual void visit(arrayTypeNode& n) = 0;
   virtual void visit(voidTypeNode& n) = 0;
   virtual void visit(userTypeNode& n) = 0;
   virtual void visit(ptrTypeNode& n) = 0;
   virtual void visit(sequenceNode& n) = 0;
   virtual void visit(invokeNode& n) = 0;
   virtual void visit(invokeFuncPtrNode& n) = 0;
   virtual void visit(fieldAccessNode& n) = 0;
   virtual void visit(callNode& n) = 0;
   virtual void visit(localDeclNode& n) = 0;
   virtual void visit(varRefNode& n) = 0;
   virtual void visit(assignmentNode& n) = 0;
   virtual void visit(bopNode& n) = 0;
   virtual void visit(indexNode& n) = 0;
   virtual void visit(ifNode& n) = 0;
   virtual void visit(loopIntrinsicNode& n) = 0;
   virtual void visit(forLoopNode& n) = 0;
   virtual void visit(loopStartNode& n) = 0;
   virtual void visit(loopBreakNode& n) = 0;
   virtual void visit(loopEndNode& n) = 0;
   virtual void visit(stringLiteralNode& n) = 0;
   virtual void visit(boolLiteralNode& n) = 0;
   virtual void visit(intLiteralNode& n) = 0;
   virtual void visit(structLiteralNode& n) = 0;
   virtual void visit(genericNode& n) = 0;
   virtual void visit(constraintNode& n) = 0;
   virtual void visit(instantiateNode& n) = 0;

protected:
   virtual void _implementLanguage() = 0;

   void visitChildren(node& n);

   template<class T>
   void unexpected(T& n)
   {
      std::stringstream stream;
      stream << "node type " << typeid(T).name() << " was unexpected";
      cdwTHROW(stream.str());
   }
};

namespace nodeFlags {
   enum {
      kVirtual             = 1 << 0,
      kOverride            = 1 << 1,
      kAbstract            = 1 << 2,
      kDynDispatchMask     = (kVirtual | kOverride),

      kStatic              = 1 << 3,
      kInterface           = 1 << 4,

      kPublic              = 1 << 5,
      kProtected           = 1 << 6,
      kPrivate             = 1 << 7,
      kAccessSpecifierMask = (kPublic | kProtected | kPrivate),

      kAddressableForWrite = 1 << 8, // used in codegen
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
   void injectAbove(node& n);
   void appendChild(node& n);
   void insertChild(size_t i, node& n);
   void insertChildBefore(node& noob, node& antecedent);
   void insertChildAfter(node& noob, node& antecedent);
   node *replaceChild(node& old, node& nu); // caller responsible for delete
   void removeChild(node& n);

   std::vector<node*>& getChildren() { return m_children; }
   node *lastChild();

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

   // ------ template helpers

   template<class T>
   T& getAncestor()
   {
      T *p = dynamic_cast<T*>(m_pParent);
      if(p)
         return *p;
      if(m_pParent)
         return m_pParent->getAncestor<T>();
      else
         cdwTHROW("can't find ancestor");
   }

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
         cdwTHROW("expected single child");
      return **candidates.begin();
   }

   template<class T>
   std::vector<T*> filterChildren(std::function<bool(T&)> filter = [](T& n){ return true; })
   {
      std::vector<T*> rval;

      for(auto it=m_children.begin();it!=m_children.end();++it)
      {
         auto candidate = dynamic_cast<T*>(*it);
         if(candidate && filter(*candidate))
            rval.push_back(candidate);
      }

      return rval;
   }

   template<class T>
   T& filterSoleChild(std::function<bool(T&)> filter = [](T& n){ return true; })
   {
      std::vector<T*> candidates = filterChildren<T>(filter);
      if(candidates.size() != 1)
         cdwTHROW("expected single child");
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
      auto *p = dynamic_cast<T*>(&dest);
      if(!p)
         cdwTHROW(
            std::string("link expected node type ") + typeid(T).name());
      m_pRefee = &dest;
   }

   T *getRefee() { return dynamic_cast<T*>(m_pRefee); }
};

// ----------------------- interfaces -----------------------

// anything that can be referred to by a varRef: e.g. fields, methods, functions, locals,
// arguments, globals, etc.
class iVarSourceNode {
public:
   virtual const std::string& getNameForVarRefee() const = 0;
};

// anything callable via an invoke (i.e. methods, functions)
class iInvokeTargetNode {
public:
   virtual bool isDynDispatch() const = 0;
   virtual const std::string& getShortName() const = 0;
   virtual cmn::node& getNode() = 0;
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
   //std::list<std::string> searchPaths;
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

class memberNode : public node, public iVarSourceNode {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual const std::string& getNameForVarRefee() const { return name; }
};

class methodNode : public memberNode, public iInvokeTargetNode {
public:
   link<methodNode> baseImpl;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual bool isDynDispatch() const { return flags & nodeFlags::kDynDispatchMask; }
   virtual const std::string& getShortName() const { return baseImpl.ref; }
   virtual cmn::node& getNode() { return *this; }
};

class fieldNode : public memberNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class constNode : public node, public iVarSourceNode {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual const std::string& getNameForVarRefee() const { return name; }
};

class funcNode : public node, public iVarSourceNode, public iInvokeTargetNode {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual const std::string& getNameForVarRefee() const { return name; }
   virtual bool isDynDispatch() const { return false; }
   virtual const std::string& getShortName() const { return name; }
   virtual cmn::node& getNode() { return *this; }
};

class intrinsicNode : public funcNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class argNode : public node, public iVarSourceNode {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual const std::string& getNameForVarRefee() const { return name; }
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

class boolTypeNode : public typeNode {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class intTypeNode : public typeNode {
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
   link<methodNode> proto;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class invokeFuncPtrNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class fieldAccessNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class callNode : public node {
public:
   link<iInvokeTargetNode> pTarget;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class localDeclNode : public node, public iVarSourceNode {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
   virtual const std::string& getNameForVarRefee() const { return name; }
};

class varRefNode : public node {
public:
   link<iVarSourceNode> pSrc;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- operators -----------------------

class assignmentNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class bopNode : public node {
public:
   std::string op;
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class indexNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- flow control -----------------------

class ifNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class loopIntrinsicNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class forLoopNode : public node {
public:
   forLoopNode() : scoped(false), decomposed(false) {}

   std::string name;
   bool scoped;
   bool decomposed;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class loopStartNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class loopBreakNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class loopEndNode : public node {
public:
   std::string name;

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
   std::string lexeme;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class structLiteralNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

// ----------------------- generics -----------------------

class genericNode : public node {
public:
   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class constraintNode : public node {
public:
   std::string name;

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }
};

class instantiateNode : public node {
public:
   instantiateNode() : impled(false) {}

   bool impled;
   std::string text;

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
   virtual void visit(constNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(funcNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(intrinsicNode& n) { visit(static_cast<funcNode&>(n)); }
   virtual void visit(argNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(typeNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(strTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(boolTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(intTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(arrayTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(voidTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(userTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(ptrTypeNode& n) { visit(static_cast<typeNode&>(n)); }
   virtual void visit(sequenceNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(invokeNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(invokeFuncPtrNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(fieldAccessNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(callNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(localDeclNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(varRefNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(assignmentNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(bopNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(indexNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(ifNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(loopIntrinsicNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(forLoopNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(loopStartNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(loopBreakNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(loopEndNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(stringLiteralNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(boolLiteralNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(intLiteralNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(structLiteralNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(genericNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(constraintNode& n) { visit(static_cast<node&>(n)); }
   virtual void visit(instantiateNode& n) { visit(static_cast<node&>(n)); }
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
   virtual void visit(constNode& n);
   virtual void visit(funcNode& n);
   virtual void visit(intrinsicNode& n);
   virtual void visit(argNode& n);
   virtual void visit(strTypeNode& n);
   virtual void visit(boolTypeNode& n);
   virtual void visit(intTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(ptrTypeNode& n);
   virtual void visit(sequenceNode& n);
   virtual void visit(invokeNode& n);
   virtual void visit(invokeFuncPtrNode& n);
   virtual void visit(fieldAccessNode& n);
   virtual void visit(callNode& n);
   virtual void visit(localDeclNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(assignmentNode& n);
   virtual void visit(bopNode& n);
   virtual void visit(indexNode& n);
   virtual void visit(ifNode& n);
   virtual void visit(loopIntrinsicNode& n);
   virtual void visit(forLoopNode& n);
   virtual void visit(loopStartNode& n);
   virtual void visit(loopBreakNode& n);
   virtual void visit(loopEndNode& n);
   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);
   virtual void visit(structLiteralNode& n);
   virtual void visit(genericNode& n);
   virtual void visit(constraintNode& n);
   virtual void visit(instantiateNode& n);

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

class astFormatter : public hNodeVisitor {
public:
   explicit astFormatter(outStream& s) : m_s(s) {}

   virtual void visit(node& n);
   virtual void visit(araceliProjectNode& n);
   virtual void visit(liamProjectNode& n);
   virtual void visit(scopeNode& n);
   virtual void visit(fileNode& n);
   virtual void visit(fileRefNode& n);
   virtual void visit(classNode& n);
   virtual void visit(memberNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(fieldNode& n);
   virtual void visit(constNode& n);
   virtual void visit(funcNode& n);
   virtual void visit(intrinsicNode& n);
   virtual void visit(argNode& n);
   virtual void visit(typeNode& n);
   virtual void visit(strTypeNode& n);
   virtual void visit(boolTypeNode& n);
   virtual void visit(intTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(ptrTypeNode& n);
   virtual void visit(sequenceNode& n);
   virtual void visit(invokeNode& n);
   virtual void visit(invokeFuncPtrNode& n);
   virtual void visit(fieldAccessNode& n);
   virtual void visit(callNode& n);
   virtual void visit(localDeclNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(assignmentNode& n);
   virtual void visit(bopNode& n);
   virtual void visit(indexNode& n);
   virtual void visit(ifNode& n);
   virtual void visit(loopIntrinsicNode& n);
   virtual void visit(forLoopNode& n);
   virtual void visit(loopStartNode& n);
   virtual void visit(loopBreakNode& n);
   virtual void visit(loopEndNode& n);
   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);
   virtual void visit(structLiteralNode& n);
   virtual void visit(genericNode& n);
   virtual void visit(constraintNode& n);
   virtual void visit(instantiateNode& n);

   virtual void _implementLanguage() {} // all

private:
   outStream& m_s;
};

template<class T = hNodeVisitor>
class araceliVisitor : public T {
public:
   virtual void visit(liamProjectNode& n) { T::unexpected(n); }
   virtual void visit(fileRefNode& n) { T::unexpected(n); }

   virtual void _implementLanguage() {} // araceli
};

template<class T = hNodeVisitor>
class liamVisitor : public T {
public:
   virtual void visit(araceliProjectNode& n) { T::unexpected(n); }
   virtual void visit(scopeNode& n) { T::unexpected(n); }
   virtual void visit(methodNode& n) { T::unexpected(n); }
   virtual void visit(invokeNode& n) { T::unexpected(n); }

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

class creatingNodeVisitor : public iNodeVisitor {
public:
   virtual void visit(node&) { inst.reset(new node()); }
   virtual void visit(araceliProjectNode&) { inst.reset(new araceliProjectNode()); }
   virtual void visit(liamProjectNode&) { inst.reset(new liamProjectNode()); }
   virtual void visit(scopeNode&) { inst.reset(new scopeNode()); }
   virtual void visit(fileNode&) { inst.reset(new fileNode()); }
   virtual void visit(fileRefNode&) { inst.reset(new fileRefNode()); }
   virtual void visit(classNode&) { inst.reset(new classNode()); }
   virtual void visit(memberNode&) { inst.reset(new memberNode()); }
   virtual void visit(methodNode&) { inst.reset(new methodNode()); }
   virtual void visit(fieldNode&) { inst.reset(new fieldNode()); }
   virtual void visit(constNode&) { inst.reset(new constNode()); }
   virtual void visit(funcNode&) { inst.reset(new funcNode()); }
   virtual void visit(intrinsicNode&) { inst.reset(new intrinsicNode()); }
   virtual void visit(argNode&) { inst.reset(new argNode()); }
   virtual void visit(typeNode&) { inst.reset(new typeNode()); }
   virtual void visit(strTypeNode&) { inst.reset(new strTypeNode()); }
   virtual void visit(boolTypeNode&) { inst.reset(new boolTypeNode()); }
   virtual void visit(intTypeNode&) { inst.reset(new intTypeNode()); }
   virtual void visit(arrayTypeNode&) { inst.reset(new arrayTypeNode()); }
   virtual void visit(voidTypeNode&) { inst.reset(new voidTypeNode()); }
   virtual void visit(userTypeNode&) { inst.reset(new userTypeNode()); }
   virtual void visit(ptrTypeNode&) { inst.reset(new ptrTypeNode()); }
   virtual void visit(sequenceNode&) { inst.reset(new sequenceNode()); }
   virtual void visit(invokeNode&) { inst.reset(new invokeNode()); }
   virtual void visit(invokeFuncPtrNode&) { inst.reset(new invokeFuncPtrNode()); }
   virtual void visit(fieldAccessNode&) { inst.reset(new fieldAccessNode()); }
   virtual void visit(callNode&) { inst.reset(new callNode()); }
   virtual void visit(localDeclNode&) { inst.reset(new localDeclNode()); }
   virtual void visit(varRefNode&) { inst.reset(new varRefNode()); }
   virtual void visit(assignmentNode&) { inst.reset(new assignmentNode()); }
   virtual void visit(bopNode&) { inst.reset(new bopNode()); }
   virtual void visit(indexNode&) { inst.reset(new indexNode()); }
   virtual void visit(ifNode&) { inst.reset(new ifNode()); }
   virtual void visit(loopIntrinsicNode&) { inst.reset(new loopIntrinsicNode()); }
   virtual void visit(forLoopNode&) { inst.reset(new forLoopNode()); }
   virtual void visit(loopStartNode&) { inst.reset(new loopStartNode()); }
   virtual void visit(loopBreakNode&) { inst.reset(new loopBreakNode()); }
   virtual void visit(loopEndNode&) { inst.reset(new loopEndNode()); }
   virtual void visit(stringLiteralNode&) { inst.reset(new stringLiteralNode()); }
   virtual void visit(boolLiteralNode&) { inst.reset(new boolLiteralNode()); }
   virtual void visit(intLiteralNode&) { inst.reset(new intLiteralNode()); }
   virtual void visit(structLiteralNode&) { inst.reset(new structLiteralNode()); }
   virtual void visit(genericNode&) { inst.reset(new genericNode()); }
   virtual void visit(constraintNode&) { inst.reset(new constraintNode()); }
   virtual void visit(instantiateNode&) { inst.reset(new instantiateNode()); }

   virtual void _implementLanguage() {} // all

   std::unique_ptr<cmn::node> inst;
};

// Out of laziness, I implement these only when needed
class cloningNodeVisitor : public hNodeVisitor {
public:
   explicit cloningNodeVisitor(node& n) : m_n(n) {}

   virtual void visit(node& n);
   virtual void visit(araceliProjectNode& n) { unexpected(n); }
   virtual void visit(liamProjectNode& n) { unexpected(n); }
   virtual void visit(scopeNode& n) { unexpected(n); }
   virtual void visit(fileNode& n) { unexpected(n); }
   virtual void visit(fileRefNode& n) { unexpected(n); }
   virtual void visit(classNode& n);
   virtual void visit(memberNode& n);
   virtual void visit(methodNode& n);
   //virtual void visit(fieldNode& n);
   virtual void visit(constNode& n) { unexpected(n); }
   virtual void visit(funcNode& n) { unexpected(n); }
   //virtual void visit(intrinsicNode& n) { unexpected(n); }
   virtual void visit(argNode& n);
   //virtual void visit(typeNode& n);
   //virtual void visit(strTypeNode& n);
   //virtual void visit(boolTypeNode& n);
   //virtual void visit(intTypeNode& n);
   //virtual void visit(arrayTypeNode& n);
   //virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   //virtual void visit(ptrTypeNode& n);
   //virtual void visit(sequenceNode& n) { unexpected(n); }
   virtual void visit(invokeNode& n) { unexpected(n); }
   //virtual void visit(invokeFuncPtrNode& n) { unexpected(n); }
   virtual void visit(fieldAccessNode& n) { unexpected(n); }
   virtual void visit(callNode& n);
   virtual void visit(localDeclNode& n) { unexpected(n); }
   virtual void visit(varRefNode& n);
   //virtual void visit(assignmentNode& n) { unexpected(n); }
   virtual void visit(bopNode& n) { unexpected(n); }
   //virtual void visit(indexNode& n) { unexpected(n); }
   //virtual void visit(ifNode& n) { unexpected(n); }
   virtual void visit(stringLiteralNode& n) { unexpected(n); }
   virtual void visit(boolLiteralNode& n) { unexpected(n); }
   virtual void visit(intLiteralNode& n) { unexpected(n); }
   //virtual void visit(structLiteralNode& n) { unexpected(n); }
   //virtual void visit(genericNode& n) { unexpected(n); }
   virtual void visit(constraintNode& n) { unexpected(n); }
   virtual void visit(instantiateNode& n) { unexpected(n); }

   virtual void _implementLanguage() {} // all

private:
   template<class T> T& as() { return dynamic_cast<T&>(m_n); }

   node& m_n;
};

node& cloneTree(node& n);

// ----------------------- transform aids -----------------------

class treeWriter {
public:
   explicit treeWriter(cmn::node& n) : m_pNode(&n) {}

   template<class T>
   treeWriter& append(std::function<void(T&)> initializer = [](T& n){})
   {
      std::unique_ptr<T> pNode(new T());
      initializer(*pNode.get());
      m_pNode->appendChild(*pNode.release());
      m_pNode = m_pNode->lastChild();
      return *this;
   }

   template<class T>
   treeWriter& backTo(std::function<bool(const T&)> pred = [](const T&){ return true; })
   {
      while(true)
      {
         if(T* pDerived = dynamic_cast<T*>(m_pNode))
            if(pred(*pDerived))
               return *this;

         parent();
      }
   }

   treeWriter& parent() { m_pNode = m_pNode->getParent(); return *this; }

   void set(cmn::node& n);
   cmn::node& get() { return *m_pNode; }

private:
   cmn::node *m_pNode;
};

} // namespace cmn

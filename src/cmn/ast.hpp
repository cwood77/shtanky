#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace cmn {

class node;

class iNodeVisitor {
public:
   virtual ~iNodeVisitor() {}
   virtual void visit(node& n) = 0;
};

class sourceLocation {
public:
   sourceLocation() : lineNumber(0) {}

   std::string fileName;
   unsigned long lineNumber;
};

class node {
public:
   node() : m_pParent(NULL) {}
   virtual ~node();

   sourceLocation origin;
   std::set<std::string> attributes;

   void appendChild(node& n);

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

private:
   node *m_pParent;
   std::vector<node*> m_children;

   node(const node&);
   node& operator=(const node&);
};

class lexorBase;

class nodeFactory {
public:
   explicit nodeFactory(lexorBase& l) : m_l(l) {}

   void deferAttribute(const std::string& attr);

   template<class T>
   T *create()
   {
      std::unique_ptr<T> pNode(new T());
      // set origin
      // set attributes
      return pNode.release();
   }

   template<class T>
   T& appendNewChild(node& parent);

private:
   lexorBase& m_l;
};

} // namespace cmn

#pragma once

#include <list>
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

   void visitChildren(node& n);
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
   std::vector<node*>& getChildren() { return m_children; }

   virtual void acceptVisitor(iNodeVisitor& v) { v.visit(*this); }

private:
   node *m_pParent;
   std::vector<node*> m_children;

   node(const node&);
   node& operator=(const node&);
};

} // namespace cmn

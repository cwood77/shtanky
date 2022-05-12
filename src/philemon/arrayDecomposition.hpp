#pragma once
#include "../cmn/ast.hpp"

namespace philemon {

class arrayTypeNameBuilder : public cmn::hNodeVisitor {
public:
   arrayTypeNameBuilder() : pLastNodeSeen(NULL) {}

   virtual void visit(cmn::node& n) { pLastNodeSeen = &n; }
   virtual void visit(cmn::strTypeNode& n) { unexpected(n); }
   virtual void visit(cmn::boolTypeNode& n);
   virtual void visit(cmn::intTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::ptrTypeNode& n);
   virtual void _implementLanguage() {} // all

   std::stringstream name;
   cmn::node *pLastNodeSeen;
};

class arrayDecomposition : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void _implementLanguage() {} // all

   void run(cmn::node& r) { replaceNodes(); injectInstances(r); }

private:
   void replaceNodes();
   void injectInstances(cmn::node& r);

   std::set<cmn::arrayTypeNode*> m_nodes;
   std::set<std::string> m_instancesNeeded;
};

} // namespace philemon

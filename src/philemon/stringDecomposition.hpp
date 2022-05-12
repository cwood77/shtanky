#pragma once
#include "../cmn/ast.hpp"

namespace philemon {

class stringDecomposition : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::strTypeNode& n);

   virtual void _implementLanguage() {} // all

   void run();

private:
   std::list<cmn::strTypeNode*> m_nodes;
};

} // namespace philemon

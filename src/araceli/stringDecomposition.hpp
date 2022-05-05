#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class stringDecomposition : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::strTypeNode& n);

   void run();

private:
   std::list<cmn::strTypeNode*> m_nodes;
};

} // namespace araceli

#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class arrayDecomposition : public hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::arrayTypeNode& n);

   void run();

private:
   std::list<cmn::arrayTypeNode*> m_nodes;
};

} // namespace araceli

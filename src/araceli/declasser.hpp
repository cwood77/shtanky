#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class declasser : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::varRefNode& n);
};

} // namespace araceli

#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

// - add 'self' parameters to methods
// - scope field usage with fieldAccessNodes
// - change invokes to invoke func ptrs
class selfDecomposition : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::methodNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::varRefNode& n);
};

} // namespace araceli

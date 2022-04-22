#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class ctorDtorGenerator : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);
};

} // namespace araceli

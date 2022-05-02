#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

// inject the base class "object" as the root of all things
class objectBaser : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);
};

} // namespace araceli


#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

// for varRefs
// - if these refer to fields, scope then with 'self'

// for methods: two things
// - inject self args
// - change to invokeFuncPtr node with vtable

class declasser : public cmn::araceliVisitor<> {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }

   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::varRefNode& n);
};

} // namespace araceli

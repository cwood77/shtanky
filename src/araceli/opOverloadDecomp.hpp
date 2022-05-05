#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

class opOverloadDecomp : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::assignmentNode& n);
   virtual void visit(cmn::indexNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace araceli

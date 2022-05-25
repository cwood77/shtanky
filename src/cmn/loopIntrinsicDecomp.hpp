#pragma once
#include "ast.hpp"

namespace cmn {

class loopIntrinsicDecomp : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(callNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace cmn

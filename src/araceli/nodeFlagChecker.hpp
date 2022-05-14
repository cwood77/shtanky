#pragma once
#include "../cmn/ast.hpp"

namespace araceli {

// issue a bunch of checks, and set default access specifiers on some things
class nodeFlagChecker : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n);
   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::memberNode& n);
   virtual void visit(cmn::methodNode& n);
   virtual void visit(cmn::fieldNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace araceli

#pragma once
#include "../cmn/ast.hpp"

namespace salome {

// implement implied flags on interfaces
// and sanity check them
class intfTransform : public cmn::hNodeVisitor {
public:
   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);

   virtual void _implementLanguage() {} // all
};

} // namespace salome

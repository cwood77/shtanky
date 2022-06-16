#pragma once
#include "../cmn/ast.hpp"
#include "../cmn/global.hpp"

namespace salome {

class intfTransformState {
public:
   std::set<cmn::classNode*> handled;
};

// implement implied flags on interfaces
// and sanity check them
class intfTransform : public cmn::hNodeVisitor {
public:
   static cmn::timedGlobal<intfTransformState> gState;

   intfTransform() : nChanges(0) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::classNode& n);

   virtual void _implementLanguage() {} // all

   size_t nChanges;
};

} // namespace salome

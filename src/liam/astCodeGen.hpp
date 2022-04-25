#pragma once
#include "../cmn/ast.hpp"
#include "../cmn/unique.hpp"
#include <ostream>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirBuilder;

// data uses weirdo syntax like .data, "foo" <b> 0 so it's special
class dataFormatter : public cmn::liamVisitor<> {
public:
   explicit dataFormatter(std::ostream& o) : m_o(o) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);

private:
   std::ostream& m_o;
};

class astCodeGen : public cmn::liamVisitor<> {
public:
   astCodeGen(lirBuilder& b, cmn::tgt::iTargetInfo& t)
   : m_b(b), m_t(t) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::constNode& n);
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::localDeclNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::assignmentNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   lirBuilder& m_b;
   cmn::tgt::iTargetInfo& m_t;
   cmn::uniquifier m_u;
};

} // namespace liam

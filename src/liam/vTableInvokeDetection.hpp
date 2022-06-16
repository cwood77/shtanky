#pragma once
#include "../cmn/ast.hpp"

namespace liam {

class vTableInvokeDetector : public cmn::hNodeVisitor {
public:
   vTableInvokeDetector(cmn::tgt::iTargetInfo& tgt) : m_t(tgt) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::invokeFuncPtrNode& n);

   virtual void _implementLanguage() {} // all

private:
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

#pragma once
#include "ast.hpp"

namespace cmn {

class loopIntrinsicDecomp : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(callNode& n);

   virtual void _implementLanguage() {} // all
};

class loopVarRefFixup : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(varRefNode& n);
   virtual void visit(forLoopNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::set<std::string> m_loopInstances;
};

class loopInstDropper : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(loopIntrinsicNode& n);
   virtual void visit(forLoopNode& n);

   virtual void _implementLanguage() {} // all
};

class markLoopPreDecomposed : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(forLoopNode& n)
   { n.decomposed = true; hNodeVisitor::visit(n); }

   virtual void _implementLanguage() {} // all
};

} // namespace cmn

#pragma once
#include "ast.hpp"

// see liam/loopTransforms.hpp for a theory of operation

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
   virtual void visit(loopBaseNode& n);

   virtual void _implementLanguage() {} // all

private:
   std::set<std::string> m_loopInstances;
};

class loopInstDropper : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(loopIntrinsicNode& n);
   virtual void visit(forLoopNode& n) { handleLoop(n,".sht.core.forLoopInst"); }
   virtual void visit(whileLoopNode& n) { handleLoop(n,".sht.core.whileLoopInst"); }

   virtual void _implementLanguage() {} // all

private:
   void handleLoop(loopBaseNode& n, const std::string& instTypeName);
};

// not redundant with above b/c used in the case that a language doesn't decompose loops
// (e.g. philemon)
class markLoopPreDecomposed : public hNodeVisitor {
public:
   virtual void visit(node& n) { visitChildren(n); }
   virtual void visit(forLoopNode& n)
   { n.decomposed = true; hNodeVisitor::visit(n); }

   virtual void _implementLanguage() {} // all
};

} // namespace cmn

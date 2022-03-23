#pragma once
#include "ast.hpp"

namespace cmn { class outBundle; }

namespace araceli {

class codeGen : public hNodeVisitor {
public:
   explicit codeGen(cmn::outBundle& out) : m_pActiveFile(NULL), m_out(out) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(fileNode& n);
   virtual void visit(classNode& n);
#if 0
   virtual void visit(memberNode& n);
   virtual void visit(methodNode& n);
   virtual void visit(argNode& n);
   virtual void visit(typeNode& n);
   virtual void visit(strTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);
   virtual void visit(fieldNode& n);
   virtual void visit(sequenceNode& n);
   virtual void visit(invokeNode& n);
   virtual void visit(callNode& n);
   virtual void visit(varRefNode& n);
   virtual void visit(stringLiteralNode& n);
   virtual void visit(boolLiteralNode& n);
   virtual void visit(intLiteralNode& n);
#endif

private:
   fileNode *m_pActiveFile;
   cmn::outBundle& m_out;
};

} // namespace araceli

#pragma once
#include "ast.hpp"

namespace cmn { class outBundle; }

namespace araceli {

class liamTypeWriter : public hNodeVisitor {
public:
   explicit liamTypeWriter(std::ostream& o) : m_o(o) {}

   virtual void visit(cmn::node& n) {}
   virtual void visit(typeNode& n);
   virtual void visit(strTypeNode& n);
   virtual void visit(arrayTypeNode& n);
   virtual void visit(voidTypeNode& n);
   virtual void visit(userTypeNode& n);

private:
   std::ostream& m_o;
};

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
   void generateClassVTable(classNode& n, std::ostream& header, std::string& vname);
   void generateClassType(classNode& n, std::ostream& header, const std::string& vname);
   void generateClassPrototypes(classNode& n, std::ostream& header);

   fileNode *m_pActiveFile;
   cmn::outBundle& m_out;
};

} // namespace araceli

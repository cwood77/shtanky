#pragma once
#include "../cmn/ast.hpp"

namespace cmn { class outBundle; }

namespace araceli {

class liamTypeWriter : public cmn::araceliVisitor<> {
public:
   explicit liamTypeWriter(std::ostream& o) : m_o(o) {}

   virtual void visit(cmn::node& n) {}
   virtual void visit(cmn::typeNode& n);
   virtual void visit(cmn::strTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);

private:
   std::ostream& m_o;
};

class codeGen : public cmn::araceliVisitor<> {
public:
   explicit codeGen(cmn::outBundle& out) : m_pActiveFile(NULL), m_out(out) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::fileNode& n);
   virtual void visit(cmn::classNode& n);
#if 0
   virtual void visit(cmn::memberNode& n);
   virtual void visit(cmn::methodNode& n);
   virtual void visit(cmn::argNode& n);
   virtual void visit(cmn::typeNode& n);
   virtual void visit(cmn::strTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::fieldNode& n);
   virtual void visit(cmn::sequenceNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);
#endif

private:
   void generateClassVTable(cmn::classNode& n, std::ostream& header, std::string& vname);
   void generateClassType(cmn::classNode& n, std::ostream& header, const std::string& vname);
   void generateClassPrototypes(cmn::classNode& n, std::ostream& header);

   cmn::fileNode *m_pActiveFile;
   cmn::outBundle& m_out;
};

} // namespace araceli

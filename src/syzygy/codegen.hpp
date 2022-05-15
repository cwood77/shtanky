#pragma once
#include "../cmn/ast.hpp"

// this code generator dumps an Araceli AST exactly as-is

namespace cmn { class outBundle; }
namespace cmn { class outStream; }

namespace syzygy {

class codegen : public cmn::hNodeVisitor {
public:
   codegen(cmn::outBundle& b, const std::string& infix, bool addOrReplaceExt)
   : m_b(b), m_infix(infix), m_addExt(addOrReplaceExt) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::liamProjectNode& n) { unexpected(n); }
   virtual void visit(cmn::fileNode& n);
   virtual void visit(cmn::fileRefNode& n) { unexpected(n); }
   virtual void visit(cmn::classNode& n);
   virtual void visit(cmn::methodNode& n);
   virtual void visit(cmn::fieldNode& n);
   virtual void visit(cmn::constNode& n);
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::intrinsicNode& n) { }
   virtual void visit(cmn::argNode& n);
   virtual void visit(cmn::strTypeNode& n);
   virtual void visit(cmn::arrayTypeNode& n);
   virtual void visit(cmn::voidTypeNode& n);
   virtual void visit(cmn::userTypeNode& n);
   virtual void visit(cmn::ptrTypeNode& n);
   virtual void visit(cmn::sequenceNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::localDeclNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::assignmentNode& n);
   virtual void visit(cmn::bopNode& n);
   virtual void visit(cmn::indexNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n) { unexpected(n); }
   virtual void visit(cmn::intLiteralNode& n);
   virtual void visit(cmn::structLiteralNode& n);
   virtual void visit(cmn::genericNode& n);
   virtual void visit(cmn::instantiateNode& n);

   virtual void _implementLanguage() {} // all

private:
   cmn::outStream& getOutStream();
   void visitFunctionLikeThing(cmn::node& n, const std::string& name);
   void visitNameTypePair(cmn::node& n, const std::string& name);
   void visitCallLikeThingAtParens(cmn::node& n, size_t startIdx);
   std::string getNameForClassFlags(cmn::classNode& n);

   cmn::outBundle& m_b;
   const std::string m_infix;
   const bool m_addExt;

   cmn::fileNode *m_pCurrFile;
   cmn::outStream *m_pCurrStream;
};

} // namespace syzygy

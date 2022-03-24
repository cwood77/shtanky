#pragma once
#include "../cmn/ast.hpp"
#include <map>

namespace liam {

class lirArg;
class lirInstr;

class astCodeGen : public cmn::liamVisitor<> {
public:
   astCodeGen() : m_pHead(NULL) {}
   ~astCodeGen();

   virtual void visit(cmn::node& n) {}
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   std::map<cmn::node*,lirArg*> m_args;

   lirInstr *m_pHead;
};

} // namespace liam

#pragma once
#include "../cmn/ast.hpp"
#include <map>

namespace liam {

class lirArg;
class lirStreams;

class astCodeGen : public cmn::liamVisitor<> {
public:
   explicit astCodeGen(lirStreams& l) : m_lir(l) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::invokeNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   lirStreams& m_lir;
   std::string m_currFunc;
};

} // namespace liam

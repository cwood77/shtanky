#pragma once
#include "../cmn/ast.hpp"
#include <map>

namespace liam {

class lirArg;
class lirStreams;
class varGenerator;

// requires #include handling
// requires varRef to user type linking
// requires user type (i.e. class) size calculation

class astCodeGen : public cmn::liamVisitor<> {
public:
   astCodeGen(lirStreams& l, varGenerator& v) : m_lir(l), m_vGen(v) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   lirStreams& m_lir;
   varGenerator& m_vGen;
   std::string m_currFunc;
};

} // namespace liam

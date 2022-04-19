#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include <ostream>
#include <vector>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirArg;
class lirGenerator;
class lirInstr;
class lirStreams;
class varGenerator;

// data uses weirdo syntax like .data, "foo" <b> 0 to it's special
class dataFormatter : public cmn::liamVisitor<> {
public:
   explicit dataFormatter(std::ostream& o) : m_o(o) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::stringLiteralNode& n);

private:
   std::ostream& m_o;
};

class astCodeGen : public cmn::liamVisitor<> {
public:
   astCodeGen(lirStreams& l, varGenerator& v, cmn::tgt::iTargetInfo& t)
   : m_lir(l), m_vGen(v), m_t(t) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::constNode& n);
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::sequenceNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   class callGenInfo {
   public:
      callGenInfo();

      lirInstr *pPreInstr;
      lirInstr *pCallInstr;
      lirInstr *pPostInstr;

      lirArg *pRvalArg;

      std::vector<size_t> argRealSizes;
   };

   void genCallPreChild(cmn::node& n, callGenInfo& i);
   void genCallPostChild(cmn::node& n, callGenInfo& i);

   lirStreams& m_lir;
   varGenerator& m_vGen;
   cmn::tgt::iTargetInfo& m_t;
   std::string m_currFunc;
};

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//
// NEW LIR API
//
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// transforms I'll need
// segment instr generation
// local gatherer
// exit function generation
// call instr arg ellision?

class lirGenVisitor : public cmn::liamVisitor<> {
public:
   lirGenVisitor(lirGenerator& l, cmn::tgt::iTargetInfo& t) : m_lGen(l), m_t(t) {}

   virtual void visit(cmn::node& n) { visitChildren(n); }
   virtual void visit(cmn::constNode& n);
   virtual void visit(cmn::funcNode& n);
   virtual void visit(cmn::invokeFuncPtrNode& n);
   virtual void visit(cmn::localDeclNode& n);
   virtual void visit(cmn::fieldAccessNode& n);
   virtual void visit(cmn::callNode& n);
   virtual void visit(cmn::varRefNode& n);
   virtual void visit(cmn::stringLiteralNode& n);
   virtual void visit(cmn::boolLiteralNode& n);
   virtual void visit(cmn::intLiteralNode& n);

private:
   lirGenerator& m_lGen;
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

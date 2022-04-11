#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include <vector>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirArg;
class lirInstr;
class lirStreams;
class varGenerator;

class astCodeGen : public cmn::liamVisitor<> {
public:
   astCodeGen(lirStreams& l, varGenerator& v, cmn::tgt::iTargetInfo& t)
   : m_lir(l), m_vGen(v), m_t(t) {}

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

} // namespace liam

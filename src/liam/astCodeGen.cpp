#include "../cmn/target.hpp"
#include "astCodeGen.hpp"
#include "lir.hpp"

namespace liam {

void astCodeGen::visit(cmn::funcNode& n)
{
   // none?
}

void astCodeGen::visit(cmn::invokeNode& n)
{
}

void astCodeGen::visit(cmn::callNode& n)
{
#if 0
   auto pInstr = lirInstr::append(m_pHead);
//   pInstr->instrId = cmn::tgt::kCall;

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      lirArg *pArg = m_args[*it];
      if(pArg)
         pInstr->addArg(*pArg);
   }
#endif
}

void astCodeGen::visit(cmn::varRefNode& n)
{
   // publish an arg
}

void astCodeGen::visit(cmn::stringLiteralNode& n)
{
   /*
   m_args[&n] = new lirArgConst(n.value,0);
   */
}

void astCodeGen::visit(cmn::boolLiteralNode& n)
{
   // publish an arg
}

void astCodeGen::visit(cmn::intLiteralNode& n)
{
   // publish an arg
}

} // namespace liam

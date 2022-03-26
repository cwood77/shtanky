#include "../cmn/target.hpp"
#include "astCodeGen.hpp"
#include "lir.hpp"

namespace liam {

void astCodeGen::visit(cmn::funcNode& n)
{
   m_currFunc = n.name;
   auto& stream = m_lir.page[m_currFunc];

   auto args = n.getChildrenOf<cmn::argNode>();
   if(args.size())
   {
      auto& i = lirInstr::append(stream.pTail,cmn::tgt::kDeclParam);
      for(auto it=args.begin();it!=args.end();++it)
      {
         auto& arg = **it;
         stream.createNamedArg(i,arg.name,0); // TODO - calc type size
      }
   }

   n.demandSoleChild<cmn::sequenceNode>().acceptVisitor(*this);
}

void astCodeGen::visit(cmn::invokeFuncPtrNode& n)
{
   auto& stream = m_lir.page[m_currFunc];
   auto& call = lirInstr::append(stream.pTail,cmn::tgt::kCall);

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      (*it)->acceptVisitor(*this);
      stream.claimArg(**it,call);
   }

   stream.createTemporary<lirArgVar>(n,call,"",0);
}

void astCodeGen::visit(cmn::fieldAccessNode& n)
{
   auto& stream = m_lir.page[m_currFunc];
   auto& mov = lirInstr::append(stream.pTail,cmn::tgt::kMov);

   //auto& dest = mov.addArg(*new lirArgVar("",0));
   stream.claimArg(n.demandSoleChild<cmn::node>(),mov);

   stream.createTemporary<lirArgVar>(n,mov,"",0);
}

// TODO this seems wrong... where is the call address passed?
void astCodeGen::visit(cmn::callNode& n)
{
   auto& stream = m_lir.page[m_currFunc];
   auto& call = lirInstr::append(stream.pTail,cmn::tgt::kCall);

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      (*it)->acceptVisitor(*this);
      stream.claimArg(**it,call);
   }

   stream.createTemporary<lirArgVar>(n,call,"",0);
}

void astCodeGen::visit(cmn::varRefNode& n)
{
   m_lir.page[m_currFunc].donate<lirArgVar>(n,n.pDef.ref,0);
}

void astCodeGen::visit(cmn::stringLiteralNode& n)
{
   m_lir.page[m_currFunc].donate<lirArgConst>(n,n.value,0);
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

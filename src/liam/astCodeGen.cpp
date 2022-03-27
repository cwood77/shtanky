#include "../cmn/target.hpp"
#include "astCodeGen.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

void astCodeGen::visit(cmn::funcNode& n)
{
   m_currFunc = n.name;
   auto& stream = m_lir.page[m_currFunc];

   auto args = n.getChildrenOf<cmn::argNode>();
   if(args.size())
   {
      // publish args as variables, if any

      auto& i = lirInstr::append(stream.pTail,cmn::tgt::kDeclParam);

      for(auto it=args.begin();it!=args.end();++it)
      {
         auto& astArg = **it;
         auto& arg = dynamic_cast<lirArgVar&>(i.addArg(*new lirArgVar(astArg.name,0))); // TODO - calc type size
         m_vGen.createNamedVar(i.orderNum,arg);
      }
   }

   n.demandSoleChild<cmn::sequenceNode>().acceptVisitor(*this);
}

void astCodeGen::visit(cmn::invokeFuncPtrNode& n)
{
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      (*it)->acceptVisitor(*this);

   auto& stream = m_lir.page[m_currFunc];
   auto& call = lirInstr::append(stream.pTail,cmn::tgt::kCall);
   auto& rval = call.addArg(*new lirArgVar("rval",0));

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      m_vGen.claimAndAddArgOffWire(call,**it);

   m_vGen.createPrivateVar(call.orderNum,rval,"rval").publishOnWire(n);
}

void astCodeGen::visit(cmn::fieldAccessNode& n)
{
   visitChildren(n);

   auto& stream = m_lir.page[m_currFunc];
   auto& mov = lirInstr::append(stream.pTail,cmn::tgt::kMov);
   auto& dest = mov.addArg(*new lirArgVar("",0));
   dest.addrOf = true;

   m_vGen.claimAndAddArgOffWire(mov,n.demandSoleChild<cmn::node>());

   m_vGen.createPrivateVar(mov.orderNum,dest,"field:%s",n.name.c_str()).publishOnWire(n);
}

// TODO this seems wrong... where is the call address passed?
void astCodeGen::visit(cmn::callNode& n)
{
   throw 3.14; // unimpl
}

void astCodeGen::visit(cmn::varRefNode& n)
{
   m_vGen.createNamedVar(0,*new lirArgVar(n.pDef.ref,0)).donateToWire(n);
}

void astCodeGen::visit(cmn::stringLiteralNode& n)
{
   m_vGen.createPrivateVar(0,*new lirArgConst(n.value,0),"strlit:%s",n.value.c_str())
      .donateToWire(n);
}

void astCodeGen::visit(cmn::boolLiteralNode& n)
{
   throw 3.14; // unimpl
   // publish an arg
}

void astCodeGen::visit(cmn::intLiteralNode& n)
{
   throw 3.14; // unimpl
   // publish an arg
}

} // namespace liam

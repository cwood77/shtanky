#include "../cmn/fmt.hpp"
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

      auto& i = lirInstr::append(
         stream.pTail,
         cmn::tgt::kDeclParam,
         cmn::fmt("func %s",n.name.c_str()));

      for(auto it=args.begin();it!=args.end();++it)
      {
         auto& astArg = **it;
         auto& arg = i.addArg<lirArgVar>(astArg.name,0); // TODO - calc type size
         m_vGen.createNamedVar(i.orderNum,arg);
      }
   }

   n.demandSoleChild<cmn::sequenceNode>().acceptVisitor(*this);
}

void astCodeGen::visit(cmn::invokeFuncPtrNode& n)
{
   auto& stream = m_lir.page[m_currFunc];
   auto& pre = lirInstr::append(
      stream.pTail,
      cmn::tgt::kPreCallStackAlloc,
      "");
   size_t subcallStackSize = m_t.getCallConvention().getShadowSpace();
   std::vector<size_t> argSizes;

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      (*it)->acceptVisitor(*this);

   auto& call = lirInstr::append(
      stream.pTail,
      cmn::tgt::kCall,
      "");
   auto& post = lirInstr::append(
      stream.pTail,
      cmn::tgt::kPostCallStackAlloc,
      "");
   auto& rval = call.addArg(*new lirArgVar("rval",0));

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      auto& arg = m_vGen.claimAndAddArgOffWire(call,**it);
      argSizes.push_back(arg.getSize());
   }
   subcallStackSize += m_t.getCallConvention().getArgumentStackSpace(argSizes);
   pre.addArg<lirArgConst>("",subcallStackSize);
   post.addArg<lirArgConst>("",subcallStackSize);

   m_vGen.createPrivateVar(call.orderNum,rval,"rval").publishOnWire(n);
}

void astCodeGen::visit(cmn::fieldAccessNode& n)
{
   visitChildren(n);

   auto& stream = m_lir.page[m_currFunc];
   auto& mov = lirInstr::append(
      stream.pTail,
      cmn::tgt::kMov,
      cmn::fmt("   :%s",n.name.c_str()));
   auto& dest = mov.addArg(*new lirArgVar("",0));

   auto& src = m_vGen.claimAndAddArgOffWire(mov,n.demandSoleChild<cmn::node>());
   src.addrOf = true;

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

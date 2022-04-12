#include "../cmn/fmt.hpp"
#include "../cmn/target.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/type.hpp"
#include "astCodeGen.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

void dataFormatter::visit(cmn::stringLiteralNode& n)
{
   // provide NULL-termination
   m_o << "\"" << n.value << "\" <b> 0";
}

void astCodeGen::visit(cmn::constNode& n)
{
   m_currFunc = n.name;
   auto& stream = m_lir.page[m_currFunc];
   auto& seg = lirInstr::append(
      stream.pTail,
      cmn::tgt::kSelectSegment,
      "");
   seg.addArg<lirArgConst>("2",0); // const seg

   auto& i = lirInstr::append(
      stream.pTail,
      cmn::tgt::kGlobalConstData,
      n.name);

   std::stringstream expr;
   { dataFormatter v(expr); n.acceptVisitor(v); }

   i.addArg<lirArgConst>(expr.str(),0);
}

void astCodeGen::visit(cmn::funcNode& n)
{
   if(n.getChildrenOf<cmn::sequenceNode>().size() == 0)
      return; // ignore forward references

   m_currFunc = n.name;
   auto& stream = m_lir.page[m_currFunc];
   auto& seg = lirInstr::append(
      stream.pTail,
      cmn::tgt::kSelectSegment,
      "");
   seg.addArg<lirArgConst>("1",0); // code seg

   auto args = n.getChildrenOf<cmn::argNode>();

   auto& i = lirInstr::append(
      stream.pTail,
      cmn::tgt::kEnterFunc,
      n.name);

   for(auto it=args.begin();it!=args.end();++it)
   {
      auto& astArg = **it;
      auto& arg = i.addArg<lirArgVar>(astArg.name,0); // TODO - calc type size
      m_vGen.createNamedVar(i.orderNum,arg);
   }

   n.demandSoleChild<cmn::sequenceNode>().acceptVisitor(*this);

   lirInstr::append(
      stream.pTail,
      cmn::tgt::kExitFunc,
      n.name);
}

void astCodeGen::visit(cmn::invokeFuncPtrNode& n)
{
   callGenInfo i;
   genCallPreChild(n,i);

   // bind child args to call instr
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      auto& arg = m_vGen.claimAndAddArgOffWire(*i.pCallInstr,**it);
      i.argRealSizes.push_back(m_t.getRealSize(arg.getSize()));
   }

   genCallPostChild(n,i);
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

   auto& child = n.demandSoleChild<cmn::node>();
   auto& src = m_vGen.claimAndAddArgOffWire(mov,child);
   src.addrOf = true;

   auto& childType = cmn::type::gNodeCache->demand(child)
      .as<cmn::type::iStructType>();
   src.disp = childType.getOffsetOfField(n.name,m_t);

   m_vGen.createPrivateVar(mov.orderNum,dest,"field:%s",n.name.c_str()).publishOnWire(n);
}

void astCodeGen::visit(cmn::callNode& n)
{
   callGenInfo i;
   genCallPreChild(n,i);

   // call node has a name to call
   auto& callLbl = i.pCallInstr->addArg(*new lirArgConst(n.name,0));
   m_vGen.createPrivateVar(i.pCallInstr->orderNum,callLbl,"calladdr(%s)",n.name.c_str());

   // bind child args to call instr
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      auto& arg = m_vGen.claimAndAddArgOffWire(*i.pCallInstr,**it);
      i.argRealSizes.push_back(m_t.getRealSize(arg.getSize()));
   }

   genCallPostChild(n,i);
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
   cdwTHROW("unimpled");
   // publish an arg
}

void astCodeGen::visit(cmn::intLiteralNode& n)
{
   m_vGen.createPrivateVar(0,*new lirArgConst(n.value,0),"intlit:%s",n.value.c_str())
      .donateToWire(n);
}

astCodeGen::callGenInfo::callGenInfo()
: pPreInstr(NULL), pCallInstr(NULL), pPostInstr(NULL), pRvalArg(NULL)
{
}

void astCodeGen::genCallPreChild(cmn::node& n, callGenInfo& i)
{
   // generate a pre-call instr
   // (add meta-args to this later)
   auto& stream = m_lir.page[m_currFunc];
   i.pPreInstr = &lirInstr::append(
      stream.pTail,
      cmn::tgt::kPreCallStackAlloc,
      "");

   // gen all the prepping call code (e.g. calculating and placing args)
   //
   // do this now so the code can use the storage prefs of the precall
   // instr, but before the actuall call
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      (*it)->acceptVisitor(*this);

   // gen the call and post-call instrs
   i.pCallInstr = &lirInstr::append(
      stream.pTail,
      cmn::tgt::kCall,
      "");
   i.pPostInstr = &lirInstr::append(
      stream.pTail,
      cmn::tgt::kPostCallStackAlloc,
      "");
   i.pRvalArg = &i.pCallInstr->addArg(*new lirArgVar("rval",0));
}

void astCodeGen::genCallPostChild(cmn::node& n, callGenInfo& i)
{
   // determine stack-size and add args to pre/post call
   size_t subcallStackSize = m_t.getCallConvention().getShadowSpace();
   subcallStackSize += m_t.getCallConvention().getArgumentStackSpace(i.argRealSizes);
   i.pPreInstr->addArg<lirArgConst>("",subcallStackSize);
   i.pPostInstr->addArg<lirArgConst>("",subcallStackSize);

   m_vGen.createPrivateVar(i.pCallInstr->orderNum,*i.pRvalArg,"rval").publishOnWire(n);
}

} // namespace liam

#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
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
   m_lir.onNewPageStarted(m_currFunc);
   seg.addArg<lirArgConst>(cmn::objfmt::obj::kLexConst,0);

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

   // start page and emit segment
   m_currFunc = n.name;
   auto& stream = m_lir.page[m_currFunc];
   auto& seg = lirInstr::append(
      stream.pTail,
      cmn::tgt::kSelectSegment,
      "");
   m_lir.onNewPageStarted(m_currFunc);
   seg.addArg<lirArgConst>(cmn::objfmt::obj::kLexCode,0);

   // determine real func name (different if entrypoint)
   std::string funcNameInAsm = n.name;
   if(n.attributes.find("entrypoint") != n.attributes.end())
      funcNameInAsm = ".entrypoint";

   auto& i = lirInstr::append(
      stream.pTail,
      cmn::tgt::kEnterFunc,
      funcNameInAsm);

   auto args = n.getChildrenOf<cmn::argNode>();
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
      funcNameInAsm);
}

void astCodeGen::visit(cmn::sequenceNode& n)
{
   std::vector<cmn::localDeclNode*>     locals;
   std::vector<cmn::node*>              statements;
   std::map<cmn::localDeclNode*,size_t> sizes;

   // allocate all locals
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      auto pLocal = dynamic_cast<cmn::localDeclNode*>(*it);
      if(!pLocal)
      {
         statements.push_back(*it);
         continue;
      }
      locals.push_back(pLocal);

      auto& stream = m_lir.page[m_currFunc];
      auto& res = lirInstr::append(
         stream.pTail,
         cmn::tgt::kReserveLocal,
         cmn::fmt("   :%s",pLocal->name.c_str()));

      auto lSize =
         cmn::type::gNodeCache->demand(
            pLocal->demandSoleChild<cmn::typeNode>()).getRealAllocSize(m_t);
      auto& arg = res.addArg<lirArgVar>(pLocal->name,lSize);
      m_vGen.createNamedVar(res.orderNum,arg);

      sizes[pLocal] = lSize; // use this later in the free
   }

   // handle all non-local children
   for(auto it=statements.begin();it!=statements.end();++it)
      (*it)->acceptVisitor(*this);

   // free all locals
   for(auto it=locals.begin();it!=locals.end();++it)
   {
      auto& stream = m_lir.page[m_currFunc];
      auto& res = lirInstr::append(
         stream.pTail,
         cmn::tgt::kUnreserveLocal,
         cmn::fmt("   :%s",(*it)->name.c_str()));
      res.addArg<lirArgVar>((*it)->name,sizes[*it]);
   }
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
   // this varRef can ref (1) a global, (2) an arg, (3) a local
   // in all but case 1, there is nothing to do but alloc a variable.
   // in case 1, you have to emit code to get the global into the variable.

   cmn::node& instSite = *n.pDef.getRefee()->getParent();
   if(dynamic_cast<cmn::constNode*>(&instSite)) // this is the only global so far
   {
      auto& stream = m_lir.page[m_currFunc];
      auto& mov = lirInstr::append(
         stream.pTail,
         cmn::tgt::kMov,
         cmn::fmt("load const '%s'",n.pDef.ref.c_str()));
      auto& dest = mov.addArg<lirArgVar>(n.pDef.ref,0);
      m_vGen.createNamedVar(mov.orderNum,dest).publishOnWire(n);
      auto& src = mov.addArg<lirArgConst>(n.pDef.ref,0);
      m_vGen.createPrivateVar(mov.orderNum,src,"constref");
   }
   else
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
   m_vGen.createPrivateVar(0,*new lirArgConst(n.lexeme,0),"intlit:%s",n.lexeme.c_str())
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

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//
// NEW LIR API
//
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void lirGenVisitor::visit(cmn::constNode& n)
{
   std::stringstream expr;
   { dataFormatter v(expr); n.acceptVisitor(v); }

   m_lGen.createNewStream(cmn::objfmt::obj::kLexConst,n.name);
   m_lGen.append(n,cmn::tgt::kGlobalConstData)
      .withArg<lirArgConst>(expr.str(),0)
      .withComment(n.name);
}

void lirGenVisitor::visit(cmn::funcNode& n)
{
   if(n.getChildrenOf<cmn::sequenceNode>().size() == 0)
      return; // ignore forward references

   m_lGen.createNewStream(cmn::objfmt::obj::kLexCode,n.name);

   // determine real func name (different if entrypoint)
   std::string funcNameInAsm = n.name;
   if(n.attributes.find("entrypoint") != n.attributes.end())
      funcNameInAsm = ".entrypoint";

   auto& i = m_lGen.append(n,cmn::tgt::kEnterFunc)
      .withArg<lirArgTemp>(m_u.makeUnique("rval"),n)
      .returnToParent(0)
      .withComment(funcNameInAsm);

   auto args = n.getChildrenOf<cmn::argNode>();
   for(auto it=args.begin();it!=args.end();++it)
      i.withArg<lirArgVar>(m_u.makeUnique((*it)->name),**it);

   n.demandSoleChild<cmn::sequenceNode>().acceptVisitor(*this);
}

void lirGenVisitor::visit(cmn::invokeFuncPtrNode& n)
{
   m_lGen.append(n,cmn::tgt::kPreCallStackAlloc);

   hNodeVisitor::visit(n);

   auto iCall = m_lGen.append(n,cmn::tgt::kCall)
      .withArg<lirArgTemp>(m_u.makeUnique("rval"),/*n*/ 0) // TODO 0 until typeprop for node is done
      .returnToParent(0)
      .withComment("(call ptr)");

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      iCall.inheritArgFromChild(**it);
}

void lirGenVisitor::visit(cmn::localDeclNode& n)
{
   hNodeVisitor::visit(n);

   m_lGen.append(n,cmn::tgt::kReserveLocal)
      .withArg<lirArgConst>("size",
         cmn::type::gNodeCache->demand(n.demandSoleChild<cmn::typeNode>())
            .getRealAllocSize(m_t))
      .withComment(n.name);

   // TODO not handling initializers here
}

void lirGenVisitor::visit(cmn::fieldAccessNode& n)
{
   hNodeVisitor::visit(n);

   auto& child = n.demandSoleChild<cmn::node>();
   auto& childType = cmn::type::gNodeCache->demand(child)
      .as<cmn::type::iStructType>();

   auto i = m_lGen.noInstr(n);
   auto& a = i.borrowArgFromChild(child);

   auto pA = new lirArgTemp(
      a.getName(),
      cmn::type::gNodeCache->demand(n).getPseudoRefSize());

   if(!a.addrOf)
   {
      // I can just deref the existing arg, no need for a mov
      pA->addrOf = true;
      pA->disp = childType.getOffsetOfField(n.name,m_t);
      i.returnToParent(*pA);
   }
   else
   {
      // my child is already dereffed, so to do it again I must mov
      m_lGen.append(n,cmn::tgt::kMov)
         .withArg<lirArgTemp>(m_u.makeUnique(n.name),n)
         .withArg(a.clone())
         .returnToParent(0);
   }
}

void lirGenVisitor::visit(cmn::callNode& n)
{
   m_lGen.append(n,cmn::tgt::kPreCallStackAlloc);

   hNodeVisitor::visit(n);

   auto iCall = m_lGen.append(n,cmn::tgt::kCall)
      .withArg<lirArgTemp>(m_u.makeUnique("rval"),/*n*/ 0) // TODO 0 until typeprop for node is done
      .returnToParent(0)
      .withArg<lirArgConst>(n.name,/*n*/ 0) // label
      .withComment("(call label)");

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      iCall.inheritArgFromChild(**it);
}

void lirGenVisitor::visit(cmn::varRefNode& n)
{
   auto pA = new lirArgVar(
      n.pDef.ref,
      cmn::type::gNodeCache->demand(*n.pDef.getRefee()).getPseudoRefSize());

   m_lGen.noInstr(n)
      .returnToParent(*pA);
}

// all literals are nearly identical (just 'value' different) - share this?

void lirGenVisitor::visit(cmn::stringLiteralNode& n)
{
   auto pA = new lirArgConst(
      n.value,
      cmn::type::gNodeCache->demand(n).getPseudoRefSize());

   m_lGen.noInstr(n)
      .returnToParent(*pA);
}

void lirGenVisitor::visit(cmn::boolLiteralNode& n)
{
   auto pA = new lirArgConst(
      n.value ? "T" : "F",
      cmn::type::gNodeCache->demand(n).getPseudoRefSize());

   m_lGen.noInstr(n)
      .returnToParent(*pA);
}

void lirGenVisitor::visit(cmn::intLiteralNode& n)
{
   auto pA = new lirArgConst(
      n.lexeme,
      cmn::type::gNodeCache->demand(n).getPseudoRefSize());

   m_lGen.noInstr(n)
      .returnToParent(*pA);
}

} // namespace liam

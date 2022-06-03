#include "../cmn/fmt.hpp"
#include "../cmn/lexor.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/target.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/type.hpp"
#include "astCodeGen.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

void dataFormatter::visit(cmn::varRefNode& n)
{
   m_o << n.pSrc.ref << " ";
}

void dataFormatter::visit(cmn::stringLiteralNode& n)
{
   // provide NULL-termination
   m_o << "\"" << n.value << "\" <b> 0 ";
}

void astCodeGen::visit(cmn::constNode& n)
{
   std::stringstream expr;
   { dataFormatter v(expr); n.acceptVisitor(v); }

   m_b.createNewStream(n.name,cmn::objfmt::obj::kLexConst);
   m_b.forNode(n)
      .append(cmn::tgt::kGlobalConstData)
         .withArg<lirArgConst>(expr.str(),0)
         .withComment(n.name);
}

void astCodeGen::visit(cmn::funcNode& n)
{
   if(n.getChildrenOf<cmn::sequenceNode>().size() == 0)
      return; // ignore forward references

   // determine real func name (different if entrypoint)
   std::string funcNameInAsm = n.name;
   if(n.attributes.find("entrypoint") != n.attributes.end())
      funcNameInAsm = ".entrypoint";

   m_b.createNewStream(funcNameInAsm,cmn::objfmt::obj::kLexCode);

   auto& i = m_b.forNode(n)
      .append(cmn::tgt::kEnterFunc)
         .withArg<lirArgTemp>(m_u.makeUnique("rval"),n)
         .returnToParent(0)
         .withComment(funcNameInAsm);

   auto args = n.getChildrenOf<cmn::argNode>();
   for(auto it=args.begin();it!=args.end();++it)
      i.withArg<lirArgVar>(m_u.makeUnique((*it)->name),**it);

   n.demandSoleChild<cmn::sequenceNode>().acceptVisitor(*this);
}

void astCodeGen::visit(cmn::invokeFuncPtrNode& n) // TODO left off here
{
   m_b.forNode(n)
      .append(cmn::tgt::kPreCallStackAlloc);

   hNodeVisitor::visit(n);

   auto& i = m_b.forNode(n)
      .append(cmn::tgt::kCall)
         .withArg<lirArgTemp>(m_u.makeUnique("rval"),/*n*/ 0) // TODO 0 until typeprop for node is done
         .returnToParent(0)
         .withComment("(call ptr)");

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      i.inheritArgFromChild(**it);

   consumeAllArgRegisters(i);
   trashScratchRegsOnCall(i);
}

void astCodeGen::visit(cmn::localDeclNode& n)
{
   hNodeVisitor::visit(n);

   size_t allocSize = cmn::type::gNodeCache->demand(n.demandSoleChild<cmn::typeNode>())
      .getRealAllocSize(m_t);

   if(cmn::type::gNodeCache->demand(n).is<cmn::type::iStructType>())
   {
      // user types are passed as pointers, so get the address of the stack
      // alloc, rather than the value

      std::string backStorName = m_u.makeUnique(n.name + "_alloc");
      m_b.forNode(n)
         .append(cmn::tgt::kReserveLocal)
            .withArg<lirArgVar>(backStorName,allocSize)
            .withComment(n.name)
            .then()
         .append(cmn::tgt::kLea)
            .withArg<lirArgVar>(n.name,allocSize)
            .withArg<lirArgVar>(backStorName,allocSize)
            .withComment(n.name);
   }
   else
   {
      m_b.forNode(n)
         .append(cmn::tgt::kReserveLocal)
            .withArg<lirArgVar>(n.name,allocSize)
            .withComment(n.name);
   }

   // TODO not handling initializers here (i.e. constant
   // initial value)
}

void astCodeGen::visit(cmn::fieldAccessNode& n)
{
   hNodeVisitor::visit(n);

   auto& child = n.demandSoleChild<cmn::node>();
   auto& childType = cmn::type::gNodeCache->demand(child)
      .as<cmn::type::iStructType>();

   auto& a = m_b.borrowArgFromChild(child);
   auto pA = new lirArgTemp(
      a.getName(),
      cmn::type::gNodeCache->demand(n).getPseudoRefSize());

   if(!a.addrOf)
   {
      // I can just deref the existing arg, no need for a mov
      pA->addrOf = true;
      pA->disp = a.disp + childType.getOffsetOfField(n.name,m_t);
      m_b.forNode(n).returnToParent(*pA);
   }
   else
   {
      // my child is already dereffed, so to do it again I must mov

      // but here lies the quandry: if I'm doing a logical _read_, then
      // I should emit a mov into a temporary for whoever asked for it
      // but if I'm doing a logical _write_, then I need to provide
      // an unmolested temporary to who asked for it, and AFTERWARDS
      // stash it back into storage
      //
      //      a:b:x = 7;
      //
      //                      =
      //           faccess(x)    7
      //       faccess(b)
      //  varRef(a)
      //
      //  "normal" order of codegen is        but what I want is
      //         7          7                   7
      //         [rbp]      varRef(a)           [rbp]                  [rbp-16] (bug)
      //         [rbp+8]    faccess(b)          [rbp+8]                [rbp-16+8]
      //         t0         faccess(x)          [[rbp+*]]**            [[rbp-16+8]+8] <- mov t0, [rbp-16+8]
      //         mov t0,7   =                   mov [rbp+8], 7         mov [t0+8], 7
      //
      // maybe I don't change anything at all, but have faccess()
      // always return the real ptr() to the storage... and let
      // callers deal with making it addressable.
      //
      // what would that mean?  I'd need lirArgs that are
      // double-dreffed?  Triple?  Arbitrary?  Just double.
      //
      // this same effect happens with *, and [].  But this is the
      // less common scenario.  So that should be the one that's
      // special
      //
      // a field access node is always a deref.

      auto *pTmp = new lirArgTemp(
         m_u.makeUnique(n.name),
         cmn::type::gNodeCache->demand(n).getPseudoRefSize());

      m_b.forNode(n)
         .append(cmn::tgt::kMov)
            .withArg(pTmp->clone())
            .withArg(a.clone())
            .withComment(std::string("fieldaccess: owner of ") + n.name);

      pTmp->addrOf = true;
      pTmp->disp = childType.getOffsetOfField(n.name,m_t);
      m_b.forNode(n).returnToParent(*pTmp);
   }
}

void astCodeGen::visit(cmn::callNode& n)
{
   m_b.forNode(n).append(cmn::tgt::kPreCallStackAlloc);

   hNodeVisitor::visit(n);

   auto iCall = m_b.forNode(n)
      .append(cmn::tgt::kCall)
         .withArg<lirArgTemp>(m_u.makeUnique("rval"),/*n*/ 0) // TODO 0 until typeprop for node is done
         .returnToParent(0)
         .withArg<lirArgLabel>(n.pTarget.ref,/*n*/ 0) // label
         .withComment("(call label)");
   iCall.tweakArgAs<lirArgLabel>(1).isCode = true;

   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      iCall.inheritArgFromChild(**it);

   consumeAllArgRegisters(iCall);
   trashScratchRegsOnCall(iCall);
}

void astCodeGen::visit(cmn::varRefNode& n)
{
   cmn::iVarSourceNode& declSite = *n.pSrc.getRefee();
   if(dynamic_cast<cmn::constNode*>(&declSite))
   {
      // references to another stream, like a global, are patched
      // patches are immediate data

      auto& ty = cmn::type::gNodeCache->demand(*n.pSrc._getRefee());
      if(ty.getName() == "string")
      {
         // strings are special and require LEA

         m_b.forNode(n)
            .append(cmn::tgt::kLea)
               .withArg<lirArgTemp>(m_u.makeUnique("str"),0)
               .withArg<lirArgLabel>(n.pSrc.ref,0)
               .returnToParent(0);
      }
      else
      {
         auto pA = new lirArgLabel(
            n.pSrc.ref,
            cmn::type::gNodeCache->demand(*n.pSrc._getRefee()).getPseudoRefSize());

         m_b.forNode(n)
            .returnToParent(*pA);
      }
   }
   else
   {
      auto pA = new lirArgVar(
         n.pSrc.ref,
         cmn::type::gNodeCache->demand(*n.pSrc._getRefee()).getPseudoRefSize());

      m_b.forNode(n)
         .returnToParent(*pA);
   }
}

void astCodeGen::visit(cmn::assignmentNode& n)
{
   n.getChildren()[1]->acceptVisitor(*this);
   n.getChildren()[0]->acceptVisitor(*this);

   m_b.forNode(n)
      .append(cmn::tgt::kMov)
         .inheritArgFromChild(*n.getChildren()[0])
         .inheritArgFromChild(*n.getChildren()[1])
         .withComment("=");
}

void astCodeGen::visit(cmn::bopNode& n)
{
   n.getChildren()[1]->acceptVisitor(*this);
   n.getChildren()[0]->acceptVisitor(*this);

   if(n.op == "+")
   {
      auto *pTmp = new lirArgTemp(
         m_u.makeUnique("agg"),
         m_b.borrowArgFromChild(*n.getChildren()[0]).getSize());

      m_b.forNode(n)
         .append(cmn::tgt::kMov)
            .withArg(pTmp->clone())
            .inheritArgFromChild(*n.getChildren()[0])
            .then()
         .append(cmn::tgt::kAdd)
            .withArg(*pTmp)
            .inheritArgFromChild(*n.getChildren()[1])
            .returnToParent(0);
   }
   else if(n.op == "<")
   {
      m_b.forNode(n)
         .append(cmn::tgt::kMacroIsLessThan)
            .withArg<lirArgTemp>(m_u.makeUnique("lt"),0)
            .inheritArgFromChild(*n.getChildren()[0])
            .inheritArgFromChild(*n.getChildren()[1])
            .returnToParent(0);
   }
   else
      cdwTHROW("unimplemented bop %s",n.op.c_str());
}

void astCodeGen::visit(cmn::ifNode& n)
{
   n.getChildren()[0]->acceptVisitor(*this);

   auto elseLabel = m_b.reserveNewLabel("else");
   auto endifLabel = m_b.reserveNewLabel("endif");

   m_b.forNode(n)
      .append(cmn::tgt::kMacroIfFalse)
         .inheritArgFromChild(*n.getChildren()[0])
         .withArg<lirArgLabel>(elseLabel,0)
         .tweakArgAs<lirArgLabel>(1).isCode = true;

   n.getChildren()[1]->acceptVisitor(*this);

   m_b.forNode(n)
      .append(cmn::tgt::kGoto)
         .withArg<lirArgLabel>(endifLabel,0)
         .tweakArgAs<lirArgLabel>(0).isCode = true;
   m_b.forNode(n)
      .append(cmn::tgt::kLabel)
         .withArg<lirArgLabel>(elseLabel,0)
         .tweakArgAs<lirArgLabel>(0).isCode = true;

   auto children = n.getChildren();
   for(size_t i=2;i<children.size();i++)
   {
      children[i]->acceptVisitor(*this);
      m_b.forNode(n)
         .append(cmn::tgt::kGoto)
            .withArg<lirArgLabel>(endifLabel,0)
            .tweakArgAs<lirArgLabel>(0).isCode = true;
   }

   m_b.forNode(n)
      .append(cmn::tgt::kLabel)
         .withArg<lirArgLabel>(endifLabel,0)
         .tweakArgAs<lirArgLabel>(0).isCode = true;
}

// all literals are nearly identical (just 'value' different) - share this?

void astCodeGen::visit(cmn::stringLiteralNode& n)
{
   auto pA = new lirArgConst(
      n.value,
      cmn::type::gNodeCache->demand(n).getPseudoRefSize());

   m_b.forNode(n)
      .returnToParent(*pA);
}

void astCodeGen::visit(cmn::boolLiteralNode& n)
{
   auto pA = new lirArgConst(n.value ? "1" : "0",1);

   m_b.forNode(n)
      .returnToParent(*pA);
}

void astCodeGen::visit(cmn::intLiteralNode& n)
{
   // get the exact size, so codeshape is as accurate as possible
   size_t size = cmn::lexorBase::getLexemeIntSize(cmn::lexorBase::getLexemeInt(n.lexeme));

   auto pA = new lirArgConst(n.lexeme,size);

   m_b.forNode(n)
      .returnToParent(*pA);
}

void astCodeGen::consumeAllArgRegisters(lirBuilder::instrBuilder& instr)
{
   // generate additional args to trash regs as necessary

   std::vector<size_t> argStorage;
   m_t.getCallConvention().getRValAndArgBank(argStorage);

   size_t storageUsed = (instr.instr().getArgs().size() - 1); // -1 for callptr

   if(storageUsed < argStorage.size())
   {
      // need some extra dummy args
      const size_t toBurn = argStorage.size() - storageUsed;
      const size_t offset = storageUsed;
      for(size_t i=0;i<toBurn;i++)
      {
         size_t stor = argStorage[i+offset];

         std::stringstream nameHint;
         nameHint << m_t.getProc().getRegName(stor) << "_burn";

         instr.withArg<lirArgTemp>(m_u.makeUnique(nameHint.str()),0);
      }
   }
}

void astCodeGen::trashScratchRegsOnCall(lirBuilder::instrBuilder& instr)
{
   // add more dummies for trashed args as required for calling convention

   std::vector<size_t> argStorage;
   m_t.getCallConvention().createScratchRegisterBank(argStorage);

   for(size_t i=0;i<argStorage.size();i++)
   {
      size_t stor = argStorage[i];

      std::stringstream nameHint;
      nameHint << m_t.getProc().getRegName(stor) << "_trash";

      instr.withArg<lirArgTemp>(m_u.makeUnique(nameHint.str()),0);
   }
}

} // namespace liam

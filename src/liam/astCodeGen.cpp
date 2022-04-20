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

   // TODO not handling initializers here (i.e. constant
   // initial value)
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
         .withComment(std::string("fieldaccess: ") + n.name)
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
   cmn::node& declSite = *n.pDef.getRefee()->getParent();
   if(dynamic_cast<cmn::constNode*>(&declSite))
   {
      // references to another stream, like a global, are patched
      // patches are immediate data

      auto pA = new lirArgConst(
         n.pDef.ref,
         cmn::type::gNodeCache->demand(*n.pDef.getRefee()).getPseudoRefSize());

      m_lGen.noInstr(n)
         .returnToParent(*pA);
   }
   else
   {
      auto pA = new lirArgVar(
         n.pDef.ref,
         cmn::type::gNodeCache->demand(*n.pDef.getRefee()).getPseudoRefSize());

      m_lGen.noInstr(n)
         .returnToParent(*pA);
   }
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

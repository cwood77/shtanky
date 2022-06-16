#include "loopIntrinsicDecomp.hpp"
#include "stlutil.hpp"

namespace cmn {

void loopIntrinsicDecomp::visit(callNode& n)
{
   if(n.pTarget.ref == "loop")
   {
      if(n.getChildren().size() != 1)
         cdwTHROW("loop keyword requires one argument");

      auto *pVr = dynamic_cast<varRefNode*>(n.getChildren()[0]);
      if(!pVr)
         cdwTHROW("loop keyword requires name argument");

      auto *pLInt = new loopIntrinsicNode();
      pLInt->name = pVr->pSrc.ref;

      delete n.getParent()->replaceChild(n,*pLInt);
   }
}

void loopVarRefFixup::visit(varRefNode& n)
{
   hNodeVisitor::visit(n);

   if(cmn::has(m_loopInstances,n.pSrc.ref))
   {
      auto *pM = new invokeNode();
      pM->proto.ref = "getValue";
      n.injectAbove(*pM);
   }
}

void loopVarRefFixup::visit(loopBaseNode& n)
{
   if(!n.decomposed)
      m_loopInstances.insert(n.name);

   hNodeVisitor::visit(n);

   if(!n.decomposed)
      m_loopInstances.erase(n.name);
}

void loopInstDropper::visit(loopIntrinsicNode& n)
{
   auto *pVarRef = new varRefNode;
   pVarRef->pSrc.ref = n.name;
   delete n.getParent()->replaceChild(n,*pVarRef);

   visitChildren(*pVarRef);
}

void loopInstDropper::handleLoop(loopBaseNode& n, const std::string& instTypeName)
{
   if(!n.decomposed)
   {
      auto *pSeq = new sequenceNode;
      auto *pLoc = new localDeclNode; pLoc->name = n.name;
      auto *pTy = new userTypeNode; pTy->pDef.ref = instTypeName;
      pLoc->appendChild(*pTy);

      n.injectAbove(*pSeq);
      if(n.scoped)
         pSeq->insertChildBefore(*pLoc,n);
      else
         pSeq->getParent()->insertChildBefore(*pLoc,*pSeq);

      n.decomposed = true;

      visitChildren(*pSeq);
   }
   else
      visitChildren(n);
}

} // namespace cmn

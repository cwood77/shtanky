#include "loopIntrinsicDecomp.hpp"

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

} // namespace cmn

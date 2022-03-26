#include "declasser.hpp"

namespace araceli {

// for varRefs
// - if these refer to fields, scope then with 'self'

// for methods: two things
// - inject self args
// - passthru vtables

void declasser::visit(cmn::invokeNode& n)
{
   std::unique_ptr<cmn::invokeFuncPtrNode> pInvoke(new cmn::invokeFuncPtrNode());
   std::unique_ptr<cmn::fieldAccessNode> pVTable(new cmn::fieldAccessNode());
   std::unique_ptr<cmn::fieldAccessNode> pFuncPtr(new cmn::fieldAccessNode());
   std::unique_ptr<cmn::varRefNode> pSelf(new cmn::varRefNode());
   std::unique_ptr<cmn::argNode> pSelfArg(new cmn::argNode());

   // stash the inst name (and assume it's a varRefnode... d'oh!)
   cmn::varRefNode& inst = dynamic_cast<cmn::varRefNode&>(*n.getChildren()[0]);
   std::string instName = inst.pDef.ref;

   // configure the nodes
   pVTable->name = "type";
   pFuncPtr->name = n.proto.ref;
   pSelf->pDef.ref = instName;

   // chain the nodes
   pVTable->appendChild(inst);
   pFuncPtr->appendChild(*pVTable.release());
   pSelfArg->appendChild(*pSelf.release());
   pInvoke->appendChild(*pFuncPtr.release());
   pInvoke->appendChild(*pSelfArg.release());
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
   {
      if(*it == &inst)
         continue;
      pInvoke->appendChild(**it);
   }
   n.getChildren().clear();

   // now replace?
   n.getParent()->replaceChild(n,*pInvoke.release());
}

} // namespace araceli

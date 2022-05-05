#include "../cmn/type.hpp"
#include "opOverloadDecomp.hpp"

namespace araceli {

// read and write index ops are totally different
//
// a[1] = 7;     ->    a->setIndex(1,7);
// print(a[3]);  ->    print(a->getIndex(3));
//
// but this difference extends only to the outermost index
//
// a[1][5] = 6   ->    a->getIndex(1)->setIndex(5,6);
//

void opOverloadDecomp::visit(cmn::assignmentNode& n)
{
   n.getChildren()[0]->flags |= cmn::nodeFlags::kAddressableForWrite;
   hNodeVisitor::visit(n);

   auto *pIndex = dynamic_cast<cmn::indexNode*>(n.getChildren()[0]);
   if(!pIndex) return;

   cmn::type::iType& t = cmn::type::gNodeCache->demand(*pIndex->getChildren()[0]);
   if(!t.is<cmn::type::iStructType>())
      return;

   auto& userType = t.as<cmn::type::iStructType>();
   if(!userType.hasMethod("indexOpSet"))
      return;

   // write case
   //
   //         =                   invoke
   //    []       val   ->   obj    idx   val
   // obj  idx

   auto *pNoob = new cmn::invokeNode();
   pNoob->proto.ref = "indexOpSet";
   for(auto it=pIndex->getChildren().begin();it!=pIndex->getChildren().end();++it)
      pNoob->appendChild(**it);
   pNoob->appendChild(*n.getChildren()[1]);
   pIndex->getChildren().clear();
   delete pIndex;
   n.getChildren().clear();
   delete n.getParent()->replaceChild(n,*pNoob);
}

void opOverloadDecomp::visit(cmn::indexNode& n)
{
   hNodeVisitor::visit(n);

   cmn::type::iType& t = cmn::type::gNodeCache->demand(*n.getChildren()[0]);
   if(!t.is<cmn::type::iStructType>())
      return;

   bool isWrite = (n.flags & cmn::nodeFlags::kAddressableForWrite);
   if(isWrite)
      return;

   auto& userType = t.as<cmn::type::iStructType>();
   if(!userType.hasMethod("indexOpGet"))
      return;

   // read case:
   //
   //     []       ->     invoke
   // obj    idx        obj    idx

   auto *pNoob = new cmn::invokeNode();
   pNoob->proto.ref = "indexOpGet";
   for(auto it=n.getChildren().begin();it!=n.getChildren().end();++it)
      pNoob->appendChild(**it);
   n.getChildren().clear();
   delete n.getParent()->replaceChild(n,*pNoob);
}

} // namespace araceli

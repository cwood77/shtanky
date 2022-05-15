#include "../cmn/userError.hpp"
#include "intfTransform.hpp"

namespace salome {

cmn::timedGlobal<intfTransformState> intfTransform::gState;

void intfTransform::visit(cmn::classNode& n)
{
   if(!(n.flags & cmn::nodeFlags::kInterface))
      return;

   if(gState->handled.find(&n) != gState->handled.end())
      return;

   if(n.getChildrenOf<cmn::fieldNode>().size())
      cmn::gUserErrors->add(n,cmn::kErrorInterfacesCannotHaveFields);

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
   {
      if((*it)->flags)
         ;//cmn::gUserErrors->add(**it,cmn::kErrorInterfacesInherentlyPublicAbstract);

      // set defaults
      (*it)->flags |= (cmn::nodeFlags::kPublic | cmn::nodeFlags::kAbstract);

      nChanges++;
      gState->handled.insert(&n);
   }
}

} // namespace salome

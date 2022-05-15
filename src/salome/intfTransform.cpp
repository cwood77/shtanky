#include "../cmn/userError.hpp"
#include "intfTransform.hpp"

namespace salome {

void intfTransform::visit(cmn::classNode& n)
{
   if(!(n.flags & cmn::nodeFlags::kInterface))
      return;

   if(n.getChildrenOf<cmn::fieldNode>().size())
      cmn::gUserErrors->add(n,cmn::kErrorInterfacesCannotHaveFields);

   auto methods = n.getChildrenOf<cmn::methodNode>();
   for(auto it=methods.begin();it!=methods.end();++it)
   {
// TEMPORARY
//  this should be enabled, but will cause havoc because philemon runs before this
//  and can't handle it.  Probably means this transform should run earlier
//      if((*it)->flags)
//         cmn::gUserErrors->add(**it,cmn::kErrorInterfacesInherentlyPublicAbstract);

      // set defaults
      (*it)->flags |= (cmn::nodeFlags::kPublic | cmn::nodeFlags::kAbstract);
   }
}

} // namespace salome

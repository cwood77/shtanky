#include "abstractGenerator.hpp"

namespace araceli {

void abstractGenerator::generate(classCatalog& cc)
{
   for(auto cit=cc.classes.begin();cit!=cc.classes.end();++cit)
   {
      classInfo& cInfo = cit->second;
      for(auto mit=cInfo.inheritedAndDirectMethods.begin();
         mit!=cInfo.inheritedAndDirectMethods.end();++mit)
      {
         methodInfo& mInfo = *mit;
         if(mInfo.flags & cmn::nodeFlags::kAbstract)
         {
            cmn::methodNode *pNoob = NULL;
            if(mInfo.inherited)
            {
               // this is an abstract method I never implemented; implement it now
               cInfo.pNode->appendChild(cmn::cloneTree(*mInfo.pMethod,true));
               pNoob = dynamic_cast<cmn::methodNode*>(cInfo.pNode->lastChild());
            }
            else
            {
               // this is an abstract method I'm introducing for my derived classes
               // provide a base implementation
               pNoob = mInfo.pMethod;
            }

            pNoob->appendChild(*new cmn::sequenceNode());
            pNoob->flags &= ~cmn::nodeFlags::kAbstract;
            pNoob->flags |= cmn::nodeFlags::kVirtual;
         }
      }
   }
}

} // namespace araceli

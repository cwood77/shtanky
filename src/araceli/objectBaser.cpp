#include "objectBaser.hpp"

namespace araceli {

void objectBaser::visit(cmn::classNode& n)
{
   if(n.baseClasses.size() == 0)
   {
      auto fqn = cmn::fullyQualifiedName::build(n);
      if(fqn != ".sht.core.object")
      {
         // all classes ultimately derive from object... except for object
         n.baseClasses.push_back(cmn::link<cmn::classNode>());
         auto& l = *(--(n.baseClasses.end()));
         l.ref = ".sht.core.object";
      }
   }
}

} // namespace araceli

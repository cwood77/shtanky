#include "methodMover.hpp"
#include <memory>

namespace araceli {

void methodMover::visit(cmn::fileNode& n)
{
   hNodeVisitor::visit(n);

   for(auto it=m_methods.begin();it!=m_methods.end();++it)
   {
      cmn::methodNode *pMethod = *it;

      std::unique_ptr<cmn::funcNode> pFunc(new cmn::funcNode());
      pFunc->attributes = pMethod->attributes;
      pFunc->name = (*it)->getAncestor<cmn::classNode>().name + "." + pMethod->name;
      for(auto jit=pMethod->getChildren().begin();jit!=pMethod->getChildren().end();++jit)
         pFunc->appendChild(**jit);
      pMethod->lTarget.tryMigrateRefers(*pFunc.get());

      pMethod->getChildren().clear();
      pMethod->getParent()->removeChild(*pMethod);
      delete pMethod;

      n.appendChild(*pFunc.release());
   }
   m_methods.clear();
}

void methodMover::visit(cmn::methodNode& n)
{
   m_methods.push_back(&n);
}

} // namespace araceli

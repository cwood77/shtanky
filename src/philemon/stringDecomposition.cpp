#include "stringDecomposition.hpp"

namespace philemon {

void stringDecomposition::visit(cmn::strTypeNode& n)
{
   hNodeVisitor::visit(n);
   m_nodes.push_back(&n);
}

void stringDecomposition::run()
{
   for(auto it=m_nodes.begin();it!=m_nodes.end();++it)
   {
      auto *pNoob = new cmn::userTypeNode();
      pNoob->pDef.ref = ".sht.core.string";

      for(auto jit=(*it)->getChildren().begin();jit!=(*it)->getChildren().end();++jit)
         pNoob->appendChild(**jit);
      (*it)->getChildren().clear();
      delete (*it)->getParent()->replaceChild(**it,*pNoob);
   }
}

} // namespace philemon

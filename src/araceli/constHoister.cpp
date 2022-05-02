#include "constHoister.hpp"

namespace araceli {

void constHoister::visit(cmn::fileNode& n)
{
   m_pFile = &n;
   m_next = 0;
   hNodeVisitor::visit(n);
}

// const will have the type and literal as children
// literal will be replaced with a varRef
// const is dumped on the file
void constHoister::visit(cmn::stringLiteralNode& n)
{
   // create
   std::unique_ptr<cmn::constNode> pConst(new cmn::constNode());
   std::unique_ptr<cmn::strTypeNode> pType(new cmn::strTypeNode());
   std::unique_ptr<cmn::varRefNode> pRef(new cmn::varRefNode());

   // configure
   {
      std::stringstream name;
      name << ".const" << m_next++;
      pConst->name = name.str();
   }
   pRef->pSrc.ref = cmn::fullyQualifiedName::build(*m_pFile,pConst->name);

   // chain
   pConst->appendChild(*pType.release());
   n.getParent()->replaceChild(n,*pRef.release());
   pConst->appendChild(n);
   m_pFile->appendChild(*pConst.release());
}

} // namespace araceli

#include "frontend.hpp"
#include "symbolTable.hpp"

namespace philemon {

void frontend::linkGraph()
{
   if(m_firstLink)
   {
      m_firstLink = false;
      m_pTgt->populateInstantiates(*m_pPrj.get());
   }
   nodeLinker().linkGraph(*m_pPrj);
}

} // namespace philemon

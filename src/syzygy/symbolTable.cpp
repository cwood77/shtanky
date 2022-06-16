#include "../cmn/symbolTable.hpp"
#include "symbolTable.hpp"

namespace syzygy {

bool nodeLinkerHelper::typePropLink(cmn::node& root, cmn::symbolTable& sTable)
{
   // salome will fail at type prop link because things like strings and arrays
   // aren't handled until araceli

   bool changes = false;//araceli::nodeLinker::typePropLink(root,sTable);

   // so cancel any needs that the base can't handle
   auto before = sTable.unresolved.size();
   for(auto *pL : sTable.linksThatNeedTypeProp)
      sTable.unresolved.erase(pL);

   return changes || (before != sTable.unresolved.size());
}

} // namespace syzygy

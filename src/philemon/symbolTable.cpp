#include "../cmn/trace.hpp"
#include "../syzygy/genericUnlinker.hpp"
#include "../syzygy/symbolTable.hpp"
#include "genericClassInstantiator.hpp"
#include "symbolTable.hpp"

namespace philemon {

bool nodeLinker::typePropLink(cmn::node& root, cmn::symbolTable& sTable)
{
   return syzygy::nodeLinkerHelper::typePropLink(root,sTable);
}

bool nodeLinker::tryFixUnresolved(cmn::node& root, cmn::symbolTable& sTable)
{
   // first, call my parent
   bool madeProgress = araceli::nodeLinker::tryFixUnresolved(root,sTable);
   if(madeProgress)
      return madeProgress;

   return syzygy::unlinkGenerics(root,sTable);
}

} // namespace philemon

#include "../cmn/trace.hpp"
#include "../syzygy/genericUnlinker.hpp"
#include "../syzygy/symbolTable.hpp"
#include "intfTransform.hpp"
#include "symbolTable.hpp"

namespace salome {

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

bool nodeLinker::loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable)
{
   // first, call my parent
   bool madeProgress = araceli::nodeLinker::loadAnotherSymbol(root,sTable);
   if(madeProgress)
      return madeProgress;

   // if that don't work, try running the intf pass.  This provides dyn dispatch keywords
   // that can make some link refees "appear" suddenly.
   intfTransform v;
   root.acceptVisitor(v);

   return v.nChanges;
}

} // namespace salome

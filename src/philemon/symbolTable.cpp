#include "../cmn/trace.hpp"
#include "../syzygy/symbolTable.hpp"
#include "genericClassInstantiator.hpp"
#include "symbolTable.hpp"

namespace philemon {

bool nodeLinker::typePropLink(cmn::node& root, cmn::symbolTable& sTable)
{
   return syzygy::nodeLinkerHelper::typePropLink(root,sTable);
}

bool nodeLinker::loadAnotherSymbol(cmn::node& root, cmn::symbolTable& sTable)
{
   bool loadedMore = araceli::nodeLinker::loadAnotherSymbol(root,sTable);
   if(loadedMore)
      return loadedMore;

   size_t nChanges = classInstantiator().run(root);
   if(nChanges == 0)
   {
      // we're not going to load anything else, so let's strip the generics
      // this is required because otherwise the templates themselves may keep the graph
      // from linking (e.g. 'T' won't be found)
      genericStripper v;
      root.acceptVisitor(v);
      nChanges = v.nRemoved;
      cdwVERBOSE("stripped %lld generics\n",nChanges);
      if(nChanges)
      {
         cdwVERBOSE("graph after generics removal ----\n");
         { cmn::diagVisitor v; root.acceptVisitor(v); }
      }
   }

   return nChanges;
}

} // namespace philemon

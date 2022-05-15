#include "../cmn/trace.hpp"
#include "../syzygy/genericUnlinker.hpp"
#include "symbolTable.hpp"
#include <cstring>

namespace salome {

bool nodeLinker::tryFixUnresolved(cmn::node& root, cmn::symbolTable& sTable)
{
   // first, call my parent
   bool madeProgress = araceli::nodeLinker::tryFixUnresolved(root,sTable);
   if(madeProgress)
      return madeProgress;

   // unlink the generics
   // this is required because otherwise the templates themselves may keep the graph
   // from linking (e.g. 'T' won't be found)
   syzygy::genericUnlinker v(sTable);
   root.acceptVisitor(v);
   size_t nChanges = v.nChanges;
   cdwVERBOSE("unlinked %lld generics\n",nChanges);

   // also, unlink anything asking for a generic instance, as these will never by satisfied
   for(auto it=sTable.unresolved.begin();it!=sTable.unresolved.end();)
   {
      if(::strchr((*it)->ref.c_str(),'<'))
      {
         sTable.unresolved.erase(*it);
         it = sTable.unresolved.begin();
         nChanges++;
      }
      else
         ++it;
   }

   return nChanges;
}

} // namespace salome

#include "../cmn/obj-fmt.hpp"
#include "../cmn/reader.hpp"
#include "../cmn/trace.hpp"
#include "layout.hpp"
#include "objdir.hpp"
#include <set>
#include <vector>

#if 0
// knows how to incorporate a new object
class layout {
public:
   static void addObject(
      std::map<cmn::objfmt::obj*,unsigned long>& ops,
      segmentBucket& b,
      cmn::objfmt::obj& o,
      std::set<std::string>& f,
      std::set<std::string>& m);
};

void layout::addObject(std::map<cmn::objfmt::obj*,unsigned long>& ops, segmentBucket& b, cmn::objfmt::obj& o, std::set<std::string>& f, std::set<std::string>& m)
{
   auto fixupAddend = b.append(o);
   ops[&o] = fixupAddend;

   // add all exports of o into f

   // add all imports of o into m
}
#endif

using namespace shlink;

int main(int argc, const char *argv[])
{
   objectDirectory oDir;
   oDir.loadObjectFile(".\\testdata\\test\\test.ara.ls.asm.o");

   {
      layoutProgress lProg;
      lProg.seedRequiredObject("entrypoint");

      while(!lProg.isDone())
      {
         auto name = lProg.getUnplacedObjectName();
         auto& o = oDir.demand(name);
      }
   }

#if 0
   cmn::objfmt::objFile o;

   {
      cmn::binFileReader r(".\\testdata\\test\\test.ara.ls.asm.o");
      o.unflatten(r);
   }

   cdwDEBUG("loaded file\n");
   cdwDEBUG("   read %lld object(s)\n",o.objects.size());

   // build a master obj directory of
   // name => obj*
   std::map<std::string,cmn::objfmt::obj*> objDir;

   // create bucket for each segment type
   std::map<cmn::objfmt::obj*,unsigned long> objPlacements;
   std::map<size_t,segmentBucket> buckets;

   // start by looking for entrypoint
   std::set<std::string> found,missing;
   missing.insert("entrypoint");

   while(true)
   {
      // pick the entrypoint object
      // - write it's bytes into the appropriate bucket
      // - add it's imports to the missing table
      // - add it's exports to the found table
      auto currOName = *missing.begin();
      auto pObj = objDir[currOName];
      auto& bucket = buckets[pObj->flags];
      layout::addObject(objPlacements,bucket,*pObj,found,missing);

      // mark imports as fulfilled (target in export table) or not
      // if there are unfulfilled relative patches, pick another object to make progress

      if(missing.empty())
         break;
   }

   // now that all buckets are filled, compute offsets of each and pick an order for them
   {}

   // LOOP over objPlacements
   // resolve relative patches
   //     - patch code
   //     - remove from missing table
   // put absolute pathces in a new table
   // LOOP

   // write final EXE format - header, absolute patches, DLL missings, entrypoint offset
#endif

   return 0;
}

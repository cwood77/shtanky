#include "../cmn/trace.hpp"
#include "../cmn/writer.hpp"
#include "formatter.hpp"
#include "layout.hpp"
#include "objdir.hpp"

using namespace shlink;

int main(int argc, const char *argv[])
{
   objectDirectory oDir;
   oDir.loadObjectFile(".\\testdata\\test\\test.ara.ls.asm.o");

   layout l;
   {
      layoutProgress lProg;
      lProg.seedRequiredObject(".test.test.run");

      while(!lProg.isDone())
      {
         auto name = lProg.getUnplacedObjectName();
         auto& o = oDir.demand(name);

         l.place(o);
         lProg.markObjectPlaced(o);
      }

      l.markDonePlacing();
   }

   l.link(oDir);

   {
      cdwVERBOSE("writing...\n");
      cmn::compositeObjWriter w;
      w.sink(
         *new cmn::retailObjWriter(
            *new cmn::binFileWriter(".\\testdata\\test\\test.ara.ls.asm.o.app")));
      w.sink(
         *new cmn::listingObjWriter(
            *new cmn::binFileWriter(".\\testdata\\test\\test.ara.ls.asm.o.app.list")));

      formatter(w).write(l);
      cdwVERBOSE("done writing\n");
   }

   return 0;
}

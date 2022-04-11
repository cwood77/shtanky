#include "../cmn/cmdline.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/writer.hpp"
#include "formatter.hpp"
#include "layout.hpp"
#include "objdir.hpp"

using namespace shlink;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string outFile = cl.getArg(".\\testdata\\test\\test.ara.ls.asm.o.app");

   objectDirectory oDir;
   for(size_t i=0;;i++)
   {
      auto path = cl.getArg(i==0 ? ".\\testdata\\test\\test.ara.ls.asm.o" : "");
      if(path.empty())
         break;

      oDir.loadObjectFile(path);
   }

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
            *new cmn::binFileWriter(outFile)));
      w.sink(
         *new cmn::listingObjWriter(
            *new cmn::binFileWriter(cmn::pathUtil::addExtension(outFile,cmn::pathUtil::kExtList))));

      formatter(w).write(l);
      cdwVERBOSE("done writing\n");
   }

   return 0;
}

#include "../cmn/binWriter.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "formatter.hpp"
#include "layout.hpp"
#include "objdir.hpp"

using namespace shlink;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string outFile = cl.getNextArg(".\\testdata\\test\\test.ara.ls.asm.o.app");

   // load all object files indicated on command-line
   objectDirectory oDir;
   for(size_t i=0;;i++)
   {
      auto path = cl.getNextArg(i==0 ? ".\\testdata\\test\\test.ara.ls.asm.o" : "");
      if(path.empty())
         break;

      oDir.loadObjectFile(path);
   }

   // layout objects by segment, only including referenced objects
   layout l;
   {
      objectProviderRecorder oDirWrapper(oDir);
      layoutProgress lProg;
      lProg.seedRequiredObject(".test.test.run"); // TODO HACK should be entrypoint
      lProg.seedRequiredObject(".sht.cons.stdout.printLn"); // TODO HACK keep this lame-o hacks until linker is totally working (i.e. no symbols are pruned inadvertently)

      while(!lProg.isDone())
      {
         auto name = lProg.getUnplacedObjectName();
         auto& o = oDirWrapper.demand(name);

         l.place(o);
         lProg.markObjectPlaced(o);
      }

      l.markDonePlacing();

      oDir.determinePruneList(oDirWrapper.demanded);
   }

   // link objects together
   l.link(oDir);

   // write
   {
      cdwVERBOSE("writing...\n");
      cmn::compositeObjWriter w;
      w.sinkNewFileWithListing(outFile);
      formatter(w).write(l);
      cdwVERBOSE("done writing\n");
   }

   return 0;
}

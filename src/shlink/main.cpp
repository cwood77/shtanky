#include "../cmn/cmdline.hpp"
#include "../cmn/trace.hpp"
#include "iTarget.hpp"
#include "layout.hpp"
#include "objdir.hpp"
#include <memory>

using namespace shlink;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);

   // determine the target
   std::unique_ptr<iTarget> pTgt(createTarget(cl));
   std::unique_ptr<iSymbolIndex> pIndx(pTgt->createIndex());

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
      pTgt->seedRequirements(lProg);

      while(!lProg.isDone())
      {
         auto name = lProg.getUnplacedObjectName();
         auto& o = oDirWrapper.demand(name);

         l.place(o);
         lProg.markObjectPlaced(o);
      }

      l.markDonePlacing();

      oDir.determinePruneList(oDirWrapper.demanded);
      l.reportSymbols(oDirWrapper.demanded,oDir,*pIndx.get());
   }

   // link objects together
   l.link(oDir);

   // write
   cdwVERBOSE("writing...\n");
   pTgt->write(l,*pIndx.get());
   cdwVERBOSE("done writing\n");

   return 0;
}

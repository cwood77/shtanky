#include "../araceli/loader.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "../syzygy/codegen.hpp"
#include "frontend.hpp"
#include "genericClassInstantiator.hpp"

using namespace philemon;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\philemon");
   std::string inExt = cl.getNextArg("");

   araceli::loaderPrefs lPrefs = { inExt, ".target.ara" };
   cmn::globalPublishTo<araceli::loaderPrefs> _lPrefs(lPrefs,araceli::gLoaderPrefs);

   // setup load infix
  // std::string infix = "1-sa";
  // cmn::globalPublishTo<std::string> _infix(infix,araceli::gLastSupportedInfix);

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   frontend(projectDir,pPrj,pTgt).run();

   // load any additional files indicated on the command line
   for(size_t i=0;;i++)
   {
      auto path = cl.getNextArg("");
      if(path.empty())
         break;

      cdwVERBOSE("loading explicit file '%s'\n",path.c_str());
      araceli::loader::findScopeAndLoadFile(*pPrj.get(),path);
   }

   // run the instantiator, so it's guaranteed at least one run
   classInstantiator().run(*pPrj.get());
   { genericStripper v; pPrj->acceptVisitor(v); }

   // write
   cmn::outBundle b;
   { syzygy::codegen v(b,"ph"); pPrj->acceptVisitor(v); }
   { cmn::unconditionalWriter f; b.updateDisk(f); }

   return 0;
}

cdwImplMain()

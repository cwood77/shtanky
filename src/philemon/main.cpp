#include "../araceli/loader.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "../syzygy/codegen.hpp"
#include "frontend.hpp"
#include "genericClassInstantiator.hpp"
#include "stringDecomposition.hpp"
#include "symbolTable.hpp"

using namespace philemon;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\philemon");
   std::string inExt = cl.getNextArg("ara");

   // I convert * (arg) -> ph
   araceli::loaderPrefs lPrefs = { inExt, ".target.ara" };
   cmn::globalPublishTo<araceli::loaderPrefs> _lPrefs(lPrefs,araceli::gLoaderPrefs);

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

   // transform native string type to class
   { stringDecomposition v; pPrj->acceptVisitor(v); v.run(); }

   // subsequent link to load more
   nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // run the instantiator, so it's guaranteed at least one run
   classInstantiator().run(*pPrj.get());
   { genericStripper v; pPrj->acceptVisitor(v); }

   // codegen
   cmn::outBundle b;
   { syzygy::codegen v(b,"ph"); pPrj->acceptVisitor(v); }
   { cmn::unconditionalWriter f; b.updateDisk(f); }

   return 0;
}

cdwImplMain()

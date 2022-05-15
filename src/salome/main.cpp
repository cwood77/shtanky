#include "../araceli/iTarget.hpp"
#include "../araceli/loader.hpp"
#include "../araceli/symbolTable.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "../syzygy/codegen.hpp"
#include "../syzygy/frontend.hpp"
//#include "frontend.hpp"
//#include "symbolTable.hpp"

//using namespace philemon;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\test");
   std::string inExt = cl.getNextArg("ara");

   // I convert * (arg) -> sa
   araceli::loaderPrefs lPrefs = { inExt, ".target.ara" };
   cmn::globalPublishTo<araceli::loaderPrefs> _lPrefs(lPrefs,araceli::gLoaderPrefs);

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   syzygy::frontend(projectDir,pPrj,pTgt).run();

   // load any additional files indicated on the command line
   cl.addNextArgDefaultsIfNoneLeft(3,
      ".\\testdata\\sht\\core\\object.ara",
      ".\\testdata\\sht\\core\\string.ara",
      ".\\testdata\\sht\\core\\array.ara"
   );
   for(size_t i=0;;i++)
   {
      auto path = cl.getNextArg("");
      if(path.empty())
         break;

      cdwVERBOSE("loading explicit file '%s'\n",path.c_str());
      araceli::loader::findScopeAndLoadFile(*pPrj.get(),path);
   }

   // subsequent link to load more
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // codegen
   cmn::outBundle b;
   { syzygy::codegen v(b,"sa",/*add*/true); pPrj->acceptVisitor(v); }
   { cmn::unconditionalWriter f; b.updateDisk(f); }

   return 0;
}

cdwImplMain()

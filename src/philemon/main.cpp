#include "../araceli/loader.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "../syzygy/codegen.hpp"
#include "arrayDecomposition.hpp"
#include "frontend.hpp"
#include "genericClassInstantiator.hpp"
#include "stringDecomposition.hpp"
#include "symbolTable.hpp"

using namespace philemon;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\philemon");
   std::string inExt = cl.getNextArg("sa");

   // I convert * (arg) -> ph
   araceli::loaderPrefs lPrefs = { inExt, "" };
   cmn::globalPublishTo<araceli::loaderPrefs> _lPrefs(lPrefs,araceli::gLoaderPrefs);

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   frontend(projectDir,pPrj,pTgt).run();

   // load any additional files indicated on the command line
   cl.addNextArgDefaultsIfNoneLeft(3,
      ".\\testdata\\sht\\core\\object.ara.sa",
      ".\\testdata\\sht\\core\\string.ara.sa",
      ".\\testdata\\sht\\core\\array.ara.sa"
   );
   for(size_t i=0;;i++)
   {
      auto path = cl.getNextArg("");
      if(path.empty())
         break;

      cdwVERBOSE("loading explicit file '%s'\n",path.c_str());
      araceli::loader::findScopeAndLoadFile(*pPrj.get(),path);
   }

   // run the instantiator to expand user generics
   // this allows string and array passes to find everything
   classInstantiator().run(*pPrj.get());

   // transform native string type to class
   { stringDecomposition v; pPrj->acceptVisitor(v); v.run(); }

   // transform native array type to class
   { arrayDecomposition v; pPrj->acceptVisitor(v); v.run(*pPrj.get()); }

   // subsequent link to load more
   nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // run the instantiator a second time, to implement arrays
   classInstantiator().run(*pPrj.get());

   // generics are now done; remove them
   { genericStripper v; pPrj->acceptVisitor(v); }

   // codegen
   cmn::outBundle b;
   { syzygy::codegen v(b,"ph",/*replace*/false); pPrj->acceptVisitor(v); }
   { cmn::unconditionalWriter f; b.updateDisk(f); }

   return 0;
}

cdwImplMain()

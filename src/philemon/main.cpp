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
   cmn::outBundle dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);

   // I convert * (arg) -> ph
   araceli::loaderPrefs lPrefs = { inExt, "" };
   cmn::globalPublishTo<araceli::loaderPrefs> _lPrefs(lPrefs,araceli::gLoaderPrefs);

   // setup project, target, AST; load & link
   cmn::rootNodeDeleteOperation _rdo;
   cmn::globalPublishTo<cmn::rootNodeDeleteOperation> _rdoRef(_rdo,cmn::gNodeDeleteOp);
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   cmn::astExceptionBarrier<cmn::araceliProjectNode> _aeb(pPrj,dbgOut,projectDir + "\\");
   std::unique_ptr<araceli::iTarget> pTgt;
   frontend(projectDir,pPrj,pTgt).run();

   // load any additional files indicated on the command line
   cl.addNextArgDefaultsIfNoneLeft(1,
      ".\\testdata\\sht\\core"
   );
   for(size_t i=0;;i++)
   {
      auto path = cl.getNextArg("");
      if(path.empty())
         break;

      cdwVERBOSE("loading explicit file '%s'\n",path.c_str());
      araceli::loader::findScopeAndLoadFolder(*pPrj.get(),path);
   }

   // run the instantiator to expand user generics
   // this allows string and array passes to find everything
   classInstantiator().run(*pPrj.get());

   // transform native string type to class
   { stringDecomposition v; pPrj->acceptVisitor(v); v.run(); }

   // transform native array type to class
   { arrayDecomposition v; pPrj->acceptVisitor(v); v.run(*pPrj.get()); }

   // subsequent link for new files/instances
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

   // clear graph
   { cmn::autoNodeDeleteOperation o; pPrj.reset(); }
   _aeb.disarm();

   return 0;
}

cdwImplMain()

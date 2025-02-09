#include "../araceli/iTarget.hpp"
#include "../araceli/loader.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/userError.hpp"
#include "../syzygy/codegen.hpp"
#include "frontend.hpp"
#include "intfTransform.hpp"
#include "nodeFlagChecker.hpp"
#include "symbolTable.hpp"

using namespace salome;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\test");
   std::string inExt = cl.getNextArg("ara");
   cmn::outBundle dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);

   // I convert * (arg) -> sa
   araceli::loaderPrefs lPrefs = { inExt, ".target.ara" };
   cmn::globalPublishTo<araceli::loaderPrefs> _lPrefs(lPrefs,araceli::gLoaderPrefs);

   // setup transform state and userErrors
   intfTransformState _s1;
   cmn::globalPublishTo<intfTransformState> _s1Ref(_s1,intfTransform::gState);
   cmn::userErrors ue;
   cmn::globalPublishTo<cmn::userErrors> _ueReg(ue,cmn::gUserErrors);

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

      cdwVERBOSE("loading explicit folder '%s'\n",path.c_str());
      araceli::loader::findScopeAndLoadFolder(*pPrj.get(),path);
   }

   // subsequent link to load more
   nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // syntax-checking / explicit-defaulting transforms
   { intfTransform v; pPrj->acceptVisitor(v); }
   { nodeFlagChecker v; pPrj->acceptVisitor(v); }

   // report errors as late as possible
   ue.throwIfAny();

   // codegen
   cmn::outBundle b;
   { syzygy::codegen v(b,"sa",/*add*/true); pPrj->acceptVisitor(v); }
   { cmn::unconditionalWriter f; b.updateDisk(f); }

   // clear graph
   cdwDEBUG("destroying the graph\r\n");
   { cmn::autoNodeDeleteOperation o; pPrj.reset(); }
   _aeb.disarm();

   return 0;
}

cdwImplMain()

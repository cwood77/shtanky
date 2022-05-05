#ifdef  cdwTraceContext
#undef  cdwTraceContext
#define cdwTraceContext "main()"
#endif

#include "../cmn/cmdline.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "../syzygy/frontend.hpp"
#include "abstractGenerator.hpp"
#include "batGen.hpp"
#include "classInfo.hpp"
#include "codegen.hpp"
#include "constHoister.hpp"
#include "ctorDtorGenerator.hpp"
#include "inheritImplementor.hpp"
#include "loader.hpp"
#include "matryoshkaDecomp.hpp"
#include "methodMover.hpp"
#include "selfDecomposition.hpp"
#include "stackClassDecomposition.hpp"
#include "symbolTable.hpp"
#include "vtableGenerator.hpp"
#include <string.h>

#include "metadata.hpp"
#include "objectBaser.hpp"

using namespace araceli;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cmn::pathUtil::toWindows(cl.getNextArg(".\\testdata\\test"));
   std::string batchBuild = projectDir + "\\.build.bat";

   // invoke philemon
   {
      std::stringstream childStream;
      childStream << "bin\\out\\debug\\philemon.exe ";
      childStream << projectDir;
      childStream << " ara";
      childStream << " .\\testdata\\sht\\core\\object.ara";
      cdwVERBOSE("calling: %s\n",childStream.str().c_str());
      ::_flushall();
      int rval = ::system(childStream.str().c_str());
      cdwDEBUG("*************************************************\n");
      cdwDEBUG("**   returned to araceli\n");
      cdwDEBUG("*************************************************\n");
      cdwVERBOSE("rval = %d\n",rval);
      if(rval != 0)
      {
         cdwINFO("philemon failed with code %d; aborting\n",rval);
         cdwTHROW("child process failed");
      }
   }

   // I convert ph -> ara.lh/ls
   loaderPrefs lPrefs = { "ph", "" };
   cmn::globalPublishTo<loaderPrefs> _lPrefs(lPrefs,gLoaderPrefs);

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   syzygy::frontend(projectDir,pPrj,pTgt).run();

   // gather metadata
   araceli::metadata md;
   {
      araceli::nodeMetadataBuilder inner(md);
      cmn::treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // use metadata to generate the target
   pTgt->araceliCodegen(*pPrj,md);

   // inject implied base class
   { araceli::objectBaser v; pPrj->acceptVisitor(v); }

   // subsequent link to update with new target and load more
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // capture class info
   classCatalog cc;
   { classInfoBuilder v(cc); pPrj->acceptVisitor(v); }
   cmn::outBundle dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);
   {
      classInfoFormatter fmt(dbgOut.get<cmn::outStream>(
         projectDir + "\\.classInfo"));
      fmt.format(cc);
   }

   // ---------------- lowering transforms ----------------

   // hoist out constants
   { araceli::constHoister v; pPrj->acceptVisitor(v); }
   cdwVERBOSE("graph after const hoist ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // --- compile-away classes ---
   { ctorDtorGenerator v; pPrj->acceptVisitor(v); }  // write cctor/cdtor
   { abstractGenerator::generate(cc); }              // implement pure virtual functions
   { selfDecomposition v; pPrj->acceptVisitor(v); }  // add self param, scope self fields,
                                                     //   invoke decomp
   // basecall decomp
   { methodMover v(cc); pPrj->acceptVisitor(v); }    // make methods functions
   { vtableGenerator().generate(cc); }               // add vtable class and global instance
   { inheritImplementor().generate(cc); }            // inject fields into derived classes
   { matryoshkaDecomposition(cc).run(); }            // write sctor/sdtor
   { stackClassDecomposition v;                      // inject sctor/sdtor calls for stack
     pPrj->acceptVisitor(v); v.inject(); }           //   allocated classes
   cdwVERBOSE("graph after declassing transforms ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // -----------------------------------------------------

   // codegen
   araceli::nodeLinker().linkGraph(*pPrj); // relink so codegen makes more fileRefs
   cmn::outBundle out;
   { codeGen v(*pTgt.get(),out); pPrj->acceptVisitor(v); }
   { batGen v(*pTgt.get(),out.get<cmn::outStream>(batchBuild)); pPrj->acceptVisitor(v); }
   out.updateDisk(wr);

   return 0;
}

cdwImplMain()

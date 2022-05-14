#ifdef  cdwTraceContext
#undef  cdwTraceContext
#define cdwTraceContext "main()"
#endif

#include "../cmn/cmdline.hpp"
#include "../cmn/main.hpp"
#include "../cmn/out.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/typeVisitor.hpp"
#include "../cmn/userError.hpp"
#include "../syzygy/frontend.hpp"
#include "abstractGenerator.hpp"
#include "batGen.hpp"
#include "classInfo.hpp"
#include "codegen.hpp"
#include "constHoister.hpp"
#include "ctorDtorGenerator.hpp"
#include "inheritImplementor.hpp"
#include "intfTransform.hpp"
#include "loader.hpp"
#include "matryoshkaDecomp.hpp"
#include "metadata.hpp"
#include "methodMover.hpp"
#include "nodeFlagChecker.hpp"
#include "objectBaser.hpp"
#include "opOverloadDecomp.hpp"
#include "selfDecomposition.hpp"
#include "stackClassDecomposition.hpp"
#include "symbolTable.hpp"
#include "vtableGenerator.hpp"
#include <string.h>

using namespace araceli;

void invokeSubProcess(const char *shortName, const char *ext, const std::string& projectDir)
{
   std::stringstream childStream;
   childStream << "bin\\out\\debug\\" << shortName << ".exe ";
   childStream << projectDir;
   childStream << " " << ext;
   childStream << " .\\testdata\\sht\\core\\object.ara";
   childStream << " .\\testdata\\sht\\core\\string.ara";
   childStream << " .\\testdata\\sht\\core\\array.ara";
   cdwVERBOSE("calling: %s\n",childStream.str().c_str());
   ::_flushall();
   int rval = ::system(childStream.str().c_str());
   cdwDEBUG("*************************************************\n");
   cdwDEBUG("**   returned to araceli\n");
   cdwDEBUG("*************************************************\n");
   cdwVERBOSE("rval = %d\n",rval);
   if(rval != 0)
   {
      cdwINFO("%s failed with code %d; aborting\n",shortName,rval);
      cdwTHROW("child process failed");
   }
}

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cmn::pathUtil::toWindows(cl.getNextArg(".\\testdata\\test"));
   std::string batchBuild = projectDir + "\\.build.bat";

   // invoke philemon
   invokeSubProcess("philemon","ara",projectDir);

   // I convert ph -> ara.lh/ls
   loaderPrefs lPrefs = { "ph", "" };
   cmn::globalPublishTo<loaderPrefs> _lPrefs(lPrefs,gLoaderPrefs);

   // setup project, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   syzygy::frontend(projectDir,pPrj,pTgt).run();

   // gather metadata
   metadata md;
   {
      nodeMetadataBuilder inner(md);
      cmn::treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // use metadata to generate the target
   pTgt->araceliCodegen(*pPrj,md);

   // ---------------- early syntax checks ----------------
   cmn::userErrors ue;
   cmn::globalPublishTo<cmn::userErrors> _ueReg(ue,cmn::gUserErrors);

   // interface has only public abstract methods
   { intfTransform v; pPrj->acceptVisitor(v); }

   { nodeFlagChecker v; pPrj->acceptVisitor(v); }

   // some possible checks:
   // abstract classes can't be instantiated
   // protected members cannot be accessed inappropriately
   // private members cannot be accessed inappropriately
   // method calls have the right parameters
   // multiple classes with same name
   // multiple methods with same name

   ue.throwIfAny();

   // inject implied base class
   { objectBaser v; pPrj->acceptVisitor(v); }

   // subsequent link to update with new target and load more
   nodeLinker().linkGraph(*pPrj);
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
   ue.throwIfAny();

   // ---------------- lowering transforms ----------------

   {
      // type prop
      cmn::type::table                           _t;
      cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
      cmn::type::nodeCache                       _c;
      cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
      cmn::propagateTypes(*pPrj.get());

      // op overloader
      { opOverloadDecomp v; pPrj->acceptVisitor(v); }
   }
   cdwVERBOSE("graph after op overloading decomp ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // hoist out constants
   { constHoister v; pPrj->acceptVisitor(v); }
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
   nodeLinker().linkGraph(*pPrj); // relink so codegen makes more fileRefs
   cmn::outBundle out;
   { codeGen v(*pTgt.get(),out); pPrj->acceptVisitor(v); }
   { batGen v(*pTgt.get(),out.get<cmn::outStream>(batchBuild)); pPrj->acceptVisitor(v); }
   out.updateDisk(wr);

   return 0;
}

cdwImplMain()

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
#include "accessChecker.hpp"
#include "batGen.hpp"
#include "classInfo.hpp"
#include "codegen.hpp"
#include "constHoister.hpp"
#include "ctorDtorGenerator.hpp"
#include "inheritImplementor.hpp"
#include "loader.hpp"
#include "matryoshkaDecomp.hpp"
#include "metadata.hpp"
#include "methodMover.hpp"
#include "objectBaser.hpp"
#include "opOverloadDecomp.hpp"
#include "selfDecomposition.hpp"
#include "stackClassDecomposition.hpp"
#include "subprocess.hpp"
#include "symbolTable.hpp"
#include "vtableGenerator.hpp"
#include <string.h>

using namespace araceli;

int _main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cmn::pathUtil::toWindows(cl.getNextArg(".\\testdata\\test"));
   std::string batchBuild = projectDir + "\\.build.bat";
   cmn::outBundle dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);

   // invoke salome
   invokeAraceliSliceProcess("salome",projectDir,"ara") // TODO can philemon call salome?
      .withArg(".\\testdata\\sht\\core")
      .withArg(".\\testdata\\sht\\cons")
      .runAndWait();

   // TODO - unneeded files should not get pulled in by symbol table
   //      - salome must parse generics enough to know what files to load...?
   //
   // invoke philemon
   invokeAraceliSliceProcess("philemon",projectDir,"sa")
      .withArg(".\\testdata\\sht\\core")
      .withArg(".\\testdata\\sht\\cons")
      .runAndWait();

   // I convert ph -> ara.lh/ls
   loaderPrefs lPrefs = { "ph", "" };
   cmn::globalPublishTo<loaderPrefs> _lPrefs(lPrefs,gLoaderPrefs);

   // setup project, AST; load & link
   cmn::rootNodeDeleteOperation _rdo;
   cmn::globalPublishTo<cmn::rootNodeDeleteOperation> _rdoRef(_rdo,cmn::gNodeDeleteOp);
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   cmn::astExceptionBarrier<cmn::araceliProjectNode> _aeb(pPrj,dbgOut,projectDir + "\\");
   std::unique_ptr<araceli::iTarget> pTgt;
   syzygy::frontend(projectDir,pPrj,pTgt).run();
   { auto& s = dbgOut.get<cmn::outStream>(projectDir + "\\.00init.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

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
   // some possible checks:
   // abstract classes can't be instantiated
   // protected members cannot be accessed inappropriately
   // private members cannot be accessed inappropriately
   // method calls have the right parameters
   // multiple classes with same name
   // multiple methods with same name

   // inject implied base class
   { objectBaser v; pPrj->acceptVisitor(v); }

   // subsequent link to update with new target and load more
   nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(projectDir + "\\.01postlink.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

   // capture class info
   classCatalog cc;
   { classInfoBuilder v(cc); pPrj->acceptVisitor(v); }
   {
      classInfoFormatter fmt(dbgOut.get<cmn::outStream>(
         projectDir + "\\.classInfo"));
      fmt.format(cc);
   }

   {
      // type prop
      cmn::type::table                           _t;
      cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
      cmn::type::nodeCache                       _c;
      cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
      cmn::propagateTypes(*pPrj.get());

      // op overloader
      { opOverloadDecomp v; pPrj->acceptVisitor(v); }
      cdwVERBOSE("graph after op overloading decomp ----\n");
      { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

      // ---------------- syntax checking ----------------
      nodeLinker().linkGraph(*pPrj); // make sure all invokes are linked
      { memberAccessChecker v(cc); pPrj->acceptVisitor(v); }
      ue.throwIfAny();
   }

   // ---------------- lowering transforms ----------------

   // hoist out constants
   { constHoister v; pPrj->acceptVisitor(v); }
   cdwVERBOSE("graph after const hoist ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(projectDir + "\\.02preDeclass.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

   // --- compile-away classes ---
   { ctorDtorGenerator v; pPrj->acceptVisitor(v); } // write cctor/cdtor
   { abstractGenerator::generate(cc); }             // implement pure virtual functions
   { selfDecomposition v; pPrj->acceptVisitor(v); } // add self param, scope self fields,
                                                    //   invoke decomp
   // basecall decomp
   { cmn::autoNodeDeleteOperation o;                // make methods functions
     methodMover v(cc); pPrj->acceptVisitor(v); }
   { vtableGenerator().generate(cc); }              // add vtable class and global instance
   { inheritImplementor().generate(cc); }           // inject fields into derived classes
   { matryoshkaDecomposition(cc).run(); }           // write sctor/sdtor
   { stackClassDecomposition v;                     // inject sctor/sdtor calls for stack
     pPrj->acceptVisitor(v); v.inject(); }          //   allocated classes
   cdwVERBOSE("graph after declassing transforms ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(projectDir + "\\.03postDeclass.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

   // -----------------------------------------------------

   // codegen
   nodeLinker().linkGraph(*pPrj); // relink so codegen makes more fileRefs
   cmn::outBundle out;
   { codeGen v(*pTgt.get(),out); pPrj->acceptVisitor(v); }
   { batGen v(*pTgt.get(),out.get<cmn::outStream>(batchBuild)); pPrj->acceptVisitor(v); }
   out.updateDisk(wr);

   // clear graph
   cdwDEBUG("destroying the graph\r\n");
   { cmn::autoNodeDeleteOperation o; pPrj.reset(); }
   _aeb.disarm();

   return 0;
}

cdwImplMain()

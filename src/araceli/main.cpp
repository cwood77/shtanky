#ifdef  cdwTraceContext
#undef  cdwTraceContext
#define cdwTraceContext "main()"
#endif

#include "../cmn/cmdline.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "abstractGenerator.hpp"
#include "batGen.hpp"
#include "classInfo.hpp"
#include "codegen2.hpp"
#include "consoleAppTarget.hpp"
#include "constHoister.hpp"
#include "ctorDtorGenerator.hpp"
#include "inheritImplementor.hpp"
#include "matryoshkaDecomp.hpp"
#include "metadata.hpp"
#include "methodMover.hpp"
#include "objectBaser.hpp"
#include "projectBuilder.hpp"
#include "selfDecomposition.hpp"
#include "symbolTable.hpp"
#include "vtableGenerator.hpp"
#include <string.h>

using namespace araceli;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\test");
   std::string batchBuild = projectDir + "\\.build.bat";

   // setup project / target
   std::unique_ptr<cmn::araceliProjectNode> pPrj = projectBuilder::create("ca");
   projectBuilder::addScope(*pPrj.get(),projectDir,/*inProject*/true);
   consoleAppTarget tgt;
   tgt.addAraceliStandardLibrary(*pPrj.get());
   tgt.populateIntrinsics(*pPrj.get());
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // initial link to discover and load everything
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // gather metadata
   metadata md;
   {
      nodeMetadataBuilder inner(md);
      cmn::treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // use metadata to generate the target
   tgt.araceliCodegen(*pPrj,md);

   // inject implied base class
   { objectBaser v; pPrj->acceptVisitor(v); }

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
   { selfDecomposition v; pPrj->acceptVisitor(v); }  // add self param, scope self fields, invoke decomp
   // basecall decomp
   { methodMover v(cc); pPrj->acceptVisitor(v); }    // make methods functions
   { vtableGenerator().generate(cc); }               // add vtable class and global instance
   { inheritImplementor().generate(cc); }            // inject fields into derived classes
   { matryoshkaDecomposition(cc).run(); }            // write sctor/sdtor
   // stack new/delete decomp
   cdwVERBOSE("graph after declassing transforms ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // -----------------------------------------------------

   // final link so fileRefs are as accurate as possible in codegen
   try
   {
      araceli::nodeLinker().linkGraph(*pPrj);
   }
   catch(std::exception&)
   {
      cdwVERBOSE("graph during throw ----\n");
      { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
      throw;
   }
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // codegen
   cmn::outBundle out;
   { araceli2::codeGen v(tgt,out); pPrj->acceptVisitor(v); }
   { batGen v(tgt,out.get<cmn::outStream>(batchBuild)); pPrj->acceptVisitor(v); }
   out.updateDisk(wr);

   return 0;
}

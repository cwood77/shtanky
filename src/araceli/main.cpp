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
#include "codegen.hpp"
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
   bool exp = cl.getSwitch("--exp","",true);
   std::string projectDir = cl.getNextArg(".\\testdata\\test");
   std::string batchBuild = projectDir + "\\.build.bat";

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
   if(exp){ objectBaser v; pPrj->acceptVisitor(v); }

   // subsequent link to update with new target
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // capture class info
   classCatalog cc;
if(exp) {
   { classInfoBuilder v(cc); pPrj->acceptVisitor(v); }
   cmn::outBundle dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);
   {
      classInfoFormatter fmt(dbgOut.get<cmn::outStream>(
         projectDir + "\\.classInfo"));
      fmt.format(cc);
   }
} // exp

   // ---------------- lowering transforms ----------------

   // hoist out constants
   { araceli::constHoister v; pPrj->acceptVisitor(v); }
   cdwVERBOSE("graph after const hoist ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // compile-away classes
   if(exp){ ctorDtorGenerator v; pPrj->acceptVisitor(v); }
   if(exp){ abstractGenerator::generate(cc); }
   // self param, self fields, invoke decomp
   { selfDecomposition v; pPrj->acceptVisitor(v); } // TODO some invokes should turn into calls
   // TODO basecall decomp
   if(exp){ methodMover v(cc); pPrj->acceptVisitor(v); }
   if(exp) { vtableGenerator().generate(cc); }
   if(exp) { inheritImplementor().generate(cc); }
   // TODO matryoshka decomp
   if(exp) { matryoshkaDecomposition(cc).run(); }
   // TODO stack new/delete decomp
   cdwVERBOSE("graph after transforms ----\n");
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
if(exp)
   { araceli2::codeGen v(tgt,out); pPrj->acceptVisitor(v); }
else
   { codeGen v(out); pPrj->acceptVisitor(v); }
   { batGen v(tgt,out.get<cmn::outStream>(batchBuild)); pPrj->acceptVisitor(v); }
   cmn::unconditionalWriter wr;
   out.updateDisk(wr);

   return 0;
}

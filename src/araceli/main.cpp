#ifdef  cdwTraceContext
#undef  cdwTraceContext
#define cdwTraceContext "main()"
#endif

#include "../cmn/cmdline.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "batGen.hpp"
#include "codegen.hpp"
#include "consoleAppTarget.hpp"
#include "constHoister.hpp"
#include "ctorDtorGenerator.hpp"
#include "declasser.hpp"
#include "metadata.hpp"
#include "projectBuilder.hpp"
#include "selfDecomposition.hpp"
#include "symbolTable.hpp"
#include <string.h>

using namespace araceli;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getArg(".\\testdata\\test");
   std::string batchBuild = projectDir + "\\.build.bat";

   std::unique_ptr<cmn::araceliProjectNode> pPrj = projectBuilder::create("ca");
   projectBuilder::addScope(*pPrj.get(),projectDir,/*inProject*/true);
   projectBuilder::addScope(*pPrj.get(),".\\testdata\\sht",/*inProject*/false);
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
   consoleAppTarget().codegen(*pPrj,md);

   // subsequent link to update with new target
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // ---------------- lowering transforms ----------------

   // hoist out constants
   { araceli::constHoister v; pPrj->acceptVisitor(v); }
   cdwVERBOSE("graph after const hoist ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

#if 0
   // ctor/dtor stuff
   { ctorDtorGenerator v; pPrj->acceptVisitor(v); }
   cdwVERBOSE("graph after transforms ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
#endif

   // compile-away classes
   { selfDecomposition v; pPrj->acceptVisitor(v); }
   { declasser v; pPrj->acceptVisitor(v); }
   cdwVERBOSE("graph after transforms ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // -----------------------------------------------------

   // codegen
   cmn::outBundle out;
   codeGen v(out);
   pPrj->acceptVisitor(v);
   { batGen v(out.get<cmn::outStream>(batchBuild)); pPrj->acceptVisitor(v); }
   cmn::unconditionalWriter wr;
   out.updateDisk(wr);

   return 0;
}

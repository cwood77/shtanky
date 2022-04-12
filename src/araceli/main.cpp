#include "../cmn/cmdline.hpp"
#include "../cmn/out.hpp"
#include "batGen.hpp"
#include "codegen.hpp"
#include "constHoister.hpp"
#include "declasser.hpp"
#include "metadata.hpp"
#include "projectBuilder.hpp"
#include "symbolTable.hpp"
#include <stdio.h>
#include <string.h>

using namespace araceli;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string batchBuild = cl.getArg(".\\testdata\\test\\.build.bat");

   std::unique_ptr<cmn::araceliProjectNode> pPrj = projectBuilder::create("ca");
   projectBuilder::addScope(*pPrj.get(),"testdata\\test",/*inProject*/true);
   projectBuilder::addScope(*pPrj.get(),"testdata\\sht",/*inProject*/false);
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   araceli::nodeLinker().linkGraph(*pPrj);
   ::printf("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   { araceli::constHoister v; pPrj->acceptVisitor(v); }
   ::printf("graph after const hoist ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // gather metadata
   metadata md;
   {
      nodeMetadataBuilder inner(md);
      cmn::treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // transforms
   { declasser v; pPrj->acceptVisitor(v); }
   ::printf("graph after transforms ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // TODO file codegen
   // -> header file
   // -> source file
   // -> make file

   // target codegen
   cmn::outBundle out;
   cmn::fileWriter wr;
   out.setAutoUpdate(wr);
   codeGen v(out);
   pPrj->acceptVisitor(v);

   // build codegen
   {
      batGen v(out.get<cmn::outStream>(batchBuild));
      pPrj->acceptVisitor(v);
   }

   return 0;
}

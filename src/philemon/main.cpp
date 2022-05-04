#include "../araceli/loader.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "../syzygy/codegen.hpp"
#include "frontend.hpp"
#include "genericClassInstantiator.hpp"

#include "windows.h"

using namespace philemon;

int main(int argc, const char *argv[])
{
   cdwDEBUG("FAIL!\n");
   ::Sleep(40*1000);
   return -2;

   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\philemon");

   // setup load infix
   std::string infix = "1-sa";
   cmn::globalPublishTo<std::string> _infix(infix,araceli::gLastSupportedInfix);

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   frontend(projectDir,pPrj,pTgt).run();

   // run the instantiator, so it's guaranteed at least one run
   classInstantiator().run(*pPrj.get());
   { genericStripper v; pPrj->acceptVisitor(v); }

   // write
   cmn::outBundle b;
   { syzygy::codegen v(b,"2-ph"); pPrj->acceptVisitor(v); }
   { cmn::unconditionalWriter f; b.updateDisk(f); }

   return 0;
}

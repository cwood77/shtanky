#include "../cmn/cmdline.hpp"
#include "frontend.hpp"
#include "genericClassInstantiator.hpp"

using namespace philemon;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\philemon");

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   frontend(projectDir,pPrj,pTgt).run();

   // run the instantiator, so it's guaranteed at least one run
   classInstantiator().run(*pPrj.get());

   return 0;
}

#include "../cmn/cmdline.hpp"
#include "../syzygy/frontend.hpp"

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string projectDir = cl.getNextArg(".\\testdata\\test");

   // setup project, target, AST; load & link
   std::unique_ptr<cmn::araceliProjectNode> pPrj;
   std::unique_ptr<araceli::iTarget> pTgt;
   syzygy::frontend::run(projectDir,pPrj,pTgt);

   return 0;
}

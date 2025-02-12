#include "../cmn/cmdline.hpp"
#include "../cmn/throw.hpp"
#include "instr.hpp"
#include "scriptWriter.hpp"
#include "test.hpp"
#include <fstream>

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   bool skipChecks = cl.getSwitch("-skipchecks","",false); // used for bootstrapping new tests
   bool verbose = cl.getSwitch("-verbose","",false);       // include prog output in script
   bool subset = cl.getSwitch("!","",false);               // run a subset of tests
   bool release = cl.getSwitch("rel","",false);            // run against release binaries

   script s(/*silentExes*/ !verbose, !release);

   // ------------------ tests start ------------------

   testWriter(s,subset,cl).add("lats",[](auto& is){
      liamTest(is,".\\testdata\\lats\\main.ls")
         .runFullBuildStack(".\\testdata\\lats\\main.ls.app")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).skipByDefault("uats",[](auto& is){
      araceliTest(is,".\\testdata\\uats")
         .runFullBuildStack()
         .expectLiamOf(".\\testdata\\uats\\if.ara")
         .expectLiamOf(".\\testdata\\uats\\loop.ara")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).add("assign",[](auto& is){
      araceliTest(is,".\\testdata\\assign")
         .expectLiamOf(".\\testdata\\assign\\main.ara")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.00init.ast","init AST")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.01postlink.ast","postlink AST")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.02preDeclass.ast","preDeclass AST")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.03postDeclass.ast","postDeclass AST")
      ;
   });

   testWriter(s,subset,cl).add("philemon",[](auto& is){
      araceliTest(is,".\\testdata\\philemon")
         .expectLiamOf(".\\testdata\\philemon\\list.ara")
      ;
   });

   testWriter(s,subset,cl).add("test",[](auto& is){
      araceliTest(is,".\\testdata\\test")
         .runFullBuildStack()
         .expectLiamOf(".\\testdata\\test\\test.ara")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).add("nostromo",[](auto& is){
      araceliTest(is,".\\testdata\\nostromo")
         .runFullBuildStack()
         .expectLiamOf(".\\testdata\\nostromo\\boot.ara")
         .expectLiamOf(".\\testdata\\nostromo\\stringout.ara")
         .expectLiamOf(".\\testdata\\nostromo\\uart16550\\driver.ara")
      ;
   });

   // ------------------ dev sandboxes ------------------

   testWriter(s,subset,cl).skipByDefault("shtemu",[](auto& is){
      shtasmTest(is,".\\testdata\\shtemu\\basic.asm")
         .runFullBuildStack(".\\testdata\\shtemu\\basic.app")
         .emulateAndCheckOutput()
      ;
   });

   // another sandbox test for experimenting with Win64 ABI
   testWriter(s,subset,cl).add("shtemu.s",[](auto& is){
      shtasmTest(is,".\\testdata\\shtemu\\simple.asm")
         .runFullBuildStack(".\\testdata\\shtemu\\simple.app")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).skipByDefault("shtemu.ls",[](auto& is){
      liamTest(is,".\\testdata\\shtemu\\basic.ls")
         .runFullBuildStack(".\\testdata\\shtemu\\basic.ls.app")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).skipByDefault("shtemu.ara",[](auto& is){
      araceliTest(is,".\\testdata\\shtemu")
         .runFullBuildStack()
         .expectLiamOf(".\\testdata\\shtemu\\basic.ara")
         .emulateAndCheckOutput()
      ;
   });

   // ------------------ end ------------------

   std::ofstream wrapper(".\\bin\\.appr.bat");
   if(!wrapper.good())
      cdwTHROW("can't open file for writing");
   scriptWriter::run(s,wrapper,skipChecks);
}

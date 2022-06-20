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
   instrStream is(s);

   // ------------------ tests start ------------------

   testWriter(s,subset,cl).add("uats",[](auto& is){
      araceliTest(is,".\\testdata\\uats")
         .wholeApp()
         .expectLiamOf(".\\testdata\\uats\\if.ara")
         .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
         .expectLiamOf(".\\testdata\\uats\\.target.ara",false) // false = no philemon
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).add("assign",[](auto& is){
      araceliTest(is,".\\testdata\\assign")
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
         .wholeApp()
         .expectLiamOf(".\\testdata\\test\\test.ara")
         .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
         .expectLiamOf(".\\testdata\\test\\.target.ara",false) // false = no philemon
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).add("nostromo",[](auto& is){
      araceliTest(is,".\\testdata\\nostromo")
         .wholeApp()
         .expectLiamOf(".\\testdata\\nostromo\\boot.ara")
         .expectLiamOf(".\\testdata\\nostromo\\stringout.ara")
         .expectLiamOf(".\\testdata\\nostromo\\uart16550\\driver.ara")
         .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
         .expectLiamOf(".\\testdata\\nostromo\\.target.ara",false) // false = no philemon
      ;
   });

   // ------------------ dev sandboxes ------------------

   testWriter(s,subset,cl).skipByDefault("shtemu",[](auto& is){
      shtasmTest(is,".\\testdata\\shtemu\\basic.asm")
         .wholeApp(".\\testdata\\shtemu\\basic.app")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).skipByDefault("shtemu.ls",[](auto& is){
      liamTest(is,".\\testdata\\shtemu\\basic.ls")
         .wholeApp(".\\testdata\\shtemu\\basic.ls.app")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).skipByDefault("shtemu.ara",[](auto& is){
      araceliTest(is,".\\testdata\\shtemu")
         .wholeApp()
         .expectLiamOf(".\\testdata\\shtemu\\basic.ara")
         .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\loopInst.ara")
         .expectLiamOf(".\\testdata\\shtemu\\.target.ara",false) // false = no philemon
         .emulateAndCheckOutput()
      ;
   });

   // ------------------ end ------------------

   std::ofstream wrapper(".\\bin\\.appr.bat");
   if(!wrapper.good())
      cdwTHROW("can't open file for writing");
   scriptWriter::run(s,wrapper,skipChecks);
}

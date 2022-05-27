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

   testWriter(s,subset,cl).add("assign",[](auto& is){
      araceliTest(is,".\\testdata\\assign")
         .wholeApp()
         .verifyAdditionalArtifact(".\\testdata\\assign\\.00init.ast","init AST")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.01postlink.ast","postlink AST")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.02preDeclass.ast","preDeclass AST")
         .verifyAdditionalArtifact(".\\testdata\\assign\\.03postDeclass.ast","postDeclass AST")
         .expectLiamOf(".\\testdata\\assign\\main.ara")
         .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
         .expectLiamOf(".\\testdata\\assign\\.target.ara",false)
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

   testWriter(s,subset,cl).add("philemon",[](auto& is){
      araceliTest(is,".\\testdata\\philemon")
         .expectLiamOf(".\\testdata\\philemon\\list.ara")
      ;
   });

   testWriter(s,subset,cl).add("shtemu.ls",[](auto& is){
      liamTest(is,".\\testdata\\shtemu\\basic.ls")
         .wholeApp(".\\testdata\\shtemu\\basic.ls.app")
         .emulateAndCheckOutput()
      ;
   });
   testWriter(s,subset,cl).add("shtemu",[](auto& is){
      shtasmTest(is,".\\testdata\\shtemu\\basic.asm")
         .wholeApp(".\\testdata\\shtemu\\basic.app")
         .emulateAndCheckOutput()
      ;
   });

   testWriter(s,subset,cl).add("test",[](auto& is){
      araceliTest(is,".\\testdata\\test")
         .wholeApp()
         .expectLiamOf(".\\testdata\\test\\test.ara")
         .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
         .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
         .expectLiamOf(".\\testdata\\test\\.target.ara",false) // false = no philemon
      ;
   });

   // ------------------ tests end ------------------

   std::ofstream wrapper(".\\bin\\.appr.bat");
   if(!wrapper.good())
      cdwTHROW("can't open file for writing");
   scriptWriter::run(s,wrapper,skipChecks);
}

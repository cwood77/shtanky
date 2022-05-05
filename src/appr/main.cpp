#include "../cmn/cmdline.hpp"
#include "../cmn/throw.hpp"
#include "instr.hpp"
#include "scriptWriter.hpp"
#include "test.hpp"
#include <fstream>

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   bool skipChecks = (cl.getNextArg("") == "-skipchecks"); // used for bootstrapping new tests
   bool verbose = (cl.getNextArg("") == "-verbose"); // include prog output in script

   script s(/*silentExes*/ !verbose);
   instrStream is(s);

   // ------------------ tests start ------------------

   araceliTest(is,".\\testdata\\test")
      .wholeApp()
      .expectLiamOf(".\\testdata\\test\\test.ara")
      .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
      .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
      .expectLiamOf(".\\testdata\\test\\.target.ara",false) // no philemon
   ;

   araceliTest(is,".\\testdata\\assign")
      .wholeApp()
      .expectLiamOf(".\\testdata\\assign\\main.ara")
      .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
      .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
      .expectLiamOf(".\\testdata\\assign\\.target.ara",false)
   ;

   shtasmTest(is,".\\testdata\\shtemu\\basic.asm")
      .wholeApp(".\\testdata\\shtemu\\basic.app")
      .emulateAndCheckOutput()
   ;

   araceliTest(is,".\\testdata\\nostromo")
      .wholeApp()
      .expectLiamOf(".\\testdata\\nostromo\\boot.ara")
      .expectLiamOf(".\\testdata\\nostromo\\stringout.ara")
      .expectLiamOf(".\\testdata\\nostromo\\uart16550\\driver.ara")
      .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
      .expectLiamOf(".\\testdata\\sht\\core\\object.ara")
      .expectLiamOf(".\\testdata\\nostromo\\.target.ara",false) // no philemon
   ;

   // ------------------ tests end ------------------

   std::ofstream wrapper(".\\bin\\.appr.bat");
   if(!wrapper.good())
      cdwTHROW("can't open file for writing");
   scriptWriter::run(s,wrapper,skipChecks);
}

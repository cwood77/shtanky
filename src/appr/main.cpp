#include "../cmn/cmdline.hpp"
#include "../cmn/throw.hpp"
#include "instr.hpp"
#include "scriptWriter.hpp"
#include "test.hpp"
#include <fstream>

// appr         - run all tests, hyper concise mode
// appr bless   - assume all outputs are now the expected standard
// appr unbless - do a git restore of test outputs

// general methodology
// - sweep over testdata folder and build tests
//   - ara -> lh, ls, bat
//   - ls -> asm
//   - asm -> o, list, mclist
//   - o* -> app, list
//
// can this be inferred with things like shlink and araceli which are N:1 or 1:N?
// maybe require each test to state it's input/outputs?
// maybe just gen batch files

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   bool skipChecks = (cl.getArg("") == "-skipchecks"); // used for bootstrapping new tests
   bool verbose = (cl.getArg("") == "-verbose"); // include prog output in script

   script s(/*silentExes*/ !verbose);
   instrStream is(s);

   araceliTest(is,".\\testdata\\test")
      .wholeApp()
      .expectLiamOf(".\\testdata\\test\\test.ara")
      .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
   ;

   std::ofstream wrapper(".\\bin\\.appr.bat");
   if(!wrapper.good())
      cdwTHROW("can't open file for writing");
   scriptWriter::run(s,wrapper,skipChecks);
}

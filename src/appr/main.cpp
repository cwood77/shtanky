#include "instr.hpp"
#include "scriptWriter.hpp"
#include "test.hpp"
#include <iostream>

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
#if 0
   instrStream s;

   araceliTest(s,".\\testdata\\test")
      .wholeApp()
      .expectLiamOf(".\\testdata\\test\\test.ara")
      .expectLiamOf(".\\testdata\\sht\\cons\\program.ara")
   ;
#endif

   script s;
   instrStream is(s);

   is.appendNew<doInstr>()
      .usingApp("dummy")
      .withArg("foo")
      .withArg("bar")
      .thenCheckReturnValue("dummy");
   is.appendNew<doInstr>()
      .usingApp("dummy")
      .withArg("foo")
      .withArg("bar")
      .thenCheckReturnValue("dummy");
   is.appendNew<compareInstr>()
      .withControl("c")
      .withVariable("v")
      .because("asm output");

   scriptWriter::run(s,std::cout);
}

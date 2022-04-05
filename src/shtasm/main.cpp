#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "frontend.hpp"
#include "processor.hpp"

int main(int argc, const char *argv[])
{
   using namespace shtasm;

   binFileWriter listingFile(".\\testdata\\test\\test.ara.ls.asm.list");

   lexor l(".\\testdata\\test\\test.ara.ls.asm");
   parser p(l);
   cmn::tgt::w64EmuTargetInfo t;
   cmn::objfmt::objFile o;
   processor(p,t,o).setListingFile(listingFile).process();

   return 0;
}

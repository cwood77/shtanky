#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/writer.hpp"
#include "frontend.hpp"
#include "processor.hpp"

int main(int argc, const char *argv[])
{
   using namespace shtasm;

   cmn::binFileWriter listingFile(".\\testdata\\test\\test.ara.ls.asm.o.mclist");

   lexor l(".\\testdata\\test\\test.ara.ls.asm");
   parser p(l);
   cmn::tgt::w64EmuTargetInfo t;
   cmn::objfmt::objFile o;
   processor(p,t,o).setListingFile(listingFile).process();

   {
      cmn::compositeObjWriter w;
      w.sink(
         *new cmn::retailObjWriter(
            *new cmn::binFileWriter(".\\testdata\\test\\test.ara.ls.asm.o")));
      w.sink(
         *new cmn::listingObjWriter(
            *new cmn::binFileWriter(".\\testdata\\test\\test.ara.ls.asm.o.list")));
      o.flatten(w);
   }

   return 0;
}

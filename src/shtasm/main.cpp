#include "../cmn/i64asm.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "frontend.hpp"
#include "processor.hpp"

#include "writer.hpp"

namespace shtasm {

} // namespace shtasm

int main(int argc, const char *argv[])
{
   using namespace shtasm;

   {
      compositeObjWriter w;
      w.sink(
         *new retailObjWriter(
            *new binFileWriter(".\\testdata\\test\\fake.shtasm.o")));
      w.sink(
         *new listingObjWriter(
            *new binFileWriter(".\\testdata\\test\\fake.shtasm.list")));

      lineWriter l(w);
      l.setLineNumber(0);
      {
         l.write("header","cdwe o fmt",10);
         { unsigned long v = 0; l.write("version",&v,sizeof(v)); }
         l.under().nextPart();
      }
   }

   {
      binFileWriter listingFile(".\\testdata\\test\\fake.shtasm.list");

      lexor l(".\\testdata\\test\\fake.shtasm");
      parser p(l);
      cmn::objfmt::objFile o;
      processor(p,o).setListingFile(listingFile).process();
   }

   return 0;
}

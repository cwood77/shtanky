#include "../cmn/cmdline.hpp"
#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/pathUtil.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/writer.hpp"
#include "frontend.hpp"
#include "processor.hpp"

int main(int argc, const char *argv[])
{
   using namespace shtasm;

   cmn::cmdLine cl(argc,argv);
   std::string input = cl.getArg(".\\testdata\\test\\test.ara.ls.asm");

   cmn::binFileWriter listingFile(cmn::pathUtil::addExtension(input,cmn::pathUtil::kExtObj_Then_McList));

   lexor l(input);
   parser p(l);
   cmn::tgt::w64EmuTargetInfo t;
   cmn::objfmt::objFile o;
   processor(p,t,o).setListingFile(listingFile).process();

   {
      cmn::compositeObjWriter w;
      w.sink(
         *new cmn::retailObjWriter(
            *new cmn::binFileWriter(cmn::pathUtil::addExtension(input,cmn::pathUtil::kExtObj))));
      w.sink(
         *new cmn::listingObjWriter(
            *new cmn::binFileWriter(cmn::pathUtil::addExtension(input,cmn::pathUtil::kExtObj_Then_List))));
      o.flatten(w);
   }

   return 0;
}

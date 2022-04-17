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
   std::string inputPath = cl.getArg(".\\testdata\\test\\test.ara.ls.asm");

   std::string oPath = cmn::pathUtil::addExt(inputPath,cmn::pathUtil::kExtObj);
   std::string oListPath = cmn::pathUtil::addExt(oPath,cmn::pathUtil::kExtList);
   std::string mcListPath = cmn::pathUtil::addExt(oPath,cmn::pathUtil::kExtMcList);
   std::string odaListPath = cmn::pathUtil::addExt(oPath,cmn::pathUtil::kExtMcOdaList);

   cmn::binFileWriter listingFile(mcListPath);
   cmn::binFileWriter odaListingFile(odaListPath);

   lineLexor l(inputPath);
   lineParser p(l);
   cmn::tgt::w64EmuTargetInfo t;
   cmn::objfmt::objFile o;
   processor(p,t,o)
      .setListingFile(listingFile)
      .setListingFile(odaListingFile)
      .process();

   {
      cmn::compositeObjWriter w;
      w.sink(
         *new cmn::retailObjWriter(
            *new cmn::binFileWriter(oPath)));
      w.sink(
         *new cmn::listingObjWriter(
            *new cmn::binFileWriter(oListPath)));
      o.flatten(w);
   }

   return 0;
}

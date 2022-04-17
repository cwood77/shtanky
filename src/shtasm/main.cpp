#include "../cmn/binWriter.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/pathUtil.hpp"
#include "frontend.hpp"
#include "processor.hpp"

using namespace shtasm;

int main(int argc, const char *argv[])
{
   cmn::cmdLine cl(argc,argv);
   std::string inputPath = cl.getArg(".\\testdata\\test\\test.ara.ls.asm");

   std::string oPath = cmn::pathUtil::addExt(inputPath,cmn::pathUtil::kExtObj);
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
      w.sinkNewFileWithListing(oPath);
      o.flatten(w);
   }

   return 0;
}

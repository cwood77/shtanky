#include "../cmn/binWriter.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/trace.hpp"
#include "formatter.hpp"
#include "layout.hpp"
#include "shtankyAppTarget.hpp"

namespace shlink {

void shtankyAppSymbolIndex::reportSymbolLocation(const std::string& name, unsigned long finalOffset)
{
   if(name == ".entrypoint")
      entrypointOffset = finalOffset;
   if(name == "._osCall_impl")
      osCallImplOffset = finalOffset;
   if(name == "._getflg_flags")
      flagsOffset = finalOffset;
}

void shtankyAppTarget::readArguments(cmn::cmdLine& cl)
{
   m_appFilePath = cl.getNextArg(".\\testdata\\test\\test.ara.ls.asm.o.app");
}

void shtankyAppTarget::seedRequirements(layoutProgress& lProg)
{
   lProg.seedRequiredObject(".entrypoint");
   lProg.seedRequiredObject("._getflg_flags");
}

void shtankyAppTarget::write(const layout& l, iSymbolIndex& x)
{
   cdwVERBOSE("writing to %s\n",m_appFilePath.c_str());

   cmn::compositeObjWriter w;
   w.sinkNewFileWithListing(m_appFilePath);

   appFormatter fmt(w);
   fmt.write(l,x);
}

} // namespace shlink

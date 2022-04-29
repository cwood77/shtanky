#include "../cmn/cmdline.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "iTarget.hpp"
#include "shtankyAppTarget.hpp"

namespace shlink {

iTarget *createTarget(cmn::cmdLine& cl)
{
   std::string name = cl.getOption("target","shtanky-app");
   if(name != "shtanky-app")
      cdwTHROW("unknown target");

   cdwVERBOSE("using target shtankyAppTarget\n");
   iTarget *pT = new shtankyAppTarget();
   pT->readArguments(cl);
   return pT;
}

} // namespace shlink

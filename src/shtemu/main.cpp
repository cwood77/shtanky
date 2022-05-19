#include "../cmn/cmdline.hpp"
#include "../cmn/main.hpp"
#include "../cmn/trace.hpp"
#include "loader.hpp"
#include "osCall.hpp"

using namespace shtemu;

int _main(int argc, const char *argv[])
{
   cdwINFO("win64 shtanky emulator\r\n");
   cmn::cmdLine cl(argc,argv);
   std::string appPath = cl.getNextArg(".\\testdata\\shtemu\\basic.app");

   cdwINFO("loading file %s\r\n",appPath.c_str());
   windowsAllocator alloc;
   loader l;
   std::unique_ptr<image> pImage(l.loadFile(alloc,appPath));
   pImage->patch(&osCallThunk::thunk);

   cdwINFO("passing control to app\r\n");
   pImage->findEntrypoint()(0);
   cdwINFO("control returned to shtemu\r\n");

   return 0;
}

cdwImplMain();

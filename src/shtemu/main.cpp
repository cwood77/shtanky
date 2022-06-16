#include "../cmn/cmdline.hpp"
#include "../cmn/main.hpp"
#include "../cmn/trace.hpp"
#include "loader.hpp"
#include "osCall.hpp"

using namespace shtemu;

int _main(int argc, const char *argv[])
{
   cdwINFO("--- win64 shtanky emulator ---\r\n");
   cmn::cmdLine cl(argc,argv);
   std::string appPath = cl.getNextArg(".\\testdata\\shtemu\\basic.app");

   cdwINFO("loading file %s\r\n",appPath.c_str());
   windowsAllocator alloc;
   loader l;
   std::unique_ptr<image> pImage(l.loadFile(alloc,appPath));
   pImage->patch(&osCallThunk::thunk);

   // set flags
   {
      __int64 f0 = (__int64)atoi(cl.getOption("-f0","0").c_str());
      __int64 f1 = (__int64)atoi(cl.getOption("-f1","1").c_str());
      __int64 f2 = (__int64)atoi(cl.getOption("-f2","2").c_str());
      __int64 f3 = (__int64)atoi(cl.getOption("-f3","3").c_str());
      pImage->setFlags(f0,f1,f2,f3);
   }

   cdwINFO("passing control to app\r\n");
   pImage->findEntrypoint()(0);
   cdwINFO("control returned to shtemu\r\n");

   return 0;
}

cdwImplMain();

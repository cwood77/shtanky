#pragma once
#include "trace.hpp"
#include <exception>

// wrapping main with catch
// if you don't do this, throws result in crashes
// which don't honor dtor code

#define cdwImplMain() \
int main(int argc,const char *argv[]) \
{ \
   try \
   { \
      return _main(argc,argv); \
   } \
   catch(std::exception& x) \
   { \
      cdwINFO("ERROR: %s\n",x.what()); \
      cdwINFO("while running app %s\n",argv[0]); \
      if(argc>1) \
      { \
         cdwINFO("with args\n"); \
         for(int i=1;i<argc;i++) \
            cdwINFO("   %d: %s\n",i,argv[i]); \
      } \
      return -2; \
   } \
}

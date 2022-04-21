#pragma once

// wrapping main with catch
// if you don't do this, throws result in crashs
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
      return -2; \
   } \
}

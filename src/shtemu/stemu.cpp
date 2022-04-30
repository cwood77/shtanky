// allow use of old-school C FILE API
#define _CRT_SECURE_NO_WARNINGS

#include "offsets.hpp"
#include "windows.h"
#include <iostream>
#include <stdio.h>

using namespace shtemu;

#if 0
class animal {
public:
   virtual size_t numLegts() const = 0;
};

class cat : public animal {
public:
   virtual size_t numLegts() const { return 4; }
   unsigned long m_arra[3];
};

const char* gString = "Hello World\n";
#endif

void osCallThunk(size_t i, void *misc)
{
   std::cout << "received osCall!" << std::endl;
   std::cout << "   i = " << i << std::endl;
   std::cout << "   misc = " << (misc ? ((const char *)misc) : "NULL") << std::endl;
}

typedef void (*oscT)(size_t, void*);
oscT gPtr = &osCallThunk;

void func(size_t i)
{
   gPtr(1, 0);
}

int main(int argc, const char *argv[])
{
#if 0
   auto c = new cat();
   std::cout << c->numLegts();
   c->m_arra[2] = -17;
   std::cout << gString;
#endif

   //gPtr(1, 0);
   func(0);

   if(argc==1)
   {
      std::cout << "usage: stemu <app-file>" << std::endl;
      return -1;
   }

   std::cout << "loading '" << argv[1] << "'" << std::endl;
   FILE* fp = ::fopen(argv[1],"rb");
   if(!fp)
   {
      std::cout << "can't open file" << std::endl;
      return -2;
   }
   ::fseek(fp,0,SEEK_END);
   long l = ::ftell(fp);
   ::fseek(fp,0,SEEK_SET);

   auto pBlock = (unsigned char*)::VirtualAlloc(NULL, l, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if(!pBlock)
   {
      std::cout << "failed to allocate" << std::endl;
      return -3;
   }

   ::fread(pBlock,1,l,fp);
   ::fclose(fp);

   std::cout << "patching osCall" << std::endl;
   {
      unsigned long offset = *(unsigned long *)(pBlock + kOsCallOffset);
      std::cout << "  offset is " << offset << std::endl;
      if(offset == 0xFFFFFFFF)
         std::cout << "    nevermind" << std::endl;
      else
      {
         typedef void (*osCall_t)(size_t, void*);
         *reinterpret_cast<osCall_t*>(pBlock + offset) = &osCallThunk;
      }
   }

   typedef void (*entrypoint_t)(size_t);
   entrypoint_t pFunc = NULL;
   {
      unsigned long offset = *(unsigned long *)(pBlock + kEntrypointOffset);
      std::cout << "  entrypoint is offset " << offset << std::endl;
      pFunc = (entrypoint_t)(pBlock + offset);
   }

   std::cout << "running" << std::endl;

   if(sizeof(void*) == 8)
      pFunc(0);
      //std::cout << "   WAAAAH?" << std::endl;
   else
      std::cout << "  nope;  running is hopeless on a 32-bit system" << std::endl;

   std::cout << "returned" << std::endl;

   ::VirtualFree(pBlock, 0, MEM_RELEASE);

   std::cout << "done" << std::endl;

   return 0;
}

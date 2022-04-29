#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"

#include <stdio.h>

#include <iostream>
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

int main(int argc, const char *argv[])
{
#if 0
   auto c = new cat();
   std::cout << c->numLegts();
   c->m_arra[2] = -17;
   std::cout << gString;
#endif

	std::cout << "loading '" << argv[1] << "'" << std::endl;
   FILE* fp = ::fopen(argv[1],"rb");
   ::fseek(fp,0,SEEK_END);
   long l = ::ftell(fp);
   ::fseek(fp,0,SEEK_SET);

   auto pBlock = (unsigned char*)::VirtualAlloc(NULL, l, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

//   auto* pBlock = new unsigned char[l];



   ::fread(pBlock,1,l,fp);
   ::fclose(fp);

	typedef void (*entrypoint_t)(size_t);
	entrypoint_t pFunc = (entrypoint_t)(pBlock + 28);

	std::cout << "running" << std::endl;
	pFunc(0);
	std::cout << "returned" << std::endl;

//	delete [] pBlock;
   ::VirtualFree(pBlock, 0, MEM_RELEASE);

	std::cout << "done" << std::endl;
}

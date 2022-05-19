#define WIN32_LEAN_AND_MEAN
#include "../cmn/throw.hpp"
#include "loader.hpp"
#include "windows.h"

namespace shtemu {

class windowsAllocation : public iImageAllocation {
public:
   explicit windowsAllocation(unsigned char *pAlloc)
   : m_pAlloc(pAlloc)
   { }

   virtual ~windowsAllocation()
   {
      ::VirtualFree(m_pAlloc, 0, MEM_RELEASE);
   }

   virtual unsigned char *getBasePtr() { return m_pAlloc; }

private:
   unsigned char *m_pAlloc;
};

iImageAllocation *windowsAllocator::allocate(size_t s)
{
   auto pBlock = (unsigned char*)::VirtualAlloc(NULL,s,
      MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
   if(!pBlock)
      cdwTHROW("failed to allocate memory with VirtualAlloc");
   return new windowsAllocation(pBlock);
}

} // namespace shtemu

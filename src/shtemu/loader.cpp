#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "loader.hpp"
#include "offsets.hpp"
#include <stdio.h>

namespace shtemu {

image::entrypoint_t image::findEntrypoint()
{
   unsigned long offset = *(unsigned long *)(m_pPtr->getBasePtr() + kEntrypointOffset);
   cdwDEBUG("  entrypoint is offset %lu\r\n",offset);
   return (entrypoint_t)(m_pPtr->getBasePtr() + offset);
}

void image::patch(osCall_t hook)
{
   //*findOsCallImpl() = hook;
   unsigned long offset = *(unsigned long *)(m_pPtr->getBasePtr() + kOsCallOffset);
   cdwDEBUG("  osCall is offset %lu\r\n",offset);
   *(osCall_t*)(m_pPtr->getBasePtr() + offset) = hook;
}

image::osCall_t image::findOsCallImpl()
{
   unsigned long offset = *(unsigned long *)(m_pPtr->getBasePtr() + kOsCallOffset);
   cdwDEBUG("  osCall is offset %lu\r\n",offset);
   return (osCall_t)(m_pPtr->getBasePtr() + offset);
}

image *loader::loadFile(iImageAllocator& a, const std::string& path)
{
   cdwVERBOSE("loading '%s'\r\n",path.c_str());
   FILE* fp = ::fopen(path.c_str(),"rb");
   if(!fp)
      cdwTHROW("can't open file '%s'",path.c_str());

   ::fseek(fp,0,SEEK_END);
   long l = ::ftell(fp);
   ::fseek(fp,0,SEEK_SET);

   std::unique_ptr<iImageAllocation> pAlloc(a.allocate(l));
   ::fread(pAlloc->getBasePtr(),1,l,fp);
   ::fclose(fp);

   return new image(*pAlloc.release());
}

} // namespace shtemu

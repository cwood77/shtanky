#include "../cmn/binReader.hpp"
#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "objdir.hpp"
#include <memory>
#include <stdexcept>

namespace shlink {

objectDirectory::~objectDirectory()
{
   for(auto it=m_files.begin();it!=m_files.end();++it)
      delete *it;
}

void objectDirectory::loadObjectFile(const std::string& path)
{
   cdwVERBOSE("loading file '%s'\n",path.c_str());
   std::unique_ptr<cmn::objfmt::objFile> pOFile(new cmn::objfmt::objFile());
   { cmn::binFileReader r(path); pOFile->unflatten(r); }

   cdwVERBOSE("  found %lld object(s)\n",pOFile->objects.size());
   size_t i=0;
   for(auto it=pOFile->objects.begin();it!=pOFile->objects.end();++it,i++)
   {
      auto& xt = (*it)->xt;

      if(xt.toc.size() == 0)
         cdwTHROW(cmn::fmt("objects with no exports are _always_ pruned?!! obj #%lld in '%s'",i,path.c_str()));

      for(auto jit=xt.toc.begin();jit!=xt.toc.end();++jit)
      {
         cdwVERBOSE("    %s => %lld\n",jit->first.c_str(),*it);
         auto& pObj = m_objDir[jit->first];
         if(!pObj)
            pObj = *it;
         else
            throw std::runtime_error(cmn::fmt("duplicate symbol %s",jit->first.c_str()));
      }
   }

   m_files.insert(pOFile.release());
}

cmn::objfmt::obj& objectDirectory::demand(const std::string& oName)
{
   auto it = m_objDir.find(oName);
   if(it == m_objDir.end())
      throw std::runtime_error(cmn::fmt("can't find symbol '%s'",oName.c_str()));
   return *it->second;
}

} // namespace shlink

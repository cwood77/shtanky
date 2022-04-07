#include "obj-fmt.hpp"
#include "reader.hpp"
#include "writer.hpp"
//#include <cstring>
#include <stdexcept>

namespace cmn {
namespace objfmt {

void exportTable::flatten(iObjWriter& w) const
{
   w.writeCommentLine("class exportTable");
   w.write<size_t>("numentries",toc.size());
   w.nextPart();

   for(auto it=toc.begin();it!=toc.end();++it)
   {
      w.write<size_t>("namelen",it->first.length());
      w.write("name",it->first.c_str(),it->first.length());
      w.write("offset",it->second);
      w.nextPart();
   }
}

void exportTable::unflatten(iObjReader& r)
{
   auto n = r.read<size_t>();
   for(size_t i=0;i<n;i++)
   {
      // TODO READSTRING & WRITESTRING
      auto key = r.readString();
      toc[key] = r.read<unsigned long>();
   }
}

void obj::flatten(iObjWriter& w) const
{
   w.writeCommentLine("class obj");
   w.write("flags",flags);
   w.nextPart();

   xt.flatten(w);

   w.writeCommentLine("returnedto: class obj");
   w.write("blockSize",blockSize);
   w.write("block",block.get(),blockSize);
   w.nextPart();
}

void obj::unflatten(iObjReader& r)
{
   r.read(flags);
   xt.unflatten(r);
}

objFile::~objFile()
{
   for(auto it=objects.begin();it!=objects.end();++it)
      delete *it;
}

void objFile::flatten(iObjWriter& w) const
{
   w.writeCommentLine("class objFile");
   w.write("thumbprint","cdwe ofmt",9);
   w.write("version",version);
   w.write<size_t>("numentries",objects.size());
   w.nextPart();

   for(auto it=objects.begin();it!=objects.end();++it)
      (*it)->flatten(w);
}

void objFile::unflatten(iObjReader& r)
{
#if 0
   char thumbprint[9+1]; // TODO push this into the reader.readThumbprint()
   ::memset(thumbprint,0,sizeof(thumbprint));
   r.read(thumbprint,9);
   if(std::string("cdwe ofmt") != thumbprint)
      throw std::runtime_error("bad file format");
#endif
   r.readThumbprint("cdwe ofmt");

   auto v = r.read<unsigned long>();
   if(v)
      throw std::runtime_error("unknown file format version");

   auto d = r.read<size_t>();
   for(size_t i=0;i<d;i++)
   {
      objects.push_back(new obj());
      auto pObj = objects.back();
      pObj->unflatten(r);
   }
}

} // namespace objfmt
} // namespace cmn

#include "binReader.hpp"
#include "obj-fmt.hpp"
#include "writer.hpp"
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
      w.writeString("name",it->first);
      w.write("offset",it->second);
      w.nextPart();
   }
}

void exportTable::unflatten(iObjReader& r)
{
   auto n = r.read<size_t>();
   for(size_t i=0;i<n;i++)
   {
      auto key = r.readString();
      toc[key] = r.read<unsigned long>();
   }
}

void importTable::flatten(iObjWriter& w) const
{
   w.writeCommentLine("class importTable");
   w.write<size_t>("numentries",patches.size());
   w.nextPart();

   for(auto it=patches.begin();it!=patches.end();++it)
   {
      w.writeString("name",it->first);
      w.write<size_t>("numentries",it->second.size());
      w.nextPart();

      for(auto jit=it->second.begin();jit!=it->second.end();++jit)
      {
         w.write<char>("type",static_cast<char>(jit->type));
         w.write("offset",jit->offset);
         w.write("fromOffsetToEndOfInstr",jit->fromOffsetToEndOfInstr);
         w.nextPart();
      }
   }
}

void importTable::unflatten(iObjReader& r)
{
   auto n = r.read<size_t>();
   for(size_t i=0;i<n;i++)
   {
      auto key = r.readString();
      auto& list = patches[key];

      auto N = r.read<size_t>();
      for(size_t j=0;j<N;j++)
      {
         patch p;
         p.type = static_cast<patch::types>(r.read<char>());
         r.read(p.offset);
         r.read(p.fromOffsetToEndOfInstr);
         list.push_back(p);
      }
   }
}

void obj::flatten(iObjWriter& w) const
{
   w.writeCommentLine("class obj");
   w.write("flags",flags);
   w.nextPart();

   xt.flatten(w);
   it.flatten(w);

   w.writeCommentLine("returnedto: class obj");
   w.write("blockSize",blockSize);
   w.write("block",block.get(),blockSize);
   w.nextPart();
}

void obj::unflatten(iObjReader& r)
{
   r.read(flags);

   xt.unflatten(r);
   it.unflatten(r);

   r.read(blockSize);
   block.reset(new unsigned char[blockSize]);
   r.read(block.get(),blockSize);
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

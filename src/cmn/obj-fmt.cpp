#include "obj-fmt.hpp"
#include "writer.hpp"

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

} // namespace objfmt
} // namespace cmn

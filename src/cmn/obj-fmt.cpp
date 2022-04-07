#include "obj-fmt.hpp"
#include "writer.hpp"

namespace cmn {
namespace objfmt {

void obj::flatten(iObjWriter& w) const
{
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
   w.write("version",&version,sizeof(unsigned long));

   for(auto it=objects.begin();it!=objects.end();++it)
      (*it)->flatten(w);
}

} // namespace objfmt
} // namespace cmn

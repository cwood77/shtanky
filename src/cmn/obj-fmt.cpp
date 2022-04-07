#include "obj-fmt.hpp"

namespace cmn {
namespace objfmt {

objFile::~objFile()
{
   for(auto it=objects.begin();it!=objects.end();++it)
      delete *it;
}

} // namespace objfmt
} // namespace cmn

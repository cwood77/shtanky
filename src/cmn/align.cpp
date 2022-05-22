#include "align.hpp"

namespace cmn {

unsigned long align16(unsigned long x)
{
   //return (x + 15) & ~0xF;
   // I read in some online lab assignment that Win64 required 16-byte aligned code, but
   // have not found this in any official documentation nor does it seem to be supported
   // by experiment.  I'm disabling this until it proves necessary.
   return x;
}

} // namespace cmn

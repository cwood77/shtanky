#include "align.hpp"

namespace cmn {

unsigned long align16(unsigned long x)
{
   //return (x + 15) & ~0xF;
   return x;
}

} // namespace cmn

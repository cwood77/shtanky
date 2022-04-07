#include "../cmn/obj-fmt.hpp"
#include "layout.hpp"

namespace shlink {

void layoutProgress::markObjectPlaced(cmn::objfmt::obj& o)
{
   for(auto it=o.xt.toc.begin();it!=o.xt.toc.end();++it)
   {
      m_placed.insert(it->first);
      m_unplaced.erase(it->first);
   }

   for(auto it=o.it.patches.begin();it!=o.it.patches.end();++it)
   {
      auto& dep = it->first;
      // dep may already be placed if it is a local label, or a previously
      // placed object
      if(m_placed.find(dep)==m_placed.end())
         m_unplaced.insert(dep);
   }
}

unsigned long segmentBlock::append(cmn::objfmt::obj& o)
{
   unsigned long rVal = m_size;

   auto left = m_bytes.size() - m_size;
   if(left < o.blockSize)
   {
      // not enough space
      auto more = m_bytes.capacity() * 2;
      if(more < o.blockSize + m_size)
         // doubling isn't even enough!
         more = o.blockSize + m_size;

      m_bytes.resize(more);
   }

   // copy
   ::memcpy(&m_bytes[0] + m_size, o.block.get(),o.blockSize);
   m_size += o.blockSize;

   return rVal;
}

} // namespace shlink

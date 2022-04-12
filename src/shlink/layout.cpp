#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "layout.hpp"
#include "objdir.hpp"
#include <stdexcept>

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

void layout::place(cmn::objfmt::obj& o)
{
   auto& seg = m_segments[o.flags];
   auto loc = seg.append(o);

   cdwVERBOSE("placing obj %lld in %lu at %lu\n",&o,o.flags,loc);

   m_objPlacements[&o] = loc;
}

void layout::markDonePlacing()
{
   unsigned long total = 0;
   for(auto it=m_segments.begin();it!=m_segments.end();++it)
   {
      it->second.offset = total;
      total += it->second.getSize();
      cdwVERBOSE("segment %lu starts at %lu\n",it->first,it->second.offset);
   }
}

void layout::link(objectDirectory& d)
{
   cdwVERBOSE("start linking %lld objects...\n",m_objPlacements.size());

   for(auto it=m_objPlacements.begin();it!=m_objPlacements.end();++it)
      link(d,*it->first);

   cdwVERBOSE("done linking\n");

#if 0
   {
      cdwVERBOSE("looking for .osCall symbol...\n");
      auto pObj = d.tryFind(".osCall");
      if(!pObj)
         cdwVERBOSE("   ...missing\n");
      else
      {
      }
   }
#endif
}

void layout::link(objectDirectory& d, cmn::objfmt::obj& o)
{
   cdwVERBOSE("  object %lld has %lld imports\n",&o,o.it.patches.size());

   int i=0;
   for(auto it=o.it.patches.begin();it!=o.it.patches.end();++it,i++)
   {
      auto& target = d.demand(it->first);
      auto targetAddr = calculateTotalOffset(target);
      cdwVERBOSE("    %d addrOf '%s' = %lu\n",i,it->first,targetAddr);

      auto& sites = it->second;
      for(auto jit=sites.begin();jit!=sites.end();++jit)
      {
         throw std::runtime_error("unimplemented!");
      }
   }
}

unsigned long layout::calculateTotalOffset(cmn::objfmt::obj& o)
{
   return m_segments[o.flags].offset + m_objPlacements[&o];
}

} // namespace shlink

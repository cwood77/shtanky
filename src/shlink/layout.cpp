#include "../cmn/obj-fmt.hpp"
#include "../cmn/throw.hpp"
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
   cdwVERBOSE("  object %lld has %lld import(s)\n",&o,o.it.patches.size());

   unsigned char *pSrcPtr
      = const_cast<unsigned char*>(m_segments[o.flags].getHeadPtr() + m_objPlacements[&o]);
   int i=0;
   for(auto it=o.it.patches.begin();it!=o.it.patches.end();++it,i++)
   {
      auto& target = d.demand(it->first);
      auto targetAddr = calculateTotalOffset(target);
      cdwVERBOSE("    %d: addrOf '%s' = %lu\n",i,it->first.c_str(),targetAddr);

      auto& sites = it->second;
      for(auto jit=sites.begin();jit!=sites.end();++jit)
      {
         switch(jit->type)
         {
               //patchDWord(jit->offset,targetAddr);
               //break;
            case cmn::objfmt::patch::kRelToNextInstr:
               {
               //patchDWord(jit->offset,(jit->offset + jit->fromOffsetToEndOfInstr) - targetAddr);
               unsigned long ans
                  = targetAddr - (calculateTotalOffset(o) + jit->offset + jit->fromOffsetToEndOfInstr);
               cdwVERBOSE("      patching site as %lu\n",ans);
               *reinterpret_cast<unsigned long*>(pSrcPtr + jit->offset) = ans;
               }
               break;
            case cmn::objfmt::patch::kAbs:
            default:
               cdwTHROW("unimpled");
         }
      }
   }
}

unsigned long layout::calculateTotalOffset(cmn::objfmt::obj& o)
{
   return m_segments[o.flags].offset + m_objPlacements[&o];
}

void layout::patchDWord(unsigned long addr, unsigned long value)
{
   // figure out which segment has addr
   for(auto it=m_segments.begin();it!=m_segments.end();++it)
      if(it->second.offset <= addr && addr <= (it->second.offset+it->second.getSize()))
         *reinterpret_cast<unsigned long*>(
            it->second.getHeadPtr()[addr - it->second.offset]) = value;
}

} // namespace shlink

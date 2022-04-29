#include "../cmn/obj-fmt.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "iTarget.hpp"
#include "layout.hpp"
#include "objdir.hpp"

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

void layout::reportSymbols(const std::set<std::string>& demanded, iObjectProvider& d, iSymbolIndex& indx)
{
   for(auto it=demanded.begin();it!=demanded.end();++it)
   {
      auto& o = d.demand(*it);
      indx.reportSymbolLocation(*it,totalOffsetOfObj(o));
   }
}

void layout::link(iObjectProvider& d)
{
   cdwVERBOSE("start linking %lld objects...\n",m_objPlacements.size());

   for(auto it=m_objPlacements.begin();it!=m_objPlacements.end();++it)
      link(d,*it->first);

   cdwVERBOSE("done linking\n");
}

void layout::link(iObjectProvider& d, cmn::objfmt::obj& refer)
{
   cdwVERBOSE("  object %lld has %lld import(s)\n",
      &refer,
      refer.it.patches.size());

   int i=0;
   for(auto patch=refer.it.patches.begin();
       patch!=refer.it.patches.end();
       ++patch,i++)
   {
      auto& refee = d.demand(patch->first);
      auto refeeOffset = totalOffsetOfObj(refee);
      cdwVERBOSE("    %d: addrOf '%s' = %lu\n",
         i,
         patch->first.c_str(),
         refeeOffset);

      auto& sites = patch->second;
      for(auto site=sites.begin();site!=sites.end();++site)
      {
         switch(site->type)
         {
            case cmn::objfmt::patch::kRelToNextInstr:
               patchRipRelDWord(refer,refee,*site);
               break;
            default:
            case cmn::objfmt::patch::kAbs:
               cdwTHROW("unimpled");
         }
      }
   }
}

unsigned long layout::totalOffsetOfObj(cmn::objfmt::obj& o)
{
   return m_segments[o.flags].offset + m_objPlacements[&o];
}

unsigned long layout::totalOffsetOfRIP(cmn::objfmt::obj& o, cmn::objfmt::patch& p)
{
   // RIP is measured from _next_ instruction
   return totalOffsetOfObj(o)
      + p.offset
      + p.fromOffsetToEndOfInstr;
}

long *layout::laidOutSitePtr(cmn::objfmt::obj& o, cmn::objfmt::patch& p)
{
   return reinterpret_cast<long*>(
      m_segments[o.flags].getHeadPtr()
      + m_objPlacements[&o]
      + p.offset
   );
}

void layout::patchRipRelDWord(cmn::objfmt::obj& refer, cmn::objfmt::obj& refee, cmn::objfmt::patch& referPatch)
{
   // RIP-rep means the disp is + if the refee is > refer, etc. thus refee - refer
   long resolvedDisp
      = (long)totalOffsetOfObj(refee)
      - (long)totalOffsetOfRIP(refer,referPatch);

   cdwVERBOSE("      patching site as %ld\n",resolvedDisp);

   *laidOutSitePtr(refer,referPatch) = resolvedDisp;
}

} // namespace shlink

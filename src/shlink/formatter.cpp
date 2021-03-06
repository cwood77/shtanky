#include "../cmn/app-fmt.hpp"
#include "../cmn/binWriter.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "formatter.hpp"
#include "iTarget.hpp"
#include "layout.hpp"
#include "shtankyAppTarget.hpp"

namespace shlink {

void appFormatter::write(const layout& l, iSymbolIndex& _x)
{
   auto& x = dynamic_cast<shtankyAppSymbolIndex&>(_x);

   auto& segs = l.getSegments();

   {
      cmn::appfmt::header h;
      h.tocEntryCnt = segs.size();
      h.entrypointOffset = x.entrypointOffset;
      h.osCallOffset = x.osCallImplOffset;
      h.flagsOffset = x.flagsOffset;

      if(h.osCallOffset == shtankyAppSymbolIndex::kUnset)
         cdwINFO("WARNING!!! application has no osCallImpl symbol\n");
      else
      {
         h.osCallOffset += sizeof(cmn::appfmt::header);
         cdwVERBOSE("osCallImpl symbol is at %lu\n",h.osCallOffset);
      }

      if(h.entrypointOffset == shtankyAppSymbolIndex::kUnset)
         cdwTHROW("application has no entrypoint symbol?");
      else
      {
         h.entrypointOffset += sizeof(cmn::appfmt::header);
         cdwVERBOSE("entrypoint symbol is at %lu\n",h.entrypointOffset);
      }

      if(h.flagsOffset == shtankyAppSymbolIndex::kUnset)
         cdwTHROW("application has no flags symbol?");
      else
      {
         h.flagsOffset += sizeof(cmn::appfmt::header);
         cdwVERBOSE("flags symbol is at %lu\n",h.flagsOffset);
      }

      m_w.write("header",&h,sizeof(h));
   }

   for(auto it=segs.begin();it!=segs.end();++it)
      write(it->first,it->second);
}

void appFormatter::write(unsigned long flags, const segmentBlock& s)
{
   cmn::appfmt::tocEntry t;
   t.flags = flags;
   t.size = s.getSize();
   m_w.write("block",s.getHeadPtr(),s.getSize());
}

} // namespace shlink

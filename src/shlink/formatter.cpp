#include "../cmn/app-fmt.hpp"
#include "../cmn/binWriter.hpp"
#include "formatter.hpp"
#include "layout.hpp"

namespace shlink {

void formatter::write(const layout& l)
{
   auto& segs = l.getSegments();

   {
      cmn::appfmt::header h;
      h.tocEntryCnt = segs.size();
      m_w.write("header",&h,sizeof(h));
   }

   for(auto it=segs.begin();it!=segs.end();++it)
      write(it->first,it->second);
}

void formatter::write(unsigned long flags, const segmentBlock& s)
{
   cmn::appfmt::tocEntry t;
   t.flags = flags;
   t.size = s.getSize();
   m_w.write("tocEntry",&t,sizeof(t));
   m_w.write("block",s.getHeadPtr(),s.getSize());
}

} // namespace shlink

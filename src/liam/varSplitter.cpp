#include "lir.hpp"
#include "varGen.hpp"
#include "varSplitter.hpp"

namespace liam {

void varSplitter::split(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
{
   varSplitter self(s,v,t);

   for(auto it=v.all().begin();it!=v.all().end();++it)
      self.checkVar(*it->second);
}

void varSplitter::checkVar(var& v)
{
   if(v.storage.size() > 1)
   {
      auto it=v.storage.begin();
      size_t prevI = it->first;
      for(++it;it!=v.storage.end();++it)
      {
         size_t currI = it->first;

         // move from prev -> cur
         emitMoveBefore(
            v,
            currI,
            v.storage[prevI],
            v.storage[currI]);

         prevI = currI;
      }
   }
}

void varSplitter::emitMoveBefore(var& v, size_t orderNum, size_t srcStor, size_t destStor)
{
   auto& mov = m_s.pTail->head().search(orderNum).injectBefore(cmn::tgt::kMov);
   auto& dest = mov.addArg<lirArgVar>("",0);
   auto& src = mov.addArg<lirArgVar>("",0);

   v.refs[mov.orderNum].push_back(&dest);
   v.refs[mov.orderNum].push_back(&src);

   v.storageOverrides[&src] = srcStor;
   v.storageOverrides[&dest] = destStor;
}

} // namespace liam

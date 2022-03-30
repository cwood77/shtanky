#pragma once
#include <cstddef>
#include <map>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class varFinder {
public:
   explicit varFinder(cmn::tgt::iTargetInfo& t) : m_t(t) {}

   void reset();
   void recordStorageUsed(size_t s) { m_inUse[s]++; }
   size_t chooseFreeStorage();

private:
   cmn::tgt::iTargetInfo& m_t;
   std::map<size_t,size_t> m_inUse;
};

} // namespace liam

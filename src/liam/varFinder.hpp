#pragma once
#include <cstddef>
#include <map>
#include <set>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class varFinder {
public:
   explicit varFinder(cmn::tgt::iTargetInfo& t) : m_t(t), m_stackLocalSpace(0) {}

   void reset();
   void recordStorageUsed(size_t s);
   size_t chooseFreeStorage(size_t pseudoSize);

   const std::set<size_t>& getUsedRegs() const { return m_regsUsed; }
   size_t getUsedStackSpace() const { return m_stackLocalSpace; }

private:
   cmn::tgt::iTargetInfo& m_t;
   std::map<size_t,size_t> m_inUse;
   std::set<size_t> m_regsUsed;
   size_t m_stackLocalSpace;
};

} // namespace liam

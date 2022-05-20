#pragma once
#include <cstddef>
#include <map>
#include <set>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

// for each instruction, keeps track of what registers are in use
// use availVarPass to program varFinder so it can determine what's available at a given time
class varFinder {
public:
   explicit varFinder(cmn::tgt::iTargetInfo& t) : m_t(t), m_stackLocalSpace(0) {}

   void resetUsedStorage();
   void recordStorageUsed(size_t s);

   size_t chooseFreeStorage(size_t pseudoSize);
   size_t decideStackStorage(size_t pseudoSize);

   const std::set<size_t>& getUsedRegs() const { return m_regsUsed; }
   size_t getUsedStackSpace() const { return m_stackLocalSpace; }

private:
   cmn::tgt::iTargetInfo& m_t;
   std::map<size_t,size_t> m_inUse; // reset for each instr
   std::set<size_t> m_regsUsed; // accumulated for entire function
   size_t m_stackLocalSpace;    // accumulated for entire function
};

} // namespace liam

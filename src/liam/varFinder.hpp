#pragma once
#include <cstddef>
#include <map>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class varFinder {
public:
   explicit varFinder(cmn::tgt::iTargetInfo& t) : m_t(t) {}

   size_t chooseFreeStorage(std::map<size_t,size_t>& inUse);

private:
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

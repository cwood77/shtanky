#pragma once
#include <cstddef>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirStream;
class var;
class varTable;

// find storage locations with multiple occupants at the same time
// emit a move to evict one of them

class varCombiner {
public:
   static void combine(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t);

private:
   varCombiner(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
   : m_s(s), m_v(v), m_t(t) {}

   lirStream& m_s;
   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

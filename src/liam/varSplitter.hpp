#pragma once
#include <cstddef>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirStream;
class var;
class varTable;

// find vars that have multiple storage locations
// emit instructions to implement moves to fullfill these

class varSplitter {
public:
   static void split(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t);

private:
   varSplitter(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
   : m_s(s), m_v(v), m_t(t) {}

   void checkVar(var& v);
   void emitMoveBefore(var& v, size_t orderNum, size_t src, size_t dest);

   lirStream& m_s;
   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

#pragma once
#include <cstddef>
#include <list>
#include <utility>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirStream;
class lirStreams;
class var;
class varTable;

// find vars that have multiple storage locations
// emit instructions to implement moves to fullfill these

class varSplitter {
public:
   static void split(lirStreams& s, varTable& v, cmn::tgt::iTargetInfo& t);
   static void split2(lirStreams& s, lirStream& s2, varTable& v, cmn::tgt::iTargetInfo& t);

private:
   varSplitter(lirStreams& s, varTable& v, cmn::tgt::iTargetInfo& t)
   : m_s(s), m_v(v), m_t(t) {}

   void checkVar(var& v);
   void emitMoveBefore(var& v, size_t orderNum, size_t src, size_t dest);

   lirStreams& m_s;
   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;

   std::list<std::pair<lirInstr*,size_t> > m_newInstrs;
};

} // namespace liam

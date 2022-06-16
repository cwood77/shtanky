#pragma once
#include <cstddef>
#include <list>
#include <utility>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirStream;
class var;
class varTable;

// find vars that have multiple storage locations
// emit instructions to implement moves to fullfill these

// why is this done in two passes?  don't remember; something about disambiguators?
// preserving as much leeway for the combiner?

class varSplitter {
public:
   static void split(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t);

private:
   varSplitter(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
   : m_s(s), m_v(v), m_t(t) {}

   void checkVar(var& v);
   void emitMoveBefore(var& v, size_t orderNum, size_t src, size_t dest);
   void preserveDisp(var& v, size_t orderNum, lirArg& splitSrcArg);

   lirStream& m_s;
   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
   std::set<var*> m_done;

   std::list<std::pair<lirInstr*,size_t> > m_newInstrs;
};

class splitResolver {
public:
   splitResolver(lirStream& s, varTable& v) : m_s(s), m_v(v) {}

   void run();

private:
   lirStream& m_s;
   varTable& m_v;
};

} // namespace liam

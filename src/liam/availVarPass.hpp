#pragma once
#include <cstddef>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirInstr;
class lirStream;
class varFinder;
class varTable;

// sweeps over every instruction, keeping track of what storage is in use
class availVarPass {
public:
   void run();

protected:
   availVarPass(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, varFinder& f)
   : m_s(s), m_v(v), m_t(t), m_f(f) {}

   virtual void onInstr(lirInstr& i);
   virtual void onInstrStorage(lirInstr& i, size_t storage);
   virtual void onInstrWithAvailVar(lirInstr& i) {}
   void restart();

   lirStream& m_s;
   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
   varFinder& m_f;

   std::map<size_t,size_t> m_inUse;
};

} // namespace liam

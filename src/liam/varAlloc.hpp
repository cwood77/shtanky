#pragma once
#include "availVarPass.hpp"

namespace liam {

// assigned each variable without storage a storage spot
class varAllocator : public availVarPass {
public:
   varAllocator(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, varFinder& f)
   : availVarPass(s,v,t,f) {}

protected:
   virtual void onInstrWithAvailVar(lirInstr& i);
};

class stackAllocator {
public:
   void run(varTable& v, varFinder& f);
};

class varAllocator2 {
public:
   varAllocator2(cmn::tgt::iTargetInfo& t) : m_t(t) {}

   void run(varTable& v, varFinder& f);

private:
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

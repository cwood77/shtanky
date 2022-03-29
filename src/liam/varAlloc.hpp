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

} // namespace liam

#pragma once
#include "availVarPass.hpp"

// TODO HACK
// this is all wrong
// it's not enough to pick storage available at a var's first access.
// the storage needs to be available for the var's entire lifetime, because
// the splitter/combiner don't run again

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

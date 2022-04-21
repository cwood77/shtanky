#pragma once
#include "availVarPass.hpp"

namespace liam {

// "decides" stack for undecided stack allocs
class stackAllocator {
public:
   void run(varTable& v, varFinder& f);
};

// assigned each variable without storage a storage spot
class varAllocator {
public:
   varAllocator(cmn::tgt::iTargetInfo& t) : m_t(t) {}

   void run(varTable& v, varFinder& f);

private:
   cmn::tgt::iTargetInfo& m_t;
};

} // namespace liam

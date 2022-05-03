#pragma once
#include "throw.hpp"
#include <map>

namespace cmn {

template<class T, class U>
void addUnique(std::map<T,U>& m, T k, U v)
{
   auto before = m.size();
   U& nv = m[k];
   if(m.size() == before)
      cdwTHROW("duplicate map key");
   nv = v;
}

} // namespace cmn

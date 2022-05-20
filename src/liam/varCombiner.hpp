#pragma once
#include "availVarPass.hpp"
#include <cstddef>
#include <map>
#include <set>

namespace liam {

class lirStream;
class var;
class varTable;

// find storage locations with multiple occupants at the same time
// emit a move to evict one of them

class varCombiner : public availVarPass {
public:
   static bool isInfiniteStorage(size_t s);

   varCombiner(lirStream& s, varTable& v, varFinder& f)
   : availVarPass(s,v,f) {}

   virtual void onInstr(lirInstr& i);
   virtual void onInstrStorage(lirInstr& i, var& v, size_t storage);
   virtual void onInstrWithAvailVar(lirInstr& i);

private:
   void emitMoveBefore(
      lirInstr& antecedent,
      var& v,
      const std::string& comment,
      size_t srcStorage,
      size_t destStorage);

   // storage => vars
   std::map<size_t,std::set<var*> > m_clients;
};

} // namespace liam

#pragma once
#include "availVarPass.hpp"
#include <cstddef>
#include <map>
#include <set>

namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirStream;
class var;
class varTable;

// find storage locations with multiple occupants at the same time
// emit a move to evict one of them

class varCombiner : public availVarPass {
public:
   static bool isInfiniteStorage(size_t s);

   varCombiner(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, varFinder& f)
   : availVarPass(s,v,t,f) {}

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

   std::map<size_t,std::set<var*> > m_clients;
};

} // namespace liam

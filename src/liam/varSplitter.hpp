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

class varSplitter {
public:
   static void split(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t);

private:
   varSplitter(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t)
   : m_s(s), m_v(v), m_t(t) {}

   void checkVar(var& v);
   void implementFirstStorageRequirements(var& v, size_t orderNum, std::set<size_t>& reqs);
   void emitMoveBefore(var& v, size_t orderNum, size_t src, size_t dest);
   void tryPreserveDisp(var& v, size_t orderNum, lirArg& splitSrcArg);

   lirStream& m_s;
   varTable& m_v;
   cmn::tgt::iTargetInfo& m_t;
   std::set<var*> m_done;

   std::list<std::pair<lirInstr*,size_t> > m_newInstrs;
};

// convert split instrs to mov instrs
//
// splits always involve one variable but two storages, which is unusual and requires
// disambiguators to generate correctly; the destination storage is known at split time,
// but the source storage is left unresolved until the second pass, which gives maximum
// flexibility to the combiner
//
// 1st pass:
//   - create instr
//   - set refs on var
//   - requireStorage on instr for dest
//   - set disambig for dest
// 2nd pass
//   - requireStorage on instr for previous storage
//   - set disambig for src of previous storage
//   - convert opcode from split to mov
//
class splitResolver {
public:
   splitResolver(lirStream& s, varTable& v) : m_s(s), m_v(v) {}

   void run();

private:
   lirStream& m_s;
   varTable& m_v;
};

} // namespace liam

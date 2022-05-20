#pragma once
#include <cstddef>
#include <map>

namespace liam {

class lirInstr;
class lirStream;
class var;
class varFinder;
class varTable;

// sweeps over every instruction, keeping track of what storage is in use
// programs the finder with this information
//
// sequence:
//   onInstr()
//     for all alive
//       onLivingVar()
//       for all storage
//         onInstrStorage()
//     onInstrWithAvailVar()
//
class varFinderProgrammer {
public:
   varFinderProgrammer(varTable& v, varFinder& f)
   : m_v(v), m_f(f) {}

   virtual void onInstr(lirInstr& i);
   virtual void onLivingVar(lirInstr& i, var& v);
   virtual void onInstrStorage(lirInstr& i, var& v, size_t storage);
   virtual void onInstrWithAvailVar(lirInstr& i) {}

protected:
   varTable& m_v;
   varFinder& m_f;
};

// adds run/restart for pass-derived implementations
// other clients use varFinderProgrammer directly (i.e. via composition)
class availVarPass : protected varFinderProgrammer {
public:
   void run();

protected:
   availVarPass(lirStream& s, varTable& v, varFinder& f)
   : varFinderProgrammer(v,f), m_s(s) {}

   void restart();

private:
   lirStream& m_s;
};

} // namespace liam

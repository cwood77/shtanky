#pragma once
#include <vector>

namespace cmn { namespace tgt { class iCallingConvention; } }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirArg;
class lirInstr;
class lirStream;
class varTable;

class instrPrefs {
public:
   static size_t publishRequirements(lirStream& s, varTable& v, const cmn::tgt::iTargetInfo& target);

private:
   instrPrefs(varTable& v, const cmn::tgt::iTargetInfo& target)
   : m_pCurrStream(NULL), m_vTable(v), m_target(target), m_stackSpace(0) {}

   void handle(lirStream& s);
   void handle(lirInstr& i);
   void handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool outOrIn, bool isInvoke);
   void handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool isLeaving);

   lirStream *m_pCurrStream;
   varTable& m_vTable;
   const cmn::tgt::iTargetInfo& m_target;
   size_t m_stackSpace;
};

} // namespace liam

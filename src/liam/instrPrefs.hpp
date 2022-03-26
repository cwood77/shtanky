#pragma once
#include <vector>

namespace cmn { namespace tgt { class iCallingConvention; } }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace liam {

class lirArg;
class lirInstr;
class lirStream;
class lirStreams;

class instrPrefs {
public:
   static size_t publishRequirements(lirStreams& s, const cmn::tgt::iTargetInfo& target);

private:
   explicit instrPrefs(const cmn::tgt::iTargetInfo& target)
   : m_pCurrStream(NULL), m_target(target), m_stackSpace(0) {}

   void handle(lirStream& s);
   void handle(lirInstr& i);
   void handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool outOrIn);

   lirStream *m_pCurrStream;
   const cmn::tgt::iTargetInfo& m_target;
   size_t m_stackSpace;
};

} // namespace liam

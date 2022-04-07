// assembler IS target-specific, to reference to I64 here are ok.  but not before

#pragma once
#include "../cmn/i64asm.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/target.hpp"
#include <map>
#include <string>
#include <vector>

namespace cmn { class iObjWriter; }

namespace shtasm {

class iTableWriter {
public:
   /* tables */
   virtual void exportSymbol(const std::string& name) = 0;
   virtual void importSymbol(const std::string& name, cmn::objfmt::patch::types t) = 0;
};

class assembler {
public:
   assembler(cmn::tgt::iTargetInfo& t, iTableWriter& tw) : m_t(t), m_tw(tw)
   { cacheGenInfos(); }

   void assemble(
      const cmn::tgt::instrFmt& f,
      std::vector<cmn::tgt::asmArgInfo>& ai,
      cmn::iObjWriter& w);

private:
   void cacheGenInfos();

   cmn::tgt::iTargetInfo& m_t;
   iTableWriter& m_tw;
   std::map<std::string,const cmn::tgt::i64::genInfo*> m_genInfos;
};

} // namespace shtasm

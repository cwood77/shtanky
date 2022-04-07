#pragma once
#include "assembler.hpp"
#include "writer.hpp"
#include <memory>

// the processor reads from the parser and decides what to do.
// instructions and directives are outsourced to the assembler.
// segment directives are handled directly.

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class objFile; } }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace shtasm {

class iObjWriterSink;
class parser;

class processor : private iTableWriter {
public:
   processor(parser& p, cmn::tgt::iTargetInfo& t, cmn::objfmt::objFile& o);
   processor& setListingFile(iObjWriterSink& s) { m_pListingFile = &s; return *this; }

   void process();

private:
   /* iTableWriter */
   virtual void exportSymbol(const std::string& name);
   virtual void importSymbol(const std::string& name, cmn::objfmt::patch::types t) {}

   parser& m_parser;
   cmn::tgt::iTargetInfo& m_t;
   cmn::objfmt::objFile& m_oFile;

   iObjWriterSink *m_pListingFile;

   std::map<std::string,cmn::tgt::instrIds> m_instrMap;

   cmn::objfmt::obj *m_pCurrObj;
   std::unique_ptr<compositeObjWriter> m_pBlock;
   std::unique_ptr<assembler> m_pAsm;
};

} // namespace shtasm

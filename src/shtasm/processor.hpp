#pragma once
#include "assembler.hpp"
#include "writer.hpp"
#include <memory>

// the processor reads from the parser and decides what to do.
// instructions and directives are outsourced to the assembler.
// segment directives are handled directly.

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class objFile; } }
namespace cmn { namespace tgt { namespace i64 { class genInfo; } } }

namespace shtasm {

class iObjWriterSink;
class parser;

class processor {
public:
   processor(parser& p, cmn::objfmt::objFile& o);
   processor& setListingFile(iObjWriterSink& s) { m_pListingFile = &s; return *this; }

   void process();

private:
   parser& m_parser;
   cmn::objfmt::objFile& m_oFile;

   iObjWriterSink *m_pListingFile;

   std::map<std::string,const cmn::tgt::i64::genInfo*> m_genInfos;

   cmn::objfmt::obj *m_pCurrObj;
   std::unique_ptr<compositeObjWriter> m_pBlock;
   std::unique_ptr<assembler> m_pAsm;
};

} // namespace shtasm

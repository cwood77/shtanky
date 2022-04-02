#pragma once

// the processor reads from the parser and decides what to do
// instructions and directives are outsourced to the assembler
// segment directives are handled directly

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class objFile; } }

namespace shtasm {

class iObjWriterSink;
class parser;

class processor {
public:
   processor(parser& p, cmn::objfmt::objFile& o)
   : m_pListingFile(NULL), m_parser(p), m_oFile(o), m_pCurrObj(NULL) {}
   processor& setListingFile(iObjWriterSink& s) { m_pListingFile = &s; return *this; }

   void process();

private:
   iObjWriterSink *m_pListingFile;
   parser& m_parser;
   cmn::objfmt::objFile& m_oFile;

   cmn::objfmt::obj *m_pCurrObj;
};

} // namespace shtasm

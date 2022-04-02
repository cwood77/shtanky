#pragma once

// the processor reads from the parser and decides what to do
// instructions and directives are outsourced to the assembler
// segment directives are handled directly

namespace cmn { class obj; }
namespace cmn { class objFile; }

namespace shtasm {

class iObjWriterSink;
class parser;

class processor {
public:
   void process();

private:
   iObjWriterSink *m_pListingFile;
   parser& m_parser;
   cmn::objFile& m_oFile;

   cmn::obj *m_pCurrObj;
};

} // namespace shtasm

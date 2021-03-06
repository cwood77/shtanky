#pragma once
#include "../cmn/binWriter.hpp"
#include "assembler.hpp"
#include <memory>

// the processor reads from the lineParser and decides what to do.
// instructions, data declarations, and segment directives are all handled differently

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class objFile; } }
namespace cmn { namespace tgt { class iTargetInfo; } }

namespace shtasm {

class lineParser;

class processor : private iTableWriter {
public:
   processor(lineParser& p, cmn::tgt::iTargetInfo& t, cmn::objfmt::objFile& o);
   processor& setListingFile(cmn::iObjWriterSink& s);

   void process();

private:
   void processSegmentDirective(const std::vector<std::string>& a);
   void processDataDecl(const std::string& l, const std::vector<std::string>& a,
      const std::string& rawLine);
   void processInstr(const std::string& l, const std::vector<std::string>& a,
      const std::string& rawLine);
   unsigned long parseObjFlags(const char *pText);

   /* iTableWriter */
   virtual void exportSymbol(const std::string& name);
   virtual void importSymbol(const std::string& name, const cmn::objfmt::patch& p);

   lineParser& m_parser;
   cmn::tgt::iTargetInfo& m_t;
   cmn::objfmt::objFile& m_oFile;

   cmn::iObjWriterSink *m_pListingFile1;
   cmn::iObjWriterSink *m_pListingFile2;

   std::map<std::string,cmn::tgt::instrIds> m_instrMap;

   cmn::objfmt::obj *m_pCurrObj;
   std::unique_ptr<cmn::compositeObjWriter> m_pBlock;
   std::unique_ptr<assembler> m_pAsm;
};

} // namespace shtasm

#pragma once
#include "../cmn/i64asm.hpp"
#include "../cmn/obj-fmt.hpp"
#include <string>

// cases                        handled by
// - label (pub/priv)           - exporTable / writer caliper
// - instr                      - assembler
// - space                      - tableWriter
// - segment dir (switch seg)   - pen
// - other dir (undefined?)     - all the above?

// handles var directives, asm code
//class assembler;

namespace shtasm {

class iObjWriter;

class iTableWriter {
public:
   /* tables */
   virtual void exportSymbol(const std::string& name);
   virtual void importSymbol(const std::string& name, cmn::objfmt::patch::types t);
};

class assembler {
public:
   void sink(iObjWriter& o) {}

   void assemble(const cmn::tgt::i64::genInfo& gi) {}
   void addArg(const std::string& a) {}
};

} // namespace shtasm

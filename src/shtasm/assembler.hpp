#pragma once

// cases                        handled by
// - label (pub/priv)           - exporTable / writer caliper
// - instr                      - assembler
// - space                      - tableWriter
// - segment dir (switch seg)   - pen
// - other dir (undefined?)     - all the above?

class iTableWriter {
public:
   /* tables */
   virtual void export(const std::string& name);
   virtual void import(const std::string& name, patch::types t);
};

// handles var directives, asm code
class assembler;


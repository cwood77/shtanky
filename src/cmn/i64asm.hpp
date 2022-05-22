// contains helper logic for assembling a specific target
//
// shared in case linker/disassembler/reflection.emit, etc., might need this besides just
// the assembler

#pragma once
#include "target.hpp"
#include <cstddef>

namespace cmn {
namespace tgt {
namespace i64 {

// all necessary information for generating machine code for a given instruction.
// genInfo's are looked up by guid.
// encoding is generally a two-step process: (1) arguments are encoded following the
// argEncoding for each, (2) a byteStream is used to arrange the results into the ordering
// desired.
//
// beware that the byte stream is evaluated/compiled in multiple phases and some bytes
// are illegal in some phases, or behave differently (i.e. have a payload only in some
// phases);
//    phase 1: argFmtBytes::computeTotalByteStream,
//    phase 2: assembler::assemble
//
class genInfo {
public:
   enum byteType {
      // ---------- safe to use in constants
      kOpcode1,
      kOpcode2,
      //kOpcode3,
      kOpcode1WithReg, // lower 3-bits of opcode are a register
      kCodeOffset32,
      kArg2Imm8,
      kArg2Imm32,
      kArg2Imm64,
      kArgFmtBytes, // optional; if omitted arg bytes are suffixed, if any
      kArgFmtBytesWithFixedOp,
      kEndOfInstr,

      // ---------- generated only; do not use in constants
      kRexByte,
      kModRmByte,
      // sib byte
      kDisp8,
      kDisp32,
      kDisp32ToLabel,
   };

   const char *guid;

   unsigned char *byteStream;

   enum argEncoding {
      kNa,
      kModRmReg,  // arg is in reg field of ModR/M byte
      kModRmRm,   // arg is in Mod/RM fields of ModR/M byte
      kRipRelCO,  // code offset in RIP-relative (relative to _next_ instr) form
      kOpcodeReg, // reg encoded in last 3-bits of opcode (and maybe also REX)
   };

   argEncoding ae[4];
};

const genInfo *getGenInfo();

// very literal class that implements the ModR/M encoding tables as described in the
// x86-64 Software Developer's Guide
class modRm {
public:
   static void encodeRegArg(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte);
   static void encodeOpcodeArg(unsigned char opcode, unsigned char& rex, unsigned char& modRmByte);
   static void encodeModRmArg(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte, char& dispSize, bool& dispToLabel);

private:
   static void encodeModRmArg_Label(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte, char& dispSize);
   static void encodeModRmArg_MemDisp(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte, char& dispSize);
   static void encodeModRmArg_RegOnly(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte);
   static unsigned char lookupRegRm(const asmArgInfo& ai, unsigned char& rex);

   modRm();
   modRm(const modRm&);
   modRm& operator=(const modRm&);
};

// register encoded in lower 3-bit of opcode
class opcodeReg {
public:
   static void encode(const asmArgInfo& ai, unsigned char& rex, unsigned char& opcode);
};

// helper class that assembles ModR/M, SIB, etc. bytes depending on arguments and instruction
// encoding information
class argFmtBytes {
public:
   explicit argFmtBytes(unsigned char *pInstrByteStream)
   : m_pInstrByteStream(pInstrByteStream), m_opcodeMask(0) {}

   void encodeArgModRmReg(asmArgInfo& a, bool regOrRm);
   void encodeFixedOp(unsigned char op);
   void encodeRegInOpcode(asmArgInfo& a);

   unsigned char *computeTotalByteStream();

private:
   void gather(unsigned char& rex);
   void release(const unsigned char& rex);
   void gather(unsigned char& rex, unsigned char& modRm);
   void release(const unsigned char& rex, const unsigned char& modRm);
   void setDisp(char size, __int64 value);
   void setDispToLabel(char size);

   unsigned char *m_pInstrByteStream;
   std::vector<unsigned char> m_prefixByteStream;
   std::vector<unsigned char> m_argFmtByteStream;
   unsigned char m_opcodeMask;
   std::vector<unsigned char> m_totalByteStream;
};

} // namespace i64
} // namespace tgt
} // namespace cmn

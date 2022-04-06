#pragma once
#include "target.hpp"
#include <cstddef>

namespace cmn {
namespace tgt {

enum argTypes;

namespace i64 {

class genInfo {
public:
   enum { kMRmYes = 8 };

   const char   *guid;

   unsigned char rex;
   unsigned char opcode[3];
   char          coSize;   // code offset size (e.g. cd = 4)
   char          modRm;    // prescribed part of Mod/RM byte, if any
   bool          sib;
   char          dispSize;
   char          immSize;

   size_t numOpcodes() const;
};

const genInfo *getGenInfo();

class genInfo2 {
public:
   enum byteType {
      // safe to use in constants
      kOpcode1,
      //kOpcode2,
      //kOpcode3,
      kCodeOffset32,
      kArg1Imm8,
      kArg2Imm8,
      kArgFmtBytes, // optional; if omitted arg bytes are suffixed, if any
      kArgFmtBytesWithFixedOp,
      kEndOfInstr,

      // generated only; do not use in constants
      kRexByte,
      kModRmByte,
      // disp 32
      // sib byte
   };

   const char *guid;

   unsigned char *byteStream;

   enum argEncoding {
      kNa,
      kModRmReg,
   };

   argEncoding ae[4];
};

const genInfo2 *getGenInfo2();

// TODO HACK LAME - is this really 64-bit specific?
// [size][r1 + (r2*s) + d]
class asmArgInfo {
public:
   enum argFlags {
      kMem8     = 1<< 1,
      kPtr      = 1<< 2,
      kHasIndex = 1<< 3,
      kScale8   = 1<< 4,
      kDisp8    = 1<< 5,
      kLabel    = 1<< 6,
      kImm8     = 1<< 7,
      kReg64    = 1<< 8,
   };
   size_t flags;
   union {
      struct {
         unsigned char v[8];
      } bytes;
      struct {
         unsigned short v[4];
      } words;
      struct {
         unsigned long v[2];
      } dwords;
      struct {
         unsigned __int64 v[1];
      } qwords;
   } data;

   argTypes computeArgType();
};

class modRm {
public:
   static void encodeRegArg(size_t storage, unsigned char& rex, unsigned char& modRmByte) {}

private:
   modRm();
   modRm(const modRm&);
   modRm& operator=(const modRm&);
};

class argFmtBytes {
public:
   explicit argFmtBytes(unsigned char *pInstrByteStream)
   : m_pInstrByteStream(pInstrByteStream) {}

   void encodeArgModRmReg(asmArgInfo& a);
   void encodeFixedOp(unsigned char op);

   unsigned char *computeTotalByteStream();

private:
   unsigned char *m_pInstrByteStream;
   std::vector<unsigned char> m_prefixByteStream;
   std::vector<unsigned char> m_argFmtByteStream;
   std::vector<unsigned char> m_totalByteStream;
};

} // namespace i64
} // namespace tgt
} // namespace cmn

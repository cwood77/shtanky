#pragma once

namespace cmn {
namespace tgt {
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
};

const genInfo *getGenInfo();

} // namespace i64
} // namespace tgt
} // namespace cmn

#include "../cmn/fmt.hpp"
#include "i64asm.hpp"
#include <stdexcept>

namespace cmn {
namespace tgt {
namespace i64 {

static const genInfo kGen[] = {

   // some examples from MSVC disassembly
   // 48 83 ec 28    == sub rsp,28h
   // e8 10 00 00 00 == call 07FF7F5891020h
   // 48 83 c4 28    == add rsp,28h
   // c3             == ret

   //                                                      *see kRmYes
   //         guid            rex        opcode       co  M*  SIB  di imm
   { "SUB{REX.W + 83 /5 ib}", 0x48, {0x83,   0,   0},  0, 5, false, 0 ,1 }, // r/m64, imm8
   { "CALL{E8 cd}",              0, {0xE8,   0,   0},  4, 0, false, 0 ,0 },
   { "ADD{REX.W + 83 /0 ib}", 0x48, {0x83,   0,   0},  0, 8, false, 0, 1 }, // r/m64, imm8
   { "RET{near}",                0, {0xC3,   0,   0},  0, 0, false, 0, 0 },

   { NULL,                       0, {   0,   0,   0},  0, 0, false, 0, 0 },

};

const genInfo *getGenInfo() { return kGen; }

static const genInfo2 kGen2[] = {
   { "PUSH{FF /6}", (unsigned char[]){
      genInfo2::kOpcode1, 0xFF,
      genInfo2::kArgFmtBytesWithFixedOp, 0x6,
      genInfo2::kEndOfInstr,
   },
   { genInfo2::kModRmReg, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },

   { "SUB{REX.W + 83 /5 ib}", (unsigned char[]){
      genInfo2::kOpcode1, 0x83,
      genInfo2::kArgFmtBytesWithFixedOp, 0x5,
      genInfo2::kArg2Imm8,
      genInfo2::kEndOfInstr,
   },
   { genInfo2::kModRmReg, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },

   { "ADD{REX.W + 83 /0 ib}", (unsigned char[]){
      genInfo2::kOpcode1, 0xDE,
      genInfo2::kOpcode1, 0xAD,
      genInfo2::kEndOfInstr,
   },
   { genInfo2::kModRmReg, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },

   { "MOV(HACK)", (unsigned char[]){
      genInfo2::kOpcode1, 0xDE,
      genInfo2::kOpcode1, 0xAD,
      genInfo2::kEndOfInstr,
   },
   { genInfo2::kNa, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },

   { "POP{HACK}", (unsigned char[]){
      genInfo2::kOpcode1, 0xDE,
      genInfo2::kOpcode1, 0xAD,
      genInfo2::kEndOfInstr,
   },
   { genInfo2::kNa, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },

   { "CALL(HACK)", (unsigned char[]){
      genInfo2::kOpcode1, 0xDE,
      genInfo2::kOpcode1, 0xAD,
      genInfo2::kEndOfInstr,
   },
   { genInfo2::kNa, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } },

   { NULL, NULL, { genInfo2::kNa, genInfo2::kNa, genInfo2::kNa, genInfo2::kNa } }
};

const genInfo2 *getGenInfo2() { return kGen2; }

argTypes asmArgInfo::computeArgType()
{
   if(flags & kMem8)
      return kM8;
   else if(flags & kReg64)
      return kR64;
   else if(flags & kImm8)
      return kI8;
   else
      throw std::runtime_error("can't compute arg type in " __FILE__);
}

void argFmtBytes::encodeArgModRmReg(asmArgInfo& a)
{
   unsigned char rex = 0;
   unsigned char _modRm = 0;

   if(m_prefixByteStream.size())
      rex = m_prefixByteStream[1];
   if(m_argFmtByteStream.size())
      _modRm = m_argFmtByteStream[2];

   modRm::encodeRegArg(a.data.qwords.v[0],rex,_modRm);

   if(m_prefixByteStream.size())
      m_prefixByteStream[1] = rex;
   else if(rex)
   {
      m_prefixByteStream.resize(2);
      m_prefixByteStream[0] = genInfo2::kRexByte;
      m_prefixByteStream[1] = (0x40 | rex);
   }
   if(m_argFmtByteStream.size())
      m_argFmtByteStream[2] = _modRm;
   else
   {
      m_argFmtByteStream.resize(2);
      m_argFmtByteStream[0] = genInfo2::kModRmByte;
      m_argFmtByteStream[1] = _modRm;
   }
}

unsigned char *argFmtBytes::computeTotalByteStream()
{
   m_totalByteStream = m_prefixByteStream;
   bool argsInserted = false;
   for(auto pThumb=m_pInstrByteStream;*pThumb!=genInfo2::kEndOfInstr;pThumb++)
   {
      if(*pThumb == genInfo2::kOpcode1)
      {
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb);
      }
      else if(*pThumb == genInfo2::kCodeOffset32)
      {
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb);
      }
      else if(*pThumb == genInfo2::kArg1Imm8)
      {
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb);
      }
      else if(*pThumb == genInfo2::kArg2Imm8)
      {
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb);
      }
      else if(*pThumb == genInfo2::kArgFmtBytes)
      {
         m_totalByteStream.insert(
            m_totalByteStream.end(),
            m_argFmtByteStream.begin(),m_argFmtByteStream.end());
         argsInserted = true;
      }
      else if(*pThumb == genInfo2::kArgFmtBytesWithFixedOp)
      {
         pThumb++;
         m_totalByteStream.insert(
            m_totalByteStream.end(),
            m_argFmtByteStream.begin(),m_argFmtByteStream.end());
         argsInserted = true;
      }
      else
         throw std::runtime_error(cmn::fmt("don't know byte %d",(int)*pThumb));
   }

   if(!argsInserted)
      m_totalByteStream.insert(
         m_totalByteStream.end(),
         m_argFmtByteStream.begin(),m_argFmtByteStream.end());

   m_totalByteStream.push_back(genInfo2::kEndOfInstr);
   return &m_totalByteStream[0];
}

} // namespace i64
} // namespace tgt
} // namespace cmn

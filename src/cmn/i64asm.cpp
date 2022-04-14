#include "../cmn/fmt.hpp"
#include "../cmn/intel64.hpp"
#include "i64asm.hpp"
#include "throw.hpp"
#include <stdexcept>

namespace cmn {
namespace tgt {
namespace i64 {

   // some examples from MSVC disassembly
   // 48 83 ec 28    == sub rsp,28h
   // e8 10 00 00 00 == call 07FF7F5891020h
   // 48 83 c4 28    == add rsp,28h
   // c3             == ret

static const genInfo kGenInfo[] = {
   { "ADD{REX.W + 83 /0 ib}", (unsigned char[]){
      genInfo::kOpcode1, 0x83,
      genInfo::kArgFmtBytesWithFixedOp, 0x0,
      genInfo::kArg2Imm8,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "CALL(E8 cd)", (unsigned char[]){
      genInfo::kOpcode1, 0xE8,
      genInfo::kCodeOffset32,
      genInfo::kEndOfInstr,
   },
   { genInfo::kRipRelCO, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "CALL(FF /2)", (unsigned char[]){
      genInfo::kOpcode1, 0xFF,
      genInfo::kArgFmtBytesWithFixedOp, 0x2,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "MOV(REX.W + 89 /r)", (unsigned char[]){
      genInfo::kOpcode1, 0x89,
      genInfo::kArgFmtBytes,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kModRmReg, genInfo::kNa, genInfo::kNa } },

   { "MOV(REX.W + 8B /r)", (unsigned char[]){
      genInfo::kOpcode1, 0x8B,
      genInfo::kArgFmtBytes,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmReg, genInfo::kModRmRm, genInfo::kNa, genInfo::kNa } },

   { "MOV(REX.W + B8+ rd io)", (unsigned char[]){
      genInfo::kOpcode1WithReg, 0xB8,
      genInfo::kArg2Imm64,
      genInfo::kEndOfInstr,
   },
   { genInfo::kOpcodeReg, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "POP{8F /0}", (unsigned char[]){
      genInfo::kOpcode1, 0x8F,
      genInfo::kArgFmtBytesWithFixedOp, 0x0,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "PUSH{FF /6}", (unsigned char[]){
      genInfo::kOpcode1, 0xFF,
      genInfo::kArgFmtBytesWithFixedOp, 0x6,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "SUB{REX.W + 83 /5 ib}", (unsigned char[]){
      genInfo::kOpcode1, 0x83,
      genInfo::kArgFmtBytesWithFixedOp, 0x5,
      genInfo::kArg2Imm8,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { NULL, NULL, { genInfo::kNa, genInfo::kNa, genInfo::kNa, genInfo::kNa } }
};

const genInfo *getGenInfo() { return kGenInfo; }

void modRm::encodeRegArg(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte)
{
   if(ai.flags != asmArgInfo::kReg64)
      cdwTHROW(cmn::fmt("don't know how to encode argument type %lld",ai.flags));

   size_t storage = ai.data.qwords.v[0];

   unsigned char reg = 0;

   if(storage == cmn::tgt::i64::kRegA)
      reg = 0;
   else if(storage == cmn::tgt::i64::kRegC)
      reg = 1;
   else if(storage == cmn::tgt::i64::kRegD)
      reg = 2;
   else if(storage == cmn::tgt::i64::kRegB)
      reg = 3;
   else if(storage == cmn::tgt::i64::kRegSP)
      reg = 4;
   else if(storage == cmn::tgt::i64::kRegBP)
      reg = 5;
   else if(storage == cmn::tgt::i64::kRegSI)
      reg = 6;
   else if(storage == cmn::tgt::i64::kRegDI)
      reg = 7;
   else if(storage == cmn::tgt::i64::kReg8)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 0;
   }
   else if(storage == cmn::tgt::i64::kReg9)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 1;
   }
   else if(storage == cmn::tgt::i64::kReg10)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 2;
   }
   else if(storage == cmn::tgt::i64::kReg11)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 3;
   }
   else if(storage == cmn::tgt::i64::kReg12)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 4;
   }
   else if(storage == cmn::tgt::i64::kReg13)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 5;
   }
   else if(storage == cmn::tgt::i64::kReg14)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 6;
   }
   else if(storage == cmn::tgt::i64::kReg15)
   {
      rex |= 0x4; // R bit extends ModR/M reg field to R8-R15
      reg = 7;
   }

   if(ai.flags & asmArgInfo::kReg64)
      rex |= 0x8; // W bit extends reflects 64-bit operand size (i.e. RAX vs. EAX)

   modRmByte |= (reg << 3);
}

void modRm::encodeOpcodeArg(unsigned char opcode, unsigned char& rex, unsigned char& modRmByte)
{
   modRmByte |= (opcode << 3);
}

void modRm::encodeModRmArg(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte)
{
   if(ai.flags == (asmArgInfo::kMem64 | asmArgInfo::kPtr | asmArgInfo::kReg64))
   {
      cdwTHROW("unimpled.... yet");
   }
   else if(ai.flags != asmArgInfo::kReg64)
      cdwTHROW(cmn::fmt("don't know how to encode argument type %lld",ai.flags));

   size_t storage = ai.data.qwords.v[0];

   unsigned char mod = 0;
   unsigned char rm = 0;

   if(storage == cmn::tgt::i64::kRegA)
   {
      mod = 3;
      rm = 0;
   }
   else if(storage == cmn::tgt::i64::kRegC)
   {
      mod = 3;
      rm = 1;
   }
   else if(storage == cmn::tgt::i64::kRegD)
   {
      mod = 3;
      rm = 2;
   }
   else if(storage == cmn::tgt::i64::kRegB)
   {
      mod = 3;
      rm = 3;
   }
   else if(storage == cmn::tgt::i64::kRegSP)
   {
      mod = 3;
      rm = 4;
   }
   else if(storage == cmn::tgt::i64::kRegBP)
   {
      mod = 3;
      rm = 5;
   }
   else if(storage == cmn::tgt::i64::kRegSI)
   {
      mod = 3;
      rm = 6;
   }
   else if(storage == cmn::tgt::i64::kRegDI)
   {
      mod = 3;
      rm = 7;
   }
   else if(storage == cmn::tgt::i64::kReg8)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 0;
   }
   else if(storage == cmn::tgt::i64::kReg9)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 1;
   }
   else if(storage == cmn::tgt::i64::kReg10)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 2;
   }
   else if(storage == cmn::tgt::i64::kReg11)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 3;
   }
   else if(storage == cmn::tgt::i64::kReg12)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 4;
   }
   else if(storage == cmn::tgt::i64::kReg13)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 5;
   }
   else if(storage == cmn::tgt::i64::kReg14)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 6;
   }
   else if(storage == cmn::tgt::i64::kReg15)
   {
      mod = 3;
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 7;
   }

   if(ai.flags & asmArgInfo::kReg64)
      rex |= 0x8; // W bit extends reflects 64-bit operand size (i.e. RAX vs. EAX)

   modRmByte |= (mod << 6);
   modRmByte |= rm;
}

void opcodeReg::encode(const asmArgInfo& ai, unsigned char& rex, unsigned char& opcode)
{
   if(ai.flags != asmArgInfo::kReg64)
      cdwTHROW("don't know how to encode argument type");

   size_t storage = ai.data.qwords.v[0];

   if(storage == cmn::tgt::i64::kRegA)
      opcode = 0;
   else if(storage == cmn::tgt::i64::kRegC)
      opcode = 1;
   else if(storage == cmn::tgt::i64::kRegD)
      opcode = 2;
   else if(storage == cmn::tgt::i64::kRegB)
      opcode = 3;
   else if(storage == cmn::tgt::i64::kRegSP)
      opcode = 4;
   else if(storage == cmn::tgt::i64::kRegBP)
      opcode = 5;
   else if(storage == cmn::tgt::i64::kRegSI)
      opcode = 6;
   else if(storage == cmn::tgt::i64::kRegDI)
      opcode = 7;
   else if(storage == cmn::tgt::i64::kReg8)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 0;
   }
   else if(storage == cmn::tgt::i64::kReg9)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 1;
   }
   else if(storage == cmn::tgt::i64::kReg10)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 2;
   }
   else if(storage == cmn::tgt::i64::kReg11)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 3;
   }
   else if(storage == cmn::tgt::i64::kReg12)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 4;
   }
   else if(storage == cmn::tgt::i64::kReg13)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 5;
   }
   else if(storage == cmn::tgt::i64::kReg14)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 6;
   }
   else if(storage == cmn::tgt::i64::kReg15)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      opcode = 7;
   }

   if(ai.flags & asmArgInfo::kReg64)
      rex |= 0x8; // W bit extends reflects 64-bit operand size (i.e. RAX vs. EAX)
}

void argFmtBytes::encodeArgModRmReg(asmArgInfo& a, bool regOrRm)
{
   unsigned char rex = 0;
   unsigned char _modRm = 0;
   gather(rex,_modRm);

   if(regOrRm)
      modRm::encodeRegArg(a,rex,_modRm);
   else
      modRm::encodeModRmArg(a,rex,_modRm);

   release(rex,_modRm);
}

void argFmtBytes::encodeFixedOp(unsigned char op)
{
   unsigned char rex = 0;
   unsigned char _modRm = 0;
   gather(rex,_modRm);

   modRm::encodeOpcodeArg(op,rex,_modRm);

   release(rex,_modRm);
}

void argFmtBytes::encodeRegInOpcode(asmArgInfo& a)
{
   unsigned char rex = 0;
   gather(rex);

   opcodeReg::encode(a,rex,m_opcodeMask);

   release(rex);
}

unsigned char *argFmtBytes::computeTotalByteStream()
{
   m_totalByteStream = m_prefixByteStream;
   bool argsInserted = false;
   for(auto pThumb=m_pInstrByteStream;*pThumb!=genInfo::kEndOfInstr;pThumb++)
   {
      if(*pThumb == genInfo::kOpcode1)
      {
         // pass thru with 1-byte payload
         m_totalByteStream.push_back(*pThumb); pThumb++;
         m_totalByteStream.push_back(*pThumb);
      }
      else if(*pThumb == genInfo::kOpcode1WithReg)
      {
         // pass thru with 1-byte payload
         m_totalByteStream.push_back(genInfo::kOpcode1); pThumb++;
         m_totalByteStream.push_back(*pThumb | m_opcodeMask);
      }
      else if(*pThumb == genInfo::kCodeOffset32 ||
              *pThumb == genInfo::kArg1Imm8 ||
              *pThumb == genInfo::kArg2Imm8 ||
              *pThumb == genInfo::kArg2Imm64)
      {
         // pass thru with 0-byte payload
         m_totalByteStream.push_back(*pThumb);
      }
      else if(*pThumb == genInfo::kArgFmtBytes)
      {
         m_totalByteStream.insert(
            m_totalByteStream.end(),
            m_argFmtByteStream.begin(),m_argFmtByteStream.end());
         argsInserted = true;
      }
      else if(*pThumb == genInfo::kArgFmtBytesWithFixedOp)
      {
         encodeFixedOp(*(++pThumb));
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

   m_totalByteStream.push_back(genInfo::kEndOfInstr);
   return &m_totalByteStream[0];
}

void argFmtBytes::gather(unsigned char& rex)
{
   if(m_prefixByteStream.size())
      rex = m_prefixByteStream[1];
}

void argFmtBytes::release(const unsigned char& rex)
{
   if(m_prefixByteStream.size())
      m_prefixByteStream[1] = rex;
   else if(rex)
   {
      m_prefixByteStream.resize(2);
      m_prefixByteStream[0] = genInfo::kRexByte;
      m_prefixByteStream[1] = (0x40 | rex);
   }
}

void argFmtBytes::gather(unsigned char& rex, unsigned char& modRm)
{
   gather(rex);
   if(m_argFmtByteStream.size())
      modRm = m_argFmtByteStream[1];
}

void argFmtBytes::release(const unsigned char& rex, const unsigned char& modRm)
{
   release(rex);
   if(m_argFmtByteStream.size())
      m_argFmtByteStream[1] = modRm;
   else
   {
      m_argFmtByteStream.resize(2);
      m_argFmtByteStream[0] = genInfo::kModRmByte;
      m_argFmtByteStream[1] = modRm;
   }
}

} // namespace i64
} // namespace tgt
} // namespace cmn

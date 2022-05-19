#include "../cmn/lexor.hpp"
#include "../cmn/intel64.hpp"
#include "i64asm.hpp"
#include "throw.hpp"

namespace cmn {
namespace tgt {
namespace i64 {

static const genInfo kGenInfo[] = {
   { "ADD{REX.W + 83 /0 ib}", (unsigned char[]){
      genInfo::kOpcode1, 0x83,
      genInfo::kArgFmtBytesWithFixedOp, 0x0,
      genInfo::kArg2Imm8,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "CALL{E8 cd}", (unsigned char[]){
      genInfo::kOpcode1, 0xE8,
      genInfo::kCodeOffset32,
      genInfo::kEndOfInstr,
   },
   { genInfo::kRipRelCO, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "CALL{FF /2}", (unsigned char[]){
      genInfo::kOpcode1, 0xFF,
      genInfo::kArgFmtBytesWithFixedOp, 0x2,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "MOV{REX.W + 89 /r}", (unsigned char[]){
      genInfo::kOpcode1, 0x89,
      genInfo::kArgFmtBytes,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kModRmReg, genInfo::kNa, genInfo::kNa } },

   { "MOV{REX.W + 8B /r}", (unsigned char[]){ // TODO - IP
      genInfo::kOpcode1, 0x8B,
      genInfo::kArgFmtBytes,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmReg, genInfo::kModRmRm, genInfo::kNa, genInfo::kNa } },

   { "MOV{REX.W + B8+ rd io}", (unsigned char[]){
      genInfo::kOpcode1WithReg, 0xB8,
      genInfo::kArg2Imm64,
      genInfo::kEndOfInstr,
   },
   { genInfo::kOpcodeReg, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "MOV{REX.W + C7 /0 id}", (unsigned char[]){
      genInfo::kOpcode1WithReg, 0xC7,
      genInfo::kArgFmtBytesWithFixedOp, 0x0,
      genInfo::kArg2Imm32,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmRm, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

   { "LEA{REX.W + 8D /r}", (unsigned char[]){
      genInfo::kOpcode1, 0x8D,
      genInfo::kArgFmtBytes,
      genInfo::kEndOfInstr,
   },
   { genInfo::kModRmReg, genInfo::kModRmRm, genInfo::kNa, genInfo::kNa } },

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

   { "RET{C3}", (unsigned char[]){
      genInfo::kOpcode1, 0xC3,
      genInfo::kEndOfInstr,
   },
   { genInfo::kNa, genInfo::kNa, genInfo::kNa, genInfo::kNa } },

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
      cdwTHROW("don't know how to encode argument type %lld",ai.flags);

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

void modRm::encodeModRmArg(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte, char& dispSize, bool& dispToLabel)
{
   dispToLabel = false;
   if(ai.flags == (asmArgInfo::kMem64 | asmArgInfo::kLabel))
   {
      dispToLabel = true;
      encodeModRmArg_Label(ai,rex,modRmByte,dispSize);
   }
   else if(ai.flags == (asmArgInfo::kMem64 | asmArgInfo::kReg64))
      encodeModRmArg_MemDisp(ai,rex,modRmByte,dispSize);
   else if(ai.flags == asmArgInfo::kReg64)
   {
      encodeModRmArg_RegOnly(ai,rex,modRmByte);
      dispSize = 0;
   }
   else
      cdwTHROW("don't know how to encode argument type %lld",ai.flags);
}

void modRm::encodeModRmArg_Label(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte, char& dispSize)
{
   // encodes as disp32
   unsigned char mod = 0;
   unsigned char rm = 5;

   dispSize = 4;

   modRmByte |= (mod << 6);
   modRmByte |= rm;
}

void modRm::encodeModRmArg_MemDisp(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte, char& dispSize)
{
   unsigned char mod = 0;
   unsigned char rm = lookupRegRm(ai,rex);

   // now pick a mod value to show memory access
   if(!ai.disp)
   {
      dispSize = 0;
      mod = 0;
   }
   else
   {
      dispSize = lexorBase::getLexemeIntSize(ai.disp);
      if(dispSize == 8)
         cdwTHROW("can't encode 64-bit displacement in x86-64");
      else if(dispSize == 1)
      {
         mod = 1;
      }
      else
      {
         // round up 16-bit values to 32-bit
         dispSize = 4;
         mod = 2;
      }
   }

   modRmByte |= (mod << 6);
   modRmByte |= rm;
}

void modRm::encodeModRmArg_RegOnly(const asmArgInfo& ai, unsigned char& rex, unsigned char& modRmByte)
{
   unsigned char mod = 3;
   unsigned char rm = lookupRegRm(ai,rex);

   modRmByte |= (mod << 6);
   modRmByte |= rm;
}

unsigned char modRm::lookupRegRm(const asmArgInfo& ai, unsigned char& rex)
{
   size_t storage = ai.data.qwords.v[0];

   unsigned char rm = 0;

   if(storage == cmn::tgt::i64::kRegA)
      rm = 0;
   else if(storage == cmn::tgt::i64::kRegC)
      rm = 1;
   else if(storage == cmn::tgt::i64::kRegD)
      rm = 2;
   else if(storage == cmn::tgt::i64::kRegB)
      rm = 3;
   else if(storage == cmn::tgt::i64::kRegSP)
      rm = 4;
   else if(storage == cmn::tgt::i64::kRegBP)
      rm = 5;
   else if(storage == cmn::tgt::i64::kRegSI)
      rm = 6;
   else if(storage == cmn::tgt::i64::kRegDI)
      rm = 7;
   else if(storage == cmn::tgt::i64::kReg8)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 0;
   }
   else if(storage == cmn::tgt::i64::kReg9)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 1;
   }
   else if(storage == cmn::tgt::i64::kReg10)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 2;
   }
   else if(storage == cmn::tgt::i64::kReg11)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 3;
   }
   else if(storage == cmn::tgt::i64::kReg12)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 4;
   }
   else if(storage == cmn::tgt::i64::kReg13)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 5;
   }
   else if(storage == cmn::tgt::i64::kReg14)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 6;
   }
   else if(storage == cmn::tgt::i64::kReg15)
   {
      rex |= 0x1; // B bit extends ModR/M r/m field to R8-R15
      rm = 7;
   }

   if(ai.flags & asmArgInfo::kReg64)
      rex |= 0x8; // W bit extends reflects 64-bit operand size (i.e. RAX vs. EAX)

   return rm;
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
   char dispSize = 0;
   bool dispToLabel = true;
   gather(rex,_modRm);

   if(regOrRm)
      modRm::encodeRegArg(a,rex,_modRm);
   else
      modRm::encodeModRmArg(a,rex,_modRm,dispSize,dispToLabel);

   release(rex,_modRm);
   if(dispSize)
   {
      if(dispToLabel)
         setDispToLabel(dispSize);
      else
         setDisp(dispSize,a.disp);
   }
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
              *pThumb == genInfo::kArg2Imm8 ||
              *pThumb == genInfo::kArg2Imm32 ||
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
         cdwTHROW("don't know byte %d",(int)*pThumb);
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

// disps are filled in now
void argFmtBytes::setDisp(char size, __int64 value)
{
   size_t arrayIdx = m_argFmtByteStream.size();
   m_argFmtByteStream.resize(arrayIdx + 1 + size);
   if(size == 4)
   {
      m_argFmtByteStream[arrayIdx] = genInfo::kDisp32;
      *reinterpret_cast<long*>(&m_argFmtByteStream[arrayIdx+1]) = static_cast<long>(value);
   }
   else
   {
      m_argFmtByteStream[arrayIdx] = genInfo::kDisp8;
      m_argFmtByteStream[arrayIdx+1] = static_cast<signed char>(value);
   }
}

// code offsets are filled in later, with patch (by assembler)
void argFmtBytes::setDispToLabel(char size)
{
   if(size != 4)
      cdwTHROW("unimplemented");

   size_t arrayIdx = m_argFmtByteStream.size();
   m_argFmtByteStream.resize(arrayIdx + 1 /*+ size*/);

   m_argFmtByteStream[arrayIdx] = genInfo::kDisp32ToLabel;
}

} // namespace i64
} // namespace tgt
} // namespace cmn

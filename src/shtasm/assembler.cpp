#include "../cmn/fmt.hpp"
#include "assembler.hpp"
#include "writer.hpp"
#include <stdexcept>

namespace shtasm {

void assembler::assemble(const cmn::tgt::instrFmt& f, std::vector<cmn::tgt::asmArgInfo>& ai, iObjWriter& w)
{
   // look up genInfo
   auto *pInfo = m_genInfos[f.guid];
   if(!pInfo)
      throw std::runtime_error(cmn::fmt("no known instr for '%s'",f.guid));

   // build args
   cmn::tgt::i64::argFmtBytes argBytes(pInfo->byteStream);
   for(size_t i=0;i<3;i++)
   {
      switch(pInfo->ae[i])
      {
         case cmn::tgt::i64::genInfo::kNa:
            break;

         case cmn::tgt::i64::genInfo::kModRmReg:
            argBytes.encodeArgModRmReg(ai[i],true);
            break;

         case cmn::tgt::i64::genInfo::kModRmRm:
            argBytes.encodeArgModRmReg(ai[i],false);
            break;

         default:
            throw std::runtime_error("unknown arg type in " __FILE__);
      }
   }

   // implement the byte stream
   unsigned char *pByte = argBytes.computeTotalByteStream();
   for(;*pByte != cmn::tgt::i64::genInfo::kEndOfInstr;++pByte)
   {
      if(*pByte == cmn::tgt::i64::genInfo::kOpcode1)
      {
         w.write("op",++pByte,1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kArg2Imm8)
      {
         w.write("i8",&ai[1].data.bytes.v[0],1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kRexByte)
      {
         w.write("rex",++pByte,1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kModRmByte)
      {
         w.write("modR/M",++pByte,1);
      }
      else
         throw std::runtime_error(cmn::fmt("don't know how to write byte %d",(int)*pByte));
   }

   w.nextPart();
}

void assembler::cacheGenInfos()
{
   auto *pInfo = cmn::tgt::i64::getGenInfo();
   while(pInfo->guid)
   {
      auto& pI = m_genInfos[pInfo->guid];
      pI = pInfo;
      pInfo++;
   }
}

} // namespace shtasm

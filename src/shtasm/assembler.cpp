#include "../cmn/binWriter.hpp"
#include "../cmn/throw.hpp"
#include "assembler.hpp"

namespace shtasm {

void assembler::assemble(const cmn::tgt::instrFmt& f, std::vector<cmn::tgt::asmArgInfo>& ai, cmn::iObjWriter& w)
{
   // look up genInfo
   auto *pInfo = m_genInfos[f.guid];
   if(!pInfo)
      cdwTHROW("no known instr for '%s'",f.guid);

   // build args
   std::string label;
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
            if(!ai[i].label.empty())
               label = ai[i].label;
            break;

         case cmn::tgt::i64::genInfo::kRipRelCO:
            if(!label.empty())
               cdwTHROW("ISE");
            label = ai[i].label;
            break;

         case cmn::tgt::i64::genInfo::kOpcodeReg:
            argBytes.encodeRegInOpcode(ai[i]);
            break;

         default:
            throw std::runtime_error("unknown arg type in " __FILE__);
      }
   }

   std::map<unsigned long,std::string> patches;

   // implement the byte stream
   unsigned char *pByte = argBytes.computeTotalByteStream();
   for(;*pByte != cmn::tgt::i64::genInfo::kEndOfInstr;++pByte)
   {
      if(*pByte == cmn::tgt::i64::genInfo::kOpcode1)
      {
         w.write("op",++pByte,1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kOpcode2)
      {
         w.write("op",(++pByte)++,2);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kCodeOffset32)
      {
         if(label.empty())
            cdwTHROW("emitting co32 but no label??");
         patches[w.tell()] = label;
         unsigned long patch = 0;
         w.write("co32",&patch,sizeof(unsigned long));
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kArg2Imm8)
      {
         w.write("i8",&ai[1].data.bytes.v[0],1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kArg2Imm32)
      {
         w.write("i32",&ai[1].data.dwords.v[0],4);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kArg2Imm64)
      {
         w.write("i64",&ai[1].data.qwords.v[0],8);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kRexByte)
      {
         w.write("rex",++pByte,1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kModRmByte)
      {
         w.write("modR/M",++pByte,1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kDisp8)
      {
         w.write("d8",++pByte,1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kDisp32)
      {
         w.write("d32",(pByte+1+4),4);
         pByte += (1+4-1);
      }
      else if(*pByte == cmn::tgt::i64::genInfo::kDisp32ToLabel)
      {
         if(label.empty())
            cdwTHROW("emitting disp32toLabel but no label??");
         patches[w.tell()] = label;
         unsigned long patch = 0;
         w.write("disp32toLabel",&patch,sizeof(unsigned long));
      }
      else
         cdwTHROW("don't know how to write byte %d",(int)*pByte);
   }

   w.nextPart();

   // schedule patches
   if(patches.size())
   {
      unsigned long nextInstr = w.tell();
      for(auto it=patches.begin();it!=patches.end();++it)
      {
         cmn::objfmt::patch p;
         p.type = cmn::objfmt::patch::kRelToNextInstr;
         p.offset = it->first;
         p.fromOffsetToEndOfInstr = nextInstr - it->first;
         m_tw.importSymbol(it->second,p);
      }
   }
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

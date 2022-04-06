#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/target.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/i64asm.hpp" // remove?
#include "assembler.hpp"
#include "frontend.hpp"
#include "processor.hpp"
#include "writer.hpp"
#include <memory>

namespace shtasm {

processor::processor(parser& p, cmn::tgt::iTargetInfo& t, cmn::objfmt::objFile& o)
: m_parser(p)
, m_t(t)
, m_oFile(o)
, m_pListingFile(NULL)
, m_pCurrObj(NULL)
{
   // prepare instrMap
   {
      for(size_t i=cmn::tgt::kFirstInstr;i<=cmn::tgt::kLastInstr;i++)
      {
         auto asId = static_cast<cmn::tgt::instrIds>(i);
         auto pInstr = t.getProc().getInstr(asId);
         m_instrMap[pInstr->name] = asId;
      }
   }

   // prepare genInfos
   {
      auto *pInfo = cmn::tgt::i64::getGenInfo2();
      while(pInfo->guid)
      {
         auto& pI = m_genInfos[pInfo->guid];
         pI = pInfo;
         pInfo++;
      }
   }
}

void processor::process()
{
   bool firstHack = true;

   while(!m_parser.getLexor().isDone())
   {
      std::string l,c,rawLine;
      std::vector<std::string> a;
      if(!firstHack)
         m_parser.parseLine(l,a,c,rawLine);
      else
      {
         // TODO HACK LAME - liam isn't generating segment directives yet
         a.push_back(".seg");
         a.push_back("1");
         firstHack = false;
      }

      {
         // dbg trace
         cdwDEBUG("lable=%s]]\n",l.c_str());
         for(auto it=a.begin();it!=a.end();++it)
            cdwDEBUG("   a=%s]]\n",it->c_str());
         cdwDEBUG("comm=%s]]\n",c.c_str());
      }

      if(a.size() == 0)
         ; // blank line
      else if(a.size() == 2 && a[0] == ".seg")
      {
         // segment directives
         m_oFile.objects.push_back(new cmn::objfmt::obj());
         m_pCurrObj = m_oFile.objects.back();
         ::sscanf(a[0].c_str()+5,"%lu",&m_pCurrObj->flags);

         // setup stream for new object
         m_pBlock.reset(new compositeObjWriter());
         m_pBlock->sink(
            *new retailObjWriter(
               *new binMemoryWriter(m_pCurrObj->block)));
         if(m_pListingFile)
            m_pBlock->sink(
               *new listingObjWriter(
                  *new singleUseWriter(*m_pListingFile)));
         m_pWriter.reset(new lineWriter(*m_pBlock));

         // prep assembler
         m_pAsm.reset(new assembler());
         m_pAsm->sink(*m_pWriter);
      }
      else
      {
         // defer to assembler

         // ---------- all below here move to assembler ---------

         m_pWriter->setLineNumber(m_parser.getLexor().getLineNumber());
         m_pWriter->writeComment(rawLine);

         // find instr
         auto instrId = cmn::tgt::kFirstInstr;
         {
            auto it = m_instrMap.find(a[0]);
            if(it == m_instrMap.end())
               throw std::runtime_error(cmn::fmt("instr '%s' isn't known?",a[0].c_str()));
            instrId = it->second;
         }

         // fine parse each argument
         std::vector<cmn::tgt::i64::asmArgInfo> ai;
         std::vector<cmn::tgt::argTypes> aTypes;
         for(size_t i=1;i<a.size();i++)
         {
            ai.push_back(cmn::tgt::i64::asmArgInfo());
            fineLexor l(a[i].c_str());
            fineParser p(l);
            p.parseArg(ai.back());
            aTypes.push_back(ai.back().computeArgType());
         }

         // locate the instr fmt to use
         auto& iFmt = m_t.getProc().getInstr(instrId)->demandFmt(aTypes);

         // look up genInfo
         auto *pInfo = m_genInfos[iFmt.guid];
         if(!pInfo)
            throw std::runtime_error(cmn::fmt("no known instr for '%s'",a[0].c_str()));

         // build args
         cmn::tgt::i64::argFmtBytes argBytes(pInfo->byteStream);
         for(size_t i=0;i<3;i++)
         {
            switch(pInfo->ae[i])
            {
               case cmn::tgt::i64::genInfo2::kNa:
                  break;

               case cmn::tgt::i64::genInfo2::kModRmReg:
                  argBytes.encodeArgModRmReg(ai[i],true);
                  break;

               case cmn::tgt::i64::genInfo2::kModRmRm:
                  argBytes.encodeArgModRmReg(ai[i],false);
                  break;

               default:
                  throw std::runtime_error("unknown arg type in " __FILE__);
            }
         }

         // implement the byte stream
         unsigned char *pByte = argBytes.computeTotalByteStream();
         for(;*pByte != cmn::tgt::i64::genInfo2::kEndOfInstr;++pByte)
         {
            if(*pByte == cmn::tgt::i64::genInfo2::kOpcode1)
            {
               m_pWriter->write("op",++pByte,1);
            }
            else if(*pByte == cmn::tgt::i64::genInfo2::kArg2Imm8)
            {
               m_pWriter->write("i8",&ai[1].data.bytes.v[0],1);
            }
            else if(*pByte == cmn::tgt::i64::genInfo2::kRexByte)
            {
               m_pWriter->write("rex",++pByte,1);
            }
            else if(*pByte == cmn::tgt::i64::genInfo2::kModRmByte)
            {
               m_pWriter->write("modR/M",++pByte,1);
            }
            else
               throw std::runtime_error(cmn::fmt("don't know how to write byte %d",(int)*pByte));
         }

         m_pWriter->under().nextPart();

#if 0
         m_pWriter->setLineNumber(m_parser.getLexor().getLineNumber());
         m_pAsm->assemble(*pInfo);

         // add each arg
         for(size_t i=1;i<a.size();i++)
            m_pAsm->addArg(a[i]);
#endif
      }
   }
}

} // namespace shtasm

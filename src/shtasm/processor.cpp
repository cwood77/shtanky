#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/target.hpp"
#include "../cmn/trace.hpp"
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
      auto *pInfo = cmn::tgt::i64::getGenInfo();
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
   while(!m_parser.getLexor().isDone())
   {
      std::string l,c;
      std::vector<std::string> a;
      m_parser.parseLine(l,a,c);

      {
         // dbg trace
         cdwDEBUG("lable=%s]]\n",l.c_str());
         for(auto it=a.begin();it!=a.end();++it)
            cdwDEBUG("   a=%s]]\n",it->c_str());
         cdwDEBUG("comm=%s]]\n",c.c_str());
      }

      if(a.size() == 0)
         ; // blank line
      else if(a.size() == 1 && a[0] == ".seg ")
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
         const cmn::tgt::i64::genInfo *pInfo = m_genInfos[iFmt.guid];
         if(!pInfo)
            throw std::runtime_error(cmn::fmt("no known instr for '%s'",a[0].c_str()));
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

#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "assembler.hpp"
#include "frontend.hpp"
#include "processor.hpp"
#include "writer.hpp"
#include <memory>

namespace shtasm {

processor::processor(parser& p, cmn::objfmt::objFile& o)
: m_parser(p)
, m_oFile(o)
, m_pListingFile(NULL)
, m_pCurrObj(NULL)
{
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

         // setup assembler for new object
         m_pBlock.reset(new compositeObjWriter());
         m_pBlock->sink(
            *new retailObjWriter(
               *new binFileWriter(".\\testdata\\test\\fake.shtasm.o")));
         /*
         w.sink(
            *new listingObjWriter(
               *new binFileWriter(".\\testdata\\test\\fake.shtasm.list")));
               */
         /*
         lineWriter l(*pBlock);
         l.setLineNumber(m_parser.getLexor().getLineNumber());
         */
         m_pAsm.reset(new assembler());
         m_pAsm->sink(*m_pBlock);
      }
      else
      {
         // defer to assembler

         // look up instr
         const cmn::tgt::i64::genInfo *pInfo = m_genInfos[a[0]];
         if(!pInfo)
            throw std::runtime_error(cmn::fmt("no known instr for '%s'",a[0].c_str()));
         m_pAsm->assemble(*pInfo);

         // add each arg
         for(size_t i=1;i<a.size();i++)
            m_pAsm->addArg(a[i]);
      }
   }
}

} // namespace shtasm

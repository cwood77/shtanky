#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "assembler.hpp"
#include "frontend.hpp"
#include "processor.hpp"
#include "writer.hpp"
#include <memory>

namespace shtasm {

void processor::process()
{
   std::unique_ptr<assembler> pAsm;
   std::unique_ptr<compositeObjWriter> pBlock;
   std::map<std::string,const cmn::tgt::i64::genInfo*> genInfos;

   {
      // prepare genInfos
      auto *pInfo = cmn::tgt::i64::getGenInfo();
      while(pInfo->guid)
      {
         auto& pI = genInfos[pInfo->guid];
         pI = pInfo;
         pInfo++;
      }
   }

   while(!m_parser.getLexor().isDone())
   {
      std::string l,c;
      std::vector<std::string> a;
      m_parser.parseLine(l,a,c);

      // dbg trace
      cdwDEBUG("lable=%s]]\n",l.c_str());
      for(auto it=a.begin();it!=a.end();++it)
         cdwDEBUG("   a=%s]]\n",it->c_str());
      cdwDEBUG("comm=%s]]\n",c.c_str());

      if(a.size() == 0)
         ; // blank line
      else if(a.size() == 1 && a[0] == ".seg ")
      {
         // segment directives
         m_oFile.objects.push_back(new cmn::objfmt::obj());
         m_pCurrObj = m_oFile.objects.back();
         ::sscanf(a[0].c_str()+5,"%lu",&m_pCurrObj->flags);

         // setup assembler for new object
         pBlock.reset(new compositeObjWriter());
         pBlock->sink(
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
         pAsm.reset(new assembler());
         pAsm->sink(*pBlock);
      }
      else
      {
         // defer to assembler

         // look up instr
         const cmn::tgt::i64::genInfo *pInfo = genInfos[a[0]];
         if(!pInfo)
            throw std::runtime_error(cmn::fmt("no known instr for '%s'",a[0].c_str()));

         // add each arg
      }
   }
}

} // namespace shtasm

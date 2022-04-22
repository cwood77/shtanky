#include "../cmn/binWriter.hpp"
#include "../cmn/fmt.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/target.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "assembler.hpp"
#include "frontend.hpp"
#include "processor.hpp"
#include <memory>
#include <string.h>

namespace shtasm {

processor::processor(lineParser& p, cmn::tgt::iTargetInfo& t, cmn::objfmt::objFile& o)
: m_parser(p)
, m_t(t)
, m_oFile(o)
, m_pListingFile1(NULL)
, m_pListingFile2(NULL)
, m_pCurrObj(NULL)
, m_pAsm(new assembler(t,*this))
{
   // prepare instrMap
   {
      for(size_t i=cmn::tgt::kFirstInstr;i<=cmn::tgt::kLastInstr;i++)
      {
         auto asId = static_cast<cmn::tgt::instrIds>(i);
         auto pInstr = t.getProc().getInstr(asId);
         if(pInstr->name[0] == '<')
            continue; // exclude pseudo-instructions like <enterFunc>
         m_instrMap[pInstr->name] = asId;
      }
   }
}

processor& processor::setListingFile(cmn::iObjWriterSink& s)
{
   if(m_pListingFile1)
      m_pListingFile2 = &s;
   else
      m_pListingFile1 = &s;

   return *this;
}

void processor::process()
{
   while(!m_parser.getLexor().isDone())
   {
      std::string l,c,rawLine;
      std::vector<std::string> a;
      m_parser.parseLine(l,a,c,rawLine);

      {
         // dbg trace
         cdwDEBUG("lable=%s]]\n",l.c_str());
         for(auto it=a.begin();it!=a.end();++it)
            cdwDEBUG("   a=%s]]\n",it->c_str());
         cdwDEBUG("comm=%s]]\n",c.c_str());
      }

      if(l.empty() && a.size() == 0)
         ; // blank line

      else if(a.size() == 1 && ::strncmp(a[0].c_str(),".seg ",5)==0)
         processSegmentDirective(a);

      else if(m_pCurrObj && a.size() > 0 && a[0] == ".data")
         processDataDecl(l,a,rawLine);

      else if(m_pCurrObj)
         processInstr(l,a,rawLine);

      else
         cdwTHROW("unknown line format or missing segment directive first");
   }
}

void processor::processSegmentDirective(const std::vector<std::string>& a)
{
   // segment directives
   m_oFile.objects.push_back(new cmn::objfmt::obj());
   m_pCurrObj = m_oFile.objects.back();
   m_pCurrObj->flags = parseObjFlags(a[0].c_str()+5);

   // setup stream for new object
   m_pBlock.reset(new cmn::compositeObjWriter());
   m_pBlock->sinkMemory<cmn::retailObjWriter>(m_pCurrObj->block);
   if(m_pListingFile1)
      m_pBlock->sinkExisting<cmn::listingObjWriter>(*m_pListingFile1);
   if(m_pListingFile2)
      m_pBlock->sinkExisting<cmn::odaObjWriter>(*m_pListingFile2);
}

void processor::processDataDecl(const std::string& l, const std::vector<std::string>& a, const std::string& rawLine)
{
   if(!l.empty()) exportSymbol(l);

   m_pBlock->writeCommentLine(m_parser.getLexor().getLineNumber(),rawLine);

   for(size_t i=1;i<a.size();i++)
   {
      dataLexor l(a[i].c_str());
      l.setFileName(m_parser.getLexor().getFileName());
      dataParser p(l,*this);
      p.parse(*m_pBlock);
      m_pCurrObj->blockSize = m_pBlock->tell();
   }
}

void processor::processInstr(const std::string& l, const std::vector<std::string>& a, const std::string& rawLine)
{
   if(!l.empty()) exportSymbol(l);

   m_pBlock->writeCommentLine(m_parser.getLexor().getLineNumber(),rawLine);

   if(a.size())
   {
      // find instr
      auto instrId = cmn::tgt::kFirstInstr;
      {
         auto it = m_instrMap.find(a[0]);
         if(it == m_instrMap.end())
            cdwTHROW("instr '%s' isn't known?",a[0].c_str());
         instrId = it->second;
      }

      // fine parse each argument
      std::vector<cmn::tgt::asmArgInfo> ai;
      std::vector<cmn::tgt::argTypes> aTypes;
      for(size_t i=1;i<a.size();i++)
      {
         ai.push_back(cmn::tgt::asmArgInfo());
         argLexor l(a[i].c_str());
         l.setFileName(m_parser.getLexor().getFileName());
         argParser p(l);
         p.parseArg(ai.back());
         aTypes.push_back(ai.back().computeArgType());
      }

      // locate the instr fmt to use
      auto& iFmt = m_t.getProc().getInstr(instrId)->demandFmt(aTypes);

      // go go gadet assembler!
      m_pAsm->assemble(iFmt,ai,*m_pBlock);
      m_pCurrObj->blockSize = m_pBlock->tell();
   }
}

unsigned long processor::parseObjFlags(const char *pText)
{
   std::string word(pText);
   if(word == cmn::objfmt::obj::kLexConst)
      return cmn::objfmt::obj::kSegConst;
   if(word == cmn::objfmt::obj::kLexData)
      return cmn::objfmt::obj::kSegData;
   if(word == cmn::objfmt::obj::kLexCode)
      return cmn::objfmt::obj::kSegCode;

   cdwTHROW("unknown segment '%s'",pText);
}

void processor::exportSymbol(const std::string& name)
{
   auto it = m_pCurrObj->xt.toc.find(name);
   if(it != m_pCurrObj->xt.toc.end())
      cdwTHROW("duplicate exported symbol '%s'",name.c_str());

   m_pCurrObj->xt.toc[name] = m_pBlock->tell();
}

void processor::importSymbol(const std::string& name, const cmn::objfmt::patch& p)
{
   m_pCurrObj->it.patches[name].push_back(p);
}

} // namespace shtasm

#include "../cmn/binWriter.hpp"
#include "../cmn/fmt.hpp"
#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/obj-fmt.hpp"
#include "../cmn/trace.hpp"
#include "assembler.hpp"
#include "frontend.hpp"
#include <string.h>

namespace shtasm {

std::string lineLexor::getNextLine()
{
   std::string line;
   std::getline(m_file,line);
   m_line++;
   return line;
}

void lineParser::parseLine(std::string& label, std::vector<std::string>& words, std::string& comment, std::string& rawLine)
{
   rawLine = m_l.getNextLine();
   const char *pThumb = rawLine.c_str();
   eatWhitespace(pThumb);

   // first, split on semicolon
   std::string code;
   bool found = shaveOffPart(pThumb,';',code);
   if(found)
   {
      eatWhitespace(pThumb);
      comment = pThumb;
      pThumb = code.c_str();
   }

   // second, split on colon
   shaveOffPart(pThumb,':',label);
   eatWhitespace(pThumb);

   // split on comma for args
   while(true)
   {
      std::string word;
      bool found = shaveOffPart(pThumb,',',word);
      eatWhitespace(pThumb);
      if(!found)
      {
         if(::strlen(pThumb))
            words.push_back(trimTrailingWhitespace(pThumb));
         break;
      }
      words.push_back(word);
   }
}

void lineParser::eatWhitespace(const char*& pThumb)
{
   for(;*pThumb==' '||*pThumb=='\t';++pThumb);
}

std::string lineParser::trimTrailingWhitespace(const std::string& w)
{
   size_t l = w.length();
   for(;l;--l)
   {
      if(w.c_str()[l-1] != ' ')
         break;
   }
   std::string copy(w.c_str(),l);
   return 1 ? copy : w; // WTH?  Without the ?: op here, GNU link tanks silently?
}

bool lineParser::shaveOffPart(const char*& pThumb, char delim, std::string& leftpart)
{
   const char *pNext = ::strchr(pThumb,delim);
   if(pNext && *pNext == delim)
   {
      leftpart = std::string(pThumb,pNext-pThumb);
      pThumb = pNext+1;
      return true;
   }
   return false;
}

static const cmn::lexemeInfo argScanTable[] = {
   { cmn::lexemeInfo::kSymbolic,     argLexor::kLBracket,    "[",        "L bracket"       },
   { cmn::lexemeInfo::kSymbolic,     argLexor::kRBracket,    "]",        "R bracket"       },

   { cmn::lexemeInfo::kSymbolic,     argLexor::kAsterisk,    "*",        "times"           },
   { cmn::lexemeInfo::kSymbolic,     argLexor::kHyphen,      "-",        "minus"           },
   { cmn::lexemeInfo::kSymbolic,     argLexor::kPlus,        "+",        "plus"            },

   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRax,      "rax",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRbx,      "rbx",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRcx,      "rcx",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRdx,      "rdx",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRbp,      "rbp",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRsp,      "rsp",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRsi,      "rsi",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegRdi,      "rdi",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR8,       "r8",       "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR9,       "r9",       "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR10,      "r10",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR11,      "r11",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR12,      "r12",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR13,      "r13",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR14,      "r14",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, argLexor::kRegR15,      "r15",      "register"        },

   { cmn::lexemeInfo::kEndOfTable,   0,                       NULL,       NULL              }
};

static const cmn::lexemeClassInfo argClassTable[] = {

   { argLexor::kClassReg64, "64-bit register",
      (size_t[]){
         argLexor::kRegRax,
         argLexor::kRegRbx,
         argLexor::kRegRcx,
         argLexor::kRegRdx,
         argLexor::kRegRbp,
         argLexor::kRegRsp,
         argLexor::kRegRsi,
         argLexor::kRegRdi,
         argLexor::kRegR8,
         argLexor::kRegR9,
         argLexor::kRegR10,
         argLexor::kRegR11,
         argLexor::kRegR12,
         argLexor::kRegR13,
         argLexor::kRegR14,
         argLexor::kRegR15,
         0,
      } },

   { argLexor::kClassRegAny, "any register",
      (size_t[]){
         argLexor::kRegRax,
         argLexor::kRegRbx,
         argLexor::kRegRcx,
         argLexor::kRegRdx,
         argLexor::kRegRbp,
         argLexor::kRegRsp,
         argLexor::kRegRsi,
         argLexor::kRegRdi,
         argLexor::kRegR8,
         argLexor::kRegR9,
         argLexor::kRegR10,
         argLexor::kRegR11,
         argLexor::kRegR12,
         argLexor::kRegR13,
         argLexor::kRegR14,
         argLexor::kRegR15,
         0,
      } },

   { argLexor::kNoClass, NULL, NULL },
};

argLexor::argLexor(const char *buffer)
: lexorBase(buffer)
{
   addPhase(*new cmn::intLiteralReader());
   addPhase(*new cmn::whitespaceEater());
   addTable(argScanTable,NULL);
   addClasses(argClassTable);

   advance();
}

void argParser::parseArg(cmn::tgt::asmArgInfo& i)
{
   m_pAi = &i;

   if(m_l.getTokenClass() & argLexor::kClassRegAny)
      parseReg();
   else if(m_l.getToken() == argLexor::kIntLiteral)
   {
      __int64 v = m_l.getLexemeInt();
      m_l.advance();
      switch(m_l.getLexemeIntSize(v))
      {
         case 1:
            m_pAi->flags |= cmn::tgt::asmArgInfo::kImm8;
            m_pAi->data.bytes.v[0] = v;
            break;
         case 2:
            m_pAi->flags |= cmn::tgt::asmArgInfo::kImm16;
            m_pAi->data.words.v[0] = v;
            break;
         case 4:
            m_pAi->flags |= cmn::tgt::asmArgInfo::kImm32;
            m_pAi->data.dwords.v[0] = v;
            break;
         case 8:
            m_pAi->flags |= cmn::tgt::asmArgInfo::kImm64;
            m_pAi->data.qwords.v[0] = v;
            break;
         default:
            cdwTHROW("unknow int size");
      }
   }
   else if(m_l.getToken() == argLexor::kName)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kLabel;
      m_pAi->label = m_l.getLexeme();
      m_l.advance();
   }
   else
      parseMemExpr();

   m_l.demand(cdwLoc,argLexor::kEOI);
}

void argParser::parseReg()
{
   if((m_l.getTokenClass() & argLexor::kClassRegAny) == 0)
      m_l.error(cdwLoc,"expected register");

   if(m_l.getTokenClass() & argLexor::kClassReg64)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kReg64;
      m_pAi->data.qwords.v[0] =
         m_l.getToken() - argLexor::kRegRax
            + cmn::tgt::i64::kRegA;
   }
   else
      m_l.error(cdwLoc,cmn::fmt("unknown register '%s'",m_l.getLexeme().c_str()));

   m_l.advance();
}

void argParser::parseMemExpr()
{
   m_l.demandAndEat(cdwLoc,argLexor::kLBracket);

   parseReg();
   parseScale();
   m_pAi->flags |= cmn::tgt::asmArgInfo::kMem64; // TODO until types are supported,
                                                 // assume all ptrs are 64-bit

   m_l.demandAndEat(cdwLoc,argLexor::kRBracket);
}

void argParser::parseScale()
{
   const bool negative = (m_l.getToken() == argLexor::kHyphen);

   if(negative || m_l.getToken() == argLexor::kPlus)
   {
      m_l.advance();

      m_l.demand(cdwLoc,argLexor::kIntLiteral);
      if(negative)
         m_pAi->disp = 0 - m_l.getLexemeInt();
      else
         m_pAi->disp = m_l.getLexemeInt();

      m_l.advance();
   }
}

static const cmn::lexemeInfo dataScanTable[] = {
   { cmn::lexemeInfo::kAlphanumeric, dataLexor::kB,      "<b>",   "byte type (b)"        },
   { cmn::lexemeInfo::kAlphanumeric, dataLexor::kW,      "<w>",   "short type (w)"       },
   { cmn::lexemeInfo::kAlphanumeric, dataLexor::kDW,     "<dw>",  "doubleword type (dw)" },
   { cmn::lexemeInfo::kAlphanumeric, dataLexor::kQW,     "<qw>",  "quadword type (qw)"   },

   { cmn::lexemeInfo::kAlphanumeric, dataLexor::kRep,    "<rep>", "rep"                  },
   { cmn::lexemeInfo::kSymbolic,     dataLexor::kLParen, "(",     "left paren"           },
   { cmn::lexemeInfo::kSymbolic,     dataLexor::kRParen, ")",     "right paren"          },

   { cmn::lexemeInfo::kEndOfTable,   0,                  NULL,    NULL                   }
};

static const cmn::lexemeClassInfo dataClassTable[] = {

   { dataLexor::kIntLitType, "int lit type",
      (size_t[]){
         dataLexor::kB,
         dataLexor::kW,
         dataLexor::kDW,
         dataLexor::kQW,
         0,
      } },

   { dataLexor::kNoClass, NULL, NULL },
};

dataLexor::dataLexor(const char *buffer)
: lexorBase(buffer)
{
   addPhase(*new cmn::stringLiteralReader());
   addPhase(*new cmn::intLiteralReader());
   addPhase(*new cmn::whitespaceEater());
   addTable(dataScanTable,NULL);
   addClasses(dataClassTable);

   advance();
}

void repObjWriter::write(const std::string& reason, const void *p, size_t n)
{
   for(size_t i=0;i<m_rep;i++)
      m_inner.write(reason,p,n);

   m_rep = 1;
}

void dataParser::parse(cmn::iObjWriter& w)
{
   parseData(w);
   m_l.demand(cdwLoc,dataLexor::kEOI);
   w.nextPart();
}

void dataParser::parseData(cmn::iObjWriter& w)
{
   if(m_l.getToken() == dataLexor::kRep)
   {
      m_l.advance();

      m_l.demandAndEat(cdwLoc,dataLexor::kLParen);

      m_l.demand(cdwLoc,dataLexor::kIntLiteral);
      auto r = m_l.getLexemeInt();
      if(r < 1)
         m_l.error(cdwLoc,"value for rep() must be > 0");
      m_l.advance();
      repObjWriter rw(w);
      rw.setRep(r);

      m_l.demandAndEat(cdwLoc,dataLexor::kRParen);

      parseRepableData(rw,true);
   }
   else if(m_l.getToken() == dataLexor::kName)
   {
      // TODO I hate absolute!  How do function pointers work in C++?
      cmn::objfmt::patch p;
      p.type = cmn::objfmt::patch::kRelToNextInstr;
      p.offset = w.tell();
      p.fromOffsetToEndOfInstr = sizeof(unsigned __int64);

      unsigned __int64 lbl = 0;
      w.write("_lbldata",lbl);

      m_tw.importSymbol(m_l.getLexeme(),p);
      m_l.advance();

      parseData(w);
   }
   else
      parseRepableData(w,false);
}

void dataParser::parseRepableData(cmn::iObjWriter& w, bool req)
{
   if(m_l.getToken() == dataLexor::kStringLiteral)
   {
      w.write("_strdata",m_l.getLexeme().c_str(),m_l.getLexeme().length());
      m_l.advance();
      parseData(w);
   }

   else if(m_l.getTokenClass() & dataLexor::kIntLitType)
   {
      auto type = m_l.getToken();
      m_l.advance();
      auto value = m_l.getLexemeInt();
      m_l.advance();

      switch(type)
      {
         case dataLexor::kB:
            writeInt<signed char>(w,value);
            break;
         case dataLexor::kW:
            writeInt<signed short>(w,value);
            break;
         case dataLexor::kDW:
            writeInt<signed long>(w,value);
            break;
         case dataLexor::kQW:
            writeInt<signed __int64>(w,value);
            break;
         default:
            cdwTHROW("unimpled");
      }

      parseData(w);
   }

   else if(!req && m_l.getToken() == dataLexor::kEOI)
      return;

   else
   {
      if(req)
         m_l.demandOneOf(cdwLoc,2,
            dataLexor::kStringLiteral,
            dataLexor::kIntLitType
         );
      else
         m_l.demandOneOf(cdwLoc,3,
            dataLexor::kStringLiteral,
            dataLexor::kIntLitType,
            dataLexor::kEOI
         );
   }
}

} // namespace shtasm

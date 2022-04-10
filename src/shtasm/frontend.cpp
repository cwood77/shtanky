#include "../cmn/fmt.hpp"
#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
#include "frontend.hpp"
#include <string.h>

namespace shtasm {

std::string lexor::getNextLine()
{
   std::string line;
   std::getline(m_file,line);
   m_line++;
   return line;
}

void parser::parseLine(std::string& label, std::vector<std::string>& words, std::string& comment, std::string& rawLine)
{
   rawLine = m_l.getNextLine();
   const char *pThumb = rawLine.c_str();
   eatWhitespace(pThumb);

   // first, split on ;
   std::string code;
   bool found = shaveOffPart(pThumb,';',code);
   if(found)
   {
      eatWhitespace(pThumb);
      comment = pThumb;
      pThumb = code.c_str();
   }

   shaveOffPart(pThumb,':',label);
   eatWhitespace(pThumb);

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

void parser::eatWhitespace(const char*& pThumb)
{
   for(;*pThumb==' '||*pThumb=='\t';++pThumb);
}

std::string parser::trimTrailingWhitespace(const std::string& w)
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

bool parser::shaveOffPart(const char*& pThumb, char delim, std::string& part)
{
   const char *pNext = ::strchr(pThumb,delim);
   if(pNext && *pNext == delim)
   {
      part = std::string(pThumb,pNext-pThumb);
      pThumb = pNext+1;
      return true;
   }
   return false;
}

static const cmn::lexemeInfo scanTable[] = {
   { cmn::lexemeInfo::kSymbolic,     fineLexor::kLBracket,    "[",        "L bracket"       },
   { cmn::lexemeInfo::kSymbolic,     fineLexor::kRBracket,    "]",        "R bracket"       },

   { cmn::lexemeInfo::kSymbolic,     fineLexor::kPlus,        "+",        "plus"            },

   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRax,      "rax",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRbx,      "rbx",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRcx,      "rcx",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRdx,      "rdx",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRbp,      "rbp",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRsp,      "rsp",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRsi,      "rsi",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegRdi,      "rdi",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR8,       "r8",       "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR9,       "r9",       "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR10,      "r10",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR11,      "r11",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR12,      "r12",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR13,      "r13",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR14,      "r14",      "register"        },
   { cmn::lexemeInfo::kAlphanumeric, fineLexor::kRegR15,      "r15",      "register"        },

   { cmn::lexemeInfo::kEndOfTable,   0,                       NULL,       NULL              }
};

static const cmn::lexemeClassInfo classTable[] = {

   { fineLexor::kClassReg64, "64-bit register",
      (size_t[]){
         fineLexor::kRegRax,
         fineLexor::kRegRbx,
         fineLexor::kRegRcx,
         fineLexor::kRegRdx,
         fineLexor::kRegRbp,
         fineLexor::kRegRsp,
         fineLexor::kRegRsi,
         fineLexor::kRegRdi,
         fineLexor::kRegR8,
         fineLexor::kRegR9,
         fineLexor::kRegR10,
         fineLexor::kRegR11,
         fineLexor::kRegR12,
         fineLexor::kRegR13,
         fineLexor::kRegR14,
         fineLexor::kRegR15,
         0,
      } },

   { fineLexor::kClassRegAny, "any register",
      (size_t[]){
         fineLexor::kRegRax,
         fineLexor::kRegRbx,
         fineLexor::kRegRcx,
         fineLexor::kRegRdx,
         fineLexor::kRegRbp,
         fineLexor::kRegRsp,
         fineLexor::kRegRsi,
         fineLexor::kRegRdi,
         fineLexor::kRegR8,
         fineLexor::kRegR9,
         fineLexor::kRegR10,
         fineLexor::kRegR11,
         fineLexor::kRegR12,
         fineLexor::kRegR13,
         fineLexor::kRegR14,
         fineLexor::kRegR15,
         0,
      } },

   { fineLexor::kNoClass, NULL, NULL },
};

fineLexor::fineLexor(const char *buffer)
: lexorBase(buffer)
{
   addPhase(*new cmn::intLiteralReader());
   addPhase(*new cmn::whitespaceEater());
   addTable(scanTable,NULL);
   addClasses(classTable);

   advance();
}

void fineParser::parseArg(cmn::tgt::asmArgInfo& i)
{
   m_pAi = &i;

   if(m_l.getTokenClass() & fineLexor::kClassRegAny)
      parseReg();
   else if(m_l.getToken() == fineLexor::kIntLiteral)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kImm8;
      m_pAi->data.bytes.v[0] = ::atoi(m_l.getLexeme().c_str());
      m_l.advance();
   }
   else if(m_l.getToken() == fineLexor::kName)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kLabel;
      m_l.advance();
   }
   else
      parseMemExpr();

   m_l.demand(fineLexor::kEOI);
}

void fineParser::parseReg()
{
   if((m_l.getTokenClass() & fineLexor::kClassRegAny) == 0)
      m_l.error("expected register");

   if(m_l.getTokenClass() & fineLexor::kClassReg64)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kReg64;
      m_pAi->data.qwords.v[0] =
         m_l.getToken() - fineLexor::kRegRax
            + cmn::tgt::i64::kRegA;
   }
   else
      m_l.error(cmn::fmt("unknown register '%s'",m_l.getLexeme().c_str()));

   m_l.advance();
}

void fineParser::parseMemExpr()
{
   if(m_l.getToken() == fineLexor::kLBracket)
   {
      m_l.advance();
      m_pAi->flags |= cmn::tgt::asmArgInfo::kPtr;
      m_pAi->flags |= cmn::tgt::asmArgInfo::kMem8; // 64?

      parseReg();
      parseScale();

      m_l.demandAndEat(fineLexor::kRBracket);
   }
   else
      m_l.demand(fineLexor::kLBracket);
}

void fineParser::parseScale()
{
   if(m_l.getToken() == fineLexor::kPlus)
   {
      m_l.advance();

      m_l.demand(fineLexor::kIntLiteral);

      m_l.advance();
   }
}

} // namespace shtasm

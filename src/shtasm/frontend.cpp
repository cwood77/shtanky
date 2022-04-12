#include "../cmn/fmt.hpp"
#include "../cmn/i64asm.hpp"
#include "../cmn/intel64.hpp"
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

bool lineParser::shaveOffPart(const char*& pThumb, char delim, std::string& part)
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
   { cmn::lexemeInfo::kSymbolic,     argLexor::kLBracket,    "[",        "L bracket"       },
   { cmn::lexemeInfo::kSymbolic,     argLexor::kRBracket,    "]",        "R bracket"       },

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

static const cmn::lexemeClassInfo classTable[] = {

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
   addTable(scanTable,NULL);
   addClasses(classTable);

   advance();
}

void argParser::parseArg(cmn::tgt::asmArgInfo& i)
{
   m_pAi = &i;

   if(m_l.getTokenClass() & argLexor::kClassRegAny)
      parseReg();
   else if(m_l.getToken() == argLexor::kIntLiteral)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kImm8;
      m_pAi->data.bytes.v[0] = ::atoi(m_l.getLexeme().c_str());
      m_l.advance();
   }
   else if(m_l.getToken() == argLexor::kName)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kLabel;
      m_pAi->label = m_l.getLexeme();
      m_l.advance();
   }
   else
      parseMemExpr();

   m_l.demand(argLexor::kEOI);
}

void argParser::parseReg()
{
   if((m_l.getTokenClass() & argLexor::kClassRegAny) == 0)
      m_l.error("expected register");

   if(m_l.getTokenClass() & argLexor::kClassReg64)
   {
      m_pAi->flags |= cmn::tgt::asmArgInfo::kReg64;
      m_pAi->data.qwords.v[0] =
         m_l.getToken() - argLexor::kRegRax
            + cmn::tgt::i64::kRegA;
   }
   else
      m_l.error(cmn::fmt("unknown register '%s'",m_l.getLexeme().c_str()));

   m_l.advance();
}

void argParser::parseMemExpr()
{
   if(m_l.getToken() == argLexor::kLBracket)
   {
      m_l.advance();
      m_pAi->flags |= cmn::tgt::asmArgInfo::kPtr;
      m_pAi->flags |= cmn::tgt::asmArgInfo::kMem8; // 64?

      parseReg();
      parseScale();

      m_l.demandAndEat(argLexor::kRBracket);
   }
   else
      m_l.demand(argLexor::kLBracket);
}

void argParser::parseScale()
{
   if(m_l.getToken() == argLexor::kPlus)
   {
      m_l.advance();

      m_l.demand(argLexor::kIntLiteral);

      m_l.advance();
   }
}

dataLexor::dataLexor(const char *buffer)
: lexorBase(buffer)
{
   addPhase(*new cmn::stringLiteralReader());
   addPhase(*new cmn::intLiteralReader());
   addPhase(*new cmn::whitespaceEater());

   advance();
}

} // namespace shtasm

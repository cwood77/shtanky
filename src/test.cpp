#include "cmn/lexor.hpp"
#include <stdio.h>
#include <string.h>

#include <fstream>
#include <sstream>

namespace cmn {

lexorState::lexorState(const char *p)
: pThumb(p)
, token(lexorBase::kUnknown)
, lineNumber(1)
{
}

void stringLiteralReader::collectTerminators(std::string& t) const
{
   t += "'\"";
}

void stringLiteralReader::advance(lexorState& s) const
{
   if(s.pThumb[0] != '\"')
      return;

   const char *pEnd = s.pThumb + 1;
   for(;::strchr("'\"",*pEnd)==NULL;pEnd++);

   s.lexeme = std::string(s.pThumb,pEnd-s.pThumb);

   // unwrap quotes
   s.lexeme = std::string(s.lexeme.c_str()+1,s.lexeme.length()-2);

   s.pThumb = pEnd+1;
   s.token = lexorBase::kStringLiteral;
}

void whitespaceEater::collectTerminators(std::string& t) const
{
   t += " \t\r\n";
}

void whitespaceEater::advance(lexorState& s) const
{
   if(s.pThumb[0] == 0)
      ;
   else if(::strchr(" \t\r",s.pThumb[0]))
   {
      s.pThumb++;
   }
   else if(::strchr("\n",s.pThumb[0]))
   {
      s.lineNumber++;
      s.pThumb++;
   }
}

lexorBase::lexorBase(const lexemeInfo *pTable, const char *buffer)
: m_state(buffer)
{
   processTable(pTable);
}

lexorBase::~lexorBase()
{
   for(auto it=m_phases.begin();it!=m_phases.end();++it)
      delete *it;
}

void lexorBase::advance()
{
   if(m_state.token == kEOI)
      return;
   m_state.token = kUnknown;
   m_state.lexeme = "";

   runPhasesUntilSteady();
   if(m_state.token != kUnknown)
      return;

   matchSymbolic();
   if(m_state.token != kUnknown)
      return;

   matchAlphanumeric();
   if(m_state.token != kUnknown)
      return;

   if(m_state.lexeme.length() > 0)
   {
      m_state.pThumb += m_state.lexeme.length();
      m_state.token = kName;
   }
   else if(m_state.pThumb[0] == 0)
      m_state.token = kEOI;
}

std::string lexorBase::getTokenName(size_t t)
{
   if(t == kEOI)
      return "EOI";
   else if(t == kUnknown)
      return "unknown";
   else if(t == kName)
      return "name";
   else if(t == kStringLiteral)
      return "string literal";
   else
      return m_lexemeDict[t]->name;
}

void lexorBase::addPhase(iLexorPhase& p)
{
   m_phases.push_back(&p);
   p.collectTerminators(m_terminators);
}

void lexorBase::processTable(const lexemeInfo *pTable)
{
   for(size_t i=0;;i++)
   {
      const lexemeInfo& l = pTable[i];
      if(l.k == lexemeInfo::kEndOfTable)
         break;

      if(l.k == lexemeInfo::kSymbolic)
      {
         m_symbolics[l.lexeme] = &l;
         m_terminators += std::string(l.lexeme,1);
      }
      else if(l.k == lexemeInfo::kAlphanumeric)
         m_alphas[l.lexeme] = &l;

      m_lexemeDict[l.token] = &l;
   }
}

void lexorBase::runPhasesUntilSteady()
{
   while(true)
   {
      auto last = m_state.pThumb;

      for(auto it=m_phases.begin();it!=m_phases.end();++it)
      {
         (*it)->advance(m_state);
         if(m_state.token != kUnknown)
            return;
      }

      // acheived steady state
      if(m_state.pThumb == last)
         break;
   }
}

void lexorBase::matchSymbolic()
{
   for(auto it=m_symbolics.rbegin();it!=m_symbolics.rend();++it)
   {
      if(::strncmp(m_state.pThumb,it->first.c_str(),it->first.length())==0)
      {
         m_state.pThumb += it->first.length();
         m_state.token = it->second->token;
         m_state.lexeme = it->second->lexeme;
         return;
      }
   }
}

void lexorBase::matchAlphanumeric()
{
   const char *pEnd = m_state.pThumb;
   for(;::strchr(m_terminators.c_str(),*pEnd)==NULL;pEnd++);

   m_state.lexeme = std::string(m_state.pThumb,pEnd-m_state.pThumb);

   auto it = m_alphas.find(m_state.lexeme);
   if(it != m_alphas.end())
   {
      m_state.pThumb += m_state.lexeme.length();
      m_state.token = it->second->token;
   }
}

} // namespace cmn

namespace araceli {

static const cmn::lexemeInfo scanTable[] = {
   { cmn::lexemeInfo::kSymbolic,     lexor::kLBracket,  "[",        "L bracket" },
   { cmn::lexemeInfo::kSymbolic,     lexor::kRBracket,  "]",        "R bracket" },
   { cmn::lexemeInfo::kSymbolic,     lexor::kLParen,    "(",        "L paren"   },
   { cmn::lexemeInfo::kSymbolic,     lexor::kRParen,    ")",        "R paren"   },
   { cmn::lexemeInfo::kSymbolic,     lexor::kLBrace,    "{",        "L brace"   },
   { cmn::lexemeInfo::kSymbolic,     lexor::kRBrace,    "}",        "R brace"   },
   { cmn::lexemeInfo::kSymbolic,     lexor::kColon,     ":",        "colon"     },
   { cmn::lexemeInfo::kSymbolic,     lexor::kSemiColon, ";",        "semicolon" },
   { cmn::lexemeInfo::kSymbolic,     lexor::kArrow,     "->",       "arrow"     },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kClass,     "class",    "class"     },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kOverride,  "override", "override"  },
   { cmn::lexemeInfo::kAlphanumeric, lexor::kStr,       "str",      "str"       },
   { cmn::lexemeInfo::kEndOfTable,   0,                 NULL,       NULL        }
};

lexor::lexor(const char *buffer)
: lexorBase(scanTable,buffer)
{
   addPhase(*new cmn::stringLiteralReader());
   addPhase(*new cmn::whitespaceEater());
   advance();
}

} // namespace araceli

int main(int,char*[])
{
   ::printf("testing lexor\n");

   std::stringstream stream;
   {
      std::ifstream src("src\\test.ara");
      while(src.good())
      {
         std::string line;
         std::getline(src,line);
         stream << line << std::endl;
      }
   }

   std::string copy = stream.str();
   araceli::lexor l(copy.c_str());

   while(true)
   {
      ::printf("%s(%s)\n",l.getTokenName().c_str(),l.getLexeme().c_str());
      l.advance();
      if(l.getToken() == cmn::lexorBase::kEOI)
         break;
   }

   return 0;
}

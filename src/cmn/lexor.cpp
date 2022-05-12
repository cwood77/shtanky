#include "inttypes.h"
#include "lexor.hpp"
#include <sstream>
#include <stdarg.h>
#include <stdexcept>
#include <string.h>

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
   s.lexeme = std::string(s.lexeme.c_str()+1,s.lexeme.length()-1);

   s.pThumb = pEnd+1;
   s.token = lexorBase::kStringLiteral;
}

void intLiteralReader::advance(lexorState& s) const
{
   if(!::isdigit(s.pThumb[0]))
      return;

   const char *pEnd = s.pThumb + 1;
   for(;::isdigit(*pEnd);pEnd++);

   s.lexeme = std::string(s.pThumb,pEnd-s.pThumb);

   s.pThumb = pEnd;
   s.token = lexorBase::kIntLiteral;
}

void genericTypeReader::advance(lexorState& s) const
{
   const char *pThumb = s.pThumb;
   for(;::isalnum(*pThumb)||*pThumb=='.'||*pThumb=='_';++pThumb);
   if(pThumb == s.pThumb) return; // must have alphanumeric prefix (. is allowed for FQNs)
   if(*pThumb != '<') return; // open <
   const char *pPayload = ++pThumb;
   for(int depth=0;;++pThumb) // anything, then > (but allow nesting)
   {
      if(*pThumb == 0) return;
      if(*pThumb == '<') depth++;
      if(*pThumb == '>')
      {
         if(depth==0) break;
         depth--;
      }
   }
   if(*pThumb != '>') return;
   ++pThumb;
   // suffixes are allowed for things like list<bool>_vtbl (i.e. derived type names)
   for(;::isalnum(*pThumb)||*pThumb=='.'||*pThumb=='_';++pThumb);
   std::string payload(pPayload,pThumb-pPayload);
   bool hasComma = (payload.find(',')!=std::string::npos);
   bool hasSpaces = (payload.find(' ')!=std::string::npos);

   if(hasComma || !hasSpaces)
   {
      s.lexeme = std::string(s.pThumb,pThumb - s.pThumb);
      s.token = lexorBase::kGenericTypeExpr;
      s.pThumb = pThumb;
   }
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

lexorBase::lexorBase(const char *buffer)
: m_state(buffer)
{
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

size_t lexorBase::getTokenClass(size_t t)
{
   return m_tokenToClassMap[t];
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
   else if(t == kIntLiteral)
      return "int literal";
   else if(t == kGenericTypeExpr)
      return "generic type expression";
   else
      return m_lexemeDict[t];
}

__int64 lexorBase::getLexemeInt(const std::string& str)
{
   __int64 v = 0;
   ::sscanf(str.c_str(),"%" PRIi64,&v);
   return v;
}

char lexorBase::getLexemeIntSize(const __int64& v)
{
   if(-128 <= v && v <= 127)
      return 1;
   if(-32768 <= v && v <= 32767)
      return 2;
   if(-2147483648 <= v && v <= 2147483647)
      return 4;
   return 8;
}

void lexorBase::demand(const char *f, unsigned long l, size_t t)
{
   if(getToken() != t)
   {
      std::stringstream msg;
      msg << "expected " << getTokenName(t) << " but got " << getTokenName();
      error(f,l,msg.str());
   }
}

void lexorBase::demandOneOf(const char *f, unsigned long l, size_t n, ...)
{
   std::stringstream msg;
   msg << "expected ";

   bool first = true;
   bool demandFailed = true;
   va_list ap;
   va_start(ap,n);
   for(size_t i=0;i<n;i++)
   {
      int t = va_arg(ap,int);
      if(m_unsupported.find(t)!=m_unsupported.end())
         continue;
      if(!first)
         msg << ", ";
      msg << getTokenName(t);
      first = false;

      if(getToken() == static_cast<size_t>(t))
         demandFailed = false;
   }
   va_end(ap);

   if(!demandFailed)
      return;

   msg << " but got " << getTokenName();
   error(f,l,msg.str());
}

void lexorBase::error(const char *f, unsigned long l, const std::string& msg)
{
   std::string fileAppendage = " in " + m_state.fileName;

   std::stringstream fullMsg;
   fullMsg << msg << " near line " << m_state.lineNumber
      << ( m_state.fileName.empty() ? "" : fileAppendage )
      << " {in parser " << f << ", " << l << "}"
      << " [lexeme=" << m_state.lexeme << "]";

   throw std::runtime_error(fullMsg.str());
}

void lexorBase::addPhase(iLexorPhase& p)
{
   m_phases.push_back(&p);
   p.collectTerminators(m_terminators);
}

void lexorBase::addTable(const lexemeInfo *pTable, const size_t *pUnsupported)
{
   if(pUnsupported)
      for(;*pUnsupported;++pUnsupported)
         m_unsupported.insert(*pUnsupported);

   for(size_t i=0;;i++)
   {
      const lexemeInfo& l = pTable[i];
      if(l.k == lexemeInfo::kEndOfTable)
         break;

      if(m_unsupported.find(l.token)==m_unsupported.end())
      {
         if(l.k == lexemeInfo::kSymbolic)
         {
            m_symbolics[l.lexeme] = &l;
            m_terminators += std::string(l.lexeme,1);
         }
         else if(l.k == lexemeInfo::kAlphanumeric)
            m_alphas[l.lexeme] = &l;
      }

      m_lexemeDict[l.token] = l.name;
   }
}

void lexorBase::addClasses(const lexemeClassInfo *pClasses)
{
   for(size_t i=0;;i++)
   {
      const lexemeClassInfo& c = pClasses[i];
      if(c.Class == kNoClass)
         break;

      m_lexemeDict[c.Class] = c.name;

      for(size_t j=0;;j++)
      {
         if(c.pTokens[j] == 0)
            break;

         m_tokenToClassMap[c.pTokens[j]] |= c.Class;
      }
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

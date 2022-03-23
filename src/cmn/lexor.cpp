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
   s.lexeme = std::string(s.lexeme.c_str()+1,s.lexeme.length()-2);

   s.pThumb = pEnd+1;
   s.token = lexorBase::kStringLiteral;
}

void intLiteralReader::collectTerminators(std::string& t) const
{
   t += "0123456789";
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
   else if(t == kIntLiteral)
      return "int literal";
   else
      return m_lexemeDict[t]->name;
}

void lexorBase::demand(size_t t)
{
   if(getToken() != t)
   {
      std::stringstream msg;
      msg << "expected " << getTokenName(t) << " but got " << getTokenName();
      error(msg.str());
   }
}

void lexorBase::demandOneOf(size_t n, ...)
{
   std::stringstream msg;
   msg << "expected ";

   va_list ap;
   va_start(ap,n);
   for(size_t i=0;i<n;i++)
   {
      if(i)
         msg << ", ";
      int t = va_arg(ap,int);
      msg << getTokenName(t);
   }
   va_end(ap);

   msg << " but got " << getTokenName();
   error(msg.str());
}

void lexorBase::error(const std::string& msg)
{
   std::stringstream fullMsg;
   fullMsg << msg << " near line " << m_state.lineNumber;

   throw std::runtime_error(fullMsg.str());
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
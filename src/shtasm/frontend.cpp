#include "frontend.hpp"
#include <string.h>

namespace shtasm {

std::string lexor::getNextLine()
{
   std::string line;
   std::getline(m_file,line);
   return line;
}

void parser::parseLine(std::string& label, std::vector<std::string>& words, std::string& comment)
{
   std::string l = m_l.getNextLine();
   const char *pThumb = l.c_str();
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
   // l is last non-space char or 0
   return std::string(w.c_str(),l);
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

} // namespace shtasm

#include "unique.hpp"
#include <sstream>

namespace cmn {

std::string uniquifier::makeUnique(const std::string& s)
{
   if(m_set.find(s)==m_set.end())
   {
      m_set.insert(s);
      return s;
   }

   unsigned long i = 0;
   while(true)
   {
      std::stringstream str;
      str << s << i;
      std::string noob = str.str();

      if(m_set.find(noob)==m_set.end())
      {
         m_set.insert(noob);
         return noob;
      }
      else
         i++;
   }
}

} // namespace cmn

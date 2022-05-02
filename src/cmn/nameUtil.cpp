#include "nameUtil.hpp"
#include <string.h>

namespace cmn {

bool nameUtil::isAbsolute(const std::string& n)
{
   return n.c_str()[0] == '.';
}

std::string nameUtil::append(const std::string& prefix, const std::string& n)
{
   std::string base = prefix;
   if(base.length() && base.c_str()[base.length()-1] != '.')
      base += ".";

   if(n.c_str()[0] == '.')
      base += (n.c_str() + 1);
   else
      base += n;

   return base;
}

std::string nameUtil::stripLast(const std::string& n, std::string& last)
{
   const char *pLastDot = ::strrchr(n.c_str(),'.');
   if(*pLastDot != '.')
      return n;

   std::string ans(n.c_str(),pLastDot-n.c_str());
   if(ans.empty())
      ans = ".";
   last = pLastDot+1;
   return ans;
}

} // namespace cmn

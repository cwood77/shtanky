#include "fmt.hpp"
#include "trace.hpp"
#include <stdarg.h>

namespace cmn {

void trace::write(const std::string& fmt, ...)
{
   va_list ap;
   va_start(ap,fmt);
   std::string buffer = vfmt(fmt,ap);
   va_end(ap);

   const char *kStrings[] = { "i", "v", "D" };

   if(m_context)
      ::printf("[%s:%s-%lu] [%s] %s",kStrings[m_le],m_fi,m_li,m_context,buffer.c_str());
   else
      ::printf("[%s:%s-%lu] %s",     kStrings[m_le],m_fi,m_li,          buffer.c_str());
}

} // namespace cmn

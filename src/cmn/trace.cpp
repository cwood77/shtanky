#include "trace.hpp"
#include <stdio.h>
#include <stdarg.h>

namespace cmn {

void trace::write(const std::string& fmt, ...)
{
   char buffer[1024];
   va_list ap;
   va_start(ap,fmt);
   ::vsnprintf(buffer,1023,fmt.c_str(),ap);
   va_end(ap);

   const char *kStrings[] = { "info", "verbose", "debug" };

   ::printf("[%s-%s-%lu] %s",kStrings[m_le],m_fi,m_li,buffer);
}

} // namespace cmn

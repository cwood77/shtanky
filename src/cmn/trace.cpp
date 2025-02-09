#define WIN32_LEAN_AND_MEAN
#include "fmt.hpp"
#include "pathUtil.hpp"
#include "trace.hpp"
#include <stdarg.h>
#include <windows.h>

namespace cmn {

trace::levels trace::filter = trace::kDebug;
std::string trace::m_appName;

void trace::write(const std::string& fmt, ...)
{
   if(m_le > filter)
      return;

   va_list ap;
   va_start(ap,fmt);
   std::string buffer = vfmt(fmt,ap);
   va_end(ap);

   const char *kStrings[] = { "i", "v", "D" };

   if(m_context)
      ::printf("[%s:%s:%s-%lu] [%s] %s",getAppName(),kStrings[m_le],m_fi,m_li,m_context,buffer.c_str());
   else
      ::printf("[%s:%s:%s-%lu] %s",     getAppName(),kStrings[m_le],m_fi,m_li,          buffer.c_str());
}

const char *trace::getAppName()
{
   if(m_appName.empty())
   {
      char buffer[1024];
      ::memset(buffer,0,1024);
      ::GetModuleFileName(NULL,buffer,1024);
      m_appName = pathUtil::getLastPart(buffer);
   }

   return m_appName.c_str();
}

} // namespace cmn

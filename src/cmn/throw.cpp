#include "fmt.hpp"
#include "throw.hpp"
#include <stdarg.h>

namespace cmn {

std::runtime_error thrower::create(const std::string& msg, ...)
{
   va_list ap;
   va_start(ap,msg);
   auto umsg = cmn::vfmt(msg,ap);
   va_end(ap);

   auto _msg = cmn::fmt("%s [line %lu, file: %s]",umsg.c_str(),m_l,m_f);
   return std::runtime_error(_msg);
}

} // namespace cmn

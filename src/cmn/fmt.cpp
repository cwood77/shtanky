#include "fmt.hpp"
#include <memory>
#include <stdio.h>

namespace cmn {

std::string fmt(const std::string& f, ...)
{
   va_list ap;
   va_start(ap,f);
   auto rval = vnfmt(1024,f,ap);
   va_end(ap);
   return rval;
}

std::string nfmt(size_t len, const std::string& f, ...)
{
   va_list ap;
   va_start(ap,f);
   auto rval = vnfmt(len,f,ap);
   va_end(ap);
   return rval;
}

std::string vnfmt(size_t len, const std::string& f, va_list ap)
{
   std::unique_ptr<char[]> pBuffer(new char[len]);
   ::vsnprintf(pBuffer.get(),len-1,f.c_str(),ap);
   return std::string(pBuffer.release());
}

} // namespace cmn

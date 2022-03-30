#pragma once
#include <stdarg.h>
#include <string>

namespace cmn {

std::string fmt(const std::string& f, ...);

std::string nfmt(size_t len, const std::string& f, ...);

std::string vnfmt(size_t len, const std::string& f, va_list ap);

} // namespace cmn

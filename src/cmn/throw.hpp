#pragma once
#include <stdexcept>

#define cdwTHROW throw cmn::thrower(__FILE__,__LINE__).create

namespace cmn {

class thrower {
public:
   thrower(const char *file, unsigned long line) : m_f(file), m_l(line) {}

   std::runtime_error create(const std::string& msg, ...);

private:
   const char *m_f;
   unsigned long m_l;
};

} // namespace cmn

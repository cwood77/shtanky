#include "fmt.hpp"
#include "throw.hpp"

namespace cmn {

std::runtime_error thrower::create(const std::string& msg)
{
   auto _msg = cmn::fmt("%s [line %lu, file: %s]",msg.c_str(),m_l,m_f);
   return std::runtime_error(_msg);
}

} // namespace cmn

#include "cmdline.hpp"

namespace cmn {

std::string cmdLine::getArg(const std::string& defValue)
{
   if(m_i < m_argc)
      return m_pArgv[m_i++];
   else
      return defValue;
}

} // namespace cmn

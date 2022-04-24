#include "cmdline.hpp"

namespace cmn {

cmdLine::cmdLine(int argc, const char *argv[]) : m_i(0)
{
   m_args.reserve(argc-1);
   for(int i=1;i<argc;i++)
      m_args.push_back(argv[i]);
}

bool cmdLine::getSwitch(const std::string& on, const std::string& off, bool defValue)
{
   for(size_t i=0;i<m_args.size();i++)
   {
      bool ans = (m_args[i] == on);
      if(ans || m_args[i] == off)
      {
         m_args.erase(m_args.begin()+i);
         return ans;
      }
   }

   return defValue;
}

std::string cmdLine::getOption(const std::string& tag, const std::string& defValue)
{
   for(size_t i=0;i<m_args.size();i++)
   {
      if(m_args[i] == tag && (i+1)<m_args.size())
      {
         std::string ans = m_args[i+1];
         m_args.erase(m_args.begin()+i);
         m_args.erase(m_args.begin()+i);
         return ans;
      }
   }

   return defValue;
}

std::string cmdLine::getNextArg(const std::string& defValue)
{
   if(m_i < m_args.size())
      return m_args[m_i++];
   else
      return defValue;
}

} // namespace cmn

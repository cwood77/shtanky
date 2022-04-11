#pragma once
#include <string>

namespace cmn {

class cmdLine {
public:
   cmdLine(int argc, const char *argv[]) : m_argc(argc), m_i(1), m_pArgv(argv) {}

   std::string getArg(const std::string& defValue);

private:
   const int m_argc;
   int m_i;
   const char **m_pArgv;
};

} // namespace cmn

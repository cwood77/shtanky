#pragma once
#include <string>
#include <vector>

namespace cmn {

class cmdLine {
public:
   cmdLine(int argc, const char *argv[]);

   bool getSwitch(const std::string& on, const std::string& off, bool defValue);

   std::string getOption(const std::string& tag, const std::string& defValue);

   // always handle switches and options FIRST before using this method
   std::string getNextArg(const std::string& defValue);

   void addNextArgDefaultsIfNoneLeft(size_t cnt, ...);

private:
   void configTrace();

   std::vector<std::string> m_args;
   size_t m_i;
};

} // namespace cmn

#pragma once
#include <sstream>

namespace araceli {

class invokeAraceliSliceProcess {
public:
   invokeAraceliSliceProcess(
      const std::string& shortExeName,
      const std::string& projectDir,
      const std::string& ext);

   invokeAraceliSliceProcess& withArg(const std::string& text);

   void runAndWait();

private:
   std::string m_shortExeName;
   std::stringstream m_invokeCmd;
};

} // namespace araceli

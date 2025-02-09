#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "subprocess.hpp"

namespace araceli {

invokeAraceliSliceProcess::invokeAraceliSliceProcess(const std::string& shortExeName, const std::string& projectDir, const std::string& ext)
: m_shortExeName(shortExeName)
{
   m_invokeCmd << "bin\\out\\debug\\" << shortExeName << ".exe ";
   m_invokeCmd << projectDir;
   m_invokeCmd << " " << ext;
}

invokeAraceliSliceProcess& invokeAraceliSliceProcess::withArg(const std::string& text)
{
   m_invokeCmd << " " << text;
   return *this;
}

void invokeAraceliSliceProcess::runAndWait()
{
   cdwVERBOSE("calling: %s\n",m_invokeCmd.str().c_str());
   ::_flushall();
   int rval = ::system(m_invokeCmd.str().c_str());
   cdwDEBUG("*************************************************\n");
   cdwDEBUG("**   returned to araceli\n");
   cdwDEBUG("*************************************************\n");
   cdwVERBOSE("rval = %d\n",rval);
   if(rval != 0)
   {
      cdwINFO("%s failed with code %d; aborting\n",m_shortExeName,rval);
      cdwTHROW("child process failed");
   }
}

} // namespace araceli

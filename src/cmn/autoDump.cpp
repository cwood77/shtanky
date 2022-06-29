#include "autoDump.hpp"
#include "out.hpp"
#include "trace.hpp"
#include <iomanip>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace cmn {

size_t exceptionFirewall::kCrashLogFile = (size_t)"crash";

exceptionFirewall::exceptionFirewall(outBundle& dbgOut, const std::string& pathPrefix)
: m_dbgOut(dbgOut)
, m_pathPrefix(pathPrefix)
, m_armed(true)
{
}

exceptionFirewall::~exceptionFirewall()
{
   if(!m_armed) return;

   log(kCrashLogFile);

   for(auto *pWall : m_walls)
   {
      pWall->onThrow();
      delete pWall;
   }
}

exceptionFirewall& exceptionFirewall::registerFile(size_t f)
{
   size_t fileNum = m_fileNames.size();

   std::stringstream stream;
   stream << std::setfill('0') << std::setw(2) << fileNum;
   stream << (const char *)f;

   m_fileNames[f] = stream.str();
   return *this;
}

void exceptionFirewall::add(iExceptionFirewall& w, firewallRegistrar *pReg)
{
   m_walls.insert(&w);
   auto *pL = dynamic_cast<iLogger*>(&w);
   m_logs[pL] = pL->getExt();
   if(pReg)
      m_regs.insert(pReg);

   deletePreExistingLog(w);
}

void exceptionFirewall::remove(iExceptionFirewall& w, firewallRegistrar *pReg)
{
   if(pReg)
      m_regs.erase(pReg);
   m_logs.erase(dynamic_cast<iLogger*>(&w));
   m_walls.erase(&w);
}

void exceptionFirewall::log(size_t f)
{
   cdwVERBOSE(" -- logging '%s' --\r\n",(const char *)f);

   for(auto it=m_logs.begin();it!=m_logs.end();++it)
      logOne(*it->first,f);
}

void exceptionFirewall::logOne(iLogger& l, size_t f)
{
   std::stringstream path;
   path
      << m_pathPrefix
      << "."
      << getFilePath(f)
      << m_logs[&l];

   auto& s = m_dbgOut.get<outStream>(path.str());
   l.dump(s);
}

void exceptionFirewall::disarm()
{
   for(auto *pReg : m_regs)
      pReg->disarm();

   m_armed = false;
}

void exceptionFirewall::deletePreExistingLog(iExceptionFirewall& w)
{
   auto *pL = dynamic_cast<iLogger*>(&w);
   if(!pL) return;

   size_t oldSeen = m_seenLoggers.size();
   m_seenLoggers.insert(pL->getExt());
   if(m_seenLoggers.size() == oldSeen)
      return; // already seen this logger

   for(auto it=m_fileNames.begin();it!=m_fileNames.end();++it)
      deletePreExistingLog(*pL,it->first);

   deletePreExistingLog(*pL,kCrashLogFile);
}

void exceptionFirewall::deletePreExistingLog(iLogger& l, size_t f)
{
   std::stringstream path;
   path
      << m_pathPrefix
      << "."
      << getFilePath(f)
      << m_logs[&l];

   ::DeleteFileA(path.str().c_str());
}

std::string exceptionFirewall::getFilePath(size_t f)
{
   if(f == kCrashLogFile)
      return std::string("99") + (const char *)f;
   return m_fileNames[f];
}

firewallRegistrar::firewallRegistrar(exceptionFirewall& xf, iLogger& l)
: m_armed(true)
, m_xf(xf)
, m_l(l)
{
   m_xf.add(m_l,this);
}

firewallRegistrar::~firewallRegistrar()
{
   if(m_armed)
      m_xf.logOne(m_l,exceptionFirewall::kCrashLogFile);

   m_xf.remove(m_l,this);
}

} // namespace cmn

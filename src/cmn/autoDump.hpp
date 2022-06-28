#pragma once
#include "throw.hpp"
#include <map>
#include <set>
#include <string>

// this component solves two closely-related problems: logging various objects at different
// points in compilation, and handling exception/crash scenarios (including more logging)

namespace cmn {

class outBundle;
class outStream;

class iExceptionFirewall {
public:
   virtual ~iExceptionFirewall() {}
   virtual void onThrow() = 0;
};

class iLogger : public iExceptionFirewall {
public:
   virtual void onThrow() {}
   virtual std::string getExt() = 0;
   virtual void dump(outStream& s) = 0;
};

class exceptionFirewall {
public:
   static size_t kCrashLogFile;

   exceptionFirewall(outBundle& dbgOut, const std::string& pathPrefix);
   ~exceptionFirewall();

   // register all files _before_ adding any firewalls
   exceptionFirewall& registerFile(size_t f);

   void add(iExceptionFirewall& w);
   void remove(iExceptionFirewall& w);

   template<class T>
   T& add(typename T::argType& o)
   {
      auto *pW = new T(o);
      add(*pW);
      return *pW;
   }

   template<class T>
   T& fetch()
   {
      for(auto *pWall : m_walls)
         if(auto *pL = dynamic_cast<T*>(pWall))
            return *pL;
      cdwTHROW("can't find logger");
   }

   void log(size_t f);
   void logOne(iLogger& l, size_t f);

   void disarm() { m_armed = false; }

private:
   void deletePreExistingLog(iExceptionFirewall& w);
   void deletePreExistingLog(iLogger& l, size_t f);
   std::string getFilePath(size_t f);

   outBundle& m_dbgOut;
   std::string m_pathPrefix;
   bool m_armed;
   std::map<size_t,std::string> m_fileNames;
   std::set<iExceptionFirewall*> m_walls;
   std::map<iLogger*,std::string> m_logs;
};

class firewallRegistrar {
public:
   firewallRegistrar(exceptionFirewall& xf, iLogger& l);
   ~firewallRegistrar();

   void disarm() { m_armed = false; }

private:
   bool m_armed;
   exceptionFirewall& m_xf;
   iLogger& m_l;
};

} // namespace cmn

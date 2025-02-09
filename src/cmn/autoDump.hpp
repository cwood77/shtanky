#pragma once
#include "throw.hpp"
#include <list>
#include <map>
#include <set>
#include <string>

// this component solves two closely-related problems: logging various objects at different
// points in compilation, and handling exception/crash scenarios (including more logging)
//
// for the AST, this is even more complex because the graph is so complex that deleting the AST
// could possible throw _more_ exceptions; so upon an exception throw the graph is leaked
//
// the logger deletes old logs the first time a logger is attached to it; but not before.
// this means old logs may still exist upon a crash if the app crashed before attaching.

namespace cmn {

class outBundle;
class outStream;

class iExceptionFirewall {
public:
   virtual ~iExceptionFirewall() {}
   virtual void onThrow() = 0;
};

// incremental loggers add to existing log files when re-run with a different datum.
// if a logger is not incremental but is logged a second time, it just appends redundant text.
class iLogger : public iExceptionFirewall {
public:
   virtual void onThrow() {}

   virtual std::string getExt() = 0;
   virtual void dump(outStream& s) = 0;
};

class iLoggerContext {
public:
   virtual void logOne(iLogger& l, size_t f, outStream& s) = 0;
};

// complains when a logger is invoked a second time, as this shouldn't happen outside loops
class rootLoggerContext : public iLoggerContext {
public:
   virtual void logOne(iLogger& l, size_t f, outStream& s);

private:
   std::set<std::string> m_writtenFiles;
};

class firewallRegistrar;

// remember to disarm
class exceptionFirewall {
public:
   static size_t kCrashLogFile;

   exceptionFirewall(outBundle& dbgOut, const std::string& pathPrefix);
   ~exceptionFirewall();

   // register all files _before_ adding any firewalls
   exceptionFirewall& registerFile(size_t f);

   void add(iExceptionFirewall& w, firewallRegistrar *pReg = NULL);
   void remove(iExceptionFirewall& w, firewallRegistrar *pReg = NULL);

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

   void disarm();

   void pushContext(iLoggerContext& c) { return m_contextStack.push_back(&c); }
   void popContext() { m_contextStack.pop_back(); }
   iLoggerContext& context() { return *m_contextStack.back(); }

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
   std::set<std::string> m_seenLoggers;
   std::set<firewallRegistrar*> m_regs;
   rootLoggerContext m_rootContext;
   std::list<iLoggerContext*> m_contextStack;
};

// use a registrar if the datum you're logging has a finite lifetime
// the firewall will disarm for you if you don't
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

class loggerLoop : private iLoggerContext {
public:
   loggerLoop(exceptionFirewall& xf, const std::string& annotation);
   ~loggerLoop();

private:
   virtual void logOne(iLogger& l, size_t f, outStream& s);

   exceptionFirewall& m_xf;
   std::string m_annotation;
};

} // namespace cmn

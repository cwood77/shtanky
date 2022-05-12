#pragma once
#include <string>

// example usage:
//
// #ifdef  cdwTraceContext
// #undef  cdwTraceContext
// #endif
// #define cdwTraceContext "main()"

#ifndef cdwTraceContext
#define cdwTraceContext NULL
#endif

#define cdwLoc __FILE__,__LINE__
#define cdwINFO    cmn::trace(cmn::trace::kInfo,   cdwLoc,cdwTraceContext).write
#define cdwVERBOSE cmn::trace(cmn::trace::kVerbose,cdwLoc,cdwTraceContext).write
#define cdwDEBUG   cmn::trace(cmn::trace::kDebug,  cdwLoc,cdwTraceContext).write

namespace cmn {

class trace {
public:
   enum levels {
      kInfo,
      kVerbose,
      kDebug,
   };

   trace(levels level, const char *file, unsigned long line, const char *context)
   : m_le(level), m_fi(file), m_li(line), m_context(context) {}

   void write(const std::string& fmt, ...);

private:
   static std::string m_appName;

   static const char *getAppName();

   levels m_le;
   const char *m_fi;
   unsigned long m_li;
   const char *m_context;
};

} // namespace cmn

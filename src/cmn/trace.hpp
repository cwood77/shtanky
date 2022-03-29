#pragma once
#include <string>

#define cdwINFO    cmn::trace(cmn::trace::kInfo,   __FILE__,__LINE__).write
#define cdwVERBOSE cmn::trace(cmn::trace::kVerbose,__FILE__,__LINE__).write
#define cdwDEBUG   cmn::trace(cmn::trace::kDebug,  __FILE__,__LINE__).write

namespace cmn {

class trace {
public:
   enum levels {
      kInfo,
      kVerbose,
      kDebug,
   };

   trace(levels level, const char *file, unsigned long line)
   : m_le(level), m_fi(file), m_li(line) {}

   void write(const std::string& fmt, ...);

private:
   levels m_le;
   const char *m_fi;
   unsigned long m_li;
};

} // namespace cmn

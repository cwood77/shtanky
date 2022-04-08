#pragma once
#include <cstring>

namespace cmn {
namespace appfmt {

class header {
public:
   header() : version(0), tocEntryCnt(0)
   { ::strcpy(thumbprint,"cdwe appfmt"); }
   char          thumbprint[11];
   unsigned long version;
   char          tocEntryCnt;
};

class tocEntry {
public:
   tocEntry() : flags(0), size(0) {}
   unsigned long flags;
   unsigned long size;
};

} // namespace appfmt
} // namespace cmn

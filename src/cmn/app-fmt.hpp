#pragma once
#include <cstring>

namespace cmn {
namespace appfmt {

// because I'm writing these structures directly to disk,
// set padding to 'none'
#pragma pack(push,1)

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

#pragma pack(pop)

} // namespace appfmt
} // namespace cmn

#pragma once
#include <cstring>

namespace cmn {
namespace appfmt {

// TODO - eventually I'll want to support laying out / linking objects with a certain block
// granularity for paging i.e. flags like "const" are implemented in HW that can only operate
// on page boundaries

// because I'm writing these structures directly to disk,
// set padding to 'none'
#pragma pack(push,1)

class header {
public:
   header() : version(0), tocEntryCnt(0), entrypointOffset(0), osCallOffset(0)
   { ::strcpy(thumbprint,"cdwe appfmt"); }
   char          thumbprint[11];
   unsigned long version;
   char          tocEntryCnt;
   unsigned long entrypointOffset;
   unsigned long osCallOffset;
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

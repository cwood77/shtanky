#pragma once

namespace cmn {
namespace appfmt {

class header {
public:
   header();
   char          thumbprint[11]; // cdwe appfmt == 11
   unsigned long verson;
   char          tocEntryCnt;
};

class tocEntry {
public:
   tocEntry();
   unsigned long flags;
   unsigned long size;
};

} // namespace appfmt
} // namespace cmn

#pragma once

namespace cmn {

namespace appfmt {

class header {
public:
   char          thumbprint[12];
   unsigned long verson;

   size_t        tocEntryCnt;
};

class tocEntry {
public:
   unsigned long id;
   unsigned long flags;
   unsigned long start;
   unsigned long size;
};

class importTable {
public:
   unsigned long count;
};

class import {
public:
   unsigned long o;
   char          name[];
};

} // namespace appfmt

} // namespace cmn

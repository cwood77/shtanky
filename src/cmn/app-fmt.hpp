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

class iBinFileInStream;
class iBinFileOutStream;

class patch {
public:
   enum types {
      kCall;
      kLea;
   } type;
   unsigned long offset;
}:

class exportTable {
public:
   std::map<std::string,unsigned long> toc;
};

class importTable {
public:
   std::map<std::string,std::list<patch> > patches;
};

class mcObj {
public:
   unsigned long flags;
   exportTable xt;
   importTable it;
   std::unique_ptr<unsigned char[]> block;
};

class objFile {
public:
   unsigned long version;
   std::list<mcObj*> objects;

   void flatten(iBinFileOutStream& s) const;
   void unflatten(iBinFileInStream& s);
};

} // namespace cmn

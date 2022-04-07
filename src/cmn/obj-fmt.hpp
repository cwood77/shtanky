#pragma once
#include <list>
#include <map>
#include <memory>
#include <string>

namespace cmn {

class iObjWriter;

namespace objfmt {

class patch {
public:
   enum types {
      kAbs,
      kRelToNextInstr,
   } type;
   unsigned long offset;
   unsigned long instrSize;
};

class exportTable {
public:
   std::map<std::string,unsigned long> toc;

   void flatten(iObjWriter& w) const;
};

class importTable {
public:
   std::map<std::string,std::list<patch> > patches;
};

class obj {
public:
   obj() : flags(0), blockSize(0) {}

   unsigned long flags;
   exportTable xt;
   importTable it;
   std::unique_ptr<unsigned char[]> block;
   size_t blockSize;

   void flatten(iObjWriter& w) const;
};

class objFile {
public:
   objFile() : version(0) {}
   ~objFile();

   unsigned long version;
   std::list<obj*> objects;

   void flatten(iObjWriter& w) const;
   //void unflatten(iBinInStream& s);
};

} // namespace objfmt
} // namespace cmn

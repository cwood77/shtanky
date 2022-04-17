#pragma once
#include <list>
#include <map>
#include <memory>
#include <string>

namespace cmn {

class iObjReader;
class iObjWriter;

namespace objfmt {

class patch {
public:
   enum types {
      kAbs, // TODO HACK - I hate absolute patches.  They necessitate a loader.  Can't I get rid of them?
      kRelToNextInstr,
   } type;
   unsigned long offset;
   unsigned long fromOffsetToEndOfInstr;
};

class exportTable {
public:
   std::map<std::string,unsigned long> toc;
   // maybe also hints at what to load to fulfill exports?

   void flatten(iObjWriter& w) const;
   void unflatten(iObjReader& r);
};

class importTable {
public:
   std::map<std::string,std::list<patch> > patches;

   void flatten(iObjWriter& w) const;
   void unflatten(iObjReader& r);
};

class obj {
public:
   /*
    * should implement this at some point...
   enum {
      kFlagUninit = 1 << 4, // lower short is for user numbering of blocks
      kFlagZero   = 1 << 5,
      kFlagConst  = 1 << 6,
   };
   */

   obj() : flags(0), blockSize(0) {}

   unsigned long flags;
   exportTable xt;
   importTable it;
   std::unique_ptr<unsigned char[]> block;
   size_t blockSize;

   void flatten(iObjWriter& w) const;
   void unflatten(iObjReader& r);
};

class objFile {
public:
   objFile() : version(0) {}
   ~objFile();

   unsigned long version;
   std::list<obj*> objects;

   void flatten(iObjWriter& w) const;
   void unflatten(iObjReader& r);
};

} // namespace objfmt
} // namespace cmn

#pragma once
#include <list>
#include <map>
#include <memory>
#include <string>

namespace cmn {

class iBinInStream;
class iBinOutStream;

namespace objfmt {

class patch {
public:
   enum types {
      kCall,
      kLea,
   } type;
   unsigned long offset;
};

class exportTable {
public:
   std::map<std::string,unsigned long> toc;
};

class importTable {
public:
   std::map<std::string,std::list<patch> > patches;
};

class obj {
public:
   unsigned long flags;
   exportTable xt;
   importTable it;
   std::unique_ptr<unsigned char[]> block;
};

class objFile {
public:
   unsigned long version;
   std::list<obj*> objects;

   void flatten(iBinOutStream& s) const;
   void unflatten(iBinInStream& s);
};

} // namespace objfmt
} // namespace cmn

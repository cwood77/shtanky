#pragma once
#include "../cmn/global.hpp"
#include <string>

namespace cmn { class scopeNode; }

namespace araceli {

class loader {
public:
   static void loadFolder(cmn::scopeNode& s);

   // typically, don't load files individually
   static void loadFile(cmn::scopeNode& s, const std::string& fullPath);

private:
   loader();
   loader(const loader&);
   loader& operator=(const loader&);
};

extern cmn::timedGlobal<std::string> gLastSupportedInfix;

} // namespace araceli

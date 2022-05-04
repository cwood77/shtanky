#pragma once
#include "../cmn/global.hpp"
#include <string>

namespace cmn { class araceliProjectNode; }
namespace cmn { class node; }
namespace cmn { class scopeNode; }

namespace araceli {

class loaderPrefs {
public:
   std::string ext;
   std::string ignoredFileName;
};

class loader {
public:
   static void loadFolder(cmn::scopeNode& s);

   // typically, don't load files individually
   static void loadFile(cmn::scopeNode& s, const std::string& fullPath);
   static void findScopeAndLoadFile(
      cmn::araceliProjectNode& s, const std::string& fullPath);

private:
   static cmn::scopeNode& findScope(cmn::node& n, const std::string& folder);

   loader();
   loader(const loader&);
   loader& operator=(const loader&);
};

extern cmn::timedGlobal<loaderPrefs> gLoaderPrefs;

} // namespace araceli

#pragma once
#include <map>
#include <set>
#include <string>

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class objFile; } }

namespace shlink {

class objectDirectory {
public:
   ~objectDirectory();

   void loadObjectFile(const std::string& path);
   cmn::objfmt::obj& demand(const std::string& oName);
   //cmn::objfmt::obj *tryFind(const std::string& oName);

private:
   std::map<std::string,cmn::objfmt::obj*> m_objDir;
   std::set<cmn::objfmt::objFile*> m_files;
};

} // namespace shlink

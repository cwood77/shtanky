#pragma once
#include <map>
#include <set>
#include <string>

namespace cmn { namespace objfmt { class obj; } }
namespace cmn { namespace objfmt { class objFile; } }

namespace shlink {

class iObjectProvider {
public:
   virtual cmn::objfmt::obj& demand(const std::string& oName) = 0;
};

class objectDirectory : public iObjectProvider {
public:
   ~objectDirectory();

   void loadObjectFile(const std::string& path);
   virtual cmn::objfmt::obj& demand(const std::string& oName);

   void determinePruneList(const std::set<std::string>& demanded);

private:
   std::map<std::string,cmn::objfmt::obj*> m_objDir;
   std::set<cmn::objfmt::objFile*> m_files;
};

class objectProviderRecorder : public iObjectProvider {
public:
   explicit objectProviderRecorder(iObjectProvider& inner) : m_inner(inner) {}

   virtual cmn::objfmt::obj& demand(const std::string& oName)
   { demanded.insert(oName); return m_inner.demand(oName); }

   std::set<std::string> demanded;

private:
   iObjectProvider& m_inner;
};

} // namespace shlink

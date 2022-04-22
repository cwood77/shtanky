#pragma once
#include <set>
#include <string>

namespace cmn {

class uniquifier {
public:
   void seed(const std::string& name) { m_set.insert(name); }

   std::string makeUnique(const std::string& s);

private:
   std::set<std::string> m_set;
};

} // namespace cmn

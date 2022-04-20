#pragma once
#include <set>
#include <string>

namespace cmn {

class uniquifier {
public:
   std::string makeUnique(const std::string& s);

private:
   std::set<std::string> m_set;
};

} // namespace cmn

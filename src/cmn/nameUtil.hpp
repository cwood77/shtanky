#pragma once
#include <string>

// in araceli/liam, names are '.' delimited.

namespace cmn {

class nameUtil {
public:
   static bool isAbsolute(const std::string& n);
   static std::string append(const std::string& prefix, const std::string& n);
   static std::string stripLast(const std::string& n)
   { std::string unused; return stripLast(n,unused); }
   static std::string stripLast(const std::string& n, std::string& last);

private:
   nameUtil();
   nameUtil(const nameUtil&);
   nameUtil& operator=(const nameUtil&);
};

} // namespace cmn

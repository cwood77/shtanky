#pragma once
#include <string>

namespace araceli {

class nameUtil {
public:
   static bool isAbsolute(const std::string& n);
   static std::string append(const std::string& prefix, const std::string& n);
   static std::string stripLast(const std::string& n);

private:
   nameUtil();
   nameUtil(const nameUtil&);
   nameUtil& operator=(const nameUtil&);
};

} // namespace araceli

#pragma once
#include <string>

namespace cmn {

class pathUtil {
public:
   static std::string getExtension(const std::string& path);
   static std::string getLastPart(const std::string& path);
   static void loadFileContents(const std::string& path, std::string& contents);

private:
   pathUtil();
   pathUtil(const pathUtil&);
   pathUtil& operator=(const pathUtil&);
};

} // namespace cmn

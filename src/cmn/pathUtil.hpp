#pragma once
#include <list>
#include <string>

namespace cmn {

class pathUtil {
public:
   static const char *kExtLiamHeader;
   static const char *kExtLiamSource;

   static const char *kExtObj;
   static const char *kExtObj_Then_List;
   static const char *kExtObj_Then_McList;

   static const char *kExtList;

   static std::string addExtension(const std::string& path, const std::string& ext);
   static std::string getExtension(const std::string& path);
   static std::string getLastPart(const std::string& path);
   static std::string addPrefixToFilePart(const std::string& path, const std::string& prefix);

   static std::string computeRefPath(const std::string& refer, const std::string& refee);
   static std::string absolutize(const std::string& refer, const std::string& refee);

   static void loadFileContents(const std::string& path, std::string& contents);

private:
   static void splitPath(const std::string& path, std::list<std::string>& list);
   static std::string combinePath(const std::list<std::string>& list);

   pathUtil();
   pathUtil(const pathUtil&);
   pathUtil& operator=(const pathUtil&);
};

} // namespace cmn

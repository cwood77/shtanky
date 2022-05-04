#pragma once
#include <list>
#include <map>
#include <string>

namespace cmn {

class pathUtil {
public:
   static const char *kExtLiamHeader;
   static const char *kExtLiamSource;

   static const char *kExtLir;
   static const char *kExtLirPost;
   static const char *kExtAsm;
   static const char *kExtObj;
   static const char *kExtList;
   static const char *kExtMcList;
   static const char *kExtMcOdaList;

   static std::string addExt(const std::string& path, const std::string& ext);
   static std::string getExt(const std::string& path);
   static std::string getLastPart(const std::string& path);
   static std::string addPrefixToFilePart(const std::string& path, const std::string& prefix);
   static std::string setInfix(const std::string& fullPath, const std::string& infix);
   static std::string getInfix(const std::string& fullPath, std::string& prefix);

   static std::string computeRefPath(const std::string& refer, const std::string& refee);
   static std::string absolutize(const std::string& refer, const std::string& refee);

   static void loadFileContents(const std::string& path, std::string& contents);

private:
   static void splitPath(const std::string& path, std::list<std::string>& list);
   static std::string combinePath(const std::list<std::string>& list);
   static void splitName(const std::string& path, std::list<std::string>& list);
   static std::string combineName(const std::list<std::string>& list);

   pathUtil();
   pathUtil(const pathUtil&);
   pathUtil& operator=(const pathUtil&);
};

// for files with the name patterns
//    nostromo.ara
//    nostromo.1-sa.ara
//    nostromo.2-ph.ara
//
// this class helps pick the latest value
//
// for salome, the lastSupportedInfix   = ""
// for philemon, the lastSupportedInfix = "1-sa"
// for araceli, the lastSupportedInfix  = "2-ph"
//
class sourceFileGroup {
public:
   explicit sourceFileGroup(const std::string& lastSupportedInfix, const std::string& ext)
   : m_lastInfix(lastSupportedInfix), m_ext(ext) {}

   void add(const std::string& filename);
   void except(const std::string& prefix);
   std::list<std::string> getLatest();

private:
   std::string m_lastInfix;
   std::string m_ext;
   std::map<std::string,std::string> m_prefixToInfix;
};

} // namespace cmn

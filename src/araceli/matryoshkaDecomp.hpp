#pragma once
#include <string>

namespace cmn { class callNode; }

namespace araceli {

class classCatalog;
class classInfo;

class matryoshkaDecomposition {
public:
   explicit matryoshkaDecomposition(classCatalog& cc) : m_cc(cc) {}

   void run();

private:
   void writeSuperFunc(classInfo& ci, const std::string& suffix, bool baseFirst);
   void writeBaseCall(classInfo& ci, const std::string& ssuffix, cmn::treeWriter& w);

   classCatalog& m_cc;
};

} // namespace araceli

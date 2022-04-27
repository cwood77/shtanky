#pragma once

namespace cmn { class callNode; }

namespace araceli {

class classCatalog;
class classInfo;

class matryoshkaDecomposition {
public:
   explicit matryoshkaDecomposition(classCatalog& cc) : m_cc(cc) {}

   void run();

private:
   void writeSuperCtor(classInfo& ci);
   void linkSuperCtors();

   classCatalog& m_cc;
   std::map<std::string,cmn::callNode*> m_scCalls;
};

} // namespace araceli

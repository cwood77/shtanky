#pragma once

namespace araceli {

class classCatalog;
class classInfo;

class matryoshkaDecomposition {
public:
   explicit matryoshkaDecomposition(classCatalog& cc) : m_cc(cc) {}

   void run();

private:
   void writeSuperCtor(classInfo& ci);

   classCatalog& m_cc;
};

} // namespace araceli

#pragma once

namespace araceli {

class classCatalog;

class inheritImplementor {
public:
   void generate(classCatalog& cc);

private:
   void combineFieldsAndRemoveBases(classCatalog& cc);
   void addVPtrs(classCatalog& cc);
};

} // namespace araceli

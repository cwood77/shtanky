#pragma once
#include "../cmn/ast.hpp"
#include <map>
#include "classInfo.hpp"

namespace araceli {

// find all abstract methods of a given class, and it's bases, and implement them
class abstractGenerator {
public:
   static void generate(classCatalog& cc);
};

} // namespace araceli

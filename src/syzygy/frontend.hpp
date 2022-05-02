#pragma once
#include "../araceli/iTarget.hpp"
#include "../cmn/ast.hpp"
#include <memory>
#include <string>

namespace syzygy {

class iTarget;

class frontend {
public:
   static void run(
      const std::string& projectDir,
      std::unique_ptr<cmn::araceliProjectNode>& pPrj,
      std::unique_ptr<araceli::iTarget>& pTgt);
};

} // namespace syzygy

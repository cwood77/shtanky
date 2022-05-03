#pragma once
#include "../araceli/iTarget.hpp"
#include "../cmn/ast.hpp"
#include <memory>
#include <string>

namespace syzygy {

class iTarget;

class frontend {
public:
   frontend(
      const std::string& projectDir,
      std::unique_ptr<cmn::araceliProjectNode>& pPrj,
      std::unique_ptr<araceli::iTarget>& pTgt);

   void run();

protected:
   virtual void linkGraph();

   const std::string& m_projectDir;
   std::unique_ptr<cmn::araceliProjectNode>& m_pPrj;
   std::unique_ptr<araceli::iTarget>& m_pTgt;
};

} // namespace syzygy

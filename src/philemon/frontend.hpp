#pragma once
#include "../syzygy/frontend.hpp"
#include "symbolTable.hpp"

namespace philemon {

class frontend : public syzygy::frontend {
public:
   frontend(
      const std::string& projectDir,
      std::unique_ptr<cmn::araceliProjectNode>& pPrj,
      std::unique_ptr<araceli::iTarget>& pTgt)
   : syzygy::frontend(projectDir,pPrj,pTgt) {}

protected:
   virtual void linkGraph() { nodeLinker().linkGraph(*m_pPrj); }
};

} // namespace philemon

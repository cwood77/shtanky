#pragma once
#include "../syzygy/frontend.hpp"

namespace philemon {

class frontend : public syzygy::frontend {
public:
   frontend(
      const std::string& projectDir,
      std::unique_ptr<cmn::araceliProjectNode>& pPrj,
      std::unique_ptr<araceli::iTarget>& pTgt)
   : syzygy::frontend(projectDir,pPrj,pTgt), m_firstLink(true) {}

protected:
   virtual void linkGraph();

private:
   bool m_firstLink;
};

} // namespace philemon

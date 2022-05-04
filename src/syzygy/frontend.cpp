#include "../araceli/consoleAppTarget.hpp"
#include "../araceli/projectBuilder.hpp"
#include "../araceli/symbolTable.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "frontend.hpp"

namespace syzygy {

frontend::frontend(const std::string& projectDir, std::unique_ptr<cmn::araceliProjectNode>& pPrj, std::unique_ptr<araceli::iTarget>& pTgt)
: m_projectDir(projectDir), m_pPrj(pPrj), m_pTgt(pTgt)
{
}

void frontend::run()
{
   // setup project / target
   m_pPrj = araceli::projectBuilder::create("ca");
   araceli::projectBuilder::addScope(*m_pPrj.get(),m_projectDir,/*inProject*/true);
   m_pTgt.reset(new araceli::consoleAppTarget());
   m_pTgt->addAraceliStandardLibrary(*m_pPrj.get());
   m_pTgt->populateIntrinsics(*m_pPrj.get());
   { cmn::diagVisitor v; m_pPrj->acceptVisitor(v); }

   // initial link to discover and load everything
   linkGraph();
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; m_pPrj->acceptVisitor(v); }
}

void frontend::linkGraph()
{
   araceli::nodeLinker().linkGraph(*m_pPrj);
}

} // namespace syzygy

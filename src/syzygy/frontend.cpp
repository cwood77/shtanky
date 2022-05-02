#include "../araceli/consoleAppTarget.hpp"
#include "../araceli/loader.hpp"
#include "../araceli/metadata.hpp"
#include "../araceli/objectBaser.hpp"
#include "../araceli/projectBuilder.hpp"
#include "../araceli/symbolTable.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/out.hpp"
#include "../cmn/trace.hpp"
#include "frontend.hpp"

namespace syzygy {

void frontend::run(const std::string& projectDir, std::unique_ptr<cmn::araceliProjectNode>& pPrj, std::unique_ptr<araceli::iTarget>& pTgt)
{
   // setup project / target
   pPrj = araceli::projectBuilder::create("ca");
   araceli::projectBuilder::addScope(*pPrj.get(),projectDir,/*inProject*/true);
   pTgt.reset(new araceli::consoleAppTarget());
   pTgt->addAraceliStandardLibrary(*pPrj.get());
   pTgt->populateIntrinsics(*pPrj.get());
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // initial link to discover and load everything
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // gather metadata
   araceli::metadata md;
   {
      araceli::nodeMetadataBuilder inner(md);
      cmn::treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // use metadata to generate the target
   pTgt->araceliCodegen(*pPrj,md);

   // inject implied base class
   { araceli::objectBaser v; pPrj->acceptVisitor(v); }

   // subsequent link to update with new target and load more
   araceli::nodeLinker().linkGraph(*pPrj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
}

} // namespace syzygy

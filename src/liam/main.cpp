#include "../cmn/ast.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/out.hpp"
#include "../cmn/symbolTable.hpp"
#include "../cmn/trace.hpp"
#include "../cmn/typeVisitor.hpp"
#include "asmCodeGen.hpp"
#include "astCodeGen.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include "projectBuilder.hpp"
#include "varAlloc.hpp"
#include "varCombiner.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"
#include "varSplitter.hpp"

using namespace liam;

int main(int argc,const char *argv[])
{
   cmn::cmdLine cl(argc,argv);

   // load
   cmn::liamProjectNode prj;
   prj.sourceFullPath = cl.getArg("testdata\\test\\test.ara.ls");
   projectBuilder::build(prj);
   cdwVERBOSE("graph after loading ----\n");
   { cmn::diagVisitor v; prj.acceptVisitor(v); }

   // link
   cmn::nodeLinker().linkGraph(prj);
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; prj.acceptVisitor(v); }

   // seed type terminals
   cmn::type::table                           _t;
   cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
   { cmn::coarseTypeVisitor v; prj.acceptVisitor(v); }

   // type propagation
   cmn::type::nodeCache                       _c;
   cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
   { cmn::fineTypeVisitor v; prj.acceptVisitor(v); }

   // generate LIR and variables
   varTable vTbl;
   lirStreams lir;
   cmn::tgt::w64EmuTargetInfo t;
   {
      varGenerator vGen(vTbl);
      { astCodeGen v(lir,vGen,t); prj.acceptVisitor(v); }
   }
   lir.dump();

   // ---------------- register allocation ----------------

   // establish variable requirements
   instrPrefs::publishRequirements(lir,vTbl,t);

   varSplitter::split(lir,vTbl,t);

   // TODO all these operations should be per stream
   //      really?  the splitter is effectively handling the
   //      whole thing now anyway
   cmn::outBundle out;
   for(auto it=lir.page.begin();it!=lir.page.end();++it)
   {
      varFinder f(t);
      { varCombiner p(it->second,vTbl,t,f); p.run(); }

      { varAllocator p(it->second,vTbl,t,f); p.run(); }

      asmCodeGen::generate(it->second,vTbl,f,t,out.get<cmn::outStream>(prj.sourceFullPath,"asm"));
   }

   _t.dump();
   _c.dump();

   cmn::fileWriter wr;
   out.updateDisk(wr);
}

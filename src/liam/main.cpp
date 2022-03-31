#include "../cmn/ast.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/out.hpp"
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

int main(int,const char*[])
{
   cmn::liamProjectNode prj;
   prj.sourceFullPath = "testdata\\test\\test.ara.ls";
   prj.searchPaths.push_back("testdata\\test");
   prj.searchPaths.push_back("testdata\\sht");
   projectBuilder::build(prj);
   { cmn::diagVisitor v; prj.acceptVisitor(v); }

   varTable vTbl;
   lirStreams lir;
   cmn::tgt::w64EmuTargetInfo t;
   {
      varGenerator vGen(vTbl);
      { astCodeGen v(lir,vGen,t); prj.acceptVisitor(v); }
   }
   lir.dump();

   instrPrefs::publishRequirements(lir,vTbl,t);

   cmn::outBundle out;
   cmn::fileWriter wr;
   out.setAutoUpdate(wr);

   // TODO all these operations should be per stream
   for(auto it=lir.page.begin();it!=lir.page.end();++it)
   {
      varFinder f(t);

      varSplitter::split(it->second,vTbl,t);
      //varCombiner::combine(it->second,vTbl,t);
      { varCombiner p(it->second,vTbl,t,f); p.run(); }

      { varAllocator p(it->second,vTbl,t,f); p.run(); }

      asmCodeGen::generate(it->second,vTbl,f,t,out.get<cmn::outStream>("testdata\\test\\test.ara.ls","asm"));
   }
}

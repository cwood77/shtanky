#include "../cmn/ast.hpp"
#include "../cmn/intel64.hpp"
#include "astCodeGen.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include "projectBuilder.hpp"
#include "varGen.hpp"

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
   {
      varGenerator vGen(vTbl);
      { astCodeGen v(lir,vGen); prj.acceptVisitor(v); }
   }
   lir.dump();

   cmn::tgt::w64EnumTargetInfo t;
   instrPrefs::publishRequirements(lir,vTbl,t);
}

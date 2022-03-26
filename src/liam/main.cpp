#include "../cmn/ast.hpp"
#include "../cmn/intel64.hpp"
#include "astCodeGen.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include "projectBuilder.hpp"

using namespace liam;

int main(int,const char*[])
{
   cmn::liamProjectNode prj;
   prj.sourceFullPath = "testdata\\test\\test.ara.ls";
   prj.searchPaths.push_back("testdata\\test");
   prj.searchPaths.push_back("testdata\\sht");
   projectBuilder::build(prj);
   { cmn::diagVisitor v; prj.acceptVisitor(v); }

   lirStreams lir;
   { astCodeGen v(lir); prj.acceptVisitor(v); }
   lir.dump();

   cmn::tgt::w64EnumTargetInfo t;
   instrPrefs::publishRequirements(lir,t);
}

#include "../cmn/ast.hpp"
#include "projectBuilder.hpp"

using namespace liam;

int main(int,const char*[])
{
   cmn::liamProjectNode prj;
   prj.sourceFullPath = "testdata\\test\\test.ara.lh";
   prj.searchPaths.push_back("testdata\\test");
   prj.searchPaths.push_back("testdata\\sht");
   projectBuilder::build(prj);
   { cmn::diagVisitor v; prj.acceptVisitor(v); }
}

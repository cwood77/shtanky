#include "../cmn/ast.hpp"

int main(int,const char*[])
{
   cmn::liamProjectNode prj;
   prj.sourceFullPath = "testdata\\test\\test.ara.ls";
   prj.searchPaths.push_back("testdata\\test");
   prj.searchPaths.push_back("testdata\\sht");
}

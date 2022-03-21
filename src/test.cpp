#include "cmn/parser.hpp"
#include "cmn/ast.hpp"
#include "cmn/lexor.hpp"
#include <stdio.h>
#include <string.h>

#include <fstream>
#include <sstream>

int main(int,char*[])
{
   ::printf("testing lexor\n");

   std::stringstream stream;
   {
      std::ifstream src("src\\test.ara");
      while(src.good())
      {
         std::string line;
         std::getline(src,line);
         stream << line << std::endl;
      }
   }

   std::string copy = stream.str();
   {
      araceli::lexor l(copy.c_str());

      while(true)
      {
         ::printf("%s(%s)\n",l.getTokenName().c_str(),l.getLexeme().c_str());
         l.advance();
         if(l.getToken() == cmn::lexorBase::kEOI)
            break;
      }

      ::printf("done with lexor, start parser\n");
   }

   {
      araceli::lexor l(copy.c_str());
      araceli::parser p(l);

      auto file = p.parseFile();

      ::printf("done with parser\n");
   }

   return 0;
}

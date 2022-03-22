#include "araceli/lexor.hpp"
#include "araceli/loader.hpp"
#include "araceli/metadata.hpp"
#include "araceli/parser.hpp"
#include "araceli/projectBuilder.hpp"
#include "araceli/symbolTable.hpp"
#include <stdio.h>
#include <string.h>

#include <fstream>
#include <sstream>

int main(int,char*[])
{
   ::printf("testing lexor\n");

   std::stringstream stream;
   {
      std::ifstream src("testdata\\test\\test.ara");
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

   {
      using namespace araceli;
      ::printf("testing build process\n");
      std::unique_ptr<projectNode> pPrj = projectBuilder::create("ca");
      projectBuilder::addScope(*pPrj.get(),"testdata\\test",/*inProject*/true);
      projectBuilder::addScope(*pPrj.get(),"testdata\\sht",/*inProject*/false);
      { diagVisitor v; pPrj->acceptVisitor(v); }

      symbolTable sTable;
      while(true)
      {
         ::printf("link/load loop\n");
         treeSymbolVisitor<nodePublisher> pub(sTable);
         pPrj->acceptVisitor(pub);
         treeSymbolVisitor<nodeResolver> res(sTable);
         pPrj->acceptVisitor(res);
         ::printf("%lld resolved; %lld unresolved\n",
            sTable.resolved.size(),
            sTable.unresolved.size());

         if(sTable.unresolved.size())
         {
            auto missing = (*sTable.unresolved.begin())->ref;
            ::printf("loading more to find symbol %s\n",missing.c_str());

            unloadedScopeFinder f(missing);
            pPrj->acceptVisitor(f);
            if(!f.any())
               throw std::runtime_error("eh? no progress?");
            scopeNode& next = f.mostLikely();
            ::printf("loading %s and trying again\n",next.path.c_str());
            loader::loadFolder(next);
            { diagVisitor v; pPrj->acceptVisitor(v); }
         }
         else
            break;
      }

      // next, gen metadata
      metadata md;
      {
      }
   }

   return 0;
}

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

   // gather metadata
   metadata md;
   {
      nodeMetadataBuilder inner(md);
      treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // file codegen
   // -> header file
   // -> source file
   // -> make file

   // target codegen

   return 0;
}

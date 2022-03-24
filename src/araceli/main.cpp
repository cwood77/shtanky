#include "../cmn/out.hpp"
#include "codegen.hpp"
#include "lexor.hpp"
#include "loader.hpp"
#include "metadata.hpp"
#include "projectBuilder.hpp"
#include "symbolTable.hpp"
#include <stdio.h>
#include <string.h>

using namespace araceli;

int main(int,char*[])
{
   std::unique_ptr<cmn::araceliProjectNode> pPrj = projectBuilder::create("ca");
   projectBuilder::addScope(*pPrj.get(),"testdata\\test",/*inProject*/true);
   projectBuilder::addScope(*pPrj.get(),"testdata\\sht",/*inProject*/false);
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   ::printf("entering link/load loop ----\n");
   symbolTable sTable;
   size_t missingLastTime = 0;
   while(true)
   {
      treeSymbolVisitor<nodePublisher> pub(sTable);
      pPrj->acceptVisitor(pub);
      treeSymbolVisitor<nodeResolver> res(sTable);
      pPrj->acceptVisitor(res);
      ::printf("%lld resolved; %lld unresolved\n",
         sTable.resolved.size(),
         sTable.unresolved.size());

      size_t nMissing = sTable.unresolved.size();
      if(!nMissing)
         break;

      cmn::scopeNode *pToLoad = NULL;
      for(auto it=sTable.unresolved.begin();it!=sTable.unresolved.end();++it)
      {
         auto refToFind = (*it)->ref;
         unloadedScopeFinder f(refToFind);
         pPrj->acceptVisitor(f);
         if(f.any())
         {
            ::printf("trying to find symbol %s\n",refToFind.c_str());
            pToLoad = &f.mostLikely();
            break;
         }
      }

      if(pToLoad)
      {
         ::printf("loading %s and trying again\n",pToLoad->path.c_str());
         loader::loadFolder(*pToLoad);
         { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
      }
      else
      {
         ::printf("no guesses on what to load to find missing symbols; try settling\n");
         if(nMissing != missingLastTime)
            missingLastTime = nMissing; // retry
         else
            throw std::runtime_error("gave up trying to resolve symbols");
      }
   }

   // gather metadata
   metadata md;
   {
      nodeMetadataBuilder inner(md);
      cmn::treeVisitor outer(inner);
      pPrj->acceptVisitor(outer);
   }

   // file codegen
   // -> header file
   // -> source file
   // -> make file

   // target codegen

   cmn::outBundle out;
   cmn::testFileWriter dummy;
   out.setAutoUpdate(dummy);

   codeGen v(out);
   pPrj->acceptVisitor(v);

   return 0;
}

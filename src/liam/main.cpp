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
#include "lirXfrm.hpp"
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

   // type propagation
   cmn::type::table                           _t;
   cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
   cmn::type::nodeCache                       _c;
   cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
   cmn::propagateTypes(prj);

   // generate LIR and variables
   //varTable vTbl;
   lirStreams lir;
   cmn::tgt::w64EmuTargetInfo t;
   cmn::outBundle out;
   if(0)
   {
#if 0
      // old LIR
      {
         varGenerator vGen(vTbl);
         { astCodeGen v(lir,vGen,t); prj.acceptVisitor(v); }
      }
      lir.dump();
#endif
   }
   else
   {
      // new LIR
      cdwDEBUG("**** LIR REWRITE ***\n");
      //lirStreams lir;
      lirGenerator lGen(lir,t);
      { lirGenVisitor v(lGen,t); prj.acceptVisitor(v); }

      { auto& s = out.get<cmn::outStream>(prj.sourceFullPath,"lir");
        lirFormatter(s,t).format(lir); }

      runLirTransforms(lir,t);

      { auto& sp = out.get<cmn::outStream>(prj.sourceFullPath,"lir-post");
        lirFormatter(sp,t).format(lir); }

      //lirVarGen(vTbl).runStreams(lir);
   }

   // ---------------- register allocation ----------------

   for(auto it=lir.page.begin();it!=lir.page.end();++it)
   {
   varTable vTbl;
      lirVarGen(vTbl).runStream(it->second);

      cdwVERBOSE("backend passes on %s\n",it->first.c_str());

      // establish variable requirements
      instrPrefs::publishRequirements(it->second,vTbl,t);

      varSplitter::split(it->second,vTbl,t);

      varFinder f(t);
      { varCombiner p(it->second,vTbl,t,f); p.run(); }

      { varAllocator p(it->second,vTbl,t,f); p.run(); }

      asmCodeGen::generate(it->second,vTbl,f,t,out.get<cmn::outStream>(prj.sourceFullPath,"asm"));
   }

   if(0)
   {
      lirStreams lir2;
      lirGenerator lGen(lir2,t);
      lirGenVisitor v(lGen,t);
      prj.acceptVisitor(v);
      cdwDEBUG("**** LIR REWRITE ***\n");
      lir2.dump();
      auto& s = out.get<cmn::outStream>(prj.sourceFullPath,"lir2");
      lirFormatter(s,t).format(lir2);

      runLirTransforms(lir2,t);
      auto& sp = out.get<cmn::outStream>(prj.sourceFullPath,"lir2-post");
      lirFormatter(sp,t).format(lir2);
   }

   _t.dump();
   _c.dump();

   cmn::unconditionalWriter wr;
   out.updateDisk(wr);
}

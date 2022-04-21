#include "../cmn/ast.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/main.hpp"
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

int _main(int argc,const char *argv[])
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

   // generate LIR
   lirStreams lir;
   cmn::tgt::w64EmuTargetInfo t;
   cmn::outBundle out,dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);
   { lirGenerator lGen(lir,t); astCodeGen v(lGen,t); prj.acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir");
     lirFormatter(s,t).format(lir); }

   // LIR transforms
   runLirTransforms(lir,t);
   { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-post");
     lirFormatter(sp,t).format(lir); }

   // ---------------- register allocation ----------------

   for(auto it=lir.objects.begin();it!=lir.objects.end();++it)
   {
      cdwVERBOSE("backend passes on %s\n",it->name.c_str());

      varTable vTbl;
      lirVarGen(vTbl).runStream(*it);

      instrPrefs::publishRequirements(*it,vTbl,t);

      varSplitter::split(*it,vTbl,t);

      varFinder f(t);
      { varCombiner p(*it,vTbl,t,f); p.run(); }

      stackAllocator().run(vTbl,f);
      varAllocator(t).run(vTbl,f);

      asmCodeGen::generate(*it,vTbl,f,t,out.get<cmn::outStream>(prj.sourceFullPath,"asm"));
   }

   _t.dump();
   _c.dump();

   out.updateDisk(wr);

   return 0;
}

cdwImplMain();

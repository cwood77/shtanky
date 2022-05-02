#include "../cmn/ast.hpp"
#include "../cmn/cmdline.hpp"
#include "../cmn/global.hpp"
#include "../cmn/intel64.hpp"
#include "../cmn/main.hpp"
#include "../cmn/obj-fmt.hpp"
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
   cdwDEBUG("compiled with C++ %u\n",__cplusplus);

   cmn::cmdLine cl(argc,argv);

   // load
   cmn::liamProjectNode prj;
   prj.sourceFullPath = cl.getNextArg("testdata\\test\\test.ara.ls");
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
   { lirBuilder b(lir,t); astCodeGen v(b,t); prj.acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir");
     lirFormatter(s,t).format(lir); }

   // LIR transforms
   runLirTransforms(lir,t);
   { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-post");
     lirFormatter(sp,t).format(lir); }

   // ---------------- register allocation ----------------

   try
   {
      { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-postreg");
        lirIncrementalFormatter(sp,t).start(lir); }
      { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-preasm");
        lirIncrementalFormatter(sp,t).start(lir); }
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

         { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-postreg");
           lirIncrementalFormatter(sp,t).format(*it); }

         if(it->segment == cmn::objfmt::obj::kLexCode)
         {
            spuriousVarStripper(vTbl).runStream(*it);
            codeShapeTransform(vTbl,f,t).runStream(*it);
         }

         { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-preasm");
           lirIncrementalFormatter(sp,t).format(*it); }

         asmCodeGen::generate(*it,vTbl,f,t,out.get<cmn::outStream>(prj.sourceFullPath,"asm"));
      }
      { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-postreg");
        lirIncrementalFormatter(sp,t).end(); }
      { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-preasm");
        lirIncrementalFormatter(sp,t).end(); }
   }
   catch(std::exception&)
   {
      cdwINFO("handling exception; writing lir-crash file\n");
      { auto& sp = dbgOut.get<cmn::outStream>(prj.sourceFullPath,"lir-crash");
        lirFormatter(sp,t).format(lir); }
      throw;
   }

   _t.dump();
   _c.dump();

   out.updateDisk(wr);

   return 0;
}

cdwImplMain();

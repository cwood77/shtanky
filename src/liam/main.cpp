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
#include "loopTransforms.hpp"
#include "projectBuilder.hpp"
#include "vTableInvokeDetection.hpp"
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
   cmn::outBundle out,dbgOut;
   cmn::unconditionalWriter wr;
   dbgOut.scheduleAutoUpdate(wr);

   // load
   cmn::rootNodeDeleteOperation _rdo;
   cmn::globalPublishTo<cmn::rootNodeDeleteOperation> _rdoRef(_rdo,cmn::gNodeDeleteOp);
   std::unique_ptr<cmn::liamProjectNode> pPrj(new cmn::liamProjectNode());
   pPrj->sourceFullPath = cl.getNextArg("testdata\\test\\test.ara.ls");
   projectBuilder::build(*pPrj.get());
   cdwVERBOSE("graph after loading ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath + ".00init.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

   // link
   cmn::nodeLinker().linkGraph(*pPrj.get());
   cdwVERBOSE("graph after linking ----\n");
   { cmn::diagVisitor v; pPrj->acceptVisitor(v); }

   // type-agnostic AST transforms
   { loopDecomposer v; pPrj->acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath + ".01postXfrm.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

   cmn::tgt::w64EmuTargetInfo t;
   {
      // initial type propagation
      cmn::nodeLinker().linkGraph(*pPrj.get());
      cmn::type::table                           _t;
      cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
      cmn::type::nodeCache                       _c;
      cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
      cmn::propagateTypes(*pPrj.get());

      // type-aware AST transforms
      { vTableInvokeDetector v(t); pPrj->acceptVisitor(v); }
      cdwVERBOSE("graph after transforms ----\n");
      { cmn::diagVisitor v; pPrj->acceptVisitor(v); }
   }

   // re-link
   cmn::nodeLinker().linkGraph(*pPrj.get());

   // final type propagation
   cmn::type::table                           _t;
   cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
   cmn::type::nodeCache                       _c;
   cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
   cmn::propagateTypes(*pPrj.get());
   { auto& s = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath + ".02postXfrm.ast");
     cmn::astFormatter v(s); pPrj->acceptVisitor(v); }

   // generate LIR
   lirStreams lir;
   { lirBuilder b(lir,t); astCodeGen v(b,t); pPrj->acceptVisitor(v); }
   { auto& s = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath,"lir");
     lirFormatter(s,t).format(lir); }

   // LIR transforms
   runLirTransforms(lir,t);
   { auto& sp = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath,"lir-post");
     lirFormatter(sp,t).format(lir); }

   // ---------------- register allocation ----------------

   try
   {
      for(auto it=lir.objects.begin();it!=lir.objects.end();++it)
      {
         cdwVERBOSE("backend passes on %s\n",it->name.c_str());

         varTable vTbl;
         lirVarGen(vTbl).runStream(*it);

         instrPrefs::publishRequirements(*it,vTbl,t);

         varSplitter::split(*it,vTbl,t);

         varFinder f(t);
         { varCombiner p(*it,vTbl,f); p.run(); }

         stackAllocator().run(vTbl,f);
         varAllocator(t).run(vTbl,f);

         { auto& sp = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath,"lir-postreg");
           lirIncrementalFormatter(sp,t).format(*it); }

         if(it->segment == cmn::objfmt::obj::kLexCode)
         {
            spuriousVarStripper(vTbl).runStream(*it);
            codeShapeTransform(vTbl,f,t).runStream(*it);
         }

         { auto& sp = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath,"lir-preasm");
           lirIncrementalFormatter(sp,t).format(*it); }

         splitResolver(*it,vTbl).run();

         asmCodeGen::generate(*it,vTbl,f,t,out.get<cmn::outStream>(pPrj->sourceFullPath,"asm"));
      }
   }
   catch(std::exception&)
   {
      cdwINFO("handling exception; writing lir-crash file\n");
      { auto& sp = dbgOut.get<cmn::outStream>(pPrj->sourceFullPath,"lir-crash");
        lirFormatter(sp,t).format(lir); }
      throw;
   }

   _t.dump();
   _c.dump();

   out.updateDisk(wr);

   // clear graph
   cdwDEBUG("destroying the graph\r\n");
   { cmn::autoNodeDeleteOperation o; pPrj.reset(); }

   return 0;
}

cdwImplMain();

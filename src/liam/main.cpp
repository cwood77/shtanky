#include "../cmn/ast.hpp"
#include "../cmn/autoDump.hpp"
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

static const size_t kLogPostLoad       = (size_t)"postLoad";
static const size_t kLogPostUntypeXfrm = (size_t)"postUntypeXfrm";
static const size_t kLogPostTypeXfrm   = (size_t)"postTypeXfrm";
static const size_t kLogPostLir        = (size_t)"postLir";
static const size_t kLogPostLirXfrm    = (size_t)"postLirXfrm";
static const size_t kLogPostVarGen     = (size_t)"postVarGen";
static const size_t kLogPostRegAlloc   = (size_t)"postRegAlloc";
static const size_t kLogPreAsm         = (size_t)"preasm";

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
   cmn::exceptionFirewall xf(dbgOut,pPrj->sourceFullPath);
   xf
      .registerFile(kLogPostLoad)
      .registerFile(kLogPostUntypeXfrm)
      .registerFile(kLogPostTypeXfrm)
      .registerFile(kLogPostLir)
      .registerFile(kLogPostLirXfrm)
      .registerFile(kLogPostVarGen)
      .registerFile(kLogPostRegAlloc)
      .registerFile(kLogPreAsm)
   ;
   xf.add<cmn::astFirewall<cmn::liamProjectNode> >(pPrj);
   projectBuilder::build(*pPrj.get());
   xf.log(kLogPostLoad);

   // link
   cmn::nodeLinker().linkGraph(*pPrj.get());

   // type-agnostic AST transforms
   { loopDecomposer v; pPrj->acceptVisitor(v); }
   xf.log(kLogPostUntypeXfrm);

   cmn::tgt::w64EmuTargetInfo t;
   {
      // initial type propagation
      cmn::nodeLinker().linkGraph(*pPrj.get());
      cmn::type::table                           _t;
      cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
      cmn::type::nodeCache                       _c;
      cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
      cmn::type::typeAutoLogger tyLogger;
      cmn::firewallRegistrar fr(xf,tyLogger);
      cmn::propagateTypes(*pPrj.get());

      // type-aware AST transforms
      { vTableInvokeDetector v(t); pPrj->acceptVisitor(v); }

      fr.disarm();
   }

   // re-link
   cmn::nodeLinker().linkGraph(*pPrj.get());

   // final type propagation
   cmn::type::table                           _t;
   cmn::globalPublishTo<cmn::type::table>     _tReg(_t,cmn::type::gTable);
   cmn::type::nodeCache                       _c;
   cmn::globalPublishTo<cmn::type::nodeCache> _cReg(_c,cmn::type::gNodeCache);
   cmn::type::typeAutoLogger tyLogger;
   cmn::firewallRegistrar fr(xf,tyLogger);
   cmn::propagateTypes(*pPrj.get());
   xf.log(kLogPostTypeXfrm);

   // generate LIR
   lirStreams lir;
   lirAutoLogger lirLogger(lir);
   lirLogger.set(t);
   cmn::firewallRegistrar _llr(xf,lirLogger);
   { lirBuilder b(lir,t); astCodeGen v(b,t); pPrj->acceptVisitor(v); }
   xf.log(kLogPostLir);

   // LIR transforms
   runLirTransforms(lir,t);
   xf.log(kLogPostLirXfrm);

   // ---------------- register allocation ----------------

   for(auto it=lir.objects.begin();it!=lir.objects.end();++it)
   {
      cdwVERBOSE("backend passes on %s\n",it->name.c_str());
      autoIncrementalSetting _s(lirLogger,*it);

      varTable vTbl;
      varTableAutoLogger varLogger(vTbl);
      cmn::firewallRegistrar fr(xf,varLogger);
      lirVarGen(vTbl).runStream(*it);
      xf.log(kLogPostVarGen);

      instrPrefs::publishRequirements(*it,vTbl,t);

      varSplitter::split(*it,vTbl,t);

      varFinder f(t);
      { varCombiner p(*it,vTbl,f); p.run(); }

      stackAllocator().run(vTbl,f);
      varAllocator(t).run(vTbl,f);
      xf.log(kLogPostRegAlloc);

      if(it->segment == cmn::objfmt::obj::kLexCode)
      {
         spuriousVarStripper(vTbl).runStream(*it);
         codeShapeTransform(vTbl,f,t).runStream(*it);
      }

      xf.log(kLogPreAsm);
      splitResolver(*it,vTbl).run();

      asmCodeGen::generate(*it,vTbl,f,t,out.get<cmn::outStream>(pPrj->sourceFullPath,"asm"));

   //   cdwTHROW("fake exception");

      fr.disarm();
   }

   out.updateDisk(wr);

   // clear graph
   cdwDEBUG("destroying the graph\r\n");
   { cmn::autoNodeDeleteOperation o; pPrj.reset(); }
   fr.disarm();
   xf.disarm();

   return 0;
}

cdwImplMain();

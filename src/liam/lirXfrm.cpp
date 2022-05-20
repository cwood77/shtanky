#include "../cmn/trace.hpp"
#include "../cmn/unique.hpp"
#include "lir.hpp"
#include "lirXfrm.hpp"
#include "varFinder.hpp"
#include "varGen.hpp"

namespace liam {

lirTransform::~lirTransform()
{
   for(auto it=m_changes.begin();it!=m_changes.end();++it)
      delete *it;
}

void lirTransform::runStreams(lirStreams& lir)
{
   for(auto it=lir.objects.begin();it!=lir.objects.end();++it)
   {
      m_pCurrStream = &*it;
      _runStream(*it);
   }

   m_pCurrStream = NULL;
   applyChanges();
}

void lirTransform::runStream(lirStream& s)
{
   m_pCurrStream = &s;

   _runStream(s);

   m_pCurrStream = NULL;
   applyChanges();
}

void lirTransform::_runStream(lirStream& s)
{
   auto *pInstr = &s.pTail->head();
   while(true)
   {
      runInstr(*pInstr);

      if(pInstr->isLast())
         break;
      pInstr = &pInstr->next();
   }
}

void lirTransform::runInstr(lirInstr& i)
{
   for(auto it=i.getArgs().begin();it!=i.getArgs().end();++it)
      runArg(i,**it);
}

void lirTransform::scheduleInjectBefore(lirInstr& noob, lirInstr& before)
{
   m_changes.push_back(new injectChange(noob,before,true,getCurrentStream()));
}

void lirTransform::scheduleInjectAfter(lirInstr& noob, lirInstr& after)
{
   m_changes.push_back(new injectChange(noob,after,false,getCurrentStream()));
}

void lirTransform::scheduleAppend(lirInstr& noob)
{
   m_changes.push_back(new appendChange(noob,getCurrentStream()));
}

void lirTransform::scheduleVarBind(lirInstr& noob, lirArg& arg, varTable& v, size_t storage)
{
   m_changes.push_back(new varBindChange(noob,arg,v,storage));
}

void lirTransform::injectChange::apply()
{
   if(m_beforeAfter)
   {
      m_antecedent.injectBefore(*m_pNoob);
      m_pNoob = NULL;
   }
   else
   {
      m_antecedent.injectAfter(*m_pNoob);
      m_pNoob = NULL;
   }

   // sure-up the tail if I injected after it
   m_s.pTail = &m_s.pTail->tail();
}

void lirTransform::appendChange::apply()
{
   m_s.pTail->append(*m_pNoob);
   m_pNoob = NULL;
}

void lirTransform::varBindChange::apply()
{
   var& v = m_v.create(m_a.getName());
   v.refs[m_i.orderNum].push_back(&m_a);
   v.instrToStorageMap[m_i.orderNum].insert(m_stor);
   v.storageToInstrMap[m_stor].insert(m_i.orderNum);
   v.storageDisambiguators[&m_a] = m_stor;
}

void lirTransform::applyChanges()
{
   for(auto it=m_changes.begin();it!=m_changes.end();++it)
      (*it)->apply();
}

void lirNameCollector::runArg(lirInstr& i, lirArg& a)
{
   m_u.seed(a.getName());
   lirTransform::runArg(i,a);
}

void lirCallVirtualStackCalculation::runInstr(lirInstr& i)
{
   if(i.instrId == cmn::tgt::kPreCallStackAlloc)
      m_preCalls.push_back(&i);

   lirTransform::runInstr(i);

   if(i.instrId == cmn::tgt::kCall)
   {
      // rval not included in stack calculation
      m_argRealSizes.erase(m_argRealSizes.begin());
      // ignore the calladdr/instptr for calls/invokes
      m_argRealSizes.erase(m_argRealSizes.begin());

      // any scratch regs that the target might have are appended to the call,
      // but should not be considered arguments for this calculation
      {
         std::vector<size_t> scratch;
         m_t.getCallConvention().createScratchRegisterBank(scratch);
         for(size_t i=0;i<scratch.size();i++)
            m_argRealSizes.erase(--m_argRealSizes.end());
      }

      lirInstr& precall = *m_preCalls.back();
      size_t subcallStackSize = m_t.getCallConvention().getShadowSpace();
      subcallStackSize += m_t.getCallConvention().getArgumentStackSpace(m_argRealSizes);
      precall.addArg<lirArgConst>("totalStackSize",subcallStackSize);

      m_argRealSizes.clear();
      m_preCalls.pop_back();
   }
}

void lirCallVirtualStackCalculation::runArg(lirInstr& i, lirArg& a)
{
   if(i.instrId == cmn::tgt::kCall)
      m_argRealSizes.push_back(m_t.getRealSize(a.getSize()));

   lirTransform::runArg(i,a);
}

void lirPairedInstrDecomposition::_runStream(lirStream& s)
{
   // inject kSelectSegment at the start of each stream
   auto i = new lirInstr(cmn::tgt::kSelectSegment);
   i->addArg<lirArgConst>(s.segment,0);
   scheduleInjectBefore(*i,s.pTail->head());

   lirTransform::_runStream(s);
}

void lirPairedInstrDecomposition::runInstr(lirInstr& i)
{
   // inject kExitFunc
   if(i.instrId == cmn::tgt::kEnterFunc)
   {
      auto noob = new lirInstr(cmn::tgt::kExitFunc);
      scheduleAppend(*noob);
   }

   // inject kUnreserveLocal
   if(i.instrId == cmn::tgt::kReserveLocal)
   {
      auto noob = new lirInstr(cmn::tgt::kUnreserveLocal);
      noob->addArg(i.getArgs()[0]->clone());
      // I know there's an kExitFunc instr; I want just ahead of that
      scheduleInjectAfter(*noob,i.tail());
   }

   // inject kPostCallStackAlloc
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& other = i.searchUp([](auto& i) { return i.instrId == cmn::tgt::kPreCallStackAlloc; });
      auto noob = new lirInstr(cmn::tgt::kPostCallStackAlloc);
      noob->addArg(other.getArgs()[0]->clone());
      scheduleInjectAfter(*noob,i);
   }

   lirTransform::runInstr(i);
}

void lirNumberingTransform::_runStream(lirStream& s)
{
   m_next = 10;
   lirTransform::_runStream(s);
}

void lirNumberingTransform::runInstr(lirInstr& i)
{
   i.orderNum = m_next;
   m_next += 10;
   lirTransform::runInstr(i);
}

void lirCodeShapeDecomposition::runInstr(lirInstr& i)
{
   // eventually, genericize this to notice when any instr fmt is unimplementable
   // and tweak until it's ok
   //
   // for now, only handle special cases

   // call can't handle immediate passed args
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& args = i.getArgs();
      for(size_t j=2;j<args.size();j++)
      {
         lirArg *pArg = args[j];
         lirArgConst *pImm = dynamic_cast<lirArgConst*>(pArg);
         if(!pImm)
            continue;

         cmn::uniquifier u;
         lirNameCollector(u).runStream(getCurrentStream());

         auto pTmp = new lirArgTemp(u.makeUnique("t"),pImm->getSize());

         auto pMov = new lirInstr(cmn::tgt::kMov);
         pMov->addArg(*pTmp);
         pMov->addArg(*pImm);
         pMov->comment = "shape:hoist imm from call";

         scheduleInjectBefore(*pMov,i);

         // go ahead and swap out the arg directly; this is safe b/c I haven't
         // traversed it yet
         args[j] = &pTmp->clone();
      }
   }

   // call can't handle any addr modifications (i.e. displacemnets or derefs)
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& args = i.getArgs();
      for(size_t j=2;j<args.size();j++)
      {
         lirArg *pArg = args[j];
         if(!pArg->disp && !pArg->addrOf)
            continue;

         cmn::uniquifier u;
         lirNameCollector(u).runStream(getCurrentStream());

         auto pTmp = new lirArgTemp(u.makeUnique("t"),pArg->getSize());

         auto pMov = new lirInstr(cmn::tgt::kMov);
         pMov->addArg(*pTmp);
         pMov->addArg(*pArg);
         pMov->comment = "shape:hoist addrOf from call";

         scheduleInjectBefore(*pMov,i);

         // go ahead and swap out the arg directly; this is safe b/c I haven't
         // traversed it yet
         args[j] = &pTmp->clone();
      }
   }

   // temp hack for bops masquarading as =
   if(i.instrId == cmn::tgt::kMov)
   {
      auto *pArg = dynamic_cast<lirArgConst*>(i.getArgs()[0]);
      if(pArg)
      {
         // obviously you can't mov into a literal

         cmn::uniquifier u;
         lirNameCollector(u).runStream(getCurrentStream());

         auto pTmp = new lirArgTemp(u.makeUnique("t"),pArg->getSize());

         auto pMov = new lirInstr(cmn::tgt::kMov);
         pMov->addArg(*pTmp);
         pMov->addArg(*pArg);
         pMov->comment = "shape:hoist const from mov lhs";

         scheduleInjectBefore(*pMov,i);

         // go ahead and swap out the arg directly; this is safe b/c I haven't
         // traversed it yet
         i.getArgs()[0] = &pTmp->clone();
      }
   }

   lirTransform::runInstr(i);
}

void runLirTransforms(lirStreams& lir, cmn::tgt::iTargetInfo& t)
{
   { lirCallVirtualStackCalculation xfrm(t); xfrm.runStreams(lir); }
   { lirPairedInstrDecomposition xfrm; xfrm.runStreams(lir); }
   { lirCodeShapeDecomposition xfrm; xfrm.runStreams(lir); }
   { lirNumberingTransform xfrm; xfrm.runStreams(lir); }
}

void lirVarGen::runArg(lirInstr& i, lirArg& a)
{
   var& v = m_v.create(a.getName());
   v.refs[i.orderNum].push_back(&a);

   if(dynamic_cast<lirArgConst*>(&a))
      v.requireStorage(i.orderNum,cmn::tgt::kStorageImmediate);

   lirTransform::runArg(i,a);
}

void spuriousVarStripper::runInstr(lirInstr& i)
{
   if(i.instrId == cmn::tgt::kCall)
   {
      auto& args = i.getArgs();
      auto *pKeeper = args[1]; // keep only the call ptr

      for(auto it=args.begin();it!=args.end();++it)
      {
         if(*it != pKeeper)
         {
            m_v.demand(**it).unbindArgButKeepStorage(i,**it);
            delete *it;
         }
      }

      args.clear();
      args.push_back(pKeeper);
   }

   lirTransform::runInstr(i);
}

void codeShapeTransform::runInstr(lirInstr& i)
{
   m_vfProg.onInstr(i);

   cdwDEBUG("checking codeshape for instr #%lld instrId=%lld\n",i.orderNum,i.instrId);

   // convert lirArgs into the target's argTypes
   std::vector<cmn::tgt::argTypes> at;
   categorizeArgs(i,at);

   // see if a compatible format exists
   const cmn::tgt::instrInfo *pIInfo = NULL;
   bool needsReshaping;
   getInstrInfo(i,at,pIInfo,needsReshaping);
   if(!needsReshaping)
   {
      lirTransform::runInstr(i);
      return;
   }

   // well, crap; let's try to spill

   // change write operands into registers and try again
   std::vector<cmn::tgt::argTypes> retryArgs;
   std::set<size_t> regOffsets;
   findWorkingInstrFmt(*pIInfo,at,retryArgs,regOffsets);
   if(regOffsets.size() != 1)
      cdwTHROW("unimpled");

   // choose registers to use
   bool needsSpill;
   size_t altStor = m_f.pickScratchRegister(needsSpill);
   if(needsSpill && (pIInfo->stackSensitive || isStackFramePtrInUse(i,*pIInfo,at)))
      cdwTHROW("can't spill, but must spill... arrgh!");

   // prep for lir gen
   cmn::uniquifier u;
   lirNameCollector(u).runStream(getCurrentStream());
   std::string varName = u.makeUnique("t");
   lirArg& origArg = *i.getArgs()[*regOffsets.begin()];
   size_t origStor = m_v.demand(origArg).getStorageFor(i.orderNum,origArg);

   // inject save
   if(needsSpill)
   {
      auto& push = *new lirInstr(cmn::tgt::kPush);
      auto& arg = push.addArg<lirArgTemp>(varName,origArg.getSize());
      push.comment = "codeshape spill";

      scheduleInjectBefore(push,i);
      scheduleVarBind(push,arg,m_v,altStor);
   }

   // mov [memA] [memB]
   //
   //    changes to
   //
   // push RX
   // mov RX [memB]
   // mov [memA] Rx
   // pop RX

   // inject move
   {
      auto& mov = *new lirInstr(cmn::tgt::kMov);
      auto& dest = mov.addArg<lirArgTemp>(varName,origArg.getSize());
      auto& src = mov.addArg(origArg.clone());
      mov.comment = "codeshape decomp";

      scheduleInjectBefore(mov,i);
      scheduleVarBind(mov,dest,m_v,altStor);
      scheduleVarBind(mov,src,m_v,origStor);
   }

   // modify original instruction
   {
      var& origVar = m_v.demand(origArg);
      origVar.unbindArgButKeepStorage(i,origArg);

      auto *pUpdatedArg = new lirArgTemp(varName,origArg.getSize());
      i.getArgs()[*regOffsets.begin()] = pUpdatedArg;

      scheduleVarBind(i,*pUpdatedArg,m_v,altStor);
   }

   // inject restore
   if(needsSpill)
   {
      auto& pop = *new lirInstr(cmn::tgt::kPop);
      auto& arg = pop.addArg<lirArgTemp>(varName,origArg.getSize());
      pop.comment = "codeshape restore";

      scheduleInjectBefore(pop,i.next());
      scheduleVarBind(pop,arg,m_v,altStor);
   }

   delete &origArg;

   lirTransform::runInstr(i);
}

void codeShapeTransform::categorizeArgs(lirInstr& i, std::vector<cmn::tgt::argTypes>& at)
{
   for(auto it=i.getArgs().begin();it!=i.getArgs().end();++it)
   {
      if((*it)->addrOf)
      {
         cdwDEBUG("   m\n");
         markMem(**it,at);
      }
      else
      {
         size_t stor = m_v.demand(**it).getStorageFor(i.orderNum,**it);
         if(cmn::tgt::isVStack(stor) || cmn::tgt::isStackStorage(stor))
         {
            cdwDEBUG("   m\n");
            markMem(**it,at);
         }
         else if(stor == cmn::tgt::kStorageImmediate)
         {
            bool isNumeric = ::isdigit((*it)->getName().c_str()[0]);
            if(isNumeric)
            {
               cdwDEBUG("   i\n");
               markImm(**it,at);
            }
            else
            {
               cdwDEBUG("   m (l)\n");
               at.push_back(cmn::tgt::kM64);
            }
         }
         else
         {
            cdwDEBUG("   r\n");
            markReg(**it,at);
         }
      }
   }
}

void codeShapeTransform::markReg(lirArg& a, std::vector<cmn::tgt::argTypes>& at)
{
   size_t size = m_t.getRealSize(a.getSize());
   if(size > 4)
      at.push_back(cmn::tgt::kR64);
   else if(size > 2)
      at.push_back(cmn::tgt::kR32);
   else if(size > 1)
      at.push_back(cmn::tgt::kR16);
   else
      at.push_back(cmn::tgt::kR8);
}

void codeShapeTransform::markImm(lirArg& a, std::vector<cmn::tgt::argTypes>& at)
{
   size_t size = m_t.getRealSize(a.getSize());
   if(size > 4)
      at.push_back(cmn::tgt::kI64);
   else if(size > 2)
      at.push_back(cmn::tgt::kI32);
   else if(size > 1)
      at.push_back(cmn::tgt::kI16);
   else
      at.push_back(cmn::tgt::kI8);
}

void codeShapeTransform::markMem(lirArg& a, std::vector<cmn::tgt::argTypes>& at)
{
   size_t size = m_t.getRealSize(a.getSize());
   if(size > 4)
      at.push_back(cmn::tgt::kM64);
   else if(size > 2)
      at.push_back(cmn::tgt::kM32);
   else if(size > 1)
      at.push_back(cmn::tgt::kM16);
   else
      at.push_back(cmn::tgt::kM8);
}

void codeShapeTransform::getInstrInfo(lirInstr& i, const std::vector<cmn::tgt::argTypes>& at, const cmn::tgt::instrInfo*& pIInfo, bool& needsReshaping)
{
   pIInfo = m_t.getProc().getInstr(i.instrId);
   if(pIInfo->fmts == NULL)
   {
      cdwDEBUG("not checking magic instruction\n");
      needsReshaping = false;
      return;
   }
   auto *pFmt = pIInfo->findFmt(at);
   if(pFmt)
   {
      cdwDEBUG("ok\n");
      needsReshaping = false;
   }
   else
      needsReshaping = true;
}

void codeShapeTransform::findWorkingInstrFmt(const cmn::tgt::instrInfo& iInfo, const std::vector<cmn::tgt::argTypes>& args, std::vector<cmn::tgt::argTypes>& retryArgs, std::set<size_t>& changedIndicies)
{
   do
   {
      bool didWork = mutateInstrFmt(iInfo,args,retryArgs,changedIndicies);
      if(!didWork)
         cdwTHROW("can't find any valid mutated instr fmt");

      auto *pFmt = iInfo.findFmt(retryArgs);
      if(pFmt)
         return;

   } while(true);
}

bool codeShapeTransform::mutateInstrFmt(const cmn::tgt::instrInfo& iInfo, const std::vector<cmn::tgt::argTypes>& args, std::vector<cmn::tgt::argTypes>& retryArgs, std::set<size_t>& changedIndicies)
{
   size_t origCnt = changedIndicies.size();
   retryArgs.clear();
   retryArgs.reserve(args.size());
   bool makeChanges = true;

   size_t idx=0;
   for(auto it=args.begin();it!=args.end();++it,idx++)
   {
      if(makeChanges && isMemory(*it) && isReadOnly(iInfo,idx))
      {
         retryArgs.push_back(makeRegister(*it));
         changedIndicies.insert(idx);
         if(changedIndicies.size() > origCnt)
            makeChanges = false;
      }
      else
      {
         retryArgs.push_back(*it);
      }
   }

   return !makeChanges;
}

bool codeShapeTransform::isReadOnly(const cmn::tgt::instrInfo& info, size_t idx)
{
   return info.argIo[idx] == 'r';
}

bool codeShapeTransform::isMemory(cmn::tgt::argTypes a)
{
   return (
      a == cmn::tgt::kM8 ||
      a == cmn::tgt::kM16 ||
      a == cmn::tgt::kM32 ||
      a == cmn::tgt::kM64
   );
}

cmn::tgt::argTypes codeShapeTransform::makeRegister(cmn::tgt::argTypes a)
{
   switch(a)
   {
      case cmn::tgt::kM8:  return cmn::tgt::kR8;
      case cmn::tgt::kM16: return cmn::tgt::kR16;
      case cmn::tgt::kM32: return cmn::tgt::kR32;
      case cmn::tgt::kM64: return cmn::tgt::kR64;
      default:
         cdwTHROW("ISE");
   }
}

bool codeShapeTransform::isStackFramePtrInUse(lirInstr& i, const cmn::tgt::instrInfo& iInfo, std::vector<cmn::tgt::argTypes> origArgs)
{
   bool stackFramePtrInUse = false;

   size_t idx=0;
   for(auto it=origArgs.begin();it!=origArgs.end();++it,idx++)
   {
      if(isMemory(*it) && isReadOnly(iInfo,idx))
         ;
      else
      {
         lirArg& arg = *i.getArgs()[idx];
         size_t stor = m_v.demand(arg).getStorageFor(i.orderNum,arg);
         stackFramePtrInUse = cmn::tgt::isVStack(stor);
         if(stackFramePtrInUse)
            break;
      }
   }

   return stackFramePtrInUse;
}

} // namespace liam

#ifdef  cdwTraceContext
#undef  cdwTraceContext
#endif
#define cdwTraceContext "instrPrefs"

#include "../cmn/target.hpp"
#include "../cmn/throw.hpp"
#include "../cmn/trace.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

size_t instrPrefs::publishRequirements(lirStream& s, varTable& v, const cmn::tgt::iTargetInfo& target)
{
   instrPrefs self(v,target);

   self.handle(s);

   return self.m_stackSpace;
}

void instrPrefs::handle(lirStream& s)
{
   m_pCurrStream = &s;
   auto pInstr = &s.pTail->head();
   for(;;pInstr=&pInstr->next())
   {
      handle(*pInstr);
      if(pInstr->isLast()) break;
   }
}

void instrPrefs::handle(lirInstr& i)
{
   switch(i.instrId)
   {
      default:
         break;

      case cmn::tgt::kEnterFunc:
         {
            auto& cc = m_target.getCallConvention();
            //handle(i,cc,/*outOrIn*/false,/*isInvoke*/false);
            handle(i,cc,/*isLeaving*/false);
         }
         break;

      case cmn::tgt::kReserveLocal:
         {
            var& v = m_vTable.demand(*i.getArgs()[0]);
            v.requireStorage(i.orderNum,cmn::tgt::kStorageUndecidedStack);
            cdwDEBUG("> local %s needs stack, any stack\n",
               //dynamic_cast<lirArgVar*>(i.getArgs()[0])->name.c_str());
               i.getArgs()[0]->getName().c_str());
         }
         break;

      case cmn::tgt::kCall:
         {
            auto& cc = m_target.getCallConvention();
            //handle(i,cc,/*outOrIn*/true,/*isInvoke*/true); // HACK!
            handle(i,cc,/*isLeaving*/true);
         }
         break;
      case cmn::tgt::kRet:
         {
            if(i.getArgs().size())
            {
               lirArg& arg = *i.getArgs()[0];
               auto& cc = m_target.getCallConvention();
               std::vector<size_t> argStorage;
               cc.getRValAndArgBank(argStorage);
               var& v = m_vTable.demand(arg.getName());
               v.requireStorage(i.orderNum,argStorage[0]);
            }
         }
         break;
      case cmn::tgt::kSyscall:
         {
            cdwTHROW("unimpled");
            auto& cc = m_target.getSyscallConvention();
            handle(i,cc,/*outOrIn*/true,/*isInvoke*/false);
         }
         break;
   }
}

void instrPrefs::handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool outOrIn, bool isInvoke)
{
   std::vector<size_t> argStorage;
   cc.getRValAndArgBank(argStorage);
   //size_t shadow = cc.getShadowSpace();
   int stackSpace = 0;

   // decl nodes have no return value
   int offset = outOrIn ? 0 : 1;

   // invoke nodes don't pass their first arg

   if(!cc.stackArgsPushRToL())
      throw std::runtime_error("unimpled! 1");
   for(size_t k = i.getArgs().size()-1;;k--)
   {
      int offset2 = offset; // awful! :(

      // in invoke nodes, the first arg is actually the addr to call
      // that doesn't particiate in calling convention
      if(isInvoke && k==1)
         continue;
      else if(isInvoke && k>0)
         offset2--;

      var& v = m_vTable.demand(*i.getArgs()[k]);

      if((k+offset2) >= argStorage.size())
      {
         // use the stack
         if(outOrIn)
         {
            // going out
            size_t stor = cmn::tgt::makeStackStorage(stackSpace);
            stor = m_vTable.getVirtualStack().reserveVirtStorage(stor);
            v.requireStorage(i.orderNum,stor);
            stackSpace -= m_target.getRealSize(v.getSize());
         }
         else
         {
            // coming in
            throw std::runtime_error("unimpled 3");
            //v.requireStorage(i.orderNum,cmn::tgt::kStorageStackArg);
            //cdwDEBUG("assigning RSP+%lld to %s\n",shadow + v.getSize(),v.name.c_str());
         }
      }
      else
      {
         // use a register
         v.requireStorage(i.orderNum,argStorage[k+offset2]);
         cdwDEBUG("assigning r%lld to %s\n",argStorage[k+offset2],v.name.c_str());
      }

      if(k==0)
         break;
   }
}

//   0    1    2    3    4     5     6     < k
// rval addr self args [foo] [bar] [baz]          using stack if k>size
// rax       rcx  rdx   r8    r9    s
//
// 3-2 = 1 <> 2
//
//  0   1   2
// rax rcx rdx r8 r9   < size=5           if k>1, k--

void instrPrefs::handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool isLeaving)
{
   std::vector<size_t> argStorage,trashedRegs;
   cc.getRValAndArgBank(argStorage);
   cc.createScratchRegisterBank(trashedRegs);
   int stackSpace = 0;

   // call instructions (going out) have an address at argument 1
   if(isLeaving)
      argStorage.insert(argStorage.begin()+1,0);

   if(!cc.stackArgsPushRToL())
      cdwTHROW("unimpled");

   if(isLeaving && i.getArgs().size() < 2)
      cdwTHROW("every call should have at least 2 args (rval, addr)");

   size_t loopMax = i.getArgs().size()-1;
   if(isLeaving)
      loopMax -= trashedRegs.size(); // when leaving the last N are dummies
   for(size_t k = loopMax;;k--)
   {
      if(isLeaving && k==1)
         continue;

      var& v = m_vTable.demand(*i.getArgs()[k]);

      if(k > argStorage.size())
      {
         // use the stack
         if(isLeaving)
         {
            // going out
            size_t stor = cmn::tgt::makeStackStorage(stackSpace);
            stor = m_vTable.getVirtualStack().reserveVirtStorage(stor);
            v.requireStorage(i.orderNum,stor);
            stackSpace += m_target.getRealSize(v.getSize());
         }
         else
         {
            throw std::runtime_error("unimpled 3");
            cdwTHROW("unimpled");
         }
      }
      else
      {
         // use a register
         v.requireStorage(i.orderNum,argStorage[k]);
         cdwDEBUG("assigning r%lld to %s\n",argStorage[k],v.name.c_str());
      }

      if(k==0)
         break;
   }

   // scratch regs are attached to the end of the call
   if(isLeaving)
   {
      size_t offset = i.getArgs().size() - trashedRegs.size();
      for(size_t k=0;k<trashedRegs.size();k++)
      {
         var& v = m_vTable.demand(*i.getArgs()[offset+k]);
         v.requireStorage(i.orderNum,trashedRegs[k]);
      }
   }
}

// deadcode kept around jic
void instrPrefs::trashVolatileRegs(lirInstr& i, const cmn::tgt::iCallingConvention& cc)
{
   cdwTHROW("unreachable");

   std::vector<size_t> argStorage;
   cc.getRValAndArgBank(argStorage);
   cc.createScratchRegisterBank(argStorage);

   // +1 b/c the postcall instr has a stack size as a literal
   if(argStorage.size() + 1 != i.getArgs().size())
      cdwTHROW("ISE: trashVolatileRegs cardinality doesn't match arg creation alg");

   for(size_t k=1;k<i.getArgs().size();k++)
   {
      var& v = m_vTable.demand(*i.getArgs()[k]);
      v.requireStorage(i.orderNum,argStorage[k-1]);
      cdwDEBUG("assigning r%lld to %s\n",argStorage[k-1],v.name.c_str());
   }
}

} // namespace liam

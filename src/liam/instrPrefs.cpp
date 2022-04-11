#include "../cmn/target.hpp"
#include "../cmn/throw.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

size_t instrPrefs::publishRequirements(lirStreams& s, varTable& v, const cmn::tgt::iTargetInfo& target)
{
   instrPrefs self(v,target);

   for(auto it=s.page.begin();it!=s.page.end();++it)
      self.handle(it->second);

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
      case cmn::tgt::kEnterFunc:
         {
            auto& cc = m_target.getCallConvention();
            handle(i,cc,/*outOrIn*/false,/*isInvoke*/false);
         }
         break;
      case cmn::tgt::kExitFunc: // no prefs
      case cmn::tgt::kPush:
      case cmn::tgt::kPop:
      case cmn::tgt::kMov:
      case cmn::tgt::kPreCallStackAlloc:
      case cmn::tgt::kPostCallStackAlloc:
         break;
      case cmn::tgt::kCall:
         {
            auto& cc = m_target.getCallConvention();
            handle(i,cc,/*outOrIn*/true,/*isInvoke*/true); // HACK!
         }
         break;
      case cmn::tgt::kRet:
         {
            cdwTHROW("unimpled");
            if(i.getArgs().size())
            {
               lirArgVar *pVar = dynamic_cast<lirArgVar*>(i.getArgs()[0]);
               if(pVar)
               {
                  auto& cc = m_target.getCallConvention();
                  std::vector<size_t> argStorage;
                  cc.getRValAndArgBank(argStorage);
                  var& v = m_vTable.demand(pVar->name);
                  v.requireStorage(i,argStorage[0]);
               }
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
      default:
         throw std::runtime_error("unknown instruction!");
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
            v.requireStorage(i,stor);
            stackSpace -= m_target.getRealSize(v.getSize());
         }
         else
         {
            // coming in
            throw std::runtime_error("unimpled 3");
            //v.requireStorage(i,cmn::tgt::kStorageStackArg);
            //::printf("> assigning RSP+%lld to %s\n",shadow + v.getSize(),v.name.c_str());
         }
      }
      else
      {
         // use a register
         v.requireStorage(i,argStorage[k+offset2]);
         ::printf("> assigning r%lld to %s\n",argStorage[k+offset2],v.name.c_str());
      }

      if(k==0)
         break;
   }
}

} // namespace liam

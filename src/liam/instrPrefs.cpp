#include "../cmn/target.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include "varGen.hpp"
#include <stdexcept>

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
      case cmn::tgt::kDeclParam:
         {
            auto& cc = m_target.getCallConvention();
            handle(i,cc,/*outOrIn*/false);
         }
         break;
      case cmn::tgt::kPush: // no prefs
      case cmn::tgt::kPop:
      case cmn::tgt::kMov:
         break;
      case cmn::tgt::kCall:
         {
            auto& cc = m_target.getCallConvention();
            handle(i,cc,/*outOrIn*/true);
         }
         break;
      case cmn::tgt::kRet:
         {
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
            auto& cc = m_target.getSyscallConvention();
            handle(i,cc,/*outOrIn*/true);
         }
         break;
      default:
         throw std::runtime_error("unknown instruction!");
   }
}

void instrPrefs::handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool outOrIn)
{
   std::vector<size_t> argStorage;
   cc.getRValAndArgBank(argStorage);
   size_t shadow = cc.getShadowSpace();

   if(!cc.stackArgsPushRToL())
      throw std::runtime_error("unimpled!");
   for(size_t k = i.getArgs().size()-1;;k--)
   {
      var& v = m_vTable.demand(*i.getArgs()[k]);

      if(k >= argStorage.size())
      {
         // use the stack
         if(outOrIn)
         {
            // going out
            v.requireStorage(i,cmn::tgt::kStorageStackLocal);
            ::printf("> assigning RSP-%lld to %s\n",m_stackSpace,v.name.c_str());
            m_stackSpace -= v.getSize();
         }
         else
         {
            // coming in
            v.requireStorage(i,cmn::tgt::kStorageStackArg);
            ::printf("> assigning RSP+%lld to %s\n",shadow + v.getSize(),v.name.c_str());
         }
      }
      else
      {
         // use a register
         v.requireStorage(i,argStorage[k]);
         ::printf("> assigning r%lld to %s\n",argStorage[k],v.name.c_str());
      }

      if(k==0)
         break;
   }
}

} // namespace liam

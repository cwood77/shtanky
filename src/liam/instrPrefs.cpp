#include "../cmn/target.hpp"
#include "instrPrefs.hpp"
#include "lir.hpp"
#include <stdexcept>

namespace liam {

size_t instrPrefs::publishRequirements(lirStreams& s, const cmn::tgt::iTargetInfo& target)
{
   instrPrefs self(target);

   for(auto it=s.page.begin();it!=s.page.end();++it)
      self.handle(it->second);

   return self.m_stackSpace;
}

void instrPrefs::handle(lirStream& s)
{
   m_pCurrStream = &s;
   auto pInstr = &s.pTail->head();
   for(;!pInstr->isLast();pInstr=&pInstr->next())
   {
      handle(*pInstr);
   }
}

void instrPrefs::handle(lirInstr& i)
{
   switch(i.instrId)
   {
      case cmn::tgt::kDeclParam:
         {
            auto& cc = m_target.getCallConvention();
            handle(i,cc,false);
         }
         break;
      case cmn::tgt::kCall:
         {
            auto& cc = m_target.getCallConvention();
            handle(i,cc,true);
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
                  auto& v = m_pCurrStream->getVariableByName(pVar->name);
                  v.storage[i.orderNum] = lirVarStorage::reg(argStorage[0]);
               }
            }
         }
         break;
      case cmn::tgt::kSyscall:
         {
            auto& cc = m_target.getSyscallConvention();
            handle(i,cc,true);
         }
         break;
      default:
         throw std::runtime_error("unknown instruction!");
   }
}

void instrPrefs::handle(lirInstr& i, const cmn::tgt::iCallingConvention& cc, bool outOrIn)
{
   if(!cc.stackArgsPushRToL())
      throw std::runtime_error("unimpled!");

   std::vector<size_t> argStorage;
   cc.getRValAndArgBank(argStorage);
   size_t shadow = cc.getShadowSpace();

   for(size_t k = i.getArgs().size()-1;;k--)
   {
      lirArgVar& a = dynamic_cast<lirArgVar&>(*i.getArgs()[k]);
      auto& v = m_pCurrStream->getVariableByName(a.name);

      if(k+1 >= argStorage.size())
      {
         // use the stack
         if(outOrIn)
         {
            // going out
            v.storage[i.orderNum] = lirVarStorage::stack(m_stackSpace);
            ::printf("> assigning RSP-%lld to %s\n",m_stackSpace,a.name.c_str());
            m_stackSpace -= v.size;
         }
         else
         {
            // coming in
            v.storage[i.orderNum] = lirVarStorage::stack(shadow + v.size);
            ::printf("> assigning RSP+%lld to %s\n",shadow + v.size,a.name.c_str());
         }
      }
      else
      {
         // use a register
         v.storage[i.orderNum] = lirVarStorage::reg(argStorage[k+1]);
         ::printf("> assigning r%lld to %s\n",argStorage[k+1],a.name.c_str());
      }

      if(k==0)
         break;
   }
}

} // namespace liam

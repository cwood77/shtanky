#include "../cmn/out.hpp"
#include "../cmn/target.hpp"
#include "../cmn/trace.hpp"
#include "asmCodeGen.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

void asmCodeGen::generate(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, cmn::columnedOutStream& o)
{
   lirInstr *pInstr = &s.pTail->head();
   while(true)
   {
      switch(pInstr->instrId)
      {
         case cmn::tgt::kDeclParam:
         case cmn::tgt::kPush:
         case cmn::tgt::kPop:
         case cmn::tgt::kMov:
         case cmn::tgt::kCall:
         case cmn::tgt::kRet:
         case cmn::tgt::kSyscall:
            {
               auto& l = o.appendLine();
               cdwDEBUG("%s\n",t.getProc().getInstr(pInstr->instrId)->name);
               //(*l[1]) << t.getProc().getInstr(pInstr->instrId)->name;
               generateArgs(*pInstr,v,t,(*l[1]));
            }
            break;
      }

      if(pInstr->isLast())
         break;
      else
         pInstr = &pInstr->next();
   }
}

void asmCodeGen::generateArgs(lirInstr& i, varTable& v, cmn::tgt::iTargetInfo& t, std::stringstream& s)
{
   bool first = false;
   auto& args = i.getArgs();
   for(auto it=args.begin();it!=args.end();++it)
   {
/*      if(!first)
         s << ", ";*/
      size_t stor = v.getStorageFor(i.orderNum,**it);

      if((*it)->addrOf)
         cdwDEBUG("   [%s]\n",t.getProc().getRegName(stor));
      else
         cdwDEBUG("   %s\n",t.getProc().getRegName(stor));

//      s << t.getProc().getRegName(stor);
   }
}

} // namespace liam

#include "../cmn/out.hpp"
#include "../cmn/target.hpp"
#include "../cmn/textTable.hpp"
#include "../cmn/trace.hpp"
#include "asmCodeGen.hpp"
#include "lir.hpp"
#include "varGen.hpp"

namespace liam {

void asmCodeGen::generate(lirStream& s, varTable& v, cmn::tgt::iTargetInfo& t, cmn::outStream& o)
{
   cmn::textTable tt;
   cmn::textTableLineWriter w(tt);

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
               cdwDEBUG("%s\n",t.getProc().getInstr(pInstr->instrId)->name);
               w[1] << t.getProc().getInstr(pInstr->instrId)->name;
               generateArgs(*pInstr,v,t,w);
               if(!pInstr->comment.empty())
                  w[2] << "; " << pInstr->comment;
               w.advanceLine();
            }
            break;
      }

      if(pInstr->isLast())
         break;
      else
         pInstr = &pInstr->next();
   }

   tt.compileAndWrite(o.stream());
}

void asmCodeGen::generateArgs(lirInstr& i, varTable& v, cmn::tgt::iTargetInfo& t, cmn::textTableLineWriter& w)
{
   bool first = false;
   auto& args = i.getArgs();
   for(auto it=args.begin();it!=args.end();++it)
   {
      if(!first)
         w[1] << ", ";
      size_t stor = v.getStorageFor(i.orderNum,**it);

      if((*it)->addrOf)
         cdwDEBUG("   [%s]\n",t.getProc().getRegName(stor));
      else
         cdwDEBUG("   %s\n",t.getProc().getRegName(stor));

      if((*it)->addrOf)
         w[1] << "[" << t.getProc().getRegName(stor) << "]";
      else
         w[1] << t.getProc().getRegName(stor);
   }
}

} // namespace liam

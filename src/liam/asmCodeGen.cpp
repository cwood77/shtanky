#include "../cmn/out.hpp"
#include "../cmn/target.hpp"
#include "../cmn/textTable.hpp"
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
      size_t framePtr = cmn::tgt::kStorageStackFramePtr;

      if(cmn::tgt::isVStack(stor))
      {
         stor = v.getVirtualStack().mapToReal(stor);
         framePtr = cmn::tgt::kStorageStackPtr;
      }

      if(cmn::tgt::isStackStorage(stor))
         w[1] << "["
            << t.getProc().getRegName(framePtr)
            << cmn::tgt::getStackDisp(stor)
         << "]";
      else if((*it)->addrOf)
         w[1] << "[" << t.getProc().getRegName(stor) << "]";
      else
         w[1] << t.getProc().getRegName(stor);
   }
}

} // namespace liam
